#include "gstreamer.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <QDebug>
#include <QDir>
#include <QUuid>

Gstreamer::Gstreamer(QObject* parent)
	:QObject(parent), isPlaying(false)
{
	gst_init(NULL, NULL);

	// Load our sound file from resources
	sound = new QFile(":/sounds/demonic_whoosh.mp3", this);
	sound->open(QIODevice::ReadOnly);
	connect(sound, SIGNAL(readyRead()), SLOT(readyRead()));

	// Construct random temp name for fifo
	QString fifopath = QDir::tempPath() + QDir::separator() + "Bequnge" + QString(QUuid::createUuid());
	
	if(mkfifo(fifopath.toAscii(), S_IRUSR | S_IWUSR))
		qFatal("Failed to create fifo");

	fifo = new QFile(fifopath, this);
	if(!fifo->open(QIODevice::ReadWrite))
		qFatal("Failed to open fifo");

	// Watch fd until ready to write
	noti = new QSocketNotifier(fifo->handle(), QSocketNotifier::Write, this);
	noti->setEnabled(false);
	connect(noti, SIGNAL(activated(int)), SLOT(readWrite()));

	// Create Gstreamer pipeline
	src = gst_element_factory_make("fdsrc", "src");
	decode = gst_element_factory_make("mad", "decode");
	convert = gst_element_factory_make("audioconvert", "convert");
	resample = gst_element_factory_make("audioresample", "resample");
	sink = gst_element_factory_make("autoaudiosink", "sink");
	if(!src || !decode || !convert || !resample || !sink)
		qFatal("Failed to create gstreamer elements");

	g_object_set(G_OBJECT(src), "fd", fifo->handle(), NULL);

	pipeline = gst_pipeline_new("pipeline");
	if(!pipeline)
		qFatal("Failed to create gstreamer pipeline");

	gst_bin_add_many(GST_BIN(pipeline), src, decode, convert, resample, sink, NULL);

	if(!gst_element_link_many(src, decode, convert, resample, sink, NULL))
		qFatal("Failed to link gstreamer elements");
}

Gstreamer::~Gstreamer()
{
	gst_object_unref(pipeline);

	fifo->close();
	sound->close();

	fifo->remove();
}

void Gstreamer::play()
{
	if(m_time.isValid() && m_time.elapsed() < 1000)
		return;

	m_time = QTime::currentTime();

	sound->seek(0);
	gst_element_set_state(pipeline, GST_STATE_NULL);

	isPlaying = true;
	noti->setEnabled(true);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Gstreamer::stop()
{
	isPlaying = false;
	noti->setEnabled(false);
	gst_element_set_state(pipeline, GST_STATE_NULL);
}

void Gstreamer::changeFile(QString file)
{
	sound->close();
	delete sound;

	sound = new QFile(file, this);
	sound->open(QIODevice::ReadOnly);
	connect(sound, SIGNAL(readyRead()), SLOT(readyRead()));
}

void Gstreamer::readWrite()
{
	noti->setEnabled(false);
	fifo->write(sound->readAll());
	fifo->flush();
}

void Gstreamer::readyRead()
{
	if(isPlaying)
		noti->setEnabled(true);
}

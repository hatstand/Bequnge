#include "openal.h"

#include <QDebug>

OggStream::OggStream(QObject* parent)
	: QObject(parent)
{
	cbs.read_func = readCb;
	cbs.seek_func = seekCb;
	cbs.close_func = closeCb;
	cbs.tell_func = tellCb;
}

size_t OggStream::readCb(void* ptr, size_t size, size_t nmemb, void* datasource)
{
	//qDebug() << __func__;

	QFile* file = static_cast<QFile*>(datasource);
	if(file->atEnd())
		return 0;

	if(!file->isReadable())
	{
		return 0;
	}

	return file->read((char*)ptr, size*nmemb);
}

int OggStream::seekCb(void* datasource, ogg_int64_t offset, int whence)
{
	//qDebug() << __func__;
	QFile* file = static_cast<QFile*>(datasource);
	if(file->isSequential())
		return -1;

	switch(whence)
	{
		case SEEK_SET:
			if(file->seek(offset))
				return 0;
			else
				return -1;
		case SEEK_CUR:
			if(file->seek(file->pos() + offset))
				return 0;
			else
				return -1;
		case SEEK_END:
			if(file->seek(file->size() + offset))
				return 0;
			else
				return -1;

		default:
			return -1;
	}
}

int OggStream::closeCb(void* datasource)
{
	//qDebug() << __func__;
	QFile* file = static_cast<QFile*>(datasource);

	file->close();
	delete file;
	file = NULL;

	return 0;
}

long OggStream::tellCb(void* datasource)
{
	//qDebug() << __func__;
	QFile* file = static_cast<QFile*>(datasource);

	return file->pos();
}

bool OggStream::open(QString path)
{
	oggFile = new QFile(path);
	if(!oggFile->open(QIODevice::ReadOnly))
		return false;

	if(ov_open_callbacks((void*)oggFile, &oggStream, NULL, 0, cbs) < 0)
		qFatal("Failed to open OGG stream");

	vorbisInfo = ov_info(&oggStream, -1);
	vorbisComment = ov_comment(&oggStream, -1);

	if(vorbisInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	alGenBuffers(2, buffers);
	check();
	alGenSources(1, &source);
	check();

	alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
	check();
	alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
	check();
	alSource3f(source, AL_DIRECTION, 0.0, 0.0, 0.0);
	check();
	alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
	check();
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	check();

	return true;
}

OggStream::~OggStream()
{
	alSourceStop(source);
	empty();
	alDeleteSources(1, &source);
	check();
	alDeleteBuffers(1, buffers);
	check();
	
	ov_clear(&oggStream);
}

void OggStream::display()
{
	qDebug() << "version" << vorbisInfo->version;
	qDebug() << "channels" << vorbisInfo->channels;
	qDebug() << "rate" << vorbisInfo->rate;
	qDebug() << "bitrate upper" << vorbisInfo->bitrate_upper;
	qDebug() << "bitrate nominal" << vorbisInfo->bitrate_nominal;
	qDebug() << "bitrate lower" << vorbisInfo->bitrate_lower;
	qDebug() << "bitrate window" << vorbisInfo->bitrate_window;
	
	qDebug() << "vendor" << vorbisComment->vendor;
}

bool OggStream::playback()
{
	if(playing())
		return true;

	//qDebug() << "Rewinding";
	empty();
	alSourceRewind(source);
	ov_time_seek(&oggStream, 0);

	//qDebug() << "Filling buffers";

	if(!stream(buffers[0]))
		return false;

	if(!stream(buffers[1]))
		return false;

	alSourceQueueBuffers(source, 2, buffers);
	alSourcePlay(source);

	connect(this, SIGNAL(test()), this, SLOT(update()), Qt::QueuedConnection);

	return true;
}

bool OggStream::playing()
{
	ALenum state;

	alGetSourcei(source, AL_SOURCE_STATE, &state);

	return(state == AL_PLAYING);
}

bool OggStream::update()
{
	int processed;
	bool active = true;

	alGetSourcei(source, AL_SOURCE_STATE, &processed);
	if(processed != AL_PLAYING)
		return false;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

	while(processed--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(source, 1, &buffer);
		check();

		active = stream(buffer);

		if(active)
		{
			alSourceQueueBuffers(source, 1, &buffer);
			check();
		}
	}


	if(!active)
	{
		empty();
		return false;
	}

	emit test();
	return active;
}

bool OggStream::stream(ALuint buffer)
{
	char data[BUFFER_SIZE];
	int size = 0;
	int section;
	int result;

	while(size < BUFFER_SIZE)
	{
		result = ov_read(&oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);

		if(result > 0)
			size += result;
		else if(result < 0)
			qFatal("Failed to stream");
		else
			break;
	}

	if(size == 0)
		return false;

	alBufferData(buffer, format, data, size, vorbisInfo->rate);
	check();

	return true;
}

void OggStream::empty()
{
	//qDebug() << __func__;
	int queued;

	alSourceStop(source);
	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

	ALuint buffer[queued];

	alSourceUnqueueBuffers(source, queued, buffer);
	check();

	disconnect();
}

void OggStream::check()
{
	int error = alGetError();

	if(error == AL_NO_ERROR)
		return;

	switch(error)
	{
		case AL_INVALID_VALUE:
			qFatal("Invalid value");
		case AL_INVALID_NAME:
			qFatal("Invalid name");
		case AL_INVALID_OPERATION:
			qFatal("Invalid operation");
	}
}

OpenAL::OpenAL()
{
	ogg = new OggStream();

	alutInit(NULL, NULL);

	if(!ogg->open(":/sounds/shortwhoosh.ogg"))
		return;
	ogg->display();
}

bool OpenAL::play()
{
	if(!ogg->playback())
	{
		qWarning("Ogg refused to play");
		return false;
	}

	return ogg->update();
}

OpenAL::~OpenAL()
{
	delete ogg;
	alutExit();
}

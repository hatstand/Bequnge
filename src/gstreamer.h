#ifndef GSTREAMERBEQ_H
#define GSTREAMERBEQ_H

#include <QFile>
#include <QSocketNotifier>
#include <QTime>

#include <gst/gst.h>

class Gstreamer: public QObject
{
	Q_OBJECT

public:
	Gstreamer(QObject* parent = 0);
	virtual ~Gstreamer();

	void play();
	void stop();
	void changeFile(QString);

private:
	QFile* sound;
	QFile* fifo;

	QSocketNotifier* noti;

	GstElement* src;
	GstElement* decode;
	GstElement* convert;
	GstElement* resample;
	GstElement* sink;

	GstElement* pipeline;

	bool isPlaying;

	QTime m_time;

private slots:
	void readWrite();
	void readyRead();
};

#endif

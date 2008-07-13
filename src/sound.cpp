#include "sound.h"
#include <phonon/backendcapabilities.h>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QUrl>

Sound::Sound(QObject* parent)
	: QObject(parent) {

	qDebug() << BackendCapabilities::availableMimeTypes();
	
	media_ = new MediaObject(this);
	output_ = new AudioOutput(MusicCategory, this);
	
	createPath(media_, output_);
	
#ifndef Q_OS_LINUX
	media_->setCurrentSource(MediaSource(QString(":/sounds/shortwhoosh.mp3")));
#else
	media_->setCurrentSource(MediaSource(QString(":/sounds/shortwhoosh.ogg")));
#endif
}

Sound::~Sound() {
}

void Sound::play() {
	media_->stop();
	media_->play();
}

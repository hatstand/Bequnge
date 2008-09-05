#include "sound.h"
#ifndef NO_SOUND
#include <phonon/backendcapabilities.h>
#endif
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QUrl>

Sound::Sound(QObject* parent)
	: QObject(parent) {
#ifndef NO_SOUND
	qDebug() << BackendCapabilities::availableMimeTypes();
	
	media_ = new MediaObject(this);
	output_ = new AudioOutput(MusicCategory, this);
	
	createPath(media_, output_);
	
#ifndef Q_OS_LINUX
	media_->setCurrentSource(MediaSource(QString(":/sounds/shortwhoosh.mp3")));
#else
	media_->setCurrentSource(MediaSource(QString(":/sounds/shortwhoosh.ogg")));
#endif
#endif
}

Sound::~Sound() {
}

void Sound::play() {
#ifndef NO_SOUND
	media_->stop();
	media_->play();
#endif
}

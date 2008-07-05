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
	
	media_->setCurrentSource(MediaSource(QString(":/sounds/shortwhoosh.mp3")));
}

Sound::~Sound() {
}

void Sound::play() {
	Q_ASSERT(QFile::exists(":/sounds/shortwhoosh.mp3"));
	media_->stop();
	media_->play();
}

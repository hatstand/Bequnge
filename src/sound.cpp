#include "sound.h"
#include <phonon/phonon>
#include <QDebug>
#include <QUrl>

Sound::Sound(QObject* parent)
	: QObject(parent) {

	qDebug() << BackendCapabilities::availableMimeTypes();
	
	media_ = new MediaObject(this);
	output_ = new AudioOutput(MusicCategory, this);
	
	createPath(media_, output_);
	
	// Hehehehehehehehe
#ifdef Q_OS_DARWIN
	media_->setCurrentSource(QUrl("https://www.purplehatstands.com/svn/bequnge/src/sounds/shortwhoosh.mp3"));
#else
	media_->setCurrentSource(QUrl("https://www.purplehatstands.com/svn/bequnge/src/sounds/shortwhoosh.ogg"));
#endif
}

Sound::~Sound() {
}

void Sound::play() {
	media_->play();
}

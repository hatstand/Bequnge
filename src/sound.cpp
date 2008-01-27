#include "sound.h"
#include <QDebug>
#include <QUrl>

Sound::Sound(QObject* parent)
	: QObject(parent) {
	
	media_ = new MediaObject(this);
	output_ = new AudioOutput(MusicCategory, this);
	
	createPath(media_, output_);
	
	// Hehehehehehehehe
	media_->setCurrentSource(QUrl("http://www.purplehatstands.com/svn/bequnge/src/sounds/shortwhoosh.ogg"));
}

Sound::~Sound() {
}

void Sound::play() {
	media_->play();
}

#include "sound.h"

#include <Phonon>
using namespace Phonon;

Sound::Sound(QObject* parent)
	: QObject(parent) {
	media_ = createPlayer(Phonon::MusicCategory, MediaSource(":/sounds/shortwhoosh.ogg"));
}

Sound::~Sound() {
	delete media_;
}

void Sound::play() {
	media_->play();
}

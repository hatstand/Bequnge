#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
using namespace Phonon;

class Sound : public QObject {
Q_OBJECT
public:
	Sound(QObject* parent = 0);
	~Sound();
	void play();
	
private:
	MediaObject* media_;
	AudioOutput* output_;
};

#endif

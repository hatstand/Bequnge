#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#ifndef NO_SOUND
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
using namespace Phonon;
#endif

class Sound : public QObject {
Q_OBJECT
public:
	Sound(QObject* parent = 0);
	~Sound();
	void play();
	
private:
#ifndef NO_SOUND
	MediaObject* media_;
	AudioOutput* output_;
#endif
};

#endif

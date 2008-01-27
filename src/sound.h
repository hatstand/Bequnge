#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <Phonon>
using namespace Phonon;

class Sound : public QObject {
Q_OBJECT
public:
	Sound(QObject* parent = 0);
	~Sound();
	void play();
	
private:
	MediaObject* media_;
};

#endif

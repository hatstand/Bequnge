#ifndef SOUND_DISABLED
#ifndef OPENALBEQ_H
#define OPENALBEQ_H

#include <QString>
#include <QFile>

#ifdef Q_OS_DARWIN
#include <openal/al.h>
#include <openal/alc.h>
#include <openal/alut.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#define BUFFER_SIZE (4096 * 8)

class OggStream : public QObject
{
	Q_OBJECT

public:
	OggStream(QObject* parent = 0);
	virtual ~OggStream();
	bool open(QString path);
	void display();
	bool playback();
	bool playing();

	QFile* oggFile;

public slots:
	bool update();
	
signals:
	void test();

private:
	bool stream(ALuint buffer);
	void empty();
	void check();

	static size_t readCb(void* ptr, size_t size, size_t nmemb, void* datasource);
	static int seekCb(void* datasource, ogg_int64_t offset, int whence);
	static int closeCb(void* datasource);
	static long tellCb(void* datasource);

	OggVorbis_File oggStream;
	vorbis_info* vorbisInfo;
	vorbis_comment* vorbisComment;

	ALuint buffers[2];
	ALuint source;
	ALenum format;

	ov_callbacks cbs;
};

class OpenAL
{
public:
	OpenAL();
	~OpenAL();

	bool play();

private:
	OggStream* ogg;
};

#endif
#endif

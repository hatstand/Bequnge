#include "openal.h"

#include <QDebug>

size_t OggStream::readCb(void* ptr, size_t size, size_t nmemb, void* datasource)
{
	QFile* file = static_cast<QFile*>(datasource);
	if(file->atEnd())
		return 0;

	if(!file->isReadable())
	{
		return 0;
	}

	return file->read((char*)ptr, size*nmemb);
}

int OggStream::seekCb(void* datasource, ogg_int64_t offset, int whence)
{
	QFile* file = static_cast<QFile*>(datasource);
	if(file->isSequential())
		return -1;

	switch(whence)
	{
		case SEEK_SET:
			if(file->seek(offset))
				return 0;
			else
				return -1;
		case SEEK_CUR:
			if(file->seek(file->pos() + offset))
				return 0;
			else
				return -1;
		case SEEK_END:
			if(file->seek(file->size() + offset))
				return 0;
			else
				return -1;

		default:
			return -1;
	}
}

int OggStream::closeCb(void* datasource)
{
	QFile* file = static_cast<QFile*>(datasource);

	file->close();
	delete file;
	file = NULL;

	return 0;
}

long OggStream::tellCb(void* datasource)
{
	QFile* file = static_cast<QFile*>(datasource);

	return file->pos();
}

bool OggStream::open(QString path)
{
	oggFile = new QFile(path);
	if(!oggFile->open(QIODevice::ReadOnly))
		return false;

	cbs.read_func = readCb;
	cbs.seek_func = seekCb;
	cbs.close_func = closeCb;
	cbs.tell_func = tellCb;

	if(ov_open_callbacks((void*)oggFile, &oggStream, NULL, 0, cbs) < 0)
		qFatal("Failed to open OGG stream");

	vorbisInfo = ov_info(&oggStream, -1);
	vorbisComment = ov_comment(&oggStream, -1);

	if(vorbisInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	alGenBuffers(2, buffers);
	check();
	alGenSources(1, &source);
	check();

	alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(source, AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

	return true;
}

void OggStream::release()
{
	alSourceStop(source);
	empty();
	alDeleteSources(1, &source);
	check();
	alDeleteBuffers(1, buffers);
	check();
	
	ov_clear(&oggStream);
}

void OggStream::display()
{
	qDebug() << "version" << vorbisInfo->version;
	qDebug() << "channels" << vorbisInfo->channels;
	qDebug() << "rate" << vorbisInfo->rate;
	qDebug() << "bitrate upper" << vorbisInfo->bitrate_upper;
	qDebug() << "bitrate nominal" << vorbisInfo->bitrate_nominal;
	qDebug() << "bitrate lower" << vorbisInfo->bitrate_lower;
	qDebug() << "bitrate window" << vorbisInfo->bitrate_window;
	
	qDebug() << "vendor" << vorbisComment->vendor;
}

bool OggStream::playback()
{
	if(playing())
		return true;

	if(!stream(buffers[0]))
		return false;

	if(!stream(buffers[1]))
		return false;

	alSourceQueueBuffers(source, 2, buffers);
	alSourcePlay(source);

	return true;
}

bool OggStream::playing()
{
	ALenum state;

	alGetSourcei(source, AL_SOURCE_STATE, &state);

	return(state == AL_PLAYING);
}

bool OggStream::update()
{
	int processed;
	bool active = true;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

	while(processed--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(source, 1, &buffer);
		check();

		active = stream(buffer);

		alSourceQueueBuffers(source, 1, &buffer);
		check();
	}

	return active;
}

bool OggStream::stream(ALuint buffer)
{
	char data[BUFFER_SIZE];
	int size = 0;
	int section;
	int result;

	while(size < BUFFER_SIZE)
	{
		result = ov_read(&oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);

		if(result > 0)
			size += result;
		else
		{
			if(result < 0)
				qFatal("Failed to stream");
			else
				break;
		}
	}

	if(!size)
		return false;

	alBufferData(buffer, format, data, size, vorbisInfo->rate);
	check();

	return false;
}

void OggStream::empty()
{
	int queued;

	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

	while(queued--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(source, 1, &buffer);
		check();
	}
}

void OggStream::check()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
		qFatal("OpenAL error");
}

OpenAL::OpenAL()
{
	OggStream ogg;

	alutInit(NULL, NULL);

	if(!ogg.open("test.ogg"))
		return;
	ogg.display();

	if(!ogg.playback())
	{
		qWarning("Ogg refused to play");
		return;
	}


	while(ogg.update())
	{
		
	}

	ogg.release();
	alutExit();
}

OpenAL::~OpenAL()
{

}

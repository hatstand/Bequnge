#include "texture.h"

#include <QImage>
#include <QtDebug>

Texture::Texture(const QString& left,
	         const QString& right,
	         const QString& front,
	         const QString& back,
	         const QString& top,
	         Settings settings)
	: m_settings(settings),
	  m_target(GL_TEXTURE_CUBE_MAP),
	  m_boundToUnit(-1)
{
	genTexture();
	load(left, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	load(right, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	load(front, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	load(back, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	load(top, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	load(top, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
}

Texture::Texture(const QString& fileName, Settings settings)
	: m_settings(settings),
	  m_target(GL_TEXTURE_2D),
	  m_boundToUnit(-1)
{
	genTexture();
	load(fileName);
}

Texture::Texture(Settings settings)
	: m_settings(settings),
	  m_target(GL_TEXTURE_2D),
	  m_boundToUnit(-1)
{
	genTexture();
}

Texture::Texture(uint id, GLenum target, bool ownsTexture)
	: m_settings(Defaults),
	  m_target(target),
	  m_id(id),
	  m_boundToUnit(-1),
	  m_ownsTexture(ownsTexture)
{
}



Texture::~Texture()
{
	if (m_ownsTexture)
		free();
}



void Texture::genTexture()
{
	m_ownsTexture = true;
	
	glGenTextures(1, (GLuint*)&m_id);
	glBindTexture(m_target, m_id);
	setTexParams();
}

void Texture::load(const QString& fileName, GLenum target)
{
	QImage image(fileName);
	
	if (image.isNull())
		qFatal("Texture not found: %s", fileName.toAscii().data());
	
	gluBuild2DMipmaps(target, GL_RGBA, image.width(), image.height(), GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
}

void Texture::free()
{
	if (glIsTexture(m_id))
		glDeleteTextures(1, &m_id);
}

void Texture::setTexParams(GLenum target, Settings settings)
{
	if (settings & Mipmaps)
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	
	if (settings & Mirrored)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

void Texture::setTexParams()
{
	setTexParams(m_target, m_settings);
}

void Texture::bind(int texUnit, int shaderLoc)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glEnable(m_target);
	glBindTexture(m_target, m_id);
	
	if (shaderLoc != -1)
		glUniform1i(shaderLoc, texUnit);
	
	m_boundToUnit = texUnit;
}

void Texture::release()
{
	if (m_boundToUnit == -1)
		return;
	
	glActiveTexture(GL_TEXTURE0 + m_boundToUnit);
	glDisable(m_target);
	
	m_boundToUnit = -1;
}


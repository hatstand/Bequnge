#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl.h"

#include <QString>
#include <QFlags>

class Texture
{
public:
	enum Setting
	{
		Defaults = 0x00,
		Mipmaps = 0x01,
		Mirrored = 0x02
	};
	Q_DECLARE_FLAGS(Settings, Setting)
	
	Texture(const QString& left,
	        const QString& right,
	        const QString& front,
	        const QString& back,
	        const QString& top,
	        Settings settings = Defaults);
	Texture(const QString& fileName, Settings settings = Defaults);
	Texture(Settings settings = Defaults);
	Texture(uint id, GLenum target = GL_TEXTURE_2D, bool takeOwnership = false);
	~Texture();
	
	void bind(int texUnit = 0, int shaderLoc = -1);
	void release();
	
	Settings settings() const { return m_settings; }
	uint id() const { return m_id; }
	GLenum target() const { return m_target; }
	
	static void setTexParams(GLenum target, Settings settings);
	
private:
	void genTexture();
	void setTexParams();
	
	void load(const QString& fileName = QString::null, GLenum target = GL_TEXTURE_2D);
	void free();
	
	Settings m_settings;
	GLenum m_target;
	uint m_id;
	int m_boundToUnit;
	bool m_ownsTexture;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Texture::Settings)

#endif

#ifndef GLFONT_H
#define GLFONT_H

#include <QChar>
#include <QMap>
#include <QPixmap>
#include <QPainter>

class Shader;

class GLFont
{
public:
	GLFont(const QString& family, const QPen& pen);
	~GLFont();
	
	void bind();
	void drawChar(const QChar& c);
	void release();
	
private:
	void initBuffers();
	uint textureForChar(const QChar& c);
	void genAtlas();
	
	QMap<QChar, uint> m_cache;
	QFont m_font;
	QPen m_pen;
	
	uint m_quadBuffers[2];
	
	QImage m_image;
	QPainter m_painter;
	uchar* m_data;

	uint m_boundTexture;

	uint m_atlasTexture;
	
	static const int s_res;
	static const float s_scale;
	static Shader* s_shader;
	static uint s_texLoc;

	static const QString s_atlas;
};

#endif

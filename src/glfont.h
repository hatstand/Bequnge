#ifndef GLFONT_H
#define GLFONT_H

#include "vector.h"
typedef Vector<float,2> float2;

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
	QMap<QChar, float2> m_atlasMap;
	uint m_atlasWidth;
	float m_atlasCharWidth;
	
	static const int s_res;
	static const float s_scale;
	static Shader* s_shader;
	static uint s_texLoc;
	static uint s_texOffsetLoc;

	static const QString s_atlas;
};

#endif

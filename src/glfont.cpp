#include "glfont.h"
#include "opengl.h"
#include "shader.h"

#include <cmath>
#include <QtDebug>

const int GLFont::s_res = 32;
const float GLFont::s_scale = 27.0;
Shader* GLFont::s_shader = NULL;
uint GLFont::s_texLoc;
// List of all valid Funge 98 characters.
const QString GLFont::s_atlas =
	"abcdefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"0123456789"
	" !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

GLFont::GLFont(const QString& family, const QPen& pen)
	: m_image(s_res, s_res, QImage::Format_ARGB32),
	  m_pen(pen),
	  m_data(new uchar[s_res * s_res]),
	  m_boundTexture(0)
{
	if (s_shader == NULL)
	{
		s_shader = new Shader(":shaders/char_vert.glsl", ":shaders/char_frag.glsl");
		s_texLoc = s_shader->uniformLocation("tex");
	}
	
	m_font.setFamily(family);
	m_font.setPixelSize(s_res);
	
	genAtlas();
	initBuffers();
}

GLFont::~GLFont()
{
	delete[] m_data;
	
	foreach (uint tex, m_cache)
		glDeleteTextures(1, &tex);
	
	glDeleteBuffers(2, m_quadBuffers);
}

void GLFont::genAtlas()
{
	// Number of chars to store in atlas
	int atlasLength = s_atlas.length();

	int totalPixelsRequired = atlasLength * s_res * s_res;
	// Width of texture in pixels
	m_atlasWidth = s_res;
	while (m_atlasWidth * m_atlasWidth < totalPixelsRequired)
		m_atlasWidth *= 2;

	// Width in chars.
	int atlasWidth = m_atlasWidth / s_res;

	QImage image(m_atlasWidth, m_atlasWidth, QImage::Format_ARGB32);
	image.fill(qRgba(0, 0, 0, 0));

	QPainter p;

	int x = 0;
	int y = 0;
	foreach(QChar c, s_atlas)
	{
		p.begin(&image);
		p.setFont(m_font);
		p.setPen(m_pen);
		p.drawText(QRect(x*s_res, y*s_res, s_res, s_res), Qt::AlignHCenter | Qt::AlignVCenter, c);
		p.end();

		float2 pos;
		pos[0] = x*s_res / m_atlasWidth;
		pos[1] = y*s_res / m_atlasWidth;
		m_atlasMap.insert(c, pos);

		++x;
		if (x == atlasWidth)
		{
			x = 0;
			y++;
		}
	}

	glGenTextures(1, &m_atlasTexture);
	glBindTexture(GL_TEXTURE_2D, m_atlasTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, m_atlasWidth, m_atlasWidth, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
}

void GLFont::initBuffers()
{
	float verts[12];
	float texCoords[8];
	
	verts[0] = 0.0;     verts[1] = 0.0;      verts[2] = 0.0;
	verts[3] = s_scale; verts[4] = 0.0;      verts[5] = 0.0;
	verts[6] = s_scale; verts[7] = s_scale;  verts[8] = 0.0;
	verts[9] = 0.0;     verts[10] = s_scale; verts[11] = 0.0;
	
	texCoords[0] = 0.0; texCoords[1] = 1.0;
	texCoords[2] = 1.0; texCoords[3] = 1.0;
	texCoords[4] = 1.0; texCoords[5] = 0.0;
	texCoords[6] = 0.0; texCoords[7] = 0.0;
	
	glGenBuffers(2, m_quadBuffers);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), verts, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), texCoords, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GLFont::bind()
{
	m_boundTexture = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffers[1]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	
	s_shader->bind();
}

void GLFont::drawChar(const QChar& c)
{
/*	if (m_atlasMap.contains(c))
	{
		uint texture = m_atlasTexture;
		if (texture != m_boundTexture)
			glBindTexture(GL_TEXTURE_2D, m_boundTexture = texture);

		glUniform1i(s_texLoc, 0);

		// Top left
		float2 pos = *m_atlasMap.find(c);
		float texCoords[8];
		// top left
		texCoords[0] = pos[0];
		texCoords[1] = pos[1];
		// top right
		texCoords[2] = pos[0] + s_res / m_atlasWidth;
		texCoords[3] = pos[1];
		// bottom right
		texCoords[4] = pos[0] + s_res / m_atlasWidth;
		texCoords[5] = pos[1] + s_res / m_atlasWidth;
		// bottom left
		texCoords[6] = pos[0];
		texCoords[7] = pos[1] + s_res / m_atlasWidth;

		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		glDrawArrays(GL_QUADS, 0, 4);

		return;
	} */

	uint texture = textureForChar(c);
	if (texture != m_boundTexture)
		glBindTexture(GL_TEXTURE_2D, m_boundTexture = texture);

	glUniform1i(s_texLoc, 0);
	
	glDrawArrays(GL_QUADS, 0, 4);
}

void GLFont::release()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	s_shader->unbind();
}

uint GLFont::textureForChar(const QChar& c)
{
	if (m_cache.contains(c))
		return m_cache[c];
	
	// Clear the pixmap
	m_image.fill(qRgba(0, 0, 0, 0));
	
	// Draw the character
	m_painter.begin(&m_image);
	m_painter.setFont(m_font);
	m_painter.setPen(m_pen);
	m_painter.drawText(QRect(0, 0, s_res, s_res), Qt::AlignHCenter | Qt::AlignVCenter, c);
	m_painter.end();
	
	// Convert ARGB to RGBA
	/*const uchar* dataEnd = m_data + (s_res * s_res);
	uchar* p = m_data;
	
	const uchar* argb = m_image.bits();
	
	while (p != dataEnd)
	{
		p[0] = ;
		p ++;
		argb += 4;
	}*/
	
	// Allocate texture
	uint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, s_res, s_res, GL_BGRA, GL_UNSIGNED_BYTE, m_image.bits());
	
	// Add to cache
	m_cache[c] = tex;
	
	return tex;
}


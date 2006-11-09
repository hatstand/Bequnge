#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <QTime>
#include <QFont>

#include <ft2build.h>
#include FT_FREETYPE_H

class QMouseEvent;

#include "OGLFT.h"

class FungeSpaceEntry
{
public:
	FungeSpaceEntry(QChar d, int x, int y, int z)
	{
		data = d;
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
	}
	
	QChar data;
	int coords[3];
};

class ThreeDFungeSpace
{
public:
	ThreeDFungeSpace(int dimensions);
	
	void setChar(int p0, int p1, int p2, QChar data);
	QChar getChar(int p0, int p1, int p2);
	QList<FungeSpaceEntry> getEntries();

private:
	QMap<int, QMap<int, QMap<int, QChar> > > m_data;
};


class GLView : public QGLWidget
{
public:
	GLView(QWidget* parent = 0);
	~GLView();
	
private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	
	void updateCamera(int i);
	float degreesToRadians(float degrees);
	float modulo(float value, float mod);
	float snapToPlane(int i, float value);
	QList<int> glToFungeSpace(float x, float y, float z);

private:
	ThreeDFungeSpace* m_fungeSpace;
	QList<int> m_cursor;
	QTime m_cursorBlinkTime;
	bool m_cursorBlinkOn;
	QTimer* m_redrawTimer;
	int m_delayMs;
	
	
	// Camera offset
	float m_actualCameraOffset[3];
	bool m_moveDragging;
	float m_destinationCameraOffset[3];
	float m_preDragCameraOffset[2];
	
	// Camera rotation
	float m_actualCameraRotation[2];
	bool m_rotateDragging;
	float m_destinationCameraRotation[2];
	float m_preDragCameraRotation[2];
	
	QPoint m_preDragMousePosition;
	
	// Fonts
	FT_Face m_fontFace;
	OGLFT::Face* m_font;
	float m_fontSize;
};

#endif

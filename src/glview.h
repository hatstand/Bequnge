#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <QTime>
#include <QFont>

#include <ft2build.h>
#include FT_FREETYPE_H

class QMouseEvent;
class QKeyEvent;

#include "OGLFT.h"

class FungeSpaceEntry
{
public:
	FungeSpaceEntry(QChar d, int x, int y, int z)
	{
		data = d;
		coords.append(x);
		coords.append(y);
		coords.append(z);
	}
	
	QChar data;
	QList<int> coords;
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
	Q_OBJECT
	
public:
	GLView(QWidget* parent = 0);
	~GLView();
	
public slots:
	void setStringMode(bool enabled);
	
signals:
	void stringModeChanged(bool enabled);
	
private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
	
	void updateCamera(int i);
	float degreesToRadians(float degrees);
	float modulo(float value, float mod);
	float snapToPlane(int i, float value);
	QList<int> glToFungeSpace(float x, float y, float z);
	QList<float> fungeSpaceToGl(QList<int> c, bool premultiplied);
	void toggleStringMode();
	void setCursorDirection(int direction);

private:
	// Funge space
	ThreeDFungeSpace* m_fungeSpace;
	
	// Cursor
	QList<int> m_cursor;
	QTime m_cursorBlinkTime;
	bool m_cursorBlinkOn;
	int m_cursorDirection;
	int m_activePlane;
	float m_actualCursorPos[3];
	
	// Frame updates
	QTimer* m_redrawTimer;
	int m_delayMs;
	
	bool m_stringMode;
	
	// Eye offset
	float m_actualEyeOffset[3];
	float m_destinationEyeOffset[3];
	
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

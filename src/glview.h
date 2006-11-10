#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <QTime>
#include <QFont>

#include <ft2build.h>
#include FT_FREETYPE_H

class QMouseEvent;
class QKeyEvent;
class FungeSpace;

#include "OGLFT.h"

class GLView : public QGLWidget
{
	Q_OBJECT
	
public:
	GLView(QWidget* parent = 0);
	~GLView();
	
	int cursorDirection();
	
	void setFungeSpace(FungeSpace* funge);
	FungeSpace* getFungeSpace() { return m_fungeSpace; }
	
	void resetView();
	
public slots:
	void setStringMode(bool enabled);
	void updateFPSCounter();
	
signals:
	void stringModeChanged(bool enabled);
	void cursorDirectionChanged(int direction);
	
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
	QList<int> glToFungeSpace(float x, float y, float z);
	QList<float> fungeSpaceToGl(QList<int> c, bool premultiplied);
	void toggleStringMode();
	void setCursorDirection(int direction);
	void setEye(float radius, float vert, float horiz);

private:
	// Funge space
	FungeSpace* m_fungeSpace;
	
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
	float m_fpsCounter;
	int m_frameCount;
	QTimer* m_fpsCounterTimer;
	
	bool m_stringMode;
	
	// Eye offset
	float m_actualEyeOffset[3];
	float m_destinationEyeOffset[3];
	float m_zoomLevel;
	
	// Camera offset
	float m_actualCameraOffset[3];
	bool m_moveDragging;
	float m_destinationCameraOffset[3];
	
	// Camera rotation
	bool m_rotateDragging;
	
	QPoint m_preDragMousePosition;
	
	// Fonts
	FT_Face m_fontFace;
	OGLFT::Face* m_font;
	float m_fontSize;
};

#endif

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
#include "fungespace.h"

class GLView : public QGLWidget
{
	Q_OBJECT
	
public:
	GLView(FungeSpace* fungeSpace, QWidget* parent = 0);
	~GLView();
	
	int cursorDirection();
	
	void setFungeSpace(FungeSpace* funge);
	FungeSpace* getFungeSpace() { return m_fungeSpace; }
	
	void setExecution(bool execution);
	void setPC(int pc, Coord position, Coord direction);
	void followPC(int pc);
	void resetView();

	void explode(Coord c);
	
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
	bool focusNextPrevChild(bool next);
	
	void updateCamera(int i);
	float degreesToRadians(float degrees);
	float modulo(float value, float mod);
	Coord pointToFungeSpace(const QPoint& pos);
	Coord glToFungeSpace(float x, float y, float z);
	QList<float> fungeSpaceToGl(Coord c, bool premultiplied);
	void toggleStringMode();
	void setCursorDirection(int direction);
	void setEye(float radius, float vert, float horiz);
	void setActivePlane(int plane);
	int otherPlane();

private:
	// Funge space
	FungeSpace* m_fungeSpace;
	
	// Debugger
	bool m_execution;
	QMap<int, QPair<Coord, Coord > > m_pcs;
	int m_followingPC;
	QString m_executionStr;
	QString m_execution2Str;
	QRect m_executionRect;
	QRect m_execution2Rect;
	
	// Cursor
	Coord m_cursor;
	QTime m_cursorBlinkTime;
	bool m_cursorBlinkOn;
	int m_cursorDirection;
	int m_activePlane;
	float m_actualCursorPos[3];
	
	Coord m_selectionStart;
	Coord m_selectionEnd;
	bool m_selectDragging;
	
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
	QFont m_fontLarge;
	QFont m_fontSmall;
	QFontMetrics* m_metricsSmall;
	
	// Display lists
	uint m_displayListsBase;
	enum DisplayListEntry
	{
		ARROW = 0,
		CURSOR = 1
	};

	//QList<Particle*> m_particles;
};

#endif

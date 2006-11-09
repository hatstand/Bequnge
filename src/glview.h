#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <QTime>
#include <QFont>

class QMouseEvent;

class TwoDFungeSpace
{
public:
	TwoDFungeSpace(int dimensions);
	
	void setChar(QList<int> position, QChar data);
	QChar getChar(QList<int> position);

private:
	QMap<int, QMap<int, QChar> > m_data;
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

private:
	TwoDFungeSpace* m_fungeSpace;
	QList<int> m_cursor;
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
};

#endif

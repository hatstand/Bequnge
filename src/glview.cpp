#include "glview.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QDebug>

#include <math.h>

TwoDFungeSpace::TwoDFungeSpace(int dimensions)
{
	if (dimensions != 2)
		qWarning("%d dimensions requested.  Only 2 dimensional funge space is supported in this implementation.", dimensions);
}

void TwoDFungeSpace::setChar(QList<int> position, QChar data)
{
	if (position.count() != 2)
	{
		qWarning("Coords with %d dimensions passed to setChar.", position.count());
		return;
	}
	
	if (data.isSpace())
	{
		if (m_data.contains(position[0]))
		{
			m_data[position[0]].remove(position[1]);
			if (m_data[position[0]].isEmpty())
				m_data.remove(position[0]);
		}
	}
	
	m_data[position[0]][position[1]] = data;
}

QChar TwoDFungeSpace::getChar(QList<int> position)
{
	if (position.count() != 2)
	{
		qWarning("Coords with %d dimensions passed to getChar.", position.count());
		return QChar::Separator_Space;
	}
	
	if (!m_data.contains(position[0]))
		return QChar::Separator_Space;
	if (!m_data[position[0]].contains(position[1]))
		return QChar::Separator_Space;
	return m_data[position[0]][position[1]];
}



GLView::GLView(QWidget* parent)
	: QGLWidget(parent),
	  m_moveDragging(false),
	  m_rotateDragging(false)
{
	m_cursor.append(0);
	m_cursor.append(0);
	
	m_fungeSpace = new TwoDFungeSpace(2);
	m_fungeSpace->setChar(m_cursor, '@');
	
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateGL()));
	
	m_delayMs = 1000/30; // 30fps
	
	m_destinationCameraOffset[0] = 0.0f;
	m_destinationCameraOffset[1] = 0.0f;
	m_destinationCameraOffset[2] = -7.0f;
	
	m_actualCameraOffset[0] = m_destinationCameraOffset[0];
	m_actualCameraOffset[1] = m_destinationCameraOffset[1];
	m_actualCameraOffset[2] = m_destinationCameraOffset[2];
	
	m_destinationCameraRotation[0] = 0.0f;
	m_destinationCameraRotation[1] = 0.0f;
	
	m_actualCameraRotation[0] = m_destinationCameraRotation[0];
	m_actualCameraRotation[1] = m_destinationCameraRotation[1];
	
	rtri = 0.0f;
	rquad = 0.0f;
}


GLView::~GLView()
{
}

void GLView::initializeGL()
{
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);			        // The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);		        // Enables Depth Testing
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// Reset The Projection Matrix
	
	//gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
	
	glMatrixMode(GL_MODELVIEW);
}

void GLView::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);		// Reset The Current Viewport And Perspective Transformation
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void GLView::updateCamera(int i)
{
	float diff = m_destinationCameraOffset[i] - m_actualCameraOffset[i];
	if (fabs(diff) < 0.01)
		m_actualCameraOffset[i] = m_destinationCameraOffset[i];
	else
		m_actualCameraOffset[i] += diff * 0.2f;
	
	if (i < 2)
	{
		diff = m_destinationCameraRotation[i] - m_actualCameraRotation[i];
		if (fabs(diff) < 1.0f)
		{
			m_actualCameraRotation[i] = m_destinationCameraRotation[i];
			if (!m_rotateDragging)
			{
				m_destinationCameraRotation[i] = modulo(m_destinationCameraRotation[i], 360.0f);
				m_actualCameraRotation[i] = m_destinationCameraRotation[i];
			}
		}
		else
			m_actualCameraRotation[i] += diff * 0.2f;
	}
}

void GLView::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset The View
	
	updateCamera(0);
	updateCamera(1);
	updateCamera(2);
	
	glTranslatef(m_actualCameraOffset[0], m_actualCameraOffset[1], m_actualCameraOffset[2]);
	glRotatef(m_actualCameraRotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(m_actualCameraRotation[1], 0.0f, 1.0f, 0.0f);
	
	
	glPushMatrix();
		// draw a cube (6 quadrilaterals)
		glBegin(GL_QUADS);				// start drawing the cube.
		
		// top of cube
		glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Blue
		glVertex3f( 1.0f, 1.0f,-1.0f);		// Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f,-1.0f);		// Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);		// Bottom Left Of The Quad (Top)
		glVertex3f( 1.0f, 1.0f, 1.0f);		// Bottom Right Of The Quad (Top)
		
		// bottom of cube
		glColor3f(1.0f,0.5f,0.0f);			// Set The Color To Orange
		glVertex3f( 1.0f,-1.0f, 1.0f);		// Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f, 1.0f);		// Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f,-1.0f);		// Bottom Left Of The Quad (Bottom)
		glVertex3f( 1.0f,-1.0f,-1.0f);		// Bottom Right Of The Quad (Bottom)
		
		// front of cube
		glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
		glVertex3f( 1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Front)
		glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Front)
		glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Front)
		
		// back of cube.QChar
		glColor3f(1.0f,1.0f,0.0f);			// Set The Color To Yellow
		glVertex3f( 1.0f,-1.0f,-1.0f);		// Top Right Of The Quad (Back)
		glVertex3f(-1.0f,-1.0f,-1.0f);		// Top Left Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f,-1.0f);		// Bottom Left Of The Quad (Back)
		glVertex3f( 1.0f, 1.0f,-1.0f);		// Bottom Right Of The Quad (Back)
		
		// left of cube
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f,-1.0f);		// Top Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f,-1.0f);		// Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Left)
		
		// Right of cube
		glColor3f(1.0f,0.0f,1.0f);			// Set The Color To Violet
		glVertex3f( 1.0f, 1.0f,-1.0f);	        // Top Right Of The Quad (Right)
		glVertex3f( 1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f,-1.0f);		// Bottom Right Of The Quad (Right)
		glEnd();					// Done Drawing The Cube
	glPopMatrix();
	
	renderText(0, 15, "Offset: " + QString::number(m_actualCameraOffset[0]) + ", " + QString::number(m_actualCameraOffset[1]) + ", " + QString::number(m_actualCameraOffset[2]));
	renderText(0, 30, "Rotation: " + QString::number(m_actualCameraRotation[0]) + ", " + QString::number(m_actualCameraRotation[1]));
	
	rtri+=15.0f;					// Increase The Rotation Variable For The Pyramid
	rquad-=15.0f;					// Decrease The Rotation Variable For The Cube
	
	m_redrawTimer->start(m_delayMs);
}

float GLView::degreesToRadians(float degrees)
{
	qDebug() << degrees << "degrees =" << (degrees / 180.0f) * M_PI << "radians";
	return (degrees / 180.0f) * M_PI;
}

float GLView::modulo(float value, float mod)
{
	if (fabs(value/mod) < 1.0f)
		return value;
	
	if (value > 0.0f)
		return value - floorf(value/mod)*mod;
	return value - ceilf(value/mod)*mod;
}

void GLView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_moveDragging)
	{
		float xOffset = event->pos().x() - m_preDragMousePosition.x();
		float yOffset = event->pos().y() - m_preDragMousePosition.y();
		
		m_destinationCameraOffset[0] = m_preDragCameraOffset[0] + (xOffset / 135.0f);
		m_destinationCameraOffset[1] = m_preDragCameraOffset[1] - (yOffset / 135.0f);
	}
	else if (m_rotateDragging)
	{
		float xOffset = event->pos().x() - m_preDragMousePosition.x();
		float yOffset = event->pos().y() - m_preDragMousePosition.y();
		
		m_destinationCameraRotation[1] = m_preDragCameraRotation[1] + (xOffset / 5.0f);
		m_destinationCameraRotation[0] = m_preDragCameraRotation[0] + (yOffset / 5.0f);
	}
}

void GLView::wheelEvent(QWheelEvent* event)
{
	m_destinationCameraOffset[2] += (((float)event->delta()) / 135.0f);
}

void GLView::mousePressEvent(QMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::MidButton:
		m_preDragMousePosition = event->pos();
		
		if (event->modifiers() & Qt::ControlModifier)
		{
			m_preDragCameraRotation[0] = m_destinationCameraRotation[0];
			m_preDragCameraRotation[1] = m_destinationCameraRotation[1];
			m_rotateDragging = true;
		}
		else
		{
			m_preDragCameraOffset[0] = m_destinationCameraOffset[0];
			m_preDragCameraOffset[1] = m_destinationCameraOffset[1];
			m_moveDragging = true;
		}
		break;
	}
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::MidButton:
		m_rotateDragging = false;
		m_moveDragging = false;
	}
}



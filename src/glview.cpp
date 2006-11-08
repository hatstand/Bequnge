#include "glview.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

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
	  m_dragging(false)
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
	m_destinationCameraOffset[2] = 0.0f;
	
	m_actualCameraOffset[0] = 0.0f;
	m_actualCameraOffset[1] = 0.0f;
	m_actualCameraOffset[2] = 0.0f;
	
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

void GLView::updateCameraOffset(int i)
{
	float distance = m_destinationCameraOffset[i] - m_actualCameraOffset[i];
	m_actualCameraOffset[i] += (distance * 0.2f);
}

void GLView::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset The View
	
	updateCameraOffset(0);
	updateCameraOffset(1);
	updateCameraOffset(2);
	
	glTranslatef(m_actualCameraOffset[0], m_actualCameraOffset[1], m_actualCameraOffset[2]);
	
	glPushMatrix();
		glTranslatef(-1.5f,0.0f,-6.0f);		// Move Left 1.5 Units And Into The Screen 6.0
		
		glRotatef(rtri,0.0f,1.0f,0.0f);		// Rotate The Pyramid On The Y axis 
		
		// draw a pyramid (in smooth coloring mode)
		glBegin(GL_POLYGON);				// start drawing a pyramid
		
		// front face of pyramid
		glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
		glVertex3f(0.0f, 1.0f, 0.0f);		        // Top of triangle (front)
		glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
		glVertex3f(-1.0f,-1.0f, 1.0f);		// left of triangle (front)
		glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
		glVertex3f(1.0f,-1.0f, 1.0f);		        // right of traingle (front)	
		
		// right face of pyramid
		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Right)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f( 1.0f,-1.0f, 1.0f);		// Left Of Triangle (Right)
		glColor3f(0.0f,1.0f,0.0f);			// Green
		glVertex3f( 1.0f,-1.0f, -1.0f);		// Right Of Triangle (Right)
		
		// back face of pyramid
		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Back)
		glColor3f(0.0f,1.0f,0.0f);			// Green
		glVertex3f( 1.0f,-1.0f, -1.0f);		// Left Of Triangle (Back)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f(-1.0f,-1.0f, -1.0f);		// Right Of Triangle (Back)
		
		// left face of pyramid.
		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Left)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f(-1.0f,-1.0f,-1.0f);		// Left Of Triangle (Left)
		glColor3f(0.0f,1.0f,0.0f);			// Green
		glVertex3f(-1.0f,-1.0f, 1.0f);		// Right Of Triangle (Left)
		
		glEnd();					// Done Drawing The Pyramid
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(1.5f,0.0f,-7.0f);		// Move Right 3 Units, and back into the screen 7
		
		glRotatef(rquad,1.0f,1.0f,1.0f);		// Rotate The Cube On X, Y, and Z
		
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
		
		// back of cube.
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
	
	rtri+=15.0f;					// Increase The Rotation Variable For The Pyramid
	rquad-=15.0f;					// Decrease The Rotation Variable For The Cube
	
	m_redrawTimer->start(m_delayMs);
}

void GLView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_dragging)
	{
		float xOffset = event->pos().x() - m_preDragMousePosition.x();
		float yOffset = event->pos().y() - m_preDragMousePosition.y();
		
		m_destinationCameraOffset[0] = m_preDragCameraOffset[0] + (xOffset / 135.0f);
		m_destinationCameraOffset[1] = m_preDragCameraOffset[1] - (yOffset / 135.0f);
	}
}

void GLView::wheelEvent(QWheelEvent* event)
{
	m_destinationCameraOffset[2] += (((float)event->delta()) / 135.0f);
}

void GLView::mousePressEvent(QMouseEvent* event)
{
	qDebug("Press");
	switch (event->button())
	{
	case Qt::MidButton:
		m_preDragCameraOffset[0] = m_destinationCameraOffset[0];
		m_preDragCameraOffset[1] = m_destinationCameraOffset[1];
		m_preDragMousePosition = event->pos();
		m_dragging = true;
		break;
	}
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::MidButton:
		m_dragging = false;
		break;
	}
}



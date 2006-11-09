#include "glview.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResource>
#include <QMessageBox>
#include <QCoreApplication>

#include <QDebug>

#include <math.h>

ThreeDFungeSpace::ThreeDFungeSpace(int dimensions)
{
	if (dimensions != 3)
		qWarning("%d dimensions requested.  Only 3 dimensional funge space is supported in this implementation.", dimensions);
}

void ThreeDFungeSpace::setChar(int p0, int p1, int p2, QChar data)
{
	if (data.isSpace())
	{
		if (m_data.contains(p0))
		{
			if (m_data[p0].contains(p1))
			{
				m_data[p0][p1].remove(p2);
				if (m_data[p0][p1].isEmpty())
					m_data[p0].remove(p1);
			}
			if (m_data[p0].isEmpty())
				m_data.remove(p0);
		}
	}
	
	m_data[p0][p1][p2] = data;
}

QChar ThreeDFungeSpace::getChar(int p0, int p1, int p2)
{
	if (!m_data.contains(p0))
		return QChar::Separator_Space;
	if (!m_data[p0].contains(p1))
		return QChar::Separator_Space;
	if (!m_data[p0][p1].contains(p2))
		return QChar::Separator_Space;
	return m_data[p0][p1][p2];
}

QList<FungeSpaceEntry> ThreeDFungeSpace::getEntries()
{
	QList<FungeSpaceEntry> ret;
	
	QMapIterator<int, QMap<int, QMap<int, QChar> > > i1(m_data);
	while (i1.hasNext())
	{
		i1.next();
		int x = i1.key();
		QMapIterator<int, QMap<int, QChar> > i2(i1.value());
		while (i2.hasNext())
		{
			i2.next();
			int y = i2.key();
			QMapIterator<int, QChar> i3(i2.value());
			while (i3.hasNext())
			{
				i3.next();
				ret.append(FungeSpaceEntry(i3.value(), x, y, i3.key()));
			}
		}
	}
	return ret;
}




GLView::GLView(QWidget* parent)
	: QGLWidget(parent),
	  m_moveDragging(false),
	  m_rotateDragging(false),
	  m_cursorBlinkOn(true)
{
	// Initialize the curser
	m_cursor.append(0);
	m_cursor.append(0);
	m_cursor.append(0);
	m_cursorBlinkTime.start();
	
	// Initialize funge space
	m_fungeSpace = new ThreeDFungeSpace(3);
	m_fungeSpace->setChar(0, 0, 0, 'a');
	m_fungeSpace->setChar(1, 0, 0, 'b');
	m_fungeSpace->setChar(1, 1, 0, 'c');
	m_fungeSpace->setChar(1, 1, -1, 'd');
	
	// Setup the redraw timer
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateGL()));
	
	m_delayMs = 1000/30; // 30fps
	
	// Setup the camera offset and rotation
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
	
	// Load the font
	QResource fontResource("luximr.ttf");
	
	FT_Open_Args args;
	args.flags = FT_OPEN_MEMORY;
	args.memory_base = fontResource.data();
	args.memory_size = fontResource.size();
	FT_Open_Face(OGLFT::Library::instance(), &args, 0, &m_fontFace);
	
	m_fontSize = 24;
	m_font = new OGLFT::TranslucentTexture(m_fontFace, m_fontSize);
	
	if ( m_font == 0 || !m_font->isValid() )
	{
		QMessageBox::critical(this, "Error loading font", "Font face could not be loaded");
		QCoreApplication::exit(1);
		return;
	}
	
	m_font->setForegroundColor(1.0f, 1.0f, 1.0f);
}


GLView::~GLView()
{
	delete m_font;
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
	
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	
	glEnable( GL_BLEND );
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
		glEnable(GL_TEXTURE_2D);
		glScalef(0.004f, 0.004f, 0.004f);
		
		QList<FungeSpaceEntry> entries = m_fungeSpace->getEntries();
		foreach(FungeSpaceEntry entry, entries)
		{
			glPushMatrix();
				glTranslatef(m_fontSize * entry.coords[0], m_fontSize * entry.coords[1], m_fontSize * entry.coords[2]);
				/*glRotatef(-m_actualCameraRotation[0], 1.0f, 0.0f, 0.0f);
				glRotatef(-m_actualCameraRotation[1], 0.0f, 1.0f, 0.0f);*/
				m_font->draw(entry.data);
			glPopMatrix();
		}
		
		glDisable(GL_TEXTURE_2D);
		
		if (m_cursorBlinkOn)
		{
			glPushMatrix();
				float s = m_fontSize/2;
				glTranslatef(m_fontSize * m_cursor[0] + s, m_fontSize * m_cursor[1] + s, m_fontSize * (-m_cursor[2]));
				
				glBegin(GL_QUADS);				// start drawing the cube.
  
					// top of cube
					glColor4f(1.0f, 1.0f, 1.0f, 0.5f);			// Set The Color To Blue
					glVertex3f( s, s,-s);		// Top Right Of The Quad (Top)
					glVertex3f(-s, s,-s);		// Top Left Of The Quad (Top)
					glVertex3f(-s, s, s);		// Bottom Left Of The Quad (Top)
					glVertex3f( s, s, s);		// Bottom Right Of The Quad (Top)
					
					// bottom of cube
					glVertex3f( s,-s, s);		// Top Right Of The Quad (Bottom)
					glVertex3f(-s,-s, s);		// Top Left Of The Quad (Bottom)
					glVertex3f(-s,-s,-s);		// Bottom Left Of The Quad (Bottom)
					glVertex3f( s,-s,-s);		// Bottom Right Of The Quad (Bottom)
					
					// front of cube
					glVertex3f( s, s, s);		// Top Right Of The Quad (Front)
					glVertex3f(-s, s, s);		// Top Left Of The Quad (Front)
					glVertex3f(-s,-s, s);		// Bottom Left Of The Quad (Front)
					glVertex3f( s,-s, s);		// Bottom Right Of The Quad (Front)
					
					// back of cube.
					glVertex3f( s,-s,-s);		// Top Right Of The Quad (Back)
					glVertex3f(-s,-s,-s);		// Top Left Of The Quad (Back)
					glVertex3f(-s, s,-s);		// Bottom Left Of The Quad (Back)
					glVertex3f( s, s,-s);		// Bottom Right Of The Quad (Back)
					
					// left of cube
					glVertex3f(-s, s, s);		// Top Right Of The Quad (Left)
					glVertex3f(-s, s,-s);		// Top Left Of The Quad (Left)
					glVertex3f(-s,-s,-s);		// Bottom Left Of The Quad (Left)
					glVertex3f(-s,-s, s);		// Bottom Right Of The Quad (Left)
					
					// Right of cube
					glVertex3f( s, s,-s);	        // Top Right Of The Quad (Right)
					glVertex3f( s, s, s);		// Top Left Of The Quad (Right)
					glVertex3f( s,-s, s);		// Bottom Left Of The Quad (Right)
					glVertex3f( s,-s,-s);		// Bottom Right Of The Quad (Right)
				glEnd();					// Done Drawing The Cube


			glPopMatrix();
		}
	glPopMatrix();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(0, 15, "Offset: " + QString::number(m_actualCameraOffset[0]) + ", " + QString::number(m_actualCameraOffset[1]) + ", " + QString::number(m_actualCameraOffset[2]));
	renderText(0, 30, "Rotation: " + QString::number(m_actualCameraRotation[0]) + ", " + QString::number(m_actualCameraRotation[1]));
	
	if (m_cursorBlinkTime.elapsed() > 500)
	{
		m_cursorBlinkTime.start();
		m_cursorBlinkOn = !m_cursorBlinkOn;
	}
	
	m_redrawTimer->start(m_delayMs);
}

float GLView::degreesToRadians(float degrees)
{
	return (degrees / 180.0f) * M_PI;
}

float GLView::modulo(float value, float mod)
{
	if (value >= 0.0f)
	{
		if (abs(value/mod) < 1.0f)
			return value;
		return value - floorf(value/mod)*mod;
	}
	return value - ceilf(value/mod)*mod + mod;
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
		
		xOffset = m_destinationCameraRotation[1] = m_preDragCameraRotation[1] + (xOffset / 5.0f);
		yOffset = m_destinationCameraRotation[0] = m_preDragCameraRotation[0] + (yOffset / 5.0f);
		
		snapToPlane(0, yOffset);
		snapToPlane(1, xOffset);
	}
}

float GLView::snapToPlane(int i, float value)
{
	float angle = modulo(value, 90.0f);
	
	if (angle < 20.0f || angle > 70.0f)
		m_destinationCameraRotation[i] = nearbyintf(value/90.0f) * 90.0f;
	else
		m_destinationCameraRotation[i] = value;
}

void GLView::wheelEvent(QWheelEvent* event)
{
	m_destinationCameraOffset[2] += (((float)event->delta()) / 135.0f);
}

QList<int> GLView::glToFungeSpace(float x, float y, float z)
{
	float size = m_fontSize * 0.004f;
	
	QList<int> ret;
	ret.append(floor(x/size));
	ret.append(floor(y/size));
	ret.append(floor((z*-1)/size) + 1);
	return ret;
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
		
	case Qt::LeftButton:
	{
		double objx, objy, objz;
		float x = event->pos().x();
		float y = event->pos().y();
		double modelview[16], projection[16];
		int viewport[4];
		float z;
			//get the projection matrix             
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
			//get the modelview matrix              
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
			//get the viewport              
		glGetIntegerv( GL_VIEWPORT, viewport );
			
		//Read the window z co-ordinate 
		//(the z value on that point in unit cube)          
		glReadPixels( x, viewport[3]-y, 1, 1,
				GL_DEPTH_COMPONENT, GL_FLOAT, &z );
		
		//Unproject the window co-ordinates to 
		//find the world co-ordinates.
		gluUnProject( x, viewport[3]-y, z, modelview, 
				projection, viewport, &objx, &objy, &objz );
		
		QList<int> p = glToFungeSpace(objx, objy, objz);
		if (m_fungeSpace->getChar(p[0], p[1], p[2]) != ' ')
			m_cursor = p;
	}
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



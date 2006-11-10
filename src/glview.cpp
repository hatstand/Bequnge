#include "glview.h"
#include "fungespace.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResource>
#include <QMessageBox>
#include <QCoreApplication>

#include <QDebug>

#include <math.h>


GLView::GLView(QWidget* parent)
	: QGLWidget(parent),
	  m_fungeSpace(NULL),
	  m_cursorBlinkOn(true),
	  m_cursorDirection(1),
	  m_fpsCounter(0.0f),
	  m_frameCount(0),
	  m_stringMode(false),
	  m_zoomLevel(6.0f),
	  m_moveDragging(false)
{
	setFocusPolicy(Qt::WheelFocus);
	
	// Initialize funge space
	setFungeSpace(new FungeSpace(3));
	
	// Setup the redraw timer
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateGL()));
	
	m_delayMs = 1000/30; // 30fps
	
	m_fpsCounterTimer = new QTimer(this);
	m_fpsCounterTimer->start(2000);
	connect(m_fpsCounterTimer, SIGNAL(timeout()), SLOT(updateFPSCounter()));
	
	// Reset the view
	resetView();
	
	// Load the font
	QResource fontResource("luximr.ttf");
	
	FT_Open_Args args;
	args.flags = FT_OPEN_MEMORY;
	args.memory_base = fontResource.data();
	args.memory_size = fontResource.size();
	FT_Open_Face(OGLFT::Library::instance(), &args, 0, &m_fontFace);
	
	m_fontSize = 29;
	m_font = new OGLFT::Filled(m_fontFace, m_fontSize - 5);
	
	if ( m_font == 0 || !m_font->isValid() )
	{
		QMessageBox::critical(this, "Error loading font", "Font face could not be loaded");
		QCoreApplication::exit(1);
		return;
	}
	
	m_font->setForegroundColor(1.0f, 1.0f, 1.0f);
	//m_font->setDepth(1.0f);
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
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// Display lists
	m_displayListsBase = glGenLists(2);
	glNewList(m_displayListsBase + ARROW, GL_COMPILE);
		glBegin(GL_LINES);
			glVertex3f(0.0f, -0.05f, 0.0f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glVertex3f(-0.03f, 0.02f, 0.0f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glVertex3f(0.03f, 0.02f, 0.0f);
		glEnd();
	glEndList();
	
	glNewList(m_displayListsBase + CURSOR, GL_COMPILE);
		glBegin(GL_QUADS);
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(m_fontSize - 5.0f, m_fontSize, 0.0f);
			glVertex3f(0.0f, m_fontSize, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(m_fontSize - 5.0f, 0.0f, 0.0f);
		glEnd();
	glEndList();
}

void GLView::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);		// Reset The Current Viewport And Perspective Transformation
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(50.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void GLView::updateCamera(int i)
{
	float diff = m_destinationCameraOffset[i] - m_actualCameraOffset[i];
	if (fabs(diff) < 0.01)
		m_actualCameraOffset[i] = m_destinationCameraOffset[i];
	else
		m_actualCameraOffset[i] += diff * 0.2f;
	
	diff = m_destinationEyeOffset[i] - m_actualEyeOffset[i];
	if (fabs(diff) < 0.01)
		m_actualEyeOffset[i] = m_destinationEyeOffset[i];
	else
		m_actualEyeOffset[i] += diff * 0.2f;
	
	QList<float> c = fungeSpaceToGl(m_cursor, false);
	diff = c[i] - m_actualCursorPos[i];
	if (fabs(diff) < 0.01)
		m_actualCursorPos[i] = c[i];
	else
		m_actualCursorPos[i] += diff * 0.2f;
}

void GLView::paintGL()
{
	QTime frameTime;
	frameTime.start();
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset The View
	
	updateCamera(0);
	updateCamera(1);
	updateCamera(2);
	
	gluLookAt(m_actualEyeOffset[0] + m_actualCursorPos[0] + m_actualCameraOffset[0],
	          m_actualEyeOffset[1] + m_actualCursorPos[1] + m_actualCameraOffset[1],
	          m_actualEyeOffset[2] + m_actualCursorPos[2] + m_actualCameraOffset[2],
	          m_actualCursorPos[0] + m_actualCameraOffset[0],
	          m_actualCursorPos[1] + m_actualCameraOffset[1],
	          m_actualCursorPos[2],
	          0.0f,
	          1.0f,
	          0.0f);
	
	glScalef(0.004f, 0.004f, 0.004f);
	glPushMatrix();
		//glEnable(GL_TEXTURE_2D);
		QList<float> coord;
		QHash<Coord, QChar> entries = m_fungeSpace->getCode();
		QHashIterator<Coord, QChar> i(entries);
		while (i.hasNext())
		{
			i.next();
			Coord coords = i.key();
			QChar data = i.value();
			
			glPushMatrix();
				int diff = abs(coords[m_activePlane] - m_cursor[m_activePlane]);
				
				if ((coords == m_cursor) && (m_cursorBlinkOn))
					glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
				else if (diff == 0)
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
				else if (diff <= 8)
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f - diff * 0.1f);
				else
					glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
				
				coord = fungeSpaceToGl(coords, true);
				glTranslatef(coord[0], coord[1], coord[2]);
				
				if (m_activePlane == 0)
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				m_font->draw(data);
			glPopMatrix();
		}
		//glDisable(GL_TEXTURE_2D);
		
		if (m_cursorBlinkOn)
		{
			coord = fungeSpaceToGl(m_cursor, true);
			glTranslatef(coord[0], coord[1] - 2.5f, coord[2] - 0.01f);
			if (m_activePlane == 0)
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glCallList(m_displayListsBase + CURSOR);
		}
	glPopMatrix();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(0, 15, "Cursor: " + QString::number(m_cursor[0]) + ", " + QString::number(m_cursor[1]) + ", " + QString::number(m_cursor[2]));
	renderText(0, 30, "FPS: " + QString::number(m_fpsCounter));
	
	glPushMatrix();
		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		glTranslatef(0.0f, -0.85f, -2.0f);
		
		switch (m_cursorDirection)
		{
		case 2:
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			break;
		case -1:
		case -3:
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 1:
		case 3:
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
		glCallList(m_displayListsBase + ARROW);
	glPopMatrix();
	
	if (m_cursorBlinkTime.elapsed() > 500)
	{
		m_cursorBlinkTime.start();
		m_cursorBlinkOn = !m_cursorBlinkOn;
	}
	
	m_redrawTimer->start(qAbs(m_delayMs - frameTime.elapsed()));
	m_frameCount++;
}

float GLView::degreesToRadians(float degrees)
{
	return (degrees / 180.0f) * M_PI;
}

float GLView::modulo(float value, float mod)
{
	if (value >= 0.0f)
	{
		if (abs((int)(value/mod)) < 1.0f)
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
		
		int otherPlane = m_activePlane == 0 ? 2 : 0;
		
		m_destinationCameraOffset[otherPlane] = - xOffset / 135.0f;
		m_destinationCameraOffset[1] = yOffset / 135.0f;
	}
	else if (m_rotateDragging)
	{
		float xOffset = - (event->pos().x() - m_preDragMousePosition.x()) / 4.0f;
		float yOffset = 30.0f + (event->pos().y() - m_preDragMousePosition.y()) / 4.0f;
		
		setEye(m_zoomLevel, yOffset, xOffset);
	}
}

void GLView::wheelEvent(QWheelEvent* event)
{
	m_zoomLevel -= event->delta() / 90.0f;
	
	if (m_zoomLevel < 1.0f)
		m_zoomLevel = 1.0f;
	
	setEye(m_zoomLevel, 35.0f, 0.0f);
}

QList<int> GLView::glToFungeSpace(float x, float y, float z)
{
	float size = m_fontSize;
	
	QList<int> ret;
	ret.append((int)(floor(x/size)));
	ret.append((int)(- floor(y/size)));
	ret.append((int)(floor((z*-1)/size - 0.5f) + 1));
	return ret;
}

QList<float> GLView::fungeSpaceToGl(QList<int> c, bool premultiplied)
{
	float size = m_fontSize;
	if (!premultiplied)
		size *= 0.004f;
	
	QList<float> ret;
	ret.append(c[0] * size);
	ret.append(- c[1] * size);
	ret.append(- c[2] * size);
	return ret;
}

void GLView::mousePressEvent(QMouseEvent* event)
{
	m_preDragMousePosition = event->pos();
	switch (event->button())
	{
	case Qt::MidButton:
		if (event->modifiers() & Qt::ControlModifier)
		{
			m_rotateDragging = true;
			event->accept();
			return;
		}
		break;
		
	case Qt::LeftButton:
	{
		if (event->modifiers() & Qt::ControlModifier)
		{
			m_moveDragging = true;
			event->accept();
			return;
		}
		
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
		glReadPixels( (GLint)x, (GLint)(viewport[3]-y), 1, 1,
				GL_DEPTH_COMPONENT, GL_FLOAT, &z );
		
		//Unproject the window co-ordinates to 
		//find the world co-ordinates.
		gluUnProject( x, viewport[3]-y, z, modelview, 
				projection, viewport, &objx, &objy, &objz );
		
		QList<int> p = glToFungeSpace(objx, objy, objz);
		if (m_fungeSpace->getChar(p) != ' ')
			m_cursor = p;
		
		event->accept();
	}
	default:
		break;
	}
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::MidButton:
		m_rotateDragging = false;
		setEye(m_zoomLevel, 30.0f, 0.0f);
		break;
		
	case Qt::LeftButton:
		m_moveDragging = false;
		m_destinationCameraOffset[0] = 0.0f;
		m_destinationCameraOffset[1] = 0.0f;
		break;
		
	default:
		break;
	}
}

void GLView::keyPressEvent(QKeyEvent* event)
{
	int otherPlane = m_activePlane == 0 ? 2 : 0;
	
	if (event->modifiers() & Qt::AltModifier)
	{
		bool handled = true;
		if (event->key() == Qt::Key_Right)
			setCursorDirection(otherPlane + 1);
		else if (event->key() == Qt::Key_Left)
			setCursorDirection(-(otherPlane + 1));
		else if (event->key() == Qt::Key_Down)
			setCursorDirection(2);
		else if (event->key() == Qt::Key_Up)
			setCursorDirection(-2);
		else
			handled = false;
		
		if (handled)
			return;
	}
	
	if (event->matches(QKeySequence::MoveToNextChar))
		m_cursor[otherPlane]++;
	else if (event->matches(QKeySequence::MoveToPreviousChar))
		m_cursor[otherPlane]--;
	else if (event->matches(QKeySequence::MoveToNextLine))
		m_cursor[1]++;
	else if (event->matches(QKeySequence::MoveToPreviousLine))
		m_cursor[1]--;
	else if (event->matches(QKeySequence::MoveToNextPage))
		m_cursor[m_activePlane]++;
	else if (event->matches(QKeySequence::MoveToPreviousPage))
		m_cursor[m_activePlane]--;
	else if (event->key() == Qt::Key_Backspace)
	{
		int a = abs(m_cursorDirection);
		m_cursor[a-1] += (m_cursorDirection < 0 ? 1 : -1);
		if (m_fungeSpace->getChar(m_cursor) == '"')
			toggleStringMode();
		
		m_fungeSpace->setChar(m_cursor, ' ');
	}
	else if (event->key() == Qt::Key_Tab)
	{
		m_activePlane = otherPlane;
		switch (m_cursorDirection)
		{
			case 1: setCursorDirection(3); break;
			case -1: setCursorDirection(-3); break;
			case 3: setCursorDirection(1); break;
			case -3: setCursorDirection(-1); break;
			default: setCursorDirection(m_cursorDirection); break;
		}
	}
	else if (!event->text().isEmpty())
	{
		QChar c = event->text()[0];
		m_fungeSpace->setChar(m_cursor, c);
		
		if (!m_stringMode)
		{
			if (c == '<')
				setCursorDirection(-1);
			else if (c == '>')
				setCursorDirection(1);
			else if (c == 'v')
				setCursorDirection(2);
			else if (c == '^')
				setCursorDirection(-2);
			else if (c == 'h')
				setCursorDirection(3);
			else if (c == 'l')
				setCursorDirection(-3);
		}
		
		if (c == '"')
			toggleStringMode();
		
		int a = abs(m_cursorDirection);
		m_cursor[a-1] += (m_cursorDirection > 0 ? 1 : -1);
	}
	else
		return;
	
	m_cursorBlinkTime.start();
	m_cursorBlinkOn = true;
}

void GLView::toggleStringMode()
{
	m_stringMode = !m_stringMode;
	emit stringModeChanged(m_stringMode);
}

void GLView::setStringMode(bool enabled)
{
	m_stringMode = enabled;
}

void GLView::setCursorDirection(int direction)
{
	m_cursorDirection = direction;
	int a = abs(direction);
	if (a == 1)
		m_activePlane = 2;
	else if (a == 3)
		m_activePlane = 0;
	
	setEye(m_zoomLevel, 30.0f, 0.0f);
	
	emit cursorDirectionChanged(direction);
}

void GLView::setEye(float radius, float vert, float horiz)
{
	int otherPlane = m_activePlane == 0 ? 2 : 0;
	
	float v = vert;
	float h = horiz;
	
	if (v > 85.0f)
		v = 85.0f;
	if (v < -85.0f)
		v = -85.0f;
	if (h > 85.0f)
		h = 85.0f;
	if (h < -85.0f)
		h = -85.0f;
	
	v = degreesToRadians(v);
	h = degreesToRadians(h);
	
	m_destinationEyeOffset[m_activePlane] = radius * cos(v) * cos(h);
	m_destinationEyeOffset[1] = radius * sin(v);
	m_destinationEyeOffset[otherPlane] = radius * sin(h);
}

int GLView::cursorDirection()
{
	return m_cursorDirection;
}

void GLView::setFungeSpace(FungeSpace* funge)
{
	delete m_fungeSpace;
	m_fungeSpace = funge;
}

void GLView::resetView()
{
	// Initialize the curser
	m_cursorBlinkTime.start();
	m_cursor.clear();
	for (uint i=0 ; i<m_fungeSpace->dimensions() ; ++i)
		m_cursor.append(0);
	
	m_destinationCameraOffset[0] = 0.0f;
	m_destinationCameraOffset[1] = 0.0f;
	m_destinationCameraOffset[2] = 0.0f;
	
	setCursorDirection(1);
	
	m_actualEyeOffset[0] = m_destinationEyeOffset[0];
	m_actualEyeOffset[1] = m_destinationEyeOffset[1];
	m_actualEyeOffset[2] = m_destinationEyeOffset[2];
	
	m_actualCameraOffset[0] = m_destinationCameraOffset[0];
	m_actualCameraOffset[1] = m_destinationCameraOffset[1];
	m_actualCameraOffset[2] = m_destinationCameraOffset[2];
	
	m_actualCursorPos[0] = 0.0f;
	m_actualCursorPos[1] = 0.0f;
	m_actualCursorPos[2] = 0.0f;
	
	m_activePlane = 2;
	
	if (m_stringMode)
		toggleStringMode();
}

void GLView::updateFPSCounter()
{
	m_fpsCounter = m_frameCount / 2.0f;
	m_frameCount = 0;
}



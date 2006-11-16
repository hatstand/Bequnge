#include "glview.h"
#include "fungespace.h"

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResource>
#include <QMessageBox>
#include <QApplication>
#include <QFocusEvent>
#include <QUndoStack>
#include <QBuffer>

#include <QDebug>

#include <math.h>


GLView::GLView(FungeSpace* fungeSpace, QWidget* parent)
	: QGLWidget(parent),
	  m_fungeSpace(fungeSpace),
	  m_cursorBlinkOn(true),
	  m_cursorDirection(1),
	  m_selectDragging(false),
	  m_fpsCounter(0.0f),
	  m_frameCount(0),
	  m_stringMode(false),
	  m_zoomLevel(6.0f),
	  m_moveDragging(false),
	  m_rotateDragging(false),
	  m_execution(false),
	  m_displayChanges(false)
{
	setFocusPolicy(Qt::WheelFocus);
	
	m_origin.append(0);
	m_origin.append(0);
	m_origin.append(0);
	
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
	
	m_fontLarge.setPointSize(14);
	m_fontLarge.setBold(true);
	m_fontSmall.setPointSize(10);
	m_metricsSmall = new QFontMetrics(m_fontSmall);
	
	m_executionStr = "Execution fungespace";
	m_execution2Str = "Changes made here will not affect your original source";
	m_executionRect = QFontMetrics(m_fontLarge).boundingRect(m_executionStr);
	m_execution2Rect = QFontMetrics(m_fontSmall).boundingRect(m_execution2Str);
	m_executionRect.setWidth(m_executionRect.width() + 7);
	
	// Enable mouse tracking
	setMouseTracking(true);

	// Sort out undo framework
	QUndoStack* currentUndo = new QUndoStack(&m_undoGroup);
	connect(currentUndo, SIGNAL(destroyed(QObject*)), SLOT(spaceDeleted(QObject*)));
	m_undos.insert(m_fungeSpace, currentUndo);
	m_undoGroup.setActiveStack(currentUndo);
}


GLView::~GLView()
{
	delete m_font;
	delete m_metricsSmall;
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
	m_displayListsBase = glGenLists(3);
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
			glVertex3f(m_fontSize - 5.0f, m_fontSize, 0.0f);
			glVertex3f(0.0f, m_fontSize, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(m_fontSize - 5.0f, 0.0f, 0.0f);
		glEnd();
	glEndList();
	
	glNewList(m_displayListsBase + GRID, GL_COMPILE);
		glBegin(GL_LINES);
			for (int y=0 ; y<=40 ; y+=5)
			{
				for (int z=0 ; z<=40 ; z+=5)
				{
					glVertex3f(0.0f, - m_fontSize * y, m_fontSize * z);
					glVertex3f(m_fontSize * 40, - m_fontSize * y, m_fontSize * z);
				}
			}
			
			for (int x=0 ; x<=40 ; x+=5)
			{
				for (int z=0 ; z<=40 ; z+=5)
				{
					glVertex3f(m_fontSize * x, 0.0f, m_fontSize * z);
					glVertex3f(m_fontSize * x, - m_fontSize * 40, m_fontSize * z);
				}
			}
			
			for (int x=0 ; x<=40 ; x+=5)
			{
				for (int y=0 ; y<=40 ; y+=5)
				{
					glVertex3f(m_fontSize * x, - m_fontSize * y, 0.0f);
					glVertex3f(m_fontSize * x, - m_fontSize * y, m_fontSize * 40);
				}
			}
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
		m_actualCameraOffset[i] += diff * m_cameraMoveSpeed;
	
	diff = m_destinationEyeOffset[i] - m_actualEyeOffset[i];
	if (fabs(diff) < 0.01)
		m_actualEyeOffset[i] = m_destinationEyeOffset[i];
	else
		m_actualEyeOffset[i] += diff * m_cameraMoveSpeed;
	
	QList<float> c;
	if ((!m_execution) || (m_followingPC == -1))
		c = fungeSpaceToGl(m_cursor, false);
	else
		c = fungeSpaceToGl(m_pcs[m_followingPC].first, false);
	diff = c[i] - m_actualCursorPos[i];
	if (fabs(diff) < 0.01)
		m_actualCursorPos[i] = c[i];
	else
		m_actualCursorPos[i] += diff * m_cameraMoveSpeed;
	
	if (i == 0)
	{
		diff = m_destinationGridAlpha - m_actualGridAlpha;
		if (fabs(diff) < 0.001)
			m_actualGridAlpha = m_destinationGridAlpha;
		else
			m_actualGridAlpha += diff * 0.05;
	}
}

/*void GLView::explode(Coord c)
{
	// GOTCHA
	QList<float> f = fungeSpaceToGl(c, true); 

	for(int i = 0; i < 100; ++i)
	{
		float radians = ((float)rand())/RAND_MAX * M_PI * 2;
		float x = sin(radians);
		float y = cos(radians);
		float z = 0;

		float p[3] = {f[0], f[1], f[2]};

		m_particles << new Particle(p,x,y,z);
	}
}*/

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
	          m_actualCursorPos[2] + m_actualCameraOffset[2],
	          0.0f,
	          1.0f,
	          0.0f);
	
	Coord selTopLeft = selectionTopLeft();
	Coord selBottomRight = selectionBottomRight();
	
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
			
			if (m_ascensionLevel > 0)
			{
				if ((coords[0] > m_cursor[0] + 20) || (coords[0] <= m_cursor[0] - 20) ||
				    (coords[1] > m_cursor[1] + 20) || (coords[1] <= m_cursor[1] - 20) ||
				    (coords[2] > m_cursor[2] + 20) || (coords[2] <= m_cursor[2] - 20))
				    continue;
			}
			
			glPushMatrix();
				int diff = abs(coords[m_activePlane] - m_cursor[m_activePlane]);
				
				bool withinSelection = ((m_selectionAnchor != m_selectionEnd) &&
				                        (coords[0] >= selTopLeft[0]) && (coords[0] <= selBottomRight[0]) &&
				                        (coords[1] >= selTopLeft[1]) && (coords[1] <= selBottomRight[1]) &&
				                        (coords[2] >= selTopLeft[2]) && (coords[2] <= selBottomRight[2]));
				
				if ((coords == m_cursor) && (m_cursorBlinkOn || !hasFocus()))
					glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
				else if (m_mouseHoveringOverChar && (m_mouseHover == coords))
					glColor4f(1.0f, 0.2f, 0.2f, 1.0f);
				else if (withinSelection)
				{
					if (diff == 0)
						glColor4f(0.5f, 1.0f, 1.0f, 1.0f);
					else
						glColor4f(0.5f, 0.5f, 1.0f, 1.0f);
				}
				else if (diff == 0)
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
				else if (diff <= 8)
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f - diff * 0.1f);
				else
					glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
				
				coord = fungeSpaceToGl(coords, true);
				glTranslatef(coord[0], coord[1], coord[2]);
				
				if (m_activePlane == 0)
				{
					glTranslatef(m_fontSize/2, 0.0f, 0.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-m_fontSize/2, 0.0f, 0.0f);
				}
				m_font->draw(data);
			glPopMatrix();
		}
		//glDisable(GL_TEXTURE_2D);
		
		if (m_cursorBlinkOn || !hasFocus())
		{
			glPushMatrix();
				coord = fungeSpaceToGl(m_cursor, true);
				glTranslatef(coord[0] - 0.01f, coord[1] - 2.5f, coord[2] - 0.01f);
				if (m_activePlane == 0)
				{
					glTranslatef(m_fontSize/2, 0.0f, 0.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-m_fontSize/2, 0.0f, 0.0f);
				}
				glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
				glCallList(m_displayListsBase + CURSOR);
			glPopMatrix();
		}
		
		if (m_execution)
		{
			QMapIterator<int, QPair<Coord, Coord > > i(m_pcs);
			while (i.hasNext())
			{
				i.next();
				glPushMatrix();
					coord = fungeSpaceToGl(i.value().first, true);
					glTranslatef(coord[0] - 0.01f, coord[1] - 2.5f, coord[2] - 0.01f);
					if (m_activePlane == 0)
					{
						glTranslatef(m_fontSize/2, 0.0f, 0.0f);
						glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
						glTranslatef(-m_fontSize/2, 0.0f, 0.0f);
					}
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					glCallList(m_displayListsBase + CURSOR);
				glPopMatrix();
			}
		}
		
		if (m_selectionAnchor != m_selectionEnd)
		{
			glPushMatrix();
				glColor4f(0.5f, 0.5f, 1.0f, 0.3f);
				drawCube(selTopLeft, selBottomRight);
			glPopMatrix();
		}
		
		if (m_displayChanges)
		{
			glColor4f(1.0f, 0.5f, 0.5f, 0.3f);
			QHash<Coord, QPair<QChar, QChar> > changes = m_fungeSpace->changes();
			QList<Coord> changeCoords = changes.keys();
			foreach (Coord c, changeCoords)
			{
				glPushMatrix();
					drawCube(c, c);
				glPopMatrix();
			}
		}
		
		if (m_actualGridAlpha != 0.0f)
		{
			glPushMatrix();
				Coord gridStart = m_cursor;
				gridStart[0] -= 20;
				gridStart[1] -= 20;
				gridStart[2] += 20;
				
				coord = fungeSpaceToGl(gridStart, true);
				coord[0] -= 2.4f;
				coord[1] += m_fontSize - 0.1f;
				coord[2] -= m_fontSize/2 - 2.4f;
				
				glTranslatef(coord[0], coord[1] - 2.5f, coord[2]);
				glColor4f(0.0f, 0.5f, 0.0f, m_actualGridAlpha);
				glCallList(m_displayListsBase + GRID);
			glPopMatrix();
		}
		
		glClear(GL_DEPTH_BUFFER_BIT);
		
		i.toFront();
		glColorMask(false, false, false, false);
		while (i.hasNext())
		{
			i.next();
			Coord coords = i.key();
			QChar data = i.value();
			
			glPushMatrix();
				coord = fungeSpaceToGl(coords, true);
				glTranslatef(coord[0], coord[1], coord[2]);
				if (m_activePlane == 0)
				{
					glTranslatef(m_fontSize/2, 0.0f, 0.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-m_fontSize/2, 0.0f, 0.0f);
				}
				glCallList(m_displayListsBase + CURSOR);
			glPopMatrix();
		}
		glColorMask(true, true, true, true);

		/*foreach(Particle* p, m_particles)
		{
			glPushMatrix();
				p->paintGL(1000/30, 1.0);
			glPopMatrix();
		}*/
	glPopMatrix();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(0, 15, "Cursor: " + QString::number(m_cursor[0]) + ", " + QString::number(m_cursor[1]) + ", " + QString::number(m_cursor[2]));
	renderText(0, 30, "FPS: " + QString::number(m_fpsCounter));
	
	if (m_execution)
	{
		QString followingStr;
		if (m_followingPC == -1)
			followingStr = "Following editor cursor";
		else
			followingStr = "Following PC " + QString::number(m_followingPC);
		QRect boundingRect = m_metricsSmall->boundingRect(followingStr);
		
		renderText(width() - boundingRect.width(), m_executionRect.height() + m_execution2Rect.height() + boundingRect.height(), followingStr, m_fontSmall);
		renderText(width() - m_execution2Rect.width(), m_executionRect.height() + m_execution2Rect.height(), m_execution2Str, m_fontSmall);
		glColor3f(0.0f, 1.0f, 0.0f);
		renderText(width() - m_executionRect.width(), m_executionRect.height(), m_executionStr, m_fontLarge);
	}
	
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

void GLView::drawCube(Coord startPos, Coord endPos)
{
	QList<float> cubeStart = fungeSpaceToGl(startPos, true);
	QList<float> cubeEnd = fungeSpaceToGl(endPos, true);
	
	float temp = cubeStart[2];
	cubeStart[2] = cubeEnd[2];
	cubeEnd[2] = temp;
	
	cubeStart[0] -= 2.4f;
	cubeStart[1] += m_fontSize - 0.1f;
	cubeStart[2] -= m_fontSize/2 - 2.4f;
	
	cubeEnd[0] += m_fontSize - 2.6f;
	cubeEnd[1] += 0.1f;
	cubeEnd[2] += m_fontSize/2 - 0.1f;
	
	cubeEnd[0] -= cubeStart[0];
	cubeEnd[1] -= cubeStart[1];
	cubeEnd[2] -= cubeStart[2];
	
	glTranslatef(cubeStart[0], cubeStart[1] - 2.5f, cubeStart[2]);
	// draw a cube (6 quadrilaterals)
	glBegin(GL_QUADS);				// start drawing the cube.
		// top of cube
		glVertex3f(cubeEnd[0], cubeEnd[1], 0.0f);		// Top Right Of The Quad (Top)
		glVertex3f(0.0f, cubeEnd[1], 0.0f);		// Top Left Of The Quad (Top)
		glVertex3f(0.0f, cubeEnd[1], cubeEnd[2]);		// Bottom Left Of The Quad (Top)
		glVertex3f(cubeEnd[0], cubeEnd[1], cubeEnd[2]);		// Bottom Right Of The Quad (Top)
		
		// bottom of cube
		glVertex3f(cubeEnd[0],0.0f, cubeEnd[2]);		// Top Right Of The Quad (Bottom)
		glVertex3f(0.0f, 0.0f, cubeEnd[2]);		// Top Left Of The Quad (Bottom)
		glVertex3f(0.0f, 0.0f, 0.0f);		// Bottom Left Of The Quad (Bottom)
		glVertex3f(cubeEnd[0], 0.0f, 0.0f);		// Bottom Right Of The Quad (Bottom)
		
		// front of cube
		glVertex3f(cubeEnd[0], cubeEnd[1], cubeEnd[2]);		// Top Right Of The Quad (Front)
		glVertex3f(0.0f, cubeEnd[1], cubeEnd[2]);		// Top Left Of The Quad (Front)
		glVertex3f(0.0f, 0.0f, cubeEnd[2]);		// Bottom Left Of The Quad (Front)
		glVertex3f(cubeEnd[0],0.0f, cubeEnd[2]);		// Bottom Right Of The Quad (Front)
		
		// back of cube.
		glVertex3f(cubeEnd[0], 0.0f, 0.0f);		// Top Right Of The Quad (Back)
		glVertex3f(0.0f, 0.0f, 0.0f);		// Top Left Of The Quad (Back)
		glVertex3f(0.0f, cubeEnd[1], 0.0f);		// Bottom Left Of The Quad (Back)
		glVertex3f(cubeEnd[0], cubeEnd[1], 0.0f);		// Bottom Right Of The Quad (Back)
		
		// left of cube
		glVertex3f(0.0f, cubeEnd[1], cubeEnd[2]);		// Top Right Of The Quad (Left)
		glVertex3f(0.0f, cubeEnd[1], 0.0f);		// Top Left Of The Quad (Left)
		glVertex3f(0.0f, 0.0f, 0.0f);		// Bottom Left Of The Quad (Left)
		glVertex3f(0.0f, 0.0f, cubeEnd[2]);		// Bottom Right Of The Quad (Left)
		
		// Right of cube
		glVertex3f(cubeEnd[0], cubeEnd[1], 0.0f);	        // Top Right Of The Quad (Right)
		glVertex3f(cubeEnd[0], cubeEnd[1], cubeEnd[2]);		// Top Left Of The Quad (Right)
		glVertex3f(cubeEnd[0], 0.0f, cubeEnd[2]);		// Bottom Left Of The Quad (Right)
		glVertex3f(cubeEnd[0], 0.0f, 0.0f);		// Bottom Right Of The Quad (Right)
	glEnd();					// Done Drawing The Cube
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
	m_mouseHoveringOverChar = false;
	if (m_moveDragging)
	{
		float xOffset = event->pos().x() - m_preDragMousePosition.x();
		float yOffset = event->pos().y() - m_preDragMousePosition.y();
		
		if (m_activePlane == 2)
			m_destinationCameraOffset[0] = - xOffset / 135.0f;
		else
			m_destinationCameraOffset[2] = xOffset / 135.0f;
		
		m_destinationCameraOffset[1] = yOffset / 135.0f;
	}
	else if (m_rotateDragging)
	{
		float xOffset = - (event->pos().x() - m_preDragMousePosition.x()) / 4.0f;
		float yOffset = 30.0f + (event->pos().y() - m_preDragMousePosition.y()) / 4.0f;
		
		if (m_activePlane == 0)
			xOffset *= -1;
		
		setEye(m_zoomLevel, yOffset, xOffset);
	}
	else if (m_selectDragging)
	{
		Coord p = pointToFungeSpace(event->pos());
		if (m_fungeSpace->getChar(p) != ' ')
			m_selectionEnd = p;
	}
	else
	{
		// Hovering
		Coord p = pointToFungeSpace(event->pos());
		if (m_fungeSpace->getChar(p) != ' ')
		{
			m_mouseHover = p;
			m_mouseHoveringOverChar = true;
		}
	}
}

void GLView::wheelEvent(QWheelEvent* event)
{
	m_zoomLevel -= event->delta() / 90.0f;
	
	if (m_zoomLevel < 1.0f)
		m_zoomLevel = 1.0f;
	
	setEye(m_zoomLevel, 35.0f, 0.0f);
}

Coord GLView::glToFungeSpace(float x, float y, float z)
{
	float size = m_fontSize;
	
	Coord ret;
	ret.append((int)(floor(x/size)));
	ret.append((int)(- floor(y/size)));
	ret.append((int)(floor((z*-1)/size - 0.5f) + 1));
	return ret;
}

QList<float> GLView::fungeSpaceToGl(Coord c, bool premultiplied)
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

Coord GLView::pointToFungeSpace(const QPoint& pos)
{
	double objx, objy, objz;
	float x = pos.x();
	float y = pos.y();
	double modelview[16], projection[16];
	GLint viewport[4];
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
	
	return glToFungeSpace(objx, objy, objz);
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
		
		Coord p = pointToFungeSpace(event->pos());
		if (m_fungeSpace->getChar(p) != ' ')
		{
			m_selectionAnchor = m_selectionEnd = p;
			m_selectDragging = true;
		}
		
		event->accept();
		break;
	}
	case Qt::RightButton:
	{
		Coord p = pointToFungeSpace(event->pos());
		if (m_fungeSpace->changes().contains(p))
			emit(copyChangeToCodeFungeSpace(p));
		event->accept();
		break;
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
		m_destinationCameraOffset[2] = 0.0f;
		
		if (m_selectDragging)
			setCursor(m_selectionEnd, QTextCursor::KeepAnchor);
		m_selectDragging = false;
		break;
		
	default:
		break;
	}
}

int GLView::otherPlane()
{
	return m_activePlane == 0 ? 2 : 0;
}

void GLView::keyPressEvent(QKeyEvent* event)
{
	if (event->modifiers() & Qt::AltModifier)
	{
		bool handled = true;
		if (event->key() == Qt::Key_Right)
			setCursorDirection(otherPlane() + 1);
		else if (event->key() == Qt::Key_Left)
			setCursorDirection(-(otherPlane() + 1));
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
		moveCursor(otherPlane() + 1);
	else if (event->matches(QKeySequence::MoveToPreviousChar))
		moveCursor(-otherPlane() - 1);
	else if (event->matches(QKeySequence::MoveToNextLine))
		moveCursor(0, 1, 0);
	else if (event->matches(QKeySequence::MoveToPreviousLine))
		moveCursor(0, -1, 0);
	else if (event->matches(QKeySequence::MoveToNextPage))
		moveCursor(-m_activePlane - 1);
	else if (event->matches(QKeySequence::MoveToPreviousPage))
		moveCursor(m_activePlane + 1);
	else if (event->matches(QKeySequence::SelectNextChar))
		moveCursor(otherPlane() + 1, QTextCursor::KeepAnchor);
	else if (event->matches(QKeySequence::SelectPreviousChar))
		moveCursor(-otherPlane() - 1, QTextCursor::KeepAnchor);
	else if (event->matches(QKeySequence::SelectNextLine))
		moveCursor(0, 1, 0, QTextCursor::KeepAnchor);
	else if (event->matches(QKeySequence::SelectPreviousLine))
		moveCursor(0, -1, 0, QTextCursor::KeepAnchor);
	else if (event->matches(QKeySequence::SelectNextPage))
		moveCursor(m_activePlane + 1, QTextCursor::KeepAnchor);
	else if (event->matches(QKeySequence::SelectPreviousPage))
		moveCursor(-m_activePlane - 1, QTextCursor::KeepAnchor);
	else if (event->key() == Qt::Key_Backspace)
	{
		moveCursor(-m_cursorDirection);
		
		if (m_fungeSpace->getChar(m_cursor) == '"')
			toggleStringMode();
		
		setChar(m_cursor, ' ');
	}
	else if (event->matches(QKeySequence::Delete))
	{
		setChar(m_cursor, ' ');
	}
	else if (event->key() == Qt::Key_Tab)
		setActivePlane(otherPlane());
	else if ((event->key() == Qt::Key_Up) && (event->modifiers() & Qt::ControlModifier))
	{
		m_cameraMoveSpeed = 0.01;
		m_destinationGridAlpha = 0.5f;
		m_ascensionLevel++;
		setEye(40.0f, 30.0f, -30.0f);
	}
	/*else if ((event->key() == Qt::Key_Down) && (event->modifiers() & Qt::ControlModifier))
	{
		m_cameraMoveSpeed = 0.01;
		m_destinationGridAlpha = 0.5f;
		m_ascensionLevel++;
		setEye(40.0f, 30.0f, -30.0f);
	}*/
	else if (!event->text().isEmpty())
	{
		QChar c = event->text()[0];
		setChar(m_cursor, c);
		
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
		
		moveCursor(m_cursorDirection);
	}
	else
		return;
	
	m_cursorBlinkTime.start();
	m_cursorBlinkOn = true;
	event->accept();
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
	m_destinationEyeOffset[otherPlane()] = radius * sin(h);
}

int GLView::cursorDirection()
{
	return m_cursorDirection;
}

void GLView::setFungeSpace(FungeSpace* funge)
{
	if(m_undos.contains(funge))
		m_undoGroup.setActiveStack(m_undos[funge]);
	else
	{
		QUndoStack* s = new QUndoStack(&m_undoGroup);
		m_undos.insert(funge, s);
		m_undoGroup.setActiveStack(s);
	}

	m_fungeSpace = funge;
}

void GLView::resetView()
{
	// Initialize the curser
	m_cursorBlinkTime.start();
	m_cursor.clear();
	for (uint i=0 ; i<m_fungeSpace->dimensions() ; ++i)
		m_cursor.append(0);
	
	m_selectionAnchor = m_selectionEnd = m_cursor;
	
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
	
	m_destinationGridAlpha = 0.0f;
	m_actualGridAlpha = m_destinationGridAlpha;
	m_ascensionLevel = 0;
	
	m_cameraMoveSpeed = 0.2;
	
	m_activePlane = 2;
	
	if (m_stringMode)
		toggleStringMode();
}

void GLView::updateFPSCounter()
{
	m_fpsCounter = m_frameCount / 2.0f;
	m_frameCount = 0;
}

void GLView::setExecution(bool execution)
{
	m_execution = execution;
}

void GLView::setPC(int pc, Coord position, Coord direction)
{
	m_pcs[pc] = QPair<Coord, Coord>(position, direction);
}

void GLView::followPC(int pc)
{
	m_followingPC = pc;
}

bool GLView::focusNextPrevChild(bool next)
{
	return false;
}

void GLView::setActivePlane(int plane)
{
	m_activePlane = plane;
	switch (m_cursorDirection)
	{
		case 1: setCursorDirection(3); break;
		case -1: setCursorDirection(-3); break;
		case 3: setCursorDirection(1); break;
		case -3: setCursorDirection(-1); break;
		default: setCursorDirection(m_cursorDirection); break;
	}
}

Coord GLView::selectionTopLeft()
{
	Coord ret;
	ret.append(qMin(m_selectionEnd[0], m_selectionAnchor[0]));
	ret.append(qMin(m_selectionEnd[1], m_selectionAnchor[1]));
	ret.append(qMin(m_selectionEnd[2], m_selectionAnchor[2]));
	return ret;
}

Coord GLView::selectionBottomRight()
{
	Coord ret;
	ret.append(qMax(m_selectionEnd[0], m_selectionAnchor[0]));
	ret.append(qMax(m_selectionEnd[1], m_selectionAnchor[1]));
	ret.append(qMax(m_selectionEnd[2], m_selectionAnchor[2]));
	return ret;
}

void GLView::moveCursor(int x, int y, int z, QTextCursor::MoveMode mode)
{
	m_cursor[0] += x;
	m_cursor[1] += y;
	m_cursor[2] += z;
	
	m_selectionEnd = m_cursor;
	
	if (mode == QTextCursor::MoveAnchor)
		m_selectionAnchor = m_cursor;
}

void GLView::moveCursor(int direction, QTextCursor::MoveMode mode)
{
	int x = 0;
	int y = 0;
	int z = 0;
	
	switch (direction)
	{
		case -3: z--; break;
		case -2: y--; break;
		case -1: x--; break;
		case 1: x++; break;
		case 2: y++; break;
		case 3: z++; break;
	}
	
	moveCursor(x, y, z, mode);
}

void GLView::setCursor(int x, int y, int z, QTextCursor::MoveMode mode)
{
	m_cursor[0] = x;
	m_cursor[1] = y;
	m_cursor[2] = z;
	
	m_selectionEnd = m_cursor;
	
	if (mode == QTextCursor::MoveAnchor)
		m_selectionAnchor = m_cursor;
}

void GLView::setCursor(Coord c, QTextCursor::MoveMode mode)
{
	setCursor(c[0], c[1], c[2], mode);
}

void GLView::setChar(Coord p, QChar newchar)
{
	FungeCommand* f = new FungeCommand(m_fungeSpace, p, newchar);
	m_undoGroup.activeStack()->push(f);
}

void GLView::spaceDeleted(QObject* space)
{
	m_undos.remove(dynamic_cast<FungeSpace*>(space));
}

void GLView::selectionToClipboard(bool cut, QClipboard::Mode mode)
{
	Coord selTopLeft = selectionTopLeft();
	Coord selBottomRight = selectionBottomRight();
	
	int selWidth = selBottomRight[0] - selTopLeft[0] + 1;
	int selHeight = selBottomRight[1] - selTopLeft[1] + 1;
	int selDepth = selTopLeft[2] - selBottomRight[2] + 1;
	
	QHash<Coord, QChar> charsWithinSelection;
	
	QString frontPlaneText;
	QString emptyLine;
	emptyLine.fill(' ', selWidth);
	emptyLine += "\n";
	for (int y=0 ; y<selHeight ; ++y)
		frontPlaneText += emptyLine;
	
	QHash<Coord, QChar> code = m_fungeSpace->getCode();
	QHashIterator<Coord, QChar> i(code);
	ChangeList changes;
	while (i.hasNext())
	{
		i.next();
		Coord c = i.key();
		QChar value = i.value();
		if ((c[0] >= selTopLeft[0]) && (c[0] <= selBottomRight[0]) &&
			(c[1] >= selTopLeft[1]) && (c[1] <= selBottomRight[1]) &&
			(c[2] >= selTopLeft[2]) && (c[2] <= selBottomRight[2]))
		{
			int x = c[0] - selTopLeft[0];
			int y = c[1] - selTopLeft[1];
			int z = c[2] - selTopLeft[2];
			if (z == 0)
				frontPlaneText[y*emptyLine.length() + x] = value;
			
			Coord transformedCoord;
			transformedCoord.append(x);
			transformedCoord.append(y);
			transformedCoord.append(z);
			
			charsWithinSelection.insert(transformedCoord, value);
			
			if (cut)
			{
				changes.insert(c, QPair<QChar, QChar>(m_fungeSpace->getChar(c), ' '));
				//m_fungeSpace->setChar(c, ' ');
			}
		}
	}

	if(cut)
		m_undoGroup.activeStack()->push(new FungeCommand(m_fungeSpace, changes));
	
	QByteArray serialisedData;
	QBuffer buffer(&serialisedData);
	buffer.open(QBuffer::ReadWrite);
	QDataStream stream(&buffer);
	stream << selWidth << selHeight << selDepth;
	stream << charsWithinSelection;
	
	QMimeData* data = new QMimeData();
	data->setText(frontPlaneText);
	data->setData("application/x-bequnge", serialisedData);
	
	QApplication::clipboard()->setMimeData(data, mode);
}

void GLView::slotCopy()
{
	selectionToClipboard(false);
}

void GLView::slotCut()
{
	selectionToClipboard(true);
}

void GLView::slotPasteTransparant()
{
	paste(true);
}

void GLView::slotPaste()
{
	paste(false);
}

void GLView::paste(bool transparant)
{
	const QMimeData* data = QApplication::clipboard()->mimeData();

	ChangeList changes;
	
	if (data->hasFormat("application/x-bequnge"))
	{
		QByteArray serialisedData = data->data("application/x-bequnge");
		QBuffer buffer(&serialisedData);
		buffer.open(QBuffer::ReadOnly);
		QDataStream stream(&buffer);
		
		int selWidth, selHeight, selDepth;
		QHash<Coord, QChar> code;
		stream >> selWidth >> selHeight >> selDepth;
		stream >> code;
		
		if (!transparant)
		{
			Coord bottomRight = m_cursor;
			bottomRight[0] += selWidth - 1;
			bottomRight[1] += selHeight - 1;
			bottomRight[2] += selDepth - 1;
			clearRect(m_cursor, bottomRight, &changes);
		}
		
		QHashIterator<Coord, QChar> i(code);
		while (i.hasNext())
		{
			i.next();
			Coord c = i.key();
			c[0] += m_cursor[0];
			c[1] += m_cursor[1];
			c[2] += m_cursor[2];
			QChar value = i.value();
			
			changes.insert(c, QPair<QChar, QChar>(m_fungeSpace->getChar(c), value));
			//m_fungeSpace->setChar(c, value);
		}
	}
	else if (data->hasText())
	{
		Coord c;
		c.append(m_cursor[0]);
		c.append(m_cursor[1]);
		c.append(m_cursor[2]);
		
		QStringList lines = data->text().split('\n');
		for (int y=0 ; y<lines.count() ; ++y)
		{
			c[0] = m_cursor[0];
			QString line = lines[y];
			for (int x=0 ; x<line.length() ; ++x)
			{
				if ((!transparant) || (line[x] != ' '))
				{
					changes.insert(c, QPair<QChar, QChar>(m_fungeSpace->getChar(c), line[x]));
					//m_fungeSpace->setChar(c, line[x]);
				}
				c[0]++;
			}
			
			c[1]++;
		}
	}

	m_undoGroup.activeStack()->push(new FungeCommand(m_fungeSpace, changes));
}

void GLView::clearRect(Coord topLeft, Coord bottomRight, ChangeList* changes)
{
	Coord c = topLeft;
	
	for ( ; c[0]<=bottomRight[0] ; ++c[0])
	{
		for (c[1]=topLeft[1] ; c[1]<=bottomRight[1] ; ++c[1])
		{
			for (c[2]=topLeft[2] ; c[2]<=bottomRight[2] ; ++c[2])
			{
				changes->insert(c, QPair<QChar, QChar>(m_fungeSpace->getChar(c), ' '));
				//m_fungeSpace->setChar(c, ' ');
			}
		}
	}
}


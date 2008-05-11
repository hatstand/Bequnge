#include "glview.h"
#include "fungespace.h"
#include "extradimensions.h"
#include "magicnumbers.h"
#include "shader.h"
#include "glfont.h"

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
#include <QGLFramebufferObject>
#include <QFontDatabase>

#include <QDebug>

#include <math.h>

QList<Shader*> GLView::s_ppShaders;

GLView::GLView(FungeSpace* fungeSpace, const QGLFormat& format, QWidget* parent)
	: QGLWidget(format, parent),
	  m_fungeSpace(fungeSpace),
	  m_execution(false),
	  m_displayChanges(false),
	  m_cursorBlinkOn(true),
	  m_cursorDirection(1),
	  m_selectDragging(false),
	  m_fpsCounter(0.0f),
	  m_frameCount(0),
	  m_stringMode(false),
	  m_zoomLevel(6.0f),
	  m_moveDragging(false),
	  m_rotateDragging(false),
	  m_enableWhoosh(false),
	  m_offsetWhoosh(5),
	  m_sceneFbo(NULL),
	  m_bloom(true)
{
	setFocusPolicy(Qt::WheelFocus);
	
	// Setup the redraw timer
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateGL()));
	
	m_delayMs = 1000/30; // 30fps
	
	m_fpsCounterTimer = new QTimer(this);
	m_fpsCounterTimer->start(2000);
	connect(m_fpsCounterTimer, SIGNAL(timeout()), SLOT(updateFPSCounter()));
	
	// Load the font
	int fontId = QFontDatabase::addApplicationFont(":luximr.ttf");
	
	m_fontLarge.setPointSize(14);
	m_fontLarge.setBold(true);
	m_fontSmall.setPointSize(10);
	m_metricsSmall = new QFontMetrics(m_fontSmall);

	m_fontWhoosh.setPointSize(36);
	m_fontWhoosh.setItalic(true);
	m_fontWhoosh.setBold(true);
	
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

	// Sound
	m_sound = new Sound(this);
	
	// Particle groups
	m_cursorPG = m_P.GenParticleGroups(1, 2000);
	m_explosionsPG = m_P.GenParticleGroups(1, 50000);
	
	m_extraDimensions = new ExtraDimensions(this);
	
	// Reset the view
	resetView();
}


GLView::~GLView()
{
	delete m_font;
	delete m_metricsSmall;
}

void GLView::recreateFbos()
{
	delete m_sceneFbo;
	m_sceneFbo = new QGLFramebufferObject(nextPowerOf2(width()), nextPowerOf2(height()), QGLFramebufferObject::Depth);
	glBindTexture(GL_TEXTURE_2D, m_sceneFbo->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	m_sceneFbo->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	m_sceneFbo->release();
	
	qDeleteAll(m_blurTargets);
	m_blurTargets.clear();
	
	QSize size(m_sceneFbo->size() / 2);
	for (int i=0 ; i<3 ; ++i)
	{
		size /= 2;
		
		QGLFramebufferObject* fbo = new QGLFramebufferObject(size, QGLFramebufferObject::Depth);
		glBindTexture(GL_TEXTURE_2D, fbo->texture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		fbo->bind();
		glClear(GL_COLOR_BUFFER_BIT);
		fbo->release();
		
		m_blurTargets << fbo;
	}
}

int GLView::nextPowerOf2(int n)
{
	int ret = 1;
	while (ret < n)
		ret *= 2;
	return ret;
}

void GLView::initializeGL()
{
	// Setup global state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_MULTISAMPLE_ARB);
	
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
			glVertex3f(FONT_SIZE, FONT_SIZE, 0.0f);
			glVertex3f(0.0, FONT_SIZE, 0.0f);
			glVertex3f(0.0, 0.0, 0.0f);
			glVertex3f(FONT_SIZE, 0.0f, 0.0f);
		glEnd();
	glEndList();
	
	m_extraDimensions->prepareCallList(m_displayListsBase + GRID);
	
	// Shaders
	for (int i=0 ; i<4 ; ++i)
		s_ppShaders << new Shader(":shaders/pp_vert.glsl", ":shaders/pp_pass" + QString::number(i) + ".glsl");
	m_font = new GLFont("Luxi Mono", QPen(Qt::blue));
}

void GLView::resizeGL(int width, int height)
{
	recreateFbos();
}

void GLView::updateCamera(int timeDelta)
{
	m_cameraOffset[0].update(timeDelta);
	m_cameraOffset[1].update(timeDelta);
	m_cameraOffset[2].update(timeDelta);
	
	m_eyeOffset[0].update(timeDelta);
	m_eyeOffset[1].update(timeDelta);
	m_eyeOffset[2].update(timeDelta);
	
	m_extraDimensions->updatePositions(timeDelta);
	
	// TODO: Put this in a SmoothVar
	float3 c = fungeSpaceToGl(cursor());
	
	for (int i=0 ; i<3 ; ++i)
	{
		float diff = c[i] - m_cursorPos[i];
		if (fabs(diff) < 0.01)
			m_cursorPos[i] = c[i];
		else
			m_cursorPos[i] += diff * 0.1;
	}
}

void GLView::drawScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	setupMatrices();
	setupCamera();
	
	glPushMatrix();
		if (m_activePlane == 2)
			drawFunge(m_fungeSpace->codeByClever());
		else
			drawFunge(m_fungeSpace->codeBySide());
		drawCursor();
		drawInstructionPointers();
		drawSelectionCube();
		drawAnnotations();
		m_extraDimensions->drawGridLines(m_cursorPos[0], m_cursorPos[1], m_cursorPos[2]);
		drawExplosionParticles();
	glPopMatrix();
}

void GLView::drawBrightParts()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	setupMatrices();
	setupCamera();
	
	glPushMatrix();
		drawCursor();
		drawInstructionPointers();
		drawAnnotations();
		drawExplosionParticles();
	glPopMatrix();
}

void GLView::setupMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, float(width())/height(), 0.1f, 100.0f); // Ugh
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLView::setupCamera()
{
	//const SmoothVar<float>* extraDimensionsOffset = m_extraDimensions->cameraOffset();
	//float scaleFactor = m_extraDimensions->scaleFactor();
	
	gluLookAt(m_eyeOffset[0] + m_cursorPos[0] /** scaleFactor*/ + m_cameraOffset[0],// + extraDimensionsOffset[0] * scaleFactor,
	          m_eyeOffset[1] + m_cursorPos[1] /** scaleFactor*/ + m_cameraOffset[1],// + extraDimensionsOffset[1] * scaleFactor,
	          m_eyeOffset[2] + m_cursorPos[2] /** scaleFactor*/ + m_cameraOffset[2],// + extraDimensionsOffset[2] * scaleFactor,
	          m_cursorPos[0] /** scaleFactor*/ + m_cameraOffset[0],// + extraDimensionsOffset[0] * scaleFactor,
	          m_cursorPos[1] /** scaleFactor*/ + m_cameraOffset[1],// + extraDimensionsOffset[1] * scaleFactor,
	          m_cursorPos[2] /** scaleFactor*/ + m_cameraOffset[2],// + extraDimensionsOffset[2] * scaleFactor,
	          0.0f,
	          1.0f,
	          0.0f);
	
	//glScalef(FONT_SCALE_FACTOR * scaleFactor, FONT_SCALE_FACTOR * scaleFactor, FONT_SCALE_FACTOR * scaleFactor);
}

void GLView::drawCursor()
{
	// Draw the cursor
	if ((m_extraDimensions->ascensionLevel() == 0) && (m_cursorBlinkOn || !hasFocus()))
	{
		glPushMatrix();
			Coord coords = m_extraDimensions->nDTo3D(m_cursor);
			float3 coord = fungeSpaceToGl(coords);
			glTranslatef(coord[0], coord[1], coord[2]);
			if (m_activePlane == 0)
			{
				glTranslatef(FONT_SIZE/2, 0.0f, 0.0f);
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				glTranslatef(-FONT_SIZE/2, 0.0f, 0.0f);
			}
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glCallList(m_displayListsBase + CURSOR);
		glPopMatrix();
	}
	
	// Draw the cursor's particles
	m_P.CurrentGroup(m_cursorPG);
	drawParticles();
}

void GLView::updateParticles()
{
	m_P.CurrentGroup(m_cursorPG);
	computeParticles(m_cursor, m_cursorDirection, Qt::green);
	
	m_P.CurrentGroup(m_explosionsPG);
	m_P.TargetAlpha(0.0f, 0.01f);
	m_P.KillOld(200.0f);
	m_P.Move();
}

void GLView::drawInstructionPointers()
{
	if (m_execution)
	{
		foreach (Interpreter::InstructionPointer* ip, m_ips)
		{
			glPushMatrix();
				Coord coords = m_extraDimensions->nDTo3D(ip->m_pos);
				float3 coord = fungeSpaceToGl(coords);
				glTranslatef(coord[0] - 0.01f, coord[1] - 2.5f, coord[2] - 0.01f);
				if (m_activePlane == 0)
				{
					glTranslatef(FONT_SIZE/2, 0.0f, 0.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-FONT_SIZE/2, 0.0f, 0.0f);
				}
				qglColor(ip->m_color);
				glCallList(m_displayListsBase + CURSOR);
			glPopMatrix();
			
			// Particles
			/*int direction = 0;
			if (ip->m_direction[0] > 0) direction = 1;
			else if (ip->m_direction[0] < 0) direction = -1;
			else if (ip->m_direction[1] > 0) direction = 2;
			else if (ip->m_direction[1] < 0) direction = -2;
			else if (ip->m_direction[2] > 0) direction = 3;
			else if (ip->m_direction[2] < 0) direction = -3;
			
			m_P.CurrentGroup(ip->m_particleGroup);
			computeParticles(ip->m_pos, direction, ip->m_color);
			drawParticles();*/
		}
	}
}

void GLView::drawSelectionCube()
{
	if (m_selectionAnchor != m_selectionEnd)
	{
		Coord selTopLeft = selectionTopLeft();
		Coord selBottomRight = selectionBottomRight();
		glPushMatrix();
			glColor4f(0.5f, 0.5f, 1.0f, 0.3f);
			drawCube(selTopLeft, selBottomRight);
		glPopMatrix();
	}
}

void GLView::drawAnnotations()
{
	// Draw changes
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
	
	// Draw watchpoints and breakpoints
	glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
	foreach (Coord c, m_fungeSpace->watchpoints())
	{
		glPushMatrix();
			drawCube(c, c);
		glPopMatrix();
	}
	
	glColor4f(1.0f, 0.5f, 0.0f, 0.3f);
	foreach (Coord c, m_fungeSpace->breakpoints())
	{
		glPushMatrix();
			drawCube(c, c);
		glPopMatrix();
	}
}

void GLView::drawExplosionParticles()
{
	m_P.CurrentGroup(m_explosionsPG);
	drawParticles();
}

void GLView::drawDepthBoxes()
{
	// Clear the depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	
	setupMatrices();
	setupCamera();
	
	// Draw bounding boxes in the depth buffer
	if (m_extraDimensions->ascensionLevel() == 0)
	{
		FungeSpace::CodeByClever& code(m_fungeSpace->codeByClever());
		FungeSpace::CodeByClever::iterator i(code.begin());
		
		glColorMask(false, false, false, false);
		while (i != code.end())
		{
			const Coord& coords(i->coord);
			const QChar& data(i->data);
			
			glPushMatrix();
				float3 coord = fungeSpaceToGl(coords);
				glTranslatef(coord[0], coord[1], coord[2]);
				if (m_activePlane == 0)
				{
					glTranslatef(FONT_SIZE/2, 0.0f, 0.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-FONT_SIZE/2, 0.0f, 0.0f);
				}
				glCallList(m_displayListsBase + CURSOR);
			glPopMatrix();
			i++;
		}
		glColorMask(true, true, true, true);
	}
}

void GLView::paintGL()
{
	updateCamera(m_frameTime.elapsed());
	updateParticles();
	
	m_frameTime.start();
	
	// Draw the scene to the scene FBO
	if (m_bloom)
		m_sceneFbo->bind();
	
	glViewport(0, 0, size());
	drawScene();
	
	if (m_bloom)
	{
		m_sceneFbo->release();
		
		m_blurTargets[0]->bind();
		glViewport(0, 0, m_blurTargets[0]->size());
		drawBrightParts();
		m_blurTargets[0]->release();
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		// Blur the bright bits
		blurPass(s_ppShaders[0], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
		blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
		
		// Downsample
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[1]);
		blurPass(s_ppShaders[1], m_blurTargets[1], m_blurTargets[1]);
		
		blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[2]);
		blurPass(s_ppShaders[1], m_blurTargets[2], m_blurTargets[2]);
		
		// Draw back to the screen
		glViewport(0, 0, size());
		s_ppShaders[3]->bind();
		
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_sceneFbo->texture());
		
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_blurTargets[0]->texture());
		
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_blurTargets[1]->texture());
		
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_blurTargets[2]->texture());
		
		glUniform1i(s_ppShaders[3]->uniformLocation("scene"), 0);
		glUniform1i(s_ppShaders[3]->uniformLocation("blur1"), 1);
		glUniform1i(s_ppShaders[3]->uniformLocation("blur2"), 2);
		glUniform1i(s_ppShaders[3]->uniformLocation("blur3"), 3);
		
		drawQuad(float(width()) / m_sceneFbo->width(), float(height()) / m_sceneFbo->height());
		
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		
		Shader::unbind();
	}
	
	
	drawDepthBoxes();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(0, 15, "Cursor: " + QString::number(m_cursor[0]) + ", " + QString::number(m_cursor[1]) + ", " + QString::number(m_cursor[2]), QFont(), 10000);
	renderText(0, 30, "FPS: " + QString::number(m_fpsCounter), QFont(), 11000);
	
	if (m_execution)
	{
		renderText(width() - m_execution2Rect.width(), m_executionRect.height() + m_execution2Rect.height(), m_execution2Str, m_fontSmall, 12000);
		glColor3f(0.0f, 1.0f, 0.0f);
		renderText(width() - m_executionRect.width(), m_executionRect.height(), m_executionStr, m_fontLarge, 13000);
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
		//glCallList(m_displayListsBase + ARROW);
	glPopMatrix();

	if(m_enableWhoosh)
		drawWhoosh();
	
	if (m_cursorBlinkTime.elapsed() > 500)
	{
		m_cursorBlinkTime.start();
		m_cursorBlinkOn = !m_cursorBlinkOn;
	}
	
	m_redrawTimer->start(qAbs(m_delayMs - m_frameTime.elapsed()));
	m_frameCount++;
}

template <typename T>
void GLView::drawFunge(T& code)
{
	Coord selTopLeft = selectionTopLeft();
	Coord selBottomRight = selectionBottomRight();
	
	Coord c = cursor();
	Coord cursorExtraDimensions = c.mid(3);
	
	m_font->bind();
	
	typename T::reverse_iterator i(code.rbegin());
	while (i != code.rend())
	{
		Coord coords(i->coord);
		const QChar& data(i->data);
		
		if (m_extraDimensions->ascensionLevel() > 0)
		{
			if ((coords[0] >= c[0] + 20) || (coords[0] < c[0] - 20) ||
				(coords[1] >= c[1] + 20) || (coords[1] < c[1] - 20) ||
				(coords[2] >= c[2] + 20) || (coords[2] < c[2] - 20))
			{
				i++;
				continue;
			}
		}
		
		if (cursorExtraDimensions != coords.mid(3))
		{
			if (m_extraDimensions->ascensionLevel() != 1)
			{
				i++;
				continue;
			}
			
			if (abs(c[3] - coords[3]) > 2 || abs(coords[4] - c[4]) > 2 || abs(coords[5] - c[5]) > 2)
			{
				i++;
				continue;
			}
		}
		
		int diff = abs(coords[m_activePlane] - m_cursor[m_activePlane]);
			
		bool withinSelection = ((m_selectionAnchor != m_selectionEnd) &&
					(coords[0] >= selTopLeft[0]) && (coords[0] <= selBottomRight[0]) &&
					(coords[1] >= selTopLeft[1]) && (coords[1] <= selBottomRight[1]) &&
					(coords[2] >= selTopLeft[2]) && (coords[2] <= selBottomRight[2]));
		
		coords[0] += coords[3] * 70;
		coords[1] += coords[4] * 70;
		coords[2] += coords[5] * 70;
		
		glPushMatrix();
			if ((coords == m_cursor) && (m_cursorBlinkOn || !hasFocus()) && (m_extraDimensions->ascensionLevel() == 0))
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
			
			float3 coord = fungeSpaceToGl(coords);
			glTranslatef(coord[0], coord[1], coord[2]);
			
			if (m_activePlane == 0)
			{
				glTranslatef(FONT_SIZE/2, 0.0f, 0.0f);
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				glTranslatef(-FONT_SIZE/2, 0.0f, 0.0f);
			}
			m_font->drawChar(data);
		glPopMatrix();
		i++;
	}
	
	m_font->release();
}

void GLView::drawCube(Coord startPos, Coord endPos)
{
	float3 cubeStart = fungeSpaceToGl(startPos);
	float3 cubeEnd = fungeSpaceToGl(endPos);
	
	float temp = cubeStart[2];
	cubeStart[2] = cubeEnd[2];
	cubeEnd[2] = temp;
	
	cubeStart[0] -= 2.4f;
	cubeStart[1] += FONT_SIZE - 0.1f;
	cubeStart[2] -= FONT_SIZE/2 - 2.4f;
	
	cubeEnd[0] += FONT_SIZE - 2.6f;
	cubeEnd[1] += 0.1f;
	cubeEnd[2] += FONT_SIZE/2 - 0.1f;
	
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
			m_cameraOffset[0] = - xOffset / 135.0f;
		else
			m_cameraOffset[2] = xOffset / 135.0f;
		
		m_cameraOffset[1] = yOffset / 135.0f;
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
	
	resetEye();
}

Coord GLView::glToFungeSpace(float x, float y, float z)
{
	float size = FONT_SIZE;
	
	Coord ret;
	ret.append((int)(floor(x/size)));
	ret.append((int)(- floor(y/size)));
	ret.append((int)(floor((z*-1)/size - 0.5f) + 1));
	return ret;
}

float3 GLView::fungeSpaceToGl(const Coord& c)
{
	float size = FONT_SIZE;
	
	float3 ret;
	ret[0] = float(c[0]) * size;
	ret[1] = - float(c[1]) * size;
	ret[2] = - float(c[2]) * size;
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

	// Trying to read a pixel that's outside the viewport crashes
	// some systems (such as Thinkpads with Intel graphics chips).
	if (x<0) x=0;
	else if (x>=width()) x=width()-1;
	if (y<0) y=0;
	else if (y>=height()) y=height()-1;

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
		resetEye();
		break;
		
	case Qt::LeftButton:
		m_moveDragging = false;
		m_cameraOffset[0] = 0.0f;
		m_cameraOffset[1] = 0.0f;
		m_cameraOffset[2] = 0.0f;
		
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
		setAscensionLevel(m_extraDimensions->ascensionLevel() + 1);
	else if ((event->key() == Qt::Key_Down) && (event->modifiers() & Qt::ControlModifier))
		setAscensionLevel(m_extraDimensions->ascensionLevel() - 1);
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
	
	resetEye();
	
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
	
	m_eyeOffset[m_activePlane] = radius * cos(v) * cos(h);
	m_eyeOffset[1] = radius * sin(v);
	m_eyeOffset[otherPlane()] = radius * sin(h);
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
	
	m_cameraOffset[0].setValueImmediately(0.0f);
	m_cameraOffset[1].setValueImmediately(0.0f);
	m_cameraOffset[2].setValueImmediately(0.0f);
	m_eyeOffset[0].setValueImmediately(0.0f);
	m_eyeOffset[1].setValueImmediately(0.0f);
	m_eyeOffset[2].setValueImmediately(0.0f);
	m_cursorPos[0] = 0.0f;
	m_cursorPos[1] = 0.0f;
	m_cursorPos[2] = 0.0f;
	
	setCursorDirection(1);
	
	m_extraDimensions->resetView();
	
	m_activePlane = 2;
	
	if (m_stringMode)
		toggleStringMode();
}

void GLView::resetEye()
{
	setEye(m_zoomLevel, 30.0f, 0.0f);
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

void GLView::ipCreated(int index, Interpreter::InstructionPointer* ip)
{
	m_ips.insert(index, ip);
	followIp(ip);
	
	ip->m_particleGroup = m_P.GenParticleGroups(1, 200);
}

void GLView::ipDestroyed(Interpreter::InstructionPointer* ip)
{
	pVec ipVec(ip->m_pos[0] * FONT_SIZE + FONT_SIZE/2,
	               - ip->m_pos[1] * FONT_SIZE + FONT_SIZE/2,
	               - ip->m_pos[2] * FONT_SIZE);
	
	m_P.CurrentGroup(m_explosionsPG);
	m_P.Color(colorToVector(ip->m_color));
	m_P.Velocity(PDSphere(pVec(0), 10.0f, 5.0f));
	m_P.Source(300, PDSphere(ipVec, FONT_SIZE/2));
	
	m_P.DeleteParticleGroups(ip->m_particleGroup);
	
	int index = m_ips.indexOf(ip);
	m_ips.removeAt(index);
	if ((index > m_ips.count()) || (m_ips.isEmpty()))
		followIp(NULL);
	else if (index == m_ips.count())
		followIp(m_ips.first());
	else
		followIp(m_ips[index]);
}

void GLView::followIp(Interpreter::InstructionPointer* ip)
{
	m_followingIP = ip;
}

bool GLView::focusNextPrevChild(bool next)
{
	Q_UNUSED(next);
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
	m_cursor[m_extraDimensions->ascensionLevel()*3 + 0] += x;
	m_cursor[m_extraDimensions->ascensionLevel()*3 + 1] += y;
	m_cursor[m_extraDimensions->ascensionLevel()*3 + 2] += z;
	
	if (m_extraDimensions->ascensionLevel() != 0)
		m_extraDimensions->move(x, y, z);
	
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
	
	// There's probably a nicer way to do this with boost limits...
	FungeSpace::CodeByClever& code(m_fungeSpace->codeByClever());
	FungeSpace::CodeByClever::iterator i(code.begin());
	ChangeList changes;
	while (i != code.end())
	{
		Coord c(i->coord);
		QChar value(i->data);
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
		i++;
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

void GLView::slotPasteTransparent()
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

void GLView::setAscensionLevel(int level)
{
	if (level == m_extraDimensions->ascensionLevel())
		return;

	m_sound->play();
	m_enableWhoosh = true;

	m_extraDimensions->setAscensionLevel(level);
}

void GLView::ipChanged(Interpreter::InstructionPointer* ip)
{
	if (ip == m_followingIP)
	{
		if (Coord() != ip->m_direction.mid(3))
		{
			setAscensionLevel(1);
			m_extraDimensions->move(ip->m_pos);
		}
		else
			setAscensionLevel(0);
	}
}

Coord GLView::cursor()
{
	if ((!m_execution) || (m_followingIP == NULL))
		return m_cursor;
	else
		return m_followingIP->m_pos;
}

void GLView::drawWhoosh()
{
	renderText(width() - m_offsetWhoosh*(width()/20), 4*height()/5, "Whoooossshhh!", m_fontWhoosh, 14000);

	if(++m_offsetWhoosh > 24)
	{
		m_offsetWhoosh = 5;
		m_enableWhoosh = false;
	}
}

pVec GLView::colorToVector(const QColor& color)
{
	return pVec(((float)color.red()) / 255.0,
	            ((float)color.green()) / 255.0,
	            ((float)color.blue()) / 255.0);
}


void GLView::computeParticles(const Coord& point, int direction, const QColor& color)
{
	Coord translatedPoint = m_extraDimensions->nDTo3D(point);
	pVec cursorVec(translatedPoint[0] * FONT_SIZE + FONT_SIZE/2,
	               - translatedPoint[1] * FONT_SIZE + FONT_SIZE/2,
	               - translatedPoint[2] * FONT_SIZE);
	
	int absDir = abs(direction);
	pVec directionVec(absDir == 1 ? 0.1f : 0.0f,
	                     absDir == 2 ? -0.1f : 0.0f,
	                     absDir == 3 ? -0.1f : 0.0f);
	if (direction > 0)
		directionVec *= -1;
	
	m_P.Velocity(pVec(0));
	m_P.Color(colorToVector(color));
	m_P.Source(1, PDSphere(cursorVec, FONT_SIZE/2));
	m_P.RandomAccel(PDCone(pVec(0.0f, 0.0f, 0.0f), directionVec * 0.02, 0.01f));
	m_P.TargetAlpha(0.0f, 0.02f);
	m_P.KillOld(100.0f);
	m_P.Move(true, false);
}

void GLView::drawParticles()
{
	size_t cnt = m_P.GetGroupCount();
	if(cnt < 1) return;
	
	float *ptr;
	size_t flstride, pos3Ofs, posB3Ofs, size3Ofs, vel3Ofs, velB3Ofs, color3Ofs, alpha1Ofs, age1Ofs;
	
	cnt = m_P.GetParticlePointer(ptr, flstride, pos3Ofs, posB3Ofs,
		size3Ofs, vel3Ofs, velB3Ofs, color3Ofs, alpha1Ofs, age1Ofs);
	if(cnt < 1) return;
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, int(flstride) * sizeof(float), ptr + color3Ofs);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, int(flstride) * sizeof(float), ptr + pos3Ofs);
	
	glDrawArrays(GL_POINTS, 0, (GLsizei)cnt);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void GLView::blurPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target)
{
	glViewport(0, 0, target->size());
	target->bind();
	
	shader->bind();
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, source->texture());
	glUniform1i(shader->uniformLocation("source"), 0);
	glUniform2f(shader->uniformLocation("pixelStep"), 1.0 / target->width(), 1.0 / target->height());
	drawQuad(1.0, 1.0);
	glDisable(GL_TEXTURE_2D);
	
	target->release();
}

void GLView::drawQuad(float width, float height)
{
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, 0.0, height);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
		glVertex2f(-1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, width, height);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
		glVertex2f(1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, width, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
		glVertex2f(1.0, -1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
		glVertex2f(-1.0, -1.0);
	glEnd();
}

void glViewport(const QRect& rect)
{
	glViewport(rect.x(), rect.y(), rect.width(), rect.height());
}

void glViewport(const QPoint& pos, const QSize& size)
{
	glViewport(pos.x(), pos.y(), size.width(), size.height());
}

void glViewport(int x, int y, const QSize& size)
{
	glViewport(x, y, size.width(), size.height());
}


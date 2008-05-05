#ifndef GLVIEW_H
#define GLVIEW_H

#include "opengl.h"

#include <QGLWidget>
#include <QTime>
#include <QFont>
#include <QTextCursor>
#include <QUndoGroup>
#include <QClipboard>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <pAPI.h>
#include <Actions.h>

using namespace PAPI;

class QMouseEvent;
class QKeyEvent;
class QGLFramebufferObject;
class GLFont;

class Shader;
class ExtraDimensions;

#include "fungespace.h"
#include "fungecommand.h"
#include "interpreter.h"
#include "sound.h"

// TODO: Put these in a utils class or namespace
void glViewport(const QRect& rect);
void glViewport(const QPoint& pos, const QSize& size);
void glViewport(int x, int y, const QSize& size);

class GLView : public QGLWidget
{
	Q_OBJECT
	
public:
	GLView(FungeSpace* fungeSpace, const QGLFormat& format, QWidget* parent = 0);
	~GLView();
	
	int cursorDirection();
	
	void setFungeSpace(FungeSpace* funge);
	FungeSpace* getFungeSpace() { return m_fungeSpace; }
	
	void setExecution(bool execution);
	void followIp(Interpreter::InstructionPointer* ip);
	void resetView();
	void displayChanges(bool displayChanges) { m_displayChanges = displayChanges; }
	Coord textCursorPos() { return m_cursor; }

	QUndoGroup* getUndo() { return &m_undoGroup; }
	
	void setCameraMoveSpeed(float cameraMoveSpeed) { m_cameraMoveSpeed = cameraMoveSpeed; }
	void setEye(float radius, float vert, float horiz);
	void resetEye();

public slots:
	void setStringMode(bool enabled);
	void updateFPSCounter();
	
	void ipCreated(int index, Interpreter::InstructionPointer* ip);
	void ipChanged(Interpreter::InstructionPointer* ip);
	void ipDestroyed(Interpreter::InstructionPointer* ip);
	void clearIps() {m_ips.clear();}
	
	void slotCopy();
	void slotCut();
	void slotPaste();
	void slotPasteTransparent();
	
	void setBloom(bool bloom) { m_bloom = bloom; }

private slots:
	void spaceDeleted(QObject* space);
	
signals:
	void stringModeChanged(bool enabled);
	void cursorDirectionChanged(int direction);
	void copyChangeToCodeFungeSpace(Coord c);
	
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
	
	void recreateFbos();
	int nextPowerOf2(int n);
	void updateParticles();
	
	void updateCamera(int i);
	float degreesToRadians(float degrees);
	float modulo(float value, float mod);
	Coord pointToFungeSpace(const QPoint& pos);
	Coord glToFungeSpace(float x, float y, float z);
	QList<float> fungeSpaceToGl(Coord c, bool premultiplied);
	void toggleStringMode();
	void setCursorDirection(int direction);
	void setActivePlane(int plane);
	int otherPlane();
	Coord selectionTopLeft();
	Coord selectionBottomRight();
	void moveCursor(int x, int y, int z, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
	void moveCursor(int direction, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
	void setCursor(int x, int y, int z, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
	void setCursor(Coord c, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
	void drawCube(Coord startPos, Coord endPos);
	void selectionToClipboard(bool cut, QClipboard::Mode mode = QClipboard::Clipboard);
	void paste(bool transparant);
	void clearRect(Coord topLeft, Coord bottomRight, ChangeList* changes);
	void drawFunge(QHash<Coord, QChar> fungeCode);
	void setAscensionLevel(int level);
	Coord cursor();
	
	void setupCamera();
	void setupMatrices();
	void drawScene();
	void drawBrightParts();
	void drawCursor();
	void drawInstructionPointers();
	void drawSelectionCube();
	void drawAnnotations();
	void drawExplosionParticles();
	void drawDepthBoxes();
	
	void blurPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target);
	void drawQuad(float width, float height);
	
	void computeParticles(const Coord& point, int direction, const QColor& color);
	void drawParticles();
	pVec colorToVector(const QColor& color);

	void setChar(Coord p, QChar newchar);

	void drawWhoosh();

private:
	Coord m_origin;
	
	// Funge space
	FungeSpace* m_fungeSpace;
	QUndoGroup m_undoGroup;
	QMap<FungeSpace*, QUndoStack*> m_undos;
	
	// Debugger
	bool m_execution;
	QList<Interpreter::InstructionPointer*> m_ips;
	Interpreter::InstructionPointer* m_followingIP;
	QString m_executionStr;
	QString m_execution2Str;
	QRect m_executionRect;
	QRect m_execution2Rect;
	bool m_displayChanges;
	
	// Cursor
	Coord m_cursor;
	QTime m_cursorBlinkTime;
	bool m_cursorBlinkOn;
	int m_cursorDirection;
	int m_activePlane;
	float m_actualCursorPos[3];
	
	bool m_mouseHoveringOverChar;
	Coord m_mouseHover;
	
	Coord m_selectionAnchor;
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
	float m_cameraMoveSpeed;
	QPoint m_preDragMousePosition;
	
	// Extra dimensions
	ExtraDimensions* m_extraDimensions;
	
	// Fonts
	GLFont* m_font;
	QFont m_fontLarge;
	QFont m_fontSmall;
	QFontMetrics* m_metricsSmall;
	
	// Display lists
	uint m_displayListsBase;
	enum DisplayListEntry
	{
		ARROW = 0,
		CURSOR = 1,
		GRID = 2
	};
	
	// Particle system
	ParticleContext_t m_P;
	int m_cursorPG;
	int m_explosionsPG;

	Sound* m_sound;

	bool m_enableWhoosh;
	QFont m_fontWhoosh;
	int m_offsetWhoosh;
	
	static QList<Shader*> s_ppShaders;
	QGLFramebufferObject* m_sceneFbo;
	QList<QGLFramebufferObject*> m_blurTargets;
	
	bool m_bloom;
};

#endif

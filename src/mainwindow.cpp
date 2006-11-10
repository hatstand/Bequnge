#include "mainwindow.h"
#include "glview.h"
#include "ui_mainwindow.h"
#include "stringmodewidget.h"
#include "interpreter.h"

#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	  m_interpreter(NULL)
{
	m_ui.setupUi(this);
	
	// Setup the GL view
	m_glView = new GLView(this);
	setCentralWidget(m_glView);
	m_glView->setFocus(Qt::OtherFocusReason);
	connect(m_glView, SIGNAL(cursorDirectionChanged(int)), SLOT(cursorDirectionChanged(int)));
	
	// Cursor direction statusbar label
	m_cursorDirectionLabel = new QLabel(this);
	m_cursorDirectionLabel->setToolTip("Use alt+arrowkey to change the cursor direction");
	statusBar()->addPermanentWidget(m_cursorDirectionLabel);
	cursorDirectionChanged(m_glView->cursorDirection());
	
	// Setup the String Mode statusbar widget
	StringModeWidget* stringMode = new StringModeWidget(this);
	statusBar()->addPermanentWidget(stringMode);
	connect(stringMode, SIGNAL(stringModeChanged(bool)), m_glView, SLOT(setStringMode(bool)));
	connect(m_glView, SIGNAL(stringModeChanged(bool)), stringMode, SLOT(setStringMode(bool)));
	
	// Hide the docks by default
	m_ui.stackDock->hide();
	m_ui.consoleDock->hide();
	
	// Connect actions
	connect(m_ui.actionOpen, SIGNAL(triggered(bool)), SLOT(slotOpen()));
	connect(m_ui.actionNew, SIGNAL(triggered(bool)), SLOT(slotNew()));
}

void MainWindow::cursorDirectionChanged(int direction)
{
	QString text("Cursor direction: ");
	text += (direction < 0) ? "negative " : "positive ";
	
	switch (abs(direction))
	{
		case 1: text += "x"; break;
		case 2: text += "y"; break;
		case 3: text += "z"; break;
	}
	
	m_cursorDirectionLabel->setText(text);
}

void MainWindow::slotOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open file", QDir::home().path(), "BeQunge source (*.beq)");
	if (fileName.isNull())
		return;
	
	delete m_interpreter;
	QFile file(fileName);
	m_interpreter = new Interpreter(&file, this);
	
	FungeSpace* space = m_interpreter->parse();
	if (space->dimensions() < 3)
		space->setDimensions(3);
	
	m_glView->setFungeSpace(space);
	m_glView->resetView();
}

void MainWindow::slotNew()
{
	m_glView->setFungeSpace(new FungeSpace(3));
	m_glView->resetView();
}


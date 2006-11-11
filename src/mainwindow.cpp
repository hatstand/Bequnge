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
	  m_interpreter(NULL),
	  m_executionFungeSpace(NULL)
{
	m_ui.setupUi(this);
	
	// Create empty funge space
	m_fungeSpace = new FungeSpace(3);
	
	// Setup the GL view
	m_glView = new GLView(m_fungeSpace, this);
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
	connect(m_ui.actionDebug, SIGNAL(triggered(bool)), SLOT(slotDebug()));
	connect(m_ui.actionStep, SIGNAL(triggered(bool)), SLOT(slotStep()));
	
	// Setup the stack list
	m_stackModel = new QStandardItemModel(this);
	m_ui.stackList->setModel(m_stackModel);
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
	
	QFile file(fileName);
	
	FungeSpace* space = new FungeSpace(&file);
	if (space->dimensions() < 3)
		space->setDimensions(3);
	
	delete m_fungeSpace;
	m_fungeSpace = space;
	
	m_glView->setFungeSpace(m_fungeSpace);
	m_glView->resetView();
	
	delete m_interpreter;
	m_interpreter = NULL;
}

void MainWindow::slotNew()
{
	delete m_fungeSpace;
	m_fungeSpace = new FungeSpace(3);
	
	m_glView->setFungeSpace(m_fungeSpace);
	m_glView->resetView();
	
	delete m_interpreter;
	m_interpreter = NULL;
}

void MainWindow::slotDebug()
{
	// Setup UI
	m_ui.stackDock->show();
	m_ui.consoleDock->show();
	
	// Copy the funge space
	delete m_executionFungeSpace;
	m_executionFungeSpace = new FungeSpace(m_fungeSpace);
	
	// Make an interpreter
	delete m_interpreter;
	m_interpreter = new Interpreter(m_executionFungeSpace, this);
	connect(m_interpreter, SIGNAL(pcChanged(Coord, Coord)), SLOT(slotPcChanged(Coord, Coord)));
	connect(m_interpreter, SIGNAL(stackPushed(int)), SLOT(slotStackPushed(int)), Qt::DirectConnection);
	connect(m_interpreter, SIGNAL(stackPopped()), SLOT(slotStackPopped()), Qt::DirectConnection);
}

void MainWindow::slotStep()
{
	if (m_interpreter == NULL)
		slotDebug();
	
	m_interpreter->step();
}

void MainWindow::slotPcChanged(Coord position, Coord direction)
{
	qDebug() << position << direction;
}

void MainWindow::slotStackPushed(int value)
{
	QList<QStandardItem*> items;
	items.append(new QStandardItem(QString::number(value) + " \"" + QChar(value) + "\""));
	
	m_stackModel->insertRow(0, items);
}

void MainWindow::slotStackPopped()
{
	m_stackModel->removeRow(0);
}


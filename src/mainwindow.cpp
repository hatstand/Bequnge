#include "mainwindow.h"
#include "glview.h"
#include "ui_mainwindow.h"
#include "stringmodewidget.h"
#include "interpreter.h"

#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	  m_interpreter(NULL),
	  m_executionFungeSpace(NULL),
	  m_settings("BeQunge", "BeQunge", this)
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
	m_ui.debuggerDock->hide();
	
	m_ui.displayFungeSpace->setEnabled(false);

	// Connect actions
	connect(m_ui.actionOpen, SIGNAL(triggered(bool)), SLOT(slotOpen()));
	connect(m_ui.actionNew, SIGNAL(triggered(bool)), SLOT(slotNew()));
	connect(m_ui.actionDebug, SIGNAL(triggered(bool)), SLOT(slotDebug()));
	connect(m_ui.actionStep, SIGNAL(triggered(bool)), SLOT(slotStep()));
	connect(m_ui.actionStop, SIGNAL(triggered(bool)), SLOT(slotStop()));
	connect(m_ui.speedSlider, SIGNAL(sliderMoved(int)), SLOT(speedSliderMoved(int)));
	connect(m_ui.actionSave, SIGNAL(triggered(bool)), SLOT(saveFile()));
	connect(m_ui.displayFungeSpace, SIGNAL(activated(int)), SLOT(slotDisplayFungeSpaceChanged(int)));
	
	// Setup the stack list
	m_stackModel = new QStandardItemModel(this);
	m_ui.stackList->setModel(m_stackModel);
	
	// Setup the auto-step timer
	m_autoStepTimer = new QTimer(this);
	connect(m_autoStepTimer, SIGNAL(timeout()), SLOT(slotStep()));

	if(!m_settings.contains("filedir"))
	{
		m_settings.setValue("filedir", QDir::home().absolutePath());
		m_settings.sync();
	}
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
	QString fileName = QFileDialog::getOpenFileName(this, "Open file", m_settings.value("filedir").toString(), "BeQunge source (*.beq)");
	if (fileName.isNull())
		return;
	
	QFile file(fileName);
	QDir dir(fileName);
	m_settings.setValue("filedir", dir.absolutePath());
	
	FungeSpace* space = new FungeSpace(&file);
	if (space->dimensions() < 3)
		space->setDimensions(3);
	
	delete m_fungeSpace;
	m_fungeSpace = space;
	
	showExecutionSpace(false);
	m_glView->resetView();
	
	delete m_interpreter;
	m_interpreter = NULL;
	m_stackModel->clear();
}

void MainWindow::slotNew()
{
	delete m_fungeSpace;
	m_fungeSpace = new FungeSpace(3);
	
	showExecutionSpace(false);
	m_glView->resetView();
	
	delete m_interpreter;
	m_interpreter = NULL;
	m_stackModel->clear();
}

void MainWindow::slotDebug()
{
	// Setup UI
	m_ui.stackDock->show();
	m_ui.consoleDock->show();
	m_ui.debuggerDock->show();
	m_ui.displayFungeSpace->setEnabled(true);
	
	// Copy the funge space
	delete m_executionFungeSpace;
	m_executionFungeSpace = new FungeSpace(m_fungeSpace);
	m_stackModel->clear();
	
	// Make an interpreter
	delete m_interpreter;
	m_interpreter = new Interpreter(m_executionFungeSpace, this);
	connect(m_interpreter, SIGNAL(pcChanged(Coord, Coord)), SLOT(slotPcChanged(Coord, Coord)));
	connect(m_interpreter, SIGNAL(stackPushed(int)), SLOT(slotStackPushed(int)), Qt::DirectConnection);
	connect(m_interpreter, SIGNAL(stackPopped()), SLOT(slotStackPopped()), Qt::DirectConnection);
	connect(m_interpreter, SIGNAL(output(QChar)), SLOT(slotOutput(QChar)));
	connect(m_interpreter, SIGNAL(output(QString)), SLOT(slotOutput(QString)));
	
	showExecutionSpace(true);
	m_glView->followPC(0);
	m_glView->setPC(0, m_interpreter->pcPosition(0), m_interpreter->pcDirection(0));
	
	speedSliderMoved(m_ui.speedSlider->value());
}

void MainWindow::slotStep()
{
	if (m_interpreter == NULL)
		slotDebug();
	
	if (m_interpreter->step() != Interpreter::Success)
		slotStop();
}

void MainWindow::slotPcChanged(Coord position, Coord direction)
{
	m_glView->setPC(0, position, direction);
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

void MainWindow::showExecutionSpace(bool execution)
{
	if ((m_executionFungeSpace == NULL) && (execution))
		return;
	m_glView->setFungeSpace(execution ? m_executionFungeSpace : m_fungeSpace);
	m_glView->setExecution(execution);
	m_ui.displayFungeSpace->setCurrentIndex(execution ? 1 : 0);
}

void MainWindow::slotDisplayFungeSpaceChanged(int index)
{
	showExecutionSpace(index == 1);
}

void MainWindow::slotStop()
{
	delete m_interpreter;
	m_interpreter = NULL;
	m_stackModel->clear();
	showExecutionSpace(false);
	
	m_autoStepTimer->stop();
}

void MainWindow::slotOutput(QChar c)
{
	slotOutput(QString(c));
}

void MainWindow::slotOutput(QString str)
{
	m_ui.consoleBox->setPlainText(m_ui.consoleBox->toPlainText() + str);
}

void MainWindow::speedSliderMoved(int value)
{
	if (value == 0)
		m_autoStepTimer->stop();
	else if (!m_autoStepTimer->isActive())
		m_autoStepTimer->start(1000 / value);
	else
		m_autoStepTimer->setInterval(1000 / value);
}

void MainWindow::saveFile()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save File...", m_settings.value("filedir").toString(), "BeQunge source (*.beq)");
	
	QDir dir(filename);
	m_settings.setValue("filedir", dir.absolutePath());

	m_fungeSpace->save(filename);
}




#include "mainwindow.h"
#include "glview.h"
#include "ui_mainwindow.h"
#include "stringmodewidget.h"

#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
	
	// Cursor direction statusbar label
	m_cursorDirectionLabel = new QLabel(this);
	statusBar()->addPermanentWidget(m_cursorDirectionLabel);
	
	// Setup the GL view
	m_glView = new GLView(this);
	setCentralWidget(m_glView);
	m_glView->setFocus(Qt::OtherFocusReason);
	connect(m_glView, SIGNAL(cursorDirectionChanged(int)), SLOT(cursorDirectionChanged(int)));
	cursorDirectionChanged(m_glView->cursorDirection());
	
	// Setup the String Mode statusbar widget
	StringModeWidget* stringMode = new StringModeWidget(this);
	statusBar()->addPermanentWidget(stringMode);
	connect(stringMode, SIGNAL(stringModeChanged(bool)), m_glView, SLOT(setStringMode(bool)));
	connect(m_glView, SIGNAL(stringModeChanged(bool)), stringMode, SLOT(setStringMode(bool)));
	
	// Hide the docks by default
	m_ui.stackDock->hide();
	m_ui.consoleDock->hide();
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


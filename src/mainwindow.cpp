#include "mainwindow.h"
#include "glview.h"
#include "ui_mainwindow.h"
#include "stringmodewidget.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
	
	// Setup the GL view
	m_glView = new GLView(this);
	setCentralWidget(m_glView);
	m_glView->setFocus(Qt::OtherFocusReason);
	
	// Setup the String Mode statusbar widget
	StringModeWidget* stringMode = new StringModeWidget(this);
	statusBar()->addPermanentWidget(stringMode);
	connect(stringMode, SIGNAL(stringModeChanged(bool)), m_glView, SLOT(setStringMode(bool)));
	connect(m_glView, SIGNAL(stringModeChanged(bool)), stringMode, SLOT(setStringMode(bool)));
	
	// Hide the docks by default
	m_ui.stackDock->hide();
	m_ui.consoleDock->hide();
}


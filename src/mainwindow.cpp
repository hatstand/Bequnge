#include "mainwindow.h"
#include "glview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
	
	m_glView = new GLView(this);
	setCentralWidget(m_glView);
	
	m_glView->setFocus(Qt::OtherFocusReason);
}


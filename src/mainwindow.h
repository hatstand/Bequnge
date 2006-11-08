#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

class GLView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);

private slots:

private:
	Ui_MainWindow m_ui;
	
	GLView* m_glView;
};


#endif


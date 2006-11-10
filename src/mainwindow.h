#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

class GLView;
class QLabel;
class Interpreter;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);

private slots:
	void cursorDirectionChanged(int direction);
	
	void slotOpen();
	void slotNew();

private:
	Ui_MainWindow m_ui;
	
	GLView* m_glView;
	QLabel* m_cursorDirectionLabel;
	
	Interpreter* m_interpreter;
};


#endif


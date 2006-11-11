#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "ui_mainwindow.h"
#include "fungespace.h"

class GLView;
class QLabel;
class Interpreter;
class FungeSpace;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);

private slots:
	void cursorDirectionChanged(int direction);
	
	void slotOpen();
	void slotNew();
	void slotDebug();
	void slotStep();
	
	void slotPcChanged(Coord position, Coord direction);
	void slotStackPushed(int value);
	void slotStackPopped();

private:
	Ui_MainWindow m_ui;
	
	GLView* m_glView;
	QLabel* m_cursorDirectionLabel;
	
	FungeSpace* m_fungeSpace;
	FungeSpace* m_executionFungeSpace;
	Interpreter* m_interpreter;
	
	QStandardItemModel* m_stackModel;
};


#endif


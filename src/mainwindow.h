#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>

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
	
private:
	void showExecutionSpace(bool execution);

private slots:
	void cursorDirectionChanged(int direction);
	
	void slotOpen();
	void slotNew();
	void slotDebug();
	void slotStep();
	void slotStop();
	
	void slotPcChanged(Coord position, Coord direction);
	void slotStackPushed(int value);
	void slotStackPopped();
	void slotOutput(QChar c);
	void slotOutput(QString str);
	
	void speedSliderMoved(int value);
	void slotDisplayFungeSpaceChanged(int index);

private:
	Ui_MainWindow m_ui;
	
	GLView* m_glView;
	QLabel* m_cursorDirectionLabel;
	
	FungeSpace* m_fungeSpace;
	FungeSpace* m_executionFungeSpace;
	QTimer* m_autoStepTimer;
	Interpreter* m_interpreter;
	
	QStandardItemModel* m_stackModel;
};


#endif


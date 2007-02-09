#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QSettings>

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
	~MainWindow();
	
private:
	void showExecutionSpace(bool execution);

private slots:
	void cursorDirectionChanged(int direction);
	void slotCopyChangeToCodeFungeSpace(Coord c);
	
	void slotOpen();
	void slotNew();
	void slotDebug();
	void slotStep();
	void slotStop();
	void slotSave();
	void slotAbout();
	void slotAboutQt();
	
	void slotToggleBreakpoint();
	void slotToggleWatchpoint();
	void slotClearAllBreakpoints();
	void slotClearAllWatchpoints();
	
	void slotIpCreated(int index, Interpreter::InstructionPointer* ip);
	void slotIpChanged(Interpreter::InstructionPointer* ip);
	void slotIpDestroyed(Interpreter::InstructionPointer* ip);
	
	void slotOutput(QChar c);
	void slotOutput(QString str);
	
	void provideInput(QChar c);
	void provideInput(int i);
	void doFullSpeedExecution();
	void slotWatchpointTriggered(Coord, QChar);
	
	void speedSliderMoved(int value);
	void pauseButtonClicked(bool checked);
	void fullSpeedButtonClicked(bool checked);
	void slotDisplayFungeSpaceChanged(int index);

private:
	Ui_MainWindow m_ui;
	
	GLView* m_glView;
	QLabel* m_cursorDirectionLabel;
	
	FungeSpace* m_fungeSpace;
	FungeSpace* m_executionFungeSpace;
	QTimer* m_autoStepTimer;
	Interpreter* m_interpreter;
	bool m_fullSpeedExecution;

	QSettings m_settings;
};


#endif


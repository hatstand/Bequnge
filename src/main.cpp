#include <QApplication>

#include <QFile>
#include <QDebug>
#include <QDateTime>

#include "console.h"
#include "mainwindow.h"
#include "interpreter.h"
#include "fungespace.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	qsrand(QDateTime::currentDateTime().toTime_t());

	app.setApplicationName("Bequnge");

	if (app.arguments().count() < 2 || app.arguments().at(1) == "--gui")
	{
		MainWindow* win = new MainWindow(NULL);
		win->showMaximized();

		return app.exec();
	}

 	QFile* f = new QFile(argv[1]);
 	FungeSpace s(f);
	StackStackCollectionModel m(0);
 	Interpreter i(&m, &s, 0);
	Console console(&i);

	Interpreter::Status status;
	do
	{
 		status = i.stepAll();
	} while (status != Interpreter::End);
 	s.save("temp");
}

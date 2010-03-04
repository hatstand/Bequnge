#include <QApplication>

#include <QFile>
#include <QFileInfo>
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
	Q_INIT_RESOURCE(commonshaders);
	app.setApplicationName("Bequnge");

	if (app.arguments().count() < 2 || app.arguments().at(1) == "--gui")
	{
		MainWindow* win = new MainWindow(NULL);
		win->show();

		return app.exec();
	}

 	QFile* f = new QFile(argv[1]);
	QFileInfo info(*f);
	FungeSpace::SourceType type = FungeSpace::Bequnge;
	if (info.suffix() == "b98") {
		type = FungeSpace::Befunge98;
	} else if (info.suffix() == "bf") {
		type = FungeSpace::Befunge93;
	}

 	FungeSpace s(f, type);
	StackStackCollectionModel m(0);
 	Interpreter i(&m, &s, 0);
	Console console(&i);

	QList<Interpreter::Status> status;
	do
	{
 		status = i.stepAll();
	} while (status.count(Interpreter::End) != status.size());
 	s.save("temp");
}

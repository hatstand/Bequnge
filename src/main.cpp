#include <QApplication>

#include <QFile>
#include <QDebug>

#include "mainwindow.h"
#include "interpreter.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	if ((app.arguments().count() > 1) && (app.arguments().at(1) == "--gui"))
	{
		MainWindow* win = new MainWindow(NULL);
		win->show();

		return app.exec();
	}

	QFile* f = new QFile(argv[1]);
	Interpreter i(f);
	i.parse();
	i.run();
}

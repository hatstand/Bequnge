#include <QApplication>

#include "interpreter.h"
#include <QFile>
#include <QDebug>

int main(int argc, char** argv)
{
//	QApplication app(argc, argv);

//	return app.exec();

	QFile* f = new QFile(argv[1]);
	Interpreter i(f);
	i.parse();
	i.run();
}

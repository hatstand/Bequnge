#ifndef ARRAY_H
#define ARRAY_H

#include <QChar>
#include <QVector>
#include <QString>

class Array
{
private:
	QChar contents;
	QVector<Array*> vector;
	Array* parent;

public:
	Array();
	Array(uint length);
	Array(QChar c);
	Array(QVector<Array*> v);
	Array(Array* a);
	~Array();

	Array& operator[](uint x);
	Array operator[](uint x) const;

	QString toString();

	void append(Array* a);
	void append(QChar c);

	bool isChar();
	QChar getContents();
	Array* getNext();
	Array* getParent();
};

#endif

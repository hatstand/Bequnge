#include "array.h"

#include <QDebug>

Array::Array()
{
	contents = '\0';
	parent = NULL;
}

Array::~Array()
{
	if(!contents.isNull())
	{
		foreach(Array* a, vector)
		{
			delete(a);
		}
	}
}

Array::Array(uint length)
{
	vector = QVector<Array*>(length);
	parent = NULL;
}

Array::Array(QChar c)
{
	contents = c;
	parent = NULL;
}

Array::Array(QVector<Array*> v)
{
	vector = v;
	contents = '\0';
	parent = NULL;
}

Array::Array(Array* a)
{
	contents = '\0';
	vector.append(a);
	a->parent = this;
	parent = NULL;
}

Array& Array::operator[](uint x)
{
	return *(vector[x]);
}

Array Array::operator[](uint x) const
{
	return *(vector[x]);
}

void Array::append(Array* a)
{
	if(!contents.isNull())
	{
		vector.append(new Array(contents));
		vector.last()->parent = this;
		contents = '\0';
	}
	
	a->parent = this;
	vector.append(a);
}

void Array::append(QChar c)
{
	if(!contents.isNull())
	{
		vector.append(new Array(contents));
		vector.last()->parent = this;
		contents = '\0';
	}
	
	vector.append(new Array(c));
}

QString Array::toString()
{
	if(!contents.isNull())
		return QString(contents);
	else
	{
		QString ret = "(";
		foreach(Array* a, vector)
		{
			ret += a->toString();
		}
		ret += ")";

		return ret;
	}
}

bool Array::isChar()
{
	if(contents.isNull())
		return false;
	else
		return true;
}

QChar Array::getContents()
{
	return contents;
}

Array* Array::getNext()
{
	if(contents.isNull())
		return vector.first()->getNext();
	else
		return this;
}

Array* Array::getParent()
{
	return parent;
}

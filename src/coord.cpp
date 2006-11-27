#include "coord.h"

uint qHash(Coord c)
{
	uint hash = 0;

	if(c.isEmpty())
		return hash;

	foreach(int i, c)
	{
		hash ^= i;
	}
	
	return hash;
}


Coord::Coord()
	: QVector<int>(),
	  zero(0)
{
}

Coord::Coord(const QVector<int>& other)
	: QVector<int>(other),
	  zero(0)
{
}

Coord Coord::operator =(const QVector<int>& other)
{
	return Coord(other);
}

const int& Coord::at(int i) const
{
	if (i >= count())
		return zero;
	return QVector<int>::at(i);
}

int& Coord::operator [](int i)
{
	while (i >= count())
		append(0);
	return QVector<int>::operator[](i);
}

const int& Coord::operator [](int i) const
{
	return at(i);
}

bool Coord::operator ==(const QVector<int>& other) const
{
	if (count() == other.count())
		return QVector<int>::operator ==(other);
	
	if (count() > other.count())
	{
		if (mid(0, other.count()) != other)
			return false;
		for (int i=other.count() ; i<count() ; ++i)
			if (at(i) != 0)
				return false;
	}
	else
	{
		if (QVector<int>::operator !=(other.mid(0, count())))
			return false;
		for (int i=count() ; i<other.count() ; ++i)
			if (other.at(i) != 0)
				return false;
	}
	return true;
}

bool Coord::operator ==(const Coord& other) const
{
	return operator ==((const QVector<int>&) other);
}

bool Coord::operator !=(const QVector<int>& other) const
{
	return !(operator ==((const QVector<int>&) other));
}

bool Coord::operator !=(const Coord& other) const
{
	return !(operator ==((const QVector<int>&) other));
}

QVector<int> Coord::mid(int pos, int length) const
{
	if (pos < count())
		return QVector<int>::mid(pos, length);
	return Coord();
}


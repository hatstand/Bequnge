#include "coord.h"

#include <boost/functional/hash.hpp>
#include <QtDebug>

uint qHash(const Coord& c)
{
	return hash_value(c);
}

std::size_t hash_value(Coord const& c)
{
	std::size_t seed = 0;
	for (int i = 0; i < c.size(); ++i)
	{
		if (c.at(i) != 0)
			boost::hash_combine(seed, c.at(i));
	}

	return seed;
}

int Coord::s_zero = 0;

Coord::Coord()
	: QList<int>()
{
}

Coord::Coord(const QList<int>& other)
	: QList<int>(other)
{
}

Coord Coord::operator =(const QList<int>& other)
{
	return Coord(other);
}

const int& Coord::at(int i) const
{
	if (i >= count())
		return s_zero;
	return QList<int>::at(i);
}

int& Coord::operator [](int i)
{
	while (i >= count())
		append(0);
	return QList<int>::operator[](i);
}

const int& Coord::operator [](int i) const
{
	return at(i);
}

bool Coord::operator ==(const QList<int>& other) const
{
	if (count() == other.count())
		return QList<int>::operator ==(other);
	
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
		if (QList<int>::operator !=(other.mid(0, count())))
			return false;
		for (int i=count() ; i<other.count() ; ++i)
			if (other.at(i) != 0)
				return false;
	}
	return true;
}

bool Coord::operator ==(const Coord& other) const
{
	return operator ==((const QList<int>&) other);
}

bool Coord::operator !=(const QList<int>& other) const
{
	return !(operator ==((const QList<int>&) other));
}

bool Coord::operator !=(const Coord& other) const
{
	return !(operator ==((const QList<int>&) other));
}

QList<int> Coord::mid(int pos, int length) const
{
	if (pos < count())
		return QList<int>::mid(pos, length);
	return Coord();
}

bool Coord::operator <(const Coord& other) const
{
	for (int i = qMax(size(), other.size()) - 1; i >= 0; --i)
	{
		if (at(i) != other.at(i))
			return at(i) < other.at(i);
	}

	return false;
}


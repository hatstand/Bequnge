#ifndef COORD_H
#define COORD_H

#include <QList>

class Coord : public QList<int>
{
public:
	Coord();
	Coord(const QList<int>& other);
	
	QList<int> mid(int pos, int length = -1) const;
	
	Coord operator =(const QList<int>& other);
	const int& at(int i) const;
	int& operator [](int i);
	const int& operator [](int i) const;
	bool operator ==(const QList<int>& other) const;
	bool operator ==(const Coord& other) const;
	bool operator !=(const QList<int>& other) const;
	bool operator !=(const Coord& other) const;

private:
	static int s_zero;
};

uint qHash(const Coord& c);
uint hash_value(const Coord& c);

#endif

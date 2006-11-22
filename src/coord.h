#ifndef COORD_H
#define COORD_H

#include <QVector>

class Coord : public QVector<int>
{
public:
	Coord();
	Coord(const QVector<int>& other);
	
	QVector<int> mid(int pos, int length = -1) const;
	
	Coord operator =(const QVector<int>& other);
	const int& at(int i) const;
	int& operator [](int i);
	const int& operator [](int i) const;
	bool operator ==(const QVector<int>& other) const;
	bool operator ==(const Coord& other) const;
	bool operator !=(const QVector<int>& other) const;
	bool operator !=(const Coord& other) const;
	
private:
	int zero;
};

uint qHash(Coord c);

#endif

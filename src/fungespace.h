#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QMap>
#include <QDebug>

#include <boost/array.hpp>

struct Element;
typedef QMap<int, Element*> Next;

template <int dimensions = 2>
class FungeSpace
{
	
public:
	// Represents a point in fungespace
	typedef boost::array<int, dimensions> Coord;

	FungeSpace();
	~FungeSpace();

	// Place a char in FungeSpace
	void setChar(Coord, QChar);
	// Get a char from a Coord in FungeSpace
	QChar getChar(Coord);

	// Get all the code back out from FungeSpace
	QMap<Coord, QChar> getCode();
	
	// Get the code edges (only correct if code doesn't shrink)
	int getPositiveEdge(int dimension){ return positiveEdges[dimension]; }
	int getNegativeEdge(int dimension) { return negativeEdges[dimension]; }

private:
	Next zeroth;
	int positiveEdges[dimensions];
	int negativeEdges[dimensions];

	void descend(Next n, Coord pos, int x, QMap<Coord, QChar>* ret);
};


// Either a final funge char or a QMap to some more Elements
struct Element
{
	QChar c;
	Next n;
	Element(QChar cp) { c = cp; }
	Element(Next np) { n = np; }

	~Element()
	{
		foreach(Element* e, n.values())
		{
			delete(e);
		}
	}
};

template <int dimensions>
FungeSpace<dimensions>::FungeSpace()
{
	for(int i = 0; i < dimensions; ++i)
	{
		positiveEdges[i] = 0;
		negativeEdges[i] = 0;
	}
}

template <int dimensions>
FungeSpace<dimensions>::~FungeSpace()
{
	foreach(Element* e, zeroth.values())
	{
		delete(e);
	}
}

template <int dimensions>
QMap<boost::array<int, dimensions>, QChar> FungeSpace<dimensions>::getCode()
{
	QMap<Coord, QChar> ret;

	Coord pos;
	for(int i = 0; i < dimensions; ++i)
		pos[i] = 0;

	// Go down through all the maps to the bottom ones with actual chars in
	descend(zeroth, pos, 0, &ret);

	foreach(QChar i, ret)
		qDebug() << i;

	return ret;
}

template <int dimensions>
void FungeSpace<dimensions>::descend(Next n, Coord pos, int x, QMap<Coord, QChar>* ret)
{
	// Move through positions
	for(Next::iterator it = n.begin(); it != n.end(); ++it)
	{
		// Set this dimension's coordinate
		pos[x] = it.key();

		// Reached the last dimension
		if(x == dimensions - 1)
		{
			qDebug() << "Inserting" << it.value()->c << "at:" << pos[0] << pos[1];
			ret->insert(pos, it.value()->c);
		}
		else
		{
			// Found a map, crawl it too into the next dimension (x+1)
			descend(it.value()->n, pos, x+1, ret);
		}
	}
}

template <int dimensions>
void FungeSpace<dimensions>::setChar(Coord pos, QChar c)
{
	// Dig through dimensions till we get to the bottom
	Next* t = &zeroth;
	for(int i = 0; i < dimensions-1; ++i)
	{
		if(!t->contains(pos[i]))
		{
			Element* e = new Element(Next());
			t->insert(pos[i], e);
			t = &(e->n);
		}
		else
		{
			t = &(t->value(pos[i])->n);
		}
	}

	// Put the char in the bottom dimension
	if(c != ' ' && c != '\n')
	{
		t->insert(pos[dimensions-1], new Element(c));
		for(int i = 0; i < dimensions; ++i)
		{
			positiveEdges[i] = qMax(positiveEdges[i], pos[i]);
			negativeEdges[i] = qMin(negativeEdges[i], pos[i]);
		}
	}
	else
		t->remove(pos[dimensions-1]);
}

template <int dimensions>
QChar FungeSpace<dimensions>::getChar(Coord pos)
{
	Next* t = &zeroth;
	// Loop dimensions
	// Check each coordinate in turn
	for(int i = 0; i < dimensions-1; ++i)
	{
		Next::iterator it = t->find(pos[i]);
		if(it != t->end())
		{
			t = &((*it)->n);
		}
		else
			return QChar(' ');
	}

	// Check the final dimension
	Next::iterator it = t->find(pos[dimensions-1]);
	if(it != t->end())
		return (*it)->c;
	else
		return QChar(' ');
}

#endif

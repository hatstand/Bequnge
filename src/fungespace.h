#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QMap>

struct Element;
typedef QMap<int, Element*> Next;

template <int dimensions = 2>
class FungeSpace
{
	
public:
	typedef int Coord[dimensions];

	FungeSpace();
	~FungeSpace();

	void setChar(Coord, QChar);
	QChar getChar(Coord);

private:
	Next zeroth;
};

struct Element
{
	QChar c;
	Next n;
	Element(QChar cp) { c = cp; }
	Element(Next np) { n = np; }
};

template <int dimensions>
FungeSpace<dimensions>::FungeSpace()
{
}

template <int dimensions>
FungeSpace<dimensions>::~FungeSpace()
{
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
	}
	else
		t->remove(pos[dimensions-1]);
		
}

template <int dimensions>
QChar FungeSpace<dimensions>::getChar(Coord pos)
{
	Next* t = &zeroth;
	for(int i = 0; i < dimensions-1; ++i)
	{
		Next::iterator it = t->find(pos[i]);
		if(it != t->end())
		{
			t = &((*it)->n);
		}
		else
			return QChar();
	}

	Next::iterator it = t->find(pos[dimensions-1]);
	if(it != t->end())
		return (*it)->c;
	else
		return QChar();
}

#endif

#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QMap>
#include <QDebug>

#include <boost/array.hpp>

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
	int positiveEdges[dimensions];
	int negativeEdges[dimensions];

	QMap<Coord, QChar> m_space;
	//QHash<Coord, QChar> m_space;
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
}

template <int dimensions>
QMap<boost::array<int, dimensions>, QChar> FungeSpace<dimensions>::getCode()
{
	return m_space;
}

template <int dimensions>
void FungeSpace<dimensions>::setChar(Coord pos, QChar c)
{
	if(c != ' ' && c != '\n')
	{
		m_space.insert(pos, c);

		for(int i = 0; i < dimensions; ++i)
		{
			positiveEdges[i] = qMax(positiveEdges[i], pos[i]);
			negativeEdges[i] = qMin(negativeEdges[i], pos[i]);
		}
	}
	else
		m_space.remove(pos);
}

template <int dimensions>
QChar FungeSpace<dimensions>::getChar(Coord pos)
{
	if(m_space.contains(pos))
		return m_space[pos];
	else
		return QChar(' ');
}

#endif

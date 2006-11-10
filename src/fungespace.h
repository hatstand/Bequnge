#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QHash>
#include <QDebug>

typedef QList<int> Coord;

class FungeSpace
{
public:

	FungeSpace(int dimensions);
	~FungeSpace();

	// Place a char in FungeSpace
	void setChar(Coord, QChar);
	// Get a char from a Coord in FungeSpace
	QChar getChar(Coord);

	// Get all the code back out from FungeSpace
	QHash<Coord, QChar> getCode();
	
	// Get the code edges (only correct if code doesn't shrink)
	int getPositiveEdge(int dimension){ return positiveEdges[dimension]; }
	int getNegativeEdge(int dimension) { return negativeEdges[dimension]; }
	
	int dimensions() { return m_dimensions; }
	void setDimensions(int dimensions);

private:
	int* positiveEdges;
	int* negativeEdges;

	int m_dimensions;

	//QMap<QList<int>, QChar> m_space;
	QHash<Coord, QChar> m_space;
};


#endif

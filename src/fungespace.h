#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QHash>
#include <QDebug>
#include <QIODevice>

#include <boost/array.hpp>

typedef QVector<int> Coord;

class FungeSpace
{
public:

	FungeSpace(int dimensions);
	FungeSpace(QIODevice* dev);
	FungeSpace(FungeSpace* space);
	~FungeSpace();

	// Place a char in FungeSpace
	void setChar(Coord, QChar);
	// Get a char from a Coord in FungeSpace
	QChar getChar(Coord);

	// Get all the code back out from FungeSpace
	QHash<Coord, QChar> getCode();
	
	// Get the code edges (only correct if code doesn't shrink)
	int getPositiveEdge(int dimension){ return m_positiveEdges[dimension]; }
	int getNegativeEdge(int dimension) { return m_negativeEdges[dimension]; }
	
	uint dimensions() { return m_dimensions; }
	void setDimensions(uint dimensions);

	void save(QString filename);

private:
	typedef boost::array<int,2> PlaneCoord;

	void parseHeader(QIODevice* dev);
	void readInAll(QIODevice* dev);

	PlaneCoord coordToPlaneCoord(Coord c);

	Coord m_positiveEdges;
	Coord m_negativeEdges;

	QString m_version;
	uint m_dimensions;

	//QMap<QList<int>, QChar> m_space;
	QHash<Coord, QChar> m_space;
};


#endif

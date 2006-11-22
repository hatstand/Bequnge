#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QHash>
#include <QDebug>
#include <QIODevice>

#include <boost/array.hpp>

/*

The pig will help with the understanding of this class.

                                                         ___
                                        ,---.        /""`_,'
                                        |    `\``""-;   /
                                        \   /`\\     ';'
                                      .')   |     __   \
                                     / (`  /     /(O\. _|
                                  .-`|  `"`      `    .-\_
                               .-'   \                  ` ;=-.
                             .'        .           ._,   / o o\
                         .-'`            .           '-._;_._.J
                     .-'`                               `.-'` 
                 _.-`                       '          .'
               .'                              '- ._.-'
             /`                                    /
            /                                     |
      ._   ;                                 |    |
       )).-|                     |           |    /
      (/`.-|                \    \           /  .;
      ( (_)|                 |    \         ; .' |
       '--';                 |    `\       / /   |
            \                |      `\    /.'   /
             \               /        ;  |`\   |
              '.           .'      _.-|  |  |  |
                '-.     .-';_"---'`.__|  |  |  \
                   `'-._     ``'-. jgs|  \  \__\\
                        `''--.___\\   \__\\


*/

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

class FungeSpace : public QObject
{
	Q_OBJECT

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
	
	void trackChanges(bool track) {m_trackChanges = track;}
	QHash<Coord, QPair<QChar, QChar> > changes() { return m_changes; }
	void removeChange(Coord c) { m_changes.remove(c); }

	void save(QString filename);

private:
	typedef boost::array<int,2> PlaneCoord;

	void parseHeader(QIODevice* dev);
	void readInAll(QIODevice* dev);
	void readPlane(QIODevice* dev);

	PlaneCoord coordToPlaneCoord(Coord c);

	Coord m_positiveEdges;
	Coord m_negativeEdges;

	QString m_version;
	uint m_dimensions;

	//QMap<QList<int>, QChar> m_space;
	QHash<Coord, QChar> m_space;
	
	bool m_trackChanges;
	QHash<Coord, QPair<QChar, QChar> > m_changes;
};


#endif

#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QChar>
#include <QHash>
#include <QDebug>
#include <QIODevice>

#include <boost/array.hpp>

#include "coord.h"

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
	QChar getChar(Coord) const;

	// Get all the code back out from FungeSpace
	QHash<Coord, QChar> getCode();
	
	// Get the code edges (only correct if code doesn't shrink)
	int getPositiveEdge(int dimension) const { return m_positiveEdges[dimension]; }
	int getNegativeEdge(int dimension) const { return m_negativeEdges[dimension]; }
	
	uint dimensions() const { return m_dimensions; }
	void setDimensions(uint dimensions);
	
	void trackChanges(bool track) {m_trackChanges = track;}
	QHash<Coord, QPair<QChar, QChar> > changes() const { return m_changes; }
	void removeChange(Coord c) { m_changes.remove(c); }
	
	void toggleBreakpoint(Coord c) { if (isBreakpoint(c)) m_breakpoints.removeAll(c); else m_breakpoints << c; }
	void clearAllBreakpoints() { m_breakpoints.clear(); }
	bool isBreakpoint(Coord c) const { return m_breakpoints.contains(c); }
	QList<Coord> breakpoints() const { return m_breakpoints; }
	
	void toggleWatchpoint(Coord c) { if (isWatchpoint(c)) m_watchpoints.removeAll(c); else m_watchpoints << c; }
	void clearAllWatchpoints() { m_watchpoints.clear(); }
	bool isWatchpoint(Coord c) const { return m_watchpoints.contains(c); }
	QList<Coord> watchpoints() const { return m_watchpoints; }

	void save(QString filename);

signals:
	void watchpointTriggered(Coord c, QChar oldValue);

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
	
	QList<Coord> m_breakpoints;
	QList<Coord> m_watchpoints;
};


#endif

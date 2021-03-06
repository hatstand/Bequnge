#ifndef FUNGESPACE_H
#define FUNGESPACE_H

#include <QHash>
#include <QDebug>
#include <QIODevice>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

#include "coord.h"
#include "vector.h"

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
            /  multi_index lol                    |
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

struct FungeChar
{
	FungeChar(const Coord& c, int d) : coord(c), data(d) {}
	bool operator <(const FungeChar& other) const;
	
	Coord coord;
	int data;
};

struct FrontComparison
{
	bool operator() (const Coord& first, const Coord& second) const
	{
		for (int i = qMax(first.size(), second.size()) - 1; i >= 0; --i)
		{
			if (first[i] != second[i])
				return first[i] < second[i];
		}

		return false;
	}
};

struct SideComparison
{
	bool operator() (const Coord& first, const Coord& second) const
	{
		return first[0] > second[0];
	}
};

struct CleverComparison
{
	bool operator() (const FungeChar& first, const FungeChar& second) const
	{
		if (first.coord[2] != second.coord[2])
			return first.coord[2] < second.coord[2];

		return first.data < second.data;
	}
};

class FungeSpace : public QObject
{
	Q_OBJECT

#ifdef TESTFUNGESPACE_H
	friend class TestFungeSpace;
#endif
	typedef Vector<int,2> PlaneCoord;
	
	// Tags for multi_index indices.
	struct side {};
	struct front {};
	struct clever {};
	struct hash {};

	typedef boost::multi_index_container<
		FungeChar,
		boost::multi_index::indexed_by<
			boost::multi_index::hashed_unique<
				boost::multi_index::tag<hash>,
				boost::multi_index::member<FungeChar, Coord, &FungeChar::coord>,
				boost::hash<Coord>
			>,
			boost::multi_index::ordered_non_unique<
				boost::multi_index::tag<front>,
				boost::multi_index::member<FungeChar, Coord, &FungeChar::coord>,
				FrontComparison
			>,
			boost::multi_index::ordered_non_unique<
				boost::multi_index::tag<side>,
				boost::multi_index::member<FungeChar, Coord, &FungeChar::coord>,
				SideComparison
			>,
			boost::multi_index::ordered_non_unique<
				boost::multi_index::tag<clever>,
				boost::multi_index::identity<FungeChar>,
				CleverComparison
			>
		>
	> Space;

public:
	typedef Space::index<front>::type CodeByFront;
	typedef Space::index<side>::type CodeBySide;
	typedef Space::index<clever>::type CodeByClever;
	typedef Space::index<hash>::type CodeByHash;

	enum SourceType {
		Bequnge = 0,
		Befunge98,
		Befunge93
	};
	
	FungeSpace(int dimensions);
	FungeSpace(QIODevice* dev, SourceType type = Bequnge);
	FungeSpace(FungeSpace* space);
	~FungeSpace();

	// Place a char in FungeSpace
	// Postpone prevents the fungespaceChanged() signal from being emitted.
	void setChar(Coord, int, bool postpone = false);
	// Get a char from a Coord in FungeSpace
	int getChar(Coord) const;

	// Get all the code back out from FungeSpace
	CodeByFront& codeByFront() { return m_space.get<front>(); }
	CodeBySide& codeBySide() { return m_space.get<side>(); }
	CodeByClever& codeByClever() { return m_space.get<clever>(); }
	
	// Get the code edges (only correct if code doesn't shrink)
	int getPositiveEdge(int dimension) const { return m_positiveEdges[dimension]; }
	int getNegativeEdge(int dimension) const { return m_negativeEdges[dimension]; }
	
	uint dimensions() const { return m_dimensions; }
	void setDimensions(uint dimensions);
	
	void trackChanges(bool track) {m_trackChanges = track;}
	QHash<Coord, QPair<int, int> > changes() const { return m_changes; }
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
	void watchpointTriggered(Coord c, int oldValue);
	void fungespaceChanged();

private:
	void parseHeader(QIODevice* dev);
	void readInAll(QIODevice* dev);
	void readPlane(QIODevice* dev);

	PlaneCoord coordToPlaneCoord(Coord c);

	Coord m_positiveEdges;
	Coord m_negativeEdges;

	QString m_version;
	uint m_dimensions;

	//QMap<QList<int>, QChar> m_space;
	//QHash<Coord, QChar> m_space;
	Space m_space;
	
	bool m_trackChanges;
	QHash<Coord, QPair<int, int> > m_changes;
	
	QList<Coord> m_breakpoints;
	QList<Coord> m_watchpoints;
};


#endif

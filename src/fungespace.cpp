#include "fungespace.h"

uint qHash(Coord c)
{
	uint hash = 0;

	if(c.isEmpty())
		return hash;

	foreach(int i, c)
	{
		hash ^= i;
	}

	return hash;
}


FungeSpace::FungeSpace(int dimensions)
{
	m_dimensions = dimensions;

	positiveEdges = new int[m_dimensions];
	negativeEdges = new int[m_dimensions];

	for(int i = 0; i < m_dimensions; ++i)
	{
		positiveEdges[i] = 0;
		negativeEdges[i] = 0;
	}
}

FungeSpace::~FungeSpace()
{
	delete[] positiveEdges;
	delete[] negativeEdges;
}

QHash<Coord, QChar> FungeSpace::getCode()
{
	return m_space;
}

void FungeSpace::setChar(Coord pos, QChar c)
{
	Q_ASSERT(pos.size() == m_dimensions);

	if(c != ' ' && c != '\n')
	{
		m_space.insert(pos, c);

		for(int i = 0; i < m_dimensions; ++i)
		{
			positiveEdges[i] = qMax(positiveEdges[i], pos[i]);
			negativeEdges[i] = qMin(negativeEdges[i], pos[i]);
		}
	}
	else
		m_space.remove(pos);
}

QChar FungeSpace::getChar(Coord pos)
{
	Q_ASSERT(pos.size() == m_dimensions);

	if(m_space.contains(pos))
		return m_space[pos];
	else
		return QChar(' ');
}


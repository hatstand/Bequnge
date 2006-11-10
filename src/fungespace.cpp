#include "fungespace.h"

#include <QStringList>

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
	: positiveEdges(NULL),
	  negativeEdges(NULL)
{
	setDimensions(dimensions);
	
	m_version = "0";
}

FungeSpace::FungeSpace(QIODevice* dev)
	: positiveEdges(NULL),
	  negativeEdges(NULL)
{
	m_version = "0";

	dev->open(QIODevice::ReadOnly);
	parseHeader(dev);
	readInAll(dev);
	dev->close();
}

void FungeSpace::parseHeader(QIODevice* dev)
{
	qDebug() << "Parsing header";

	m_dimensions = 0;
	int new_dimensions = 0;

	QString line;
	while(((line = dev->readLine()).trimmed().isEmpty()));

	QStringList args = line.split(",");
	foreach(QString i, args)
	{
		QStringList t = i.split(" ");
		if(t.size() != 2)
		{
			qFatal("Expected space separated key->value");
		}
		else
		{
			if(t[0].toLower() == "version")
			{
				m_version = t[1];
			}
			else if(t[0].toLower() == "dimensions")
			{
				bool ok = false;
				new_dimensions = t[1].toUInt(&ok);
				if(!ok)
				{
					qFatal("Expected unsigned integer for value of dimensions");
				}
			}
		}
	}

	if(!new_dimensions)
		setDimensions(2);
	else
		setDimensions(new_dimensions);

	qDebug() << "Finished parsing header";
}

void FungeSpace::readInAll(QIODevice* dev)
{
	qDebug() << "Reading in code";
	QString line;

	Coord pos;
	for(uint i = 0; i < m_dimensions; ++i)
		pos << 0;

	while((line = dev->readLine()) != NULL)
	{
		int i = 0;
		for(; i < line.length(); ++i)
		{
			if(line[i] == '\n')
				break;

			pos[0] = i;
			//qDebug() << "Putting:" << line[i] << "in:" << pos[0] << pos[1];
			setChar(pos, line[i]);
		}

		++pos[1];
	}

	qDebug() << "Finished reading code";
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
	Q_ASSERT((uint)pos.size() == m_dimensions);

	//qDebug() << pos << c;

	if(c != ' ' && c != '\n')
	{
		m_space.insert(pos, c);

		for(uint i = 0; i < m_dimensions; ++i)
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
	Q_ASSERT((uint)pos.size() == m_dimensions);

	if(m_space.contains(pos))
		return m_space[pos];
	else
		return QChar(' ');
}

void FungeSpace::setDimensions(uint dimensions)
{
	if (dimensions == m_dimensions)
		return;
	
	QHash<Coord, QChar> newSpace;
	
	QHashIterator<Coord, QChar> i(m_space);
	while (i.hasNext())
	{
		i.next();
		Coord coord = i.key();
		
		while ((uint)coord.count() > dimensions)
			coord.removeLast();
		while ((uint)coord.count() < dimensions)
			coord.append(0);
		
		newSpace.insert(coord, i.value());
	}
	
	m_space = newSpace;
	
	m_dimensions = dimensions;
	
	delete[] positiveEdges;
	delete[] negativeEdges;
	positiveEdges = new int[m_dimensions];
	negativeEdges = new int[m_dimensions];

	for(uint i = 0; i < m_dimensions; ++i)
	{
		positiveEdges[i] = 0;
		negativeEdges[i] = 0;
	}
}


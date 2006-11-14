#include "fungespace.h"

#include <QStringList>
#include <QFile>

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
	: m_dimensions(0)
{
	setDimensions(dimensions);
	
	m_version = "0";
}

FungeSpace::FungeSpace(QIODevice* dev)
	: m_dimensions(0)
{
	m_version = "0";

	dev->open(QIODevice::ReadOnly);
	parseHeader(dev);
	readInAll(dev);
	dev->close();
}

FungeSpace::FungeSpace(FungeSpace* space)
	: m_dimensions(0)
{
	m_version = "0";
	
	setDimensions(space->dimensions());
	m_space = space->getCode();
	for (int i=0 ; i<m_dimensions ; ++i)
	{
		m_positiveEdges[i] = space->getPositiveEdge(i);
		m_negativeEdges[i] = space->getNegativeEdge(i);
	}
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
			if ((line[i] == '\n') || (line[i] == '\r'))
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
}

QHash<Coord, QChar> FungeSpace::getCode()
{
	return m_space;
}

void FungeSpace::setChar(Coord pos, QChar c)
{
	Q_ASSERT((uint)pos.size() == m_dimensions);

	//qDebug() << pos << c;

	if(c != ' ')
	{
		m_space.insert(pos, c);

		for(uint i = 0; i < m_dimensions; ++i)
		{
			m_positiveEdges[i] = qMax(m_positiveEdges[i], pos[i]);
			m_negativeEdges[i] = qMin(m_negativeEdges[i], pos[i]);
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
			coord.pop_back();
		while ((uint)coord.count() < dimensions)
			coord.append(0);
		
		newSpace.insert(coord, i.value());
	}
	
	m_space = newSpace;
	
	m_dimensions = dimensions;
	
	while (m_positiveEdges.count() < m_dimensions)
		m_positiveEdges.append(0);
	while (m_negativeEdges.count() < m_dimensions)
		m_negativeEdges.append(0);
}

void FungeSpace::save(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
	{
		qWarning() << "Cannot open file:" << filename << ":-S";
		return;
	}

	// Maps x,y to a QChar
	// QMap as indexes in order
	typedef QMap<PlaneCoord, QChar> Plane;

	// Maps a Coord (stripped of x,y) to an x-y plane
	QHash<Coord, Plane*> planes;
	foreach(Coord p, m_space.keys())
	{
		Q_ASSERT(p.size() == m_dimensions);

		// Convert Coord to PlaneCoord and Coord
		PlaneCoord pc = coordToPlaneCoord(p);
		Coord pp = p.mid(2);

		// Find the right plane
		Plane* current;
		if(planes.contains(pp))
		{
			current = planes[pp];
			// If we get a default constructed-value
			// something has gone wrong
			Q_ASSERT(current);
		}
		else
		{
			current = new Plane();
			planes[pp] = current;
		}

		// Insert QChar in the correct plane
		(*current)[pc] = m_space[p];
	}

	/*for(QHash<Coord,Plane*>::const_iterator it = planes.constBegin(); it != planes.constEnd(); ++it)
	{
		qDebug() << it.key() << it.value()->size();
		foreach(QChar c, it.value()->values())
		{
			qDebug() << c;
		}
	}*/

	QTextStream stream(&file);

	// Write header
	stream << "Version 1,Dimensions " << m_dimensions << '\n';
	QString line;

	stream.setString(&line);

	for(QHash<Coord,Plane*>::const_iterator it = planes.constBegin(); it != planes.constEnd(); ++it)
	{
		int miny = it.value()->constBegin().key()[0];

		int minx = it.value()->constBegin().key()[1];
		foreach(PlaneCoord q, it.value()->keys())
		{
			minx = qMin(minx, q[1]);
		}

		int currentx = minx;
		int currenty = miny;

		int lines = 0;
		
		for(Plane::const_iterator jt = it.value()->constBegin(); jt != it.value()->constEnd(); ++jt)
		{
			// Write newlines until we get to the y position
			for(; currenty < jt.key()[0]; ++currenty)
			{
				stream << '\n';
				currentx = minx;
				++lines;
			}

			// Write spaces until we get to the x position
			for(; currentx < jt.key()[1]; ++currentx)
				stream << ' ';

			// Finally write our char
			stream << jt.value();
			++currentx;
		}
		++lines;

		stream.setDevice(&file);
		stream << "Lines " << lines << ',';
		stream << "Origin " << minx << ':' << miny;
		if(!it.key().isEmpty())
		{
			stream << ",Coord ";
			foreach(int i, it.key())
			{
				stream << i << ':';
			}
		}
		stream << '\n';
		stream << line << '\n';

		line.clear();
		stream.setString(&line);
	}

	file.close();
}

FungeSpace::PlaneCoord FungeSpace::coordToPlaneCoord(Coord c)
{
	PlaneCoord p;
	// Swapped so that they are line-by-line in QMap
	p[1] = c[0];
	p[0] = c[1];

	return p;
}

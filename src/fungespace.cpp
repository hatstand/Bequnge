#include "fungespace.h"

#include <QStringList>
#include <QFile>


FungeSpace::FungeSpace(int dimensions)
	: m_dimensions(0),
	  m_trackChanges(false)
{
	setDimensions(dimensions);
	
	m_version = "0";
}

FungeSpace::FungeSpace(QIODevice* dev)
	: m_dimensions(0),
	  m_trackChanges(false)
{
	m_version = "0";

	dev->open(QIODevice::ReadOnly);
	parseHeader(dev);
	readInAll(dev);
	dev->close();
}

FungeSpace::FungeSpace(FungeSpace* space)
	: m_dimensions(0),
	  m_trackChanges(false)
{
	m_version = "0";
	
	setDimensions(space->dimensions());
	m_space = space->m_space;
	for (uint i=0 ; i<m_dimensions ; ++i)
	{
		m_positiveEdges[i] = space->getPositiveEdge(i);
		m_negativeEdges[i] = space->getNegativeEdge(i);
	}
	
	m_breakpoints = space->m_breakpoints;
	m_watchpoints = space->m_watchpoints;
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

	if(m_dimensions < 3)
	{
		Coord pos;
		for(uint i = 0; i < m_dimensions; ++i)
			pos << 0;

		while(!dev->atEnd())
		{
			line = dev->readLine();
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
	}

	// Parse planes
	while(dev->bytesAvailable() > 1)
		readPlane(dev);

	qDebug() << "Finished reading code";
}

void FungeSpace::readPlane(QIODevice* dev)
{
	QString line = dev->readLine();
	int noLines;
	PlaneCoord origin;
	Coord pos;
	QStringList t = line.split(',');
	foreach(QString i, t)
	{
		QStringList l = i.split(' ');
		if(l[0] == "Lines")
			noLines = l[1].toInt();
		else if(l[0] == "Origin")
		{
			QStringList x = l[1].split(':');
			origin[0] = x[0].toInt();
			origin[1] = x[1].toInt();
		}
		else if(l[0] == "Coord")
		{
			QStringList x = l[1].split(':', QString::SkipEmptyParts);
			foreach(QString j, x)
			{
				pos << j.toInt();
			}
		}
	}

	for(int i = 0; i < noLines; ++i)
	{
		line = dev->readLine();
		for(int j = 0; j < line.length() -1; ++j)
		{
			Coord p;
			// Insert x and y
			p[0] = j + origin[0];
			p[1] = i + origin[1];

			// Insert z...
			for(uint z = 2; z < m_dimensions; ++z)
				p[z] = pos[z-2];

			setChar(p, line[j]);
		}
	}
}

FungeSpace::~FungeSpace()
{
}

void FungeSpace::setChar(Coord pos, QChar c)
{
	if((uint)pos.count() > m_dimensions)
		setDimensions(pos.count());

	QChar oldValue;
	if (m_trackChanges)
		oldValue = getChar(pos);

	if(c != ' ')
	{
		m_space.insert(FungeChar(pos, c));

		for(uint i = 0; i < m_dimensions; ++i)
		{
			m_positiveEdges[i] = qMax(m_positiveEdges[i], pos[i]);
			m_negativeEdges[i] = qMin(m_negativeEdges[i], pos[i]);
		}
	}
	else
		m_space.erase(pos);
	
	if (m_trackChanges)
	{
		if (m_changes.contains(pos))
			m_changes[pos].second = c;
		else
			m_changes.insert(pos, QPair<QChar, QChar>(oldValue, c));
	}
	
	if (isWatchpoint(pos))
		emit watchpointTriggered(pos, oldValue);
}

QChar FungeSpace::getChar(Coord pos) const
{
	CodeByHash::iterator it = m_space.get<hash>().find(pos);
	if(it != m_space.get<hash>().end())
		return it->data;
	else
		return QChar(' ');
}

void FungeSpace::setDimensions(uint dimensions)
{
	if (dimensions == m_dimensions)
		return;
	
	Space newSpace;
	
	CodeByHash::iterator i(m_space.get<hash>().begin());
	while (i != m_space.get<hash>().end())
	{
		Coord coord = i->coord;;
		
		while ((uint)coord.count() > dimensions)
			coord.pop_back();
		while ((uint)coord.count() < dimensions)
			coord.append(0);
		
		newSpace.insert(FungeChar(coord, i->data));

		++i;
	}
	
	m_space = newSpace;
	
	m_dimensions = dimensions;
	
	while ((uint)m_positiveEdges.count() < m_dimensions)
		m_positiveEdges.append(0);
	while ((uint)m_negativeEdges.count() < m_dimensions)
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

	QTextStream stream(&file);

	// Iterate through in Z order.
	CodeByFront::iterator it(m_space.get<front>().begin());
	if (it == m_space.get<front>().end())
		return;

	int current_z = it->coord[2];
	int neg_edge = getNegativeEdge(0);
	int pos_edge = getPositiveEdge(0);
	int current_offset = 0;
	// Receives in z, y, x order
	while (it != m_space.get<front>().end())
	{
		++it;
	}

	/*
	// Maps x,y to a QChar
	// QMap as indexes in order
	typedef QMap<PlaneCoord, QChar> Plane;

	// Maps a Coord (stripped of x,y) to an x-y plane
	QHash<Coord, Plane*> planes;
	foreach(Coord p, m_space.keys())
	{
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
	}*/

	/*for(QHash<Coord,Plane*>::const_iterator it = planes.constBegin(); it != planes.constEnd(); ++it)
	{
		qDebug() << it.key() << it.value()->size();
		foreach(QChar c, it.value()->values())
		{
			qDebug() << c;
		}
	}*/

	/*QTextStream stream(&file);

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

	foreach(Plane* i, planes)
	{
		delete i;
	}

	file.close();*/
}

FungeSpace::PlaneCoord FungeSpace::coordToPlaneCoord(Coord c)
{
	PlaneCoord p;
	// Swapped so that they are line-by-line in QMap
	p[1] = c[0];
	p[0] = c[1];

	return p;
}

#ifndef EXTRADIMENSIONS_H
#define EXTRADIMENSIONS_H

#include <QMap>

#include "smoothvar.h"

class GLView;
class Coord;

class ExtraDimensions
{
public:
	ExtraDimensions(GLView* m_glView);
	
	void resetView();
	void prepareCallList(int id);
	
	void setAscensionLevel(int level);
	void ascendDimensions();
	void descendDimensions();
	
	void move(int x, int y, int z);
	void move(Coord pos);
	
	int ascensionLevel() const { return m_ascensionLevel; }
	float scaleFactor() const { return m_scaleFactor; }
	const SmoothVar<float>* cameraOffset() const { return m_cameraOffset; }
	
	void drawGridLines(float offsetX, float offsetY, float offsetZ);
	void updatePositions(float timeDelta);
	
	Coord nDTo3D(const Coord& c) const;

private:
	float gridSize(int ascensionLevel);
	int oldPosFromDiff(float diff);
	void drawGridLines(int i);
	void updateScaleFactor(float cameraMoveSpeed);
	
private:
	GLView* m_glView;
	
	int m_callListId;
	
	int m_ascensionLevel;
	
	QMap<int, SmoothVar<float> > m_gridAlpha;
	
	SmoothVar<float> m_cameraOffset[3];
	SmoothVar<float> m_scaleFactor;
};

#endif


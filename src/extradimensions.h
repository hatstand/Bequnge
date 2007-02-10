#ifndef EXTRADIMENSIONS_H
#define EXTRADIMENSIONS_H

#include <QMap>

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
	
	int ascensionLevel() const;
	const float* cameraOffset() const;
	float scaleFactor() const;
	
	void drawGridLines(const float* offset);
	void updatePositions();
	
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
	
	QMap<int, float> m_actualGridAlpha;
	QMap<int, float> m_destinationGridAlpha;
	
	float m_actualCameraOffset[3];
	float m_destinationCameraOffset[3];
	
	float m_actualScaleFactor;
	float m_destinationScaleFactor;
	float m_scaleFactorDiff;
};

#endif


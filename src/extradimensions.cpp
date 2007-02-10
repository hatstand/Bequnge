#include "extradimensions.h"
#include "glview.h"
#include "coord.h"
#include "magicnumbers.h"

#include <math.h>
#include <QDebug>

ExtraDimensions::ExtraDimensions(GLView* glView)
	: m_ascensionLevel(0),
	  m_glView(glView)
{
}

void ExtraDimensions::resetView()
{
	m_actualCameraOffset[0] = 0.0f;
	m_actualCameraOffset[1] = 0.0f;
	m_actualCameraOffset[2] = 0.0f;
	
	m_destinationCameraOffset[0] = m_actualCameraOffset[0];
	m_destinationCameraOffset[1] = m_actualCameraOffset[1];
	m_destinationCameraOffset[2] = m_actualCameraOffset[2];
	
	m_actualScaleFactor = 1.0f;
	m_destinationScaleFactor = 1.0f;
	
	m_destinationGridAlpha.clear();
	m_actualGridAlpha.clear();
	m_ascensionLevel = 0;
}

void ExtraDimensions::prepareCallList(int id)
{
	m_callListId = id;
	
	glNewList(m_callListId, GL_COMPILE);
		glBegin(GL_LINES);
			for (int y=-20 ; y<=20 ; y+=10)
			{
				for (int z=-20 ; z<=20 ; z+=10)
				{
					glVertex3f(FONT_SIZE * -20, - FONT_SIZE * y, FONT_SIZE * z);
					glVertex3f(FONT_SIZE * 20, - FONT_SIZE * y, FONT_SIZE * z);
				}
			}
			
			for (int x=-20 ; x<=20 ; x+=10)
			{
				for (int z=-20 ; z<=20 ; z+=10)
				{
					glVertex3f(FONT_SIZE * x, FONT_SIZE * 20, FONT_SIZE * z);
					glVertex3f(FONT_SIZE * x, - FONT_SIZE * 20, FONT_SIZE * z);
				}
			}
			
			for (int x=-20 ; x<=20 ; x+=10)
			{
				for (int y=-20 ; y<=20 ; y+=10)
				{
					glVertex3f(FONT_SIZE * x, - FONT_SIZE * y, FONT_SIZE * -20);
					glVertex3f(FONT_SIZE * x, - FONT_SIZE * y, FONT_SIZE * 20);
				}
			}
		glEnd();
	glEndList();
}

void ExtraDimensions::setAscensionLevel(int level)
{
	while (m_ascensionLevel < level)
		ascendDimensions();
	while (m_ascensionLevel > level)
		descendDimensions();
}

void ExtraDimensions::updateScaleFactor(float cameraMoveSpeed)
{
	m_glView->setCameraMoveSpeed(cameraMoveSpeed);
	if (m_ascensionLevel == 0)
		m_glView->resetEye();
	else
		m_glView->setEye(6.0f, 30.0f, -30.0f);
	
	m_destinationScaleFactor = 10.0f / powf(10.0f, 1 + m_ascensionLevel);
	m_scaleFactorDiff = 0.001f / powf(10.0f, m_ascensionLevel) - 0.001f / powf(10.0f, 1 + m_ascensionLevel);
}

void ExtraDimensions::ascendDimensions()
{
	if (m_ascensionLevel > 34)
		return;
	
	m_ascensionLevel++;
	m_destinationGridAlpha[m_ascensionLevel] = 0.75f;
	
	updateScaleFactor(0.07);
}

void ExtraDimensions::descendDimensions()
{
	if (m_ascensionLevel <= 0)
		return;
	
	m_destinationGridAlpha[m_ascensionLevel] = 0.0f;
	m_ascensionLevel--;
	
	updateScaleFactor(0.2);
}

void ExtraDimensions::move(int x, int y, int z)
{
	int direction = x | y | z; // Only one of x, y or z can be set
	
	int dimension;
	if (x) dimension = 0;
	else if (y) dimension = 1;
	else dimension = 2;
	
	if (dimension != 0) direction *= -1;
	
	// Update the camera offset to follow the text cursor
	m_destinationCameraOffset[dimension] += direction * gridSize(m_ascensionLevel);
}

int ExtraDimensions::ascensionLevel() const
{
	return m_ascensionLevel;
}

const float* ExtraDimensions::cameraOffset() const
{
	return (const float*) &m_actualCameraOffset;
}

float ExtraDimensions::scaleFactor() const
{
	return m_actualScaleFactor;
}

int ExtraDimensions::oldPosFromDiff(float diff)
{
	if (diff < 0)
		return 1;
	else if (diff == 0)
		return 0;
	else
		return -1;
}

float ExtraDimensions::gridSize(int ascensionLevel)
{
	return FONT_SIZE * 0.028f * powf(10.0, ascensionLevel);
}

void ExtraDimensions::drawGridLines(int i)
{
	float xDiff = (m_destinationCameraOffset[0] - m_actualCameraOffset[0]) * m_actualScaleFactor;
	float yDiff = (m_actualCameraOffset[1] - m_destinationCameraOffset[1]) * m_actualScaleFactor;
	
	if (xDiff > 6.496f)
		xDiff = 6.496f;
	if (yDiff > 6.496f)
		yDiff = 6.496f;
	
	int oldX = oldPosFromDiff(xDiff);
	int oldY = oldPosFromDiff(yDiff);
	
	float size = 250.0f * gridSize(i);
	float scaleFactor = 0.1f * pow(10.0f, i);
	
	int n = (i == m_ascensionLevel ? 3 : 2);
	
	for (int x=-n ; x<=n ; ++x)
	{
		for (int y=-n ; y<=n ; ++y)
		{
			float alphaMultiplier = 1.0f - sqrtf(powf(x, 2) + powf(y, 2)) / 5;
			float oldAlphaMultiplier = 1.0f - sqrtf(powf(x + oldX, 2) + powf(y + oldY, 2)) / 5;
			
			alphaMultiplier += (alphaMultiplier - oldAlphaMultiplier) * (sqrtf(powf(xDiff, 2) + powf(yDiff, 2))) / 9.1867313f;
			
			glPushMatrix();
				glTranslatef(x*size, - y*size, 0);
				
				if (x == 0 && y == 0)
					glColor4f(0.0f, 0.5f * alphaMultiplier, 0.0f, m_actualGridAlpha[i] * alphaMultiplier);
				else
					glColor4f(0.5f * alphaMultiplier, 0.5f * alphaMultiplier, 0.5f * alphaMultiplier, m_actualGridAlpha[i] * alphaMultiplier);
				
				
				glScalef(scaleFactor, scaleFactor, scaleFactor);
				glCallList(m_callListId);
			glPopMatrix();
		}
	}
}

void ExtraDimensions::drawGridLines(const float* offset)
{
	glPushMatrix();
		glTranslatef(m_destinationCameraOffset[0] * 250.0f + offset[0] * R_FONT_SCALE_FACTOR,
		             m_destinationCameraOffset[1] * 250.0f + offset[1] * R_FONT_SCALE_FACTOR,
		             m_destinationCameraOffset[2] * 250.0f + offset[2] * R_FONT_SCALE_FACTOR);
		
		foreach (int i, m_actualGridAlpha.keys())
		{
			if (i >= m_ascensionLevel - 2)
				drawGridLines(i);
		}
	glPopMatrix();
}

void ExtraDimensions::updatePositions()
{
	float diff = m_destinationScaleFactor - m_actualScaleFactor;
	
	if (isnan(diff) || fabs(diff) < m_scaleFactorDiff)
		m_actualScaleFactor = m_destinationScaleFactor;
	else
	{
		float normalisedDiff = fabs((diff / m_scaleFactorDiff) / 10000.0f);
		if (isnan(normalisedDiff) || normalisedDiff < 1.0f)
			normalisedDiff = 1.0f;
		m_actualScaleFactor += diff * normalisedDiff * 0.05;
	}
	
	for (int i=0 ; i<3 ; ++i)
	{
		diff = m_destinationCameraOffset[i] - m_actualCameraOffset[i];
		if (fabs(diff) < 0.01)
			m_actualCameraOffset[i] = m_destinationCameraOffset[i];
		else
			m_actualCameraOffset[i] += diff * 0.2;
	}
	
	foreach (int index, m_destinationGridAlpha.keys())
	{
		diff = m_destinationGridAlpha[index] - m_actualGridAlpha[index];
		if (fabs(diff) < 0.001)
		{
			m_actualGridAlpha[index] = m_destinationGridAlpha[index];
			if (m_actualGridAlpha[index] == 0.0f)
			{
				m_actualGridAlpha.remove(index);
				m_destinationGridAlpha.remove(index);
			}
		}
		else
			m_actualGridAlpha[index] += diff * 0.05;
	}
}

Coord ExtraDimensions::nDTo3D(const Coord& c) const
{
	Coord ret = c;
	for (int i=0 ; i<c.count() ; ++i)
		ret[i%3] += ret[i] * int(0.1f * pow(10.0f, int(i/3))) * CHARS_PER_CUBE;
	
	return ret;
}


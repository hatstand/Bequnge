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
	m_cameraOffset[0].setValueImmediately(0.0);
	m_cameraOffset[1].setValueImmediately(0.0);
	m_cameraOffset[2].setValueImmediately(0.0);
	
	m_scaleFactor.setValueImmediately(0.0);
	
	m_gridAlpha.clear();
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
	// TODO
	//m_glView->setCameraMoveSpeed(cameraMoveSpeed);
	if (m_ascensionLevel == 0)
		m_glView->resetEye();
	else
		m_glView->setEye(6.0f, 30.0f, -30.0f);
	
	m_scaleFactor = 10.0f / powf(10.0f, 1 + m_ascensionLevel);
}

void ExtraDimensions::ascendDimensions()
{
	if (m_ascensionLevel > 34)
		return;
	
	m_ascensionLevel++;
	m_gridAlpha[m_ascensionLevel] = 0.75;
	
	updateScaleFactor(0.07);
}

void ExtraDimensions::descendDimensions()
{
	if (m_ascensionLevel <= 0)
		return;
	
	m_gridAlpha[m_ascensionLevel] = 0.0;
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
	m_cameraOffset[dimension] += direction * gridSize(m_ascensionLevel);
}

void ExtraDimensions::move(Coord pos)
{
	m_cameraOffset[0] = 0.0f;
	m_cameraOffset[1] = 0.0f;
	m_cameraOffset[2] = 0.0f;
	
	for (int i=3 ; i<pos.count() ; ++i)
	{
		float sign = 1.0f;
		if (i%3 != 0) sign = -1.0f;
		
		m_cameraOffset[i%3] += sign * pos[i] * gridSize(i/3);
	}
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
	float xDiff = (m_cameraOffset[0].targetValue() - m_cameraOffset[0]) * m_scaleFactor;
	float yDiff = (m_cameraOffset[1] - m_cameraOffset[1].targetValue()) * m_scaleFactor;
	
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
					glColor4f(0.0f, 0.5f * alphaMultiplier, 0.0f, m_gridAlpha[i] * alphaMultiplier);
				else
					glColor4f(0.5f * alphaMultiplier, 0.5f * alphaMultiplier, 0.5f * alphaMultiplier, m_gridAlpha[i] * alphaMultiplier);
				
				
				glScalef(scaleFactor, scaleFactor, scaleFactor);
				glCallList(m_callListId);
			glPopMatrix();
		}
	}
}

void ExtraDimensions::drawGridLines(float offsetX, float offsetY, float offsetZ)
{
	glPushMatrix();
		glTranslatef(m_cameraOffset[0].targetValue() * 250.0f + offsetX * R_FONT_SCALE_FACTOR,
		             m_cameraOffset[1].targetValue() * 250.0f + offsetY * R_FONT_SCALE_FACTOR,
		             m_cameraOffset[2].targetValue() * 250.0f + offsetZ * R_FONT_SCALE_FACTOR);
		
		foreach (int i, m_gridAlpha.keys())
		{
			if (i >= m_ascensionLevel - 2)
				drawGridLines(i);
		}
	glPopMatrix();
}

void ExtraDimensions::updatePositions(float timeDelta)
{
	m_cameraOffset[0].update((int)timeDelta);
	m_cameraOffset[1].update((int)timeDelta);
	m_cameraOffset[2].update((int)timeDelta);
	m_scaleFactor.update((int)timeDelta);
	
	QList<int> toBeRemoved;
	
	foreach (int index, m_gridAlpha.keys())
	{
		SmoothVar<float>& var = m_gridAlpha[index];
		
		if (var < 0.0)
			toBeRemoved << index;
		else
			var.update((int)timeDelta);
	}
	
	foreach (int index, toBeRemoved)
		m_gridAlpha.remove(index);
}

Coord ExtraDimensions::nDTo3D(const Coord& c) const
{
	Coord ret = c;
	for (int i=0 ; i<c.count() ; ++i)
		ret[i%3] += ret[i] * int(0.1f * pow(10.0f, int(i/3))) * CHARS_PER_CUBE;
	
	return ret;
}


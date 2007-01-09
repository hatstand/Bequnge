#include "extradimensions.h"
#include "glview.h"

#include <math.h>
#include <QDebug>

ExtraDimensions::ExtraDimensions(GLView* glView)
	: m_ascensionLevel(0),
	  m_glView(glView)
{
	m_fontSize = m_glView->fontSize();
}

void ExtraDimensions::resetView()
{
	m_actualCameraOffset[0] = 0.0f;
	m_actualCameraOffset[1] = 0.0f;
	m_actualCameraOffset[2] = 0.0f;
	
	m_destinationCameraOffset[0] = m_actualCameraOffset[0];
	m_destinationCameraOffset[1] = m_actualCameraOffset[1];
	m_destinationCameraOffset[2] = m_actualCameraOffset[2];
	
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
					glVertex3f(m_fontSize * -20, - m_fontSize * y, m_fontSize * z);
					glVertex3f(m_fontSize * 20, - m_fontSize * y, m_fontSize * z);
				}
			}
			
			for (int x=-20 ; x<=20 ; x+=10)
			{
				for (int z=-20 ; z<=20 ; z+=10)
				{
					glVertex3f(m_fontSize * x, m_fontSize * 20, m_fontSize * z);
					glVertex3f(m_fontSize * x, - m_fontSize * 20, m_fontSize * z);
				}
			}
			
			for (int x=-20 ; x<=20 ; x+=10)
			{
				for (int y=-20 ; y<=20 ; y+=10)
				{
					glVertex3f(m_fontSize * x, - m_fontSize * y, m_fontSize * -20);
					glVertex3f(m_fontSize * x, - m_fontSize * y, m_fontSize * 20);
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

void ExtraDimensions::ascendDimensions()
{
	m_ascensionLevel++;
	m_destinationGridAlpha[m_ascensionLevel] = 0.75f;
	
	m_glView->setCameraMoveSpeed(0.01);
	m_glView->setEye(4.0f * powf(10.0f, m_ascensionLevel), 30.0f, -30.0f);
}

void ExtraDimensions::descendDimensions()
{
	if (m_ascensionLevel == 0)
		return;
	
	m_destinationGridAlpha[m_ascensionLevel] = 0.0f;
	m_ascensionLevel--;
	
	m_glView->setCameraMoveSpeed(0.2);
	
	if (m_ascensionLevel == 0)
		m_glView->resetEye();
	else
		m_glView->setEye(4.0f * powf(10.0f, m_ascensionLevel), 30.0f, -30.0f);
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
	return m_fontSize * 0.028f * powf(10.0, ascensionLevel);
}

void ExtraDimensions::drawGridLines(int i)
{
	float xDiff = m_destinationCameraOffset[0] - m_actualCameraOffset[0];
	float yDiff = m_actualCameraOffset[1] - m_destinationCameraOffset[1];
	
	if (xDiff > 6.496f)
		xDiff = 6.496f;
	if (yDiff > 6.496f)
		yDiff = 6.496f;
	
	int oldX = oldPosFromDiff(xDiff);
	int oldY = oldPosFromDiff(yDiff);
	
	float size = 250.0f * gridSize(i);
	float scaleFactor = 0.1f * pow(10.0f, i);
	
	int n = 3;
	if (i != m_ascensionLevel) n=2;
	
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

void ExtraDimensions::drawGridLines()
{
	glPushMatrix();
		glTranslatef(m_destinationCameraOffset[0] * 250.0f, m_destinationCameraOffset[1] * 250.0f, m_destinationCameraOffset[2] * 250.0f);
		foreach (int i, m_actualGridAlpha.keys())
			drawGridLines(i);
	glPopMatrix();
}

void ExtraDimensions::updatePositions()
{
	for (int i=0 ; i<3 ; ++i)
	{
		float diff = m_destinationCameraOffset[i] - m_actualCameraOffset[i];
		if (fabs(diff) < 0.01)
			m_actualCameraOffset[i] = m_destinationCameraOffset[i];
		else
			m_actualCameraOffset[i] += diff * 0.2;
	}
	
	foreach (int index, m_destinationGridAlpha.keys())
	{
		float diff = m_destinationGridAlpha[index] - m_actualGridAlpha[index];
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


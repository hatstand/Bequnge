/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SMOOTHVAR_H
#define SMOOTHVAR_H

#include <cmath>
#include <algorithm>

template <typename T>
class SmoothVar
{
public:
	SmoothVar();

	void setSpeed(float speed) { m_speed = speed; }
	void setLimit(float limit) { m_limit = limit; }
	void setModulo(float modulo) { m_modulo = modulo; }

	void setTargetValue(const T& value);
	void setValueImmediately(const T& value);
	void updateImmediately();
	void update(int timeDelta);
	void finishedEditing();

	T targetValue() const { return m_targetValue; }
	T currentValue() const { return m_currentValue; }

	float speed() const { return m_speed; }
	float limit() const { return m_limit; }
	float modulo() const { return m_modulo; }

	bool needsUpdating() const { return m_needsUpdating; }

	//operator T &() { return m_currentValue; }
	operator const T &() const { return m_currentValue; }
	
	SmoothVar<T>& operator =(const T& other) { setTargetValue(other); return *this; }
	SmoothVar<T>& operator +=(const T& other) { setTargetValue(m_targetValue + other); return *this; }
	SmoothVar<T>& operator -=(const T& other) { setTargetValue(m_targetValue - other); return *this; }
	SmoothVar<T>& operator *=(const T& other) { setTargetValue(m_targetValue * other); return *this; }
	SmoothVar<T>& operator /=(const T& other) { setTargetValue(m_targetValue / other); return *this; }
	SmoothVar<T>& operator +=(const SmoothVar<T>& other) { setTargetValue(m_targetValue + other.targetValue()); return *this; }
	SmoothVar<T>& operator -=(const SmoothVar<T>& other) { setTargetValue(m_targetValue - other.targetValue()); return *this; }
	SmoothVar<T>& operator *=(const SmoothVar<T>& other) { setTargetValue(m_targetValue * other.targetValue()); return *this; }
	SmoothVar<T>& operator /=(const SmoothVar<T>& other) { setTargetValue(m_targetValue / other.targetValue()); return *this; }

private:
	void init();
	
	T m_targetValue;
	T m_currentValue;

	float m_speed;
	float m_limit;
	float m_modulo;

	bool m_needsUpdating;
};

template <>
SmoothVar<float>::SmoothVar();

template <typename T>
SmoothVar<T>::SmoothVar()
{
	init();
}

template <typename T>
void SmoothVar<T>::setTargetValue(const T& value)
{
	m_targetValue = value;
	m_needsUpdating = true;
}

template <typename T>
void SmoothVar<T>::updateImmediately()
{
	m_currentValue = m_targetValue;
	m_needsUpdating = false;
}

template <typename T>
void SmoothVar<T>::setValueImmediately(const T& value)
{
	m_targetValue = value;
	m_currentValue = value;
	m_needsUpdating = false;
}

template <typename T>
void SmoothVar<T>::update(int timeDelta)
{
	if (!m_needsUpdating)
		return;
	
	T diff = m_targetValue - m_currentValue;
	if (diff > m_modulo/2.0f)
		diff -= m_modulo;
	else if (diff < - m_modulo/2.0f)
		diff += m_modulo;
	
	if (std::abs(diff) < m_limit)
	{
		m_currentValue = m_targetValue;
		m_needsUpdating = false;
	}
	else
	{
		m_currentValue += diff * std::min(1.0f, m_speed * timeDelta);

		if (m_currentValue > m_modulo)
			m_currentValue -= m_modulo;
		else if (m_currentValue < 0.0f)
			m_currentValue += m_modulo;
	}
}

template <typename T>
void SmoothVar<T>::finishedEditing()
{
	if (m_modulo == 0.0)
		return;
	
	if (m_targetValue > m_modulo)
	{
		m_targetValue -= m_modulo;
		m_currentValue -= m_modulo;
	}
	else if (m_targetValue < 0.0f)
	{
		m_targetValue += m_modulo;
		m_currentValue += m_modulo;
	}
}



#endif

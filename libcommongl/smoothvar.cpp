#include "smoothvar.h"

template <>
SmoothVar<float>::SmoothVar()
	: m_targetValue(0.0),
	  m_currentValue(0.0)
{
	init();
}

template <typename T>
void SmoothVar<T>::init()
{
	m_needsUpdating = false;
	m_speed = 0.006;
	m_limit = 0.001;
	m_modulo = 0;
}



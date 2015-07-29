#include "commonutils.h"
#include "opengl.h"

#include <QSize>
#include <QPoint>
#include <QRect>

int nextPowerOf2(int n)
{
	int ret = 1;
	while (ret < n)
		ret *= 2;
	return ret;
}

void glViewport(const QRect& rect)
{
	glViewport(rect.x(), rect.y(), rect.width(), rect.height());
}

void glViewport(const QPoint& pos, const QSize& size)
{
	glViewport(pos.x(), pos.y(), size.width(), size.height());
}

void glViewport(int x, int y, const QSize& size)
{
	glViewport(x, y, size.width(), size.height());
}

void glUniform2f(int loc, const Vector<float, 2>& v)
{
	glUniform2fv(loc, 1, v.data());
}

void glUniform3f(int loc, const Vector<float, 3>& v)
{
	glUniform3fv(loc, 1, v.data());
}

void glUniform4f(int loc, const Vector<float, 4>& v)
{
	glUniform4fv(loc, 1, v.data());
}

void glUniform2i(int loc, const Vector<int, 2>& v)
{
	glUniform2iv(loc, 1, v.data());
}

void glUniform3i(int loc, const Vector<int, 3>& v)
{
	glUniform3iv(loc, 1, v.data());
}

void glUniform4i(int loc, const Vector<int, 4>& v)
{
	glUniform4iv(loc, 1, v.data());
}



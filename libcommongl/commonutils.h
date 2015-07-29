#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include "vector.h"

class QPoint;
class QSize;
class QRect;

int nextPowerOf2(int n);

void glViewport(const QRect& rect);
void glViewport(const QPoint& pos, const QSize& size);
void glViewport(int x, int y, const QSize& size);

void glUniform2f(int loc, const Vector<float, 2>& v);
void glUniform3f(int loc, const Vector<float, 3>& v);
void glUniform4f(int loc, const Vector<float, 4>& v);

void glUniform2i(int loc, const Vector<int, 2>& v);
void glUniform3i(int loc, const Vector<int, 3>& v);
void glUniform4i(int loc, const Vector<int, 4>& v);

#endif

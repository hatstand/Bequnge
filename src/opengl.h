#ifndef GLWIN_H
#define GLWIN_H

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

void setupWinGLFunctions();

#ifdef _WIN32
#include <windows.h>

extern PFNGLBINDBUFFERPROC pglBindBuffer;
extern PFNGLGENBUFFERSPROC pglGenBuffers;
extern PFNGLBUFFERDATAPROC pglBufferData;
extern PFNGLDELETEBUFFERSPROC pglDeleteBuffers;
extern PFNGLDRAWRANGEELEMENTSPROC pglDrawRangeElements;

#define glBindBuffer pglBindBuffer
#define glGenBuffers pglGenBuffers
#define glBufferData pglBufferData
#define glDeleteBuffers pglDeleteBuffers
#define glDrawRangeElements pglDrawRangeElements

#endif
#endif

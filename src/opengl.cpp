#include "opengl.h"

#ifndef _WIN32
void setupWinGLFunctions()
{
}
#else

PFNGLBINDBUFFERPROC pglBindBuffer = NULL;
PFNGLGENBUFFERSPROC pglGenBuffers = NULL;
PFNGLBUFFERDATAPROC pglBufferData = NULL;
PFNGLDELETEBUFFERSPROC pglDeleteBuffers = NULL;
PFNGLDRAWRANGEELEMENTSPROC pglDrawRangeElements = NULL;

void setupWinGLFunctions()
{
	static bool alreadySetup = false;
	if (alreadySetup)
		return;
	alreadySetup = true;
	
	pglBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
	pglGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	pglBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
	pglDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
	pglDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) wglGetProcAddress("glDrawRangeElements");
}

#endif

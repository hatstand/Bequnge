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
PFNGLCLIENTACTIVETEXTUREPROC pglClientActiveTexture = NULL;
PFNGLUNIFORM3FVPROC pglUniform3fv = NULL;
PFNGLUNIFORM1FPROC pglUniform1f = NULL;
PFNGLACTIVETEXTUREPROC pglActiveTexture = NULL;
PFNGLUNIFORM1IPROC pglUniform1i = NULL;
PFNGLUNIFORM2FPROC pglUniform2f = NULL;
PFNGLMULTITEXCOORD2FPROC pglMultiTexCoord2f = NULL;
PFNGLLINKPROGRAMPROC pglLinkProgram = NULL;
PFNGLCREATEPROGRAMPROC pglCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC pglDeleteProgram = NULL;
PFNGLISPROGRAMPROC pglIsProgram = NULL;
PFNGLDELETESHADERPROC pglDeleteShader = NULL;
PFNGLISSHADERPROC pglIsShader = NULL;
PFNGLATTACHSHADERPROC pglAttachShader = NULL;
PFNGLGETSHADERINFOLOGPROC pglGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC pglGetShaderiv = NULL;
PFNGLCOMPILESHADERPROC pglCompileShader = NULL;
PFNGLSHADERSOURCEPROC pglShaderSource = NULL;
PFNGLCREATESHADERPROC pglCreateShader = NULL;
PFNGLUSEPROGRAMPROC pglUseProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC pglGetUniformLocation = NULL;
PFNGLUNIFORM2FVPROC pglUniform2fv = NULL;
PFNGLUNIFORM4FVPROC pglUniform4fv = NULL;
PFNGLUNIFORM2IVPROC pglUniform2iv = NULL;
PFNGLUNIFORM4IVPROC pglUniform4iv = NULL;
PFNGLUNIFORM3IVPROC pglUniform3iv = NULL;

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
	pglClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) wglGetProcAddress("glClientActiveTexture");
	pglUniform3fv = (PFNGLUNIFORM3FVPROC) wglGetProcAddress("glUniform3fv");
	pglUniform1f = (PFNGLUNIFORM1FPROC) wglGetProcAddress("glUniform1f");
	pglActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
	pglUniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
	pglUniform2f = (PFNGLUNIFORM2FPROC) wglGetProcAddress("glUniform2f");
	pglMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) wglGetProcAddress("glMultiTexCoord2f");
	pglLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
	pglCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
	pglDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
	pglIsProgram = (PFNGLISPROGRAMPROC) wglGetProcAddress("glIsProgram");
	pglDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
	pglIsShader = (PFNGLISSHADERPROC) wglGetProcAddress("glIsShader");
	pglAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
	pglGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
	pglGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
	pglCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
	pglShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
	pglCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
	pglUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	pglGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
	pglUniform2fv = (PFNGLUNIFORM2FVPROC) wglGetProcAddress("glUniform2fv");
	pglUniform4fv = (PFNGLUNIFORM4FVPROC) wglGetProcAddress("glUniform4fv");
	pglUniform2iv = (PFNGLUNIFORM2IVPROC) wglGetProcAddress("glUniform2iv");
	pglUniform4iv = (PFNGLUNIFORM4IVPROC) wglGetProcAddress("glUniform4iv");
	pglUniform3iv = (PFNGLUNIFORM3IVPROC) wglGetProcAddress("glUniform3iv");
}

#endif

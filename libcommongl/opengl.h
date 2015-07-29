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
extern PFNGLCLIENTACTIVETEXTUREPROC pglClientActiveTexture;
extern PFNGLUNIFORM3FVPROC pglUniform3fv;
extern PFNGLUNIFORM1FPROC pglUniform1f;
extern PFNGLACTIVETEXTUREPROC pglActiveTexture;
extern PFNGLUNIFORM1IPROC pglUniform1i;
extern PFNGLUNIFORM2FPROC pglUniform2f;
extern PFNGLMULTITEXCOORD2FPROC pglMultiTexCoord2f;
extern PFNGLLINKPROGRAMPROC pglLinkProgram;
extern PFNGLCREATEPROGRAMPROC pglCreateProgram;
extern PFNGLDELETEPROGRAMPROC pglDeleteProgram;
extern PFNGLISPROGRAMPROC pglIsProgram;
extern PFNGLDELETESHADERPROC pglDeleteShader;
extern PFNGLISSHADERPROC pglIsShader;
extern PFNGLATTACHSHADERPROC pglAttachShader;
extern PFNGLGETSHADERINFOLOGPROC pglGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC pglGetShaderiv;
extern PFNGLCOMPILESHADERPROC pglCompileShader;
extern PFNGLSHADERSOURCEPROC pglShaderSource;
extern PFNGLCREATESHADERPROC pglCreateShader;
extern PFNGLUSEPROGRAMPROC pglUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC pglGetUniformLocation;
extern PFNGLUNIFORM2FVPROC pglUniform2fv;
extern PFNGLUNIFORM4FVPROC pglUniform4fv;
extern PFNGLUNIFORM2IVPROC pglUniform2iv;
extern PFNGLUNIFORM4IVPROC pglUniform4iv;
extern PFNGLUNIFORM3IVPROC pglUniform3iv;

inline void glBindBuffer(GLenum a, GLuint b) { pglBindBuffer(a, b); }
inline void glGenBuffers(GLsizei a, GLuint * b) { pglGenBuffers(a, b); }
inline void glBufferData(GLenum a, GLsizeiptr b, const GLvoid * c, GLenum d) { pglBufferData(a, b, c, d); }
inline void glDeleteBuffers(GLsizei a, const GLuint * b) { pglDeleteBuffers(a, b); }
inline void glDrawRangeElements(GLenum a, GLuint b, GLuint c, GLsizei d, GLenum e, const GLvoid * f) { pglDrawRangeElements(a, b, c, d, e, f); }
inline void glClientActiveTexture(GLenum a) { pglClientActiveTexture(a); }
inline void glUniform3fv(GLint a, GLsizei b, const GLfloat * c) { pglUniform3fv(a, b, c); }
inline void glUniform1f(GLint a, GLfloat b) { pglUniform1f(a, b); }
inline void glActiveTexture(GLenum a) { pglActiveTexture(a); }
inline void glUniform1i(GLint a, GLint b) { pglUniform1i(a, b); }
inline void glUniform2f(GLint a, GLfloat b, GLfloat c) { pglUniform2f(a, b, c); }
inline void glMultiTexCoord2f(GLenum a, GLfloat b, GLfloat c) { pglMultiTexCoord2f(a, b, c); }
inline void glLinkProgram(GLuint a) { pglLinkProgram(a); }
inline GLuint glCreateProgram() { return pglCreateProgram(); }
inline void glDeleteProgram(GLuint a) { pglDeleteProgram(a); }
inline GLboolean glIsProgram(GLuint a) { return pglIsProgram(a); }
inline void glDeleteShader(GLuint a) { pglDeleteShader(a); }
inline GLboolean glIsShader(GLuint a) { return pglIsShader(a); }
inline void glAttachShader(GLuint a, GLuint b) { pglAttachShader(a, b); }
inline void glGetShaderInfoLog(GLuint a, GLsizei b, GLsizei * c, GLchar * d) { pglGetShaderInfoLog(a, b, c, d); }
inline void glGetShaderiv(GLuint a, GLenum b, GLint * c) { pglGetShaderiv(a, b, c); }
inline void glCompileShader(GLuint a) { pglCompileShader(a); }
inline void glShaderSource(GLuint a, GLsizei b, const GLchar* * c, const GLint * d) { pglShaderSource(a, b, c, d); }
inline GLuint glCreateShader(GLenum a) { return pglCreateShader(a); }
inline void glUseProgram(GLuint a) { pglUseProgram(a); }
inline GLint glGetUniformLocation(GLuint a, const GLchar * b) { return pglGetUniformLocation(a, b); }
inline void glUniform2fv(GLint a, GLsizei b, const GLfloat * c) { pglUniform2fv(a, b, c); }
inline void glUniform4fv(GLint a, GLsizei b, const GLfloat * c) { pglUniform4fv(a, b, c); }
inline void glUniform2iv(GLint a, GLsizei b, const GLint * c) { pglUniform2iv(a, b, c); }
inline void glUniform4iv(GLint a, GLsizei b, const GLint * c) { pglUniform4iv(a, b, c); }
inline void glUniform3iv(GLint a, GLsizei b, const GLint * c) { pglUniform3iv(a, b, c); }

#endif
#endif

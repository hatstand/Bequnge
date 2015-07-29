functions = [
	"glBindBuffer",
	"glGenBuffers",
	"glBufferData",
	"glDeleteBuffers",
	"glDrawRangeElements",
	"glClientActiveTexture",
	"glUniform3fv",
	"glUniform1f",
	"glActiveTexture",
	"glUniform1i",
	"glUniform2f",
	"glMultiTexCoord2f",
	"glLinkProgram",
	"glCreateProgram",
	"glDeleteProgram",
	"glIsProgram",
	"glDeleteShader",
	"glIsShader",
	"glAttachShader",
	"glGetShaderInfoLog",
	"glGetShaderiv",
	"glCompileShader",
	"glShaderSource",
	"glCreateShader",
	"glUseProgram",
	"glGetUniformLocation",
	"glUniform2fv",
	"glUniform4fv",
	"glUniform2iv",
	"glUniform4iv",
	"glUniform3iv"
]

argtypes = {}
returntypes = {}

glext_h = open("C:\\MinGW\\include\\GL\\glext.h")
for line in glext_h:
	for function in functions:
		searchstring = "APIENTRY " + function + " "
		loc = line.find(searchstring)
		if loc == -1:
			continue
		args = line[loc+len(searchstring):].strip("\n ();")
		argtypes[function] = args.split(",")
		argtypes[function] = [arg.strip() for arg in argtypes[function]]
		if argtypes[function][0] == "void":
			argtypes[function] = []
		returntypes[function] = line.split()[1]

h = open("opengl.h", "w")

h.write("""#ifndef GLWIN_H
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

""")

for function in functions:
	h.write("extern PFN%sPROC p%s;\n" % (function.upper(), function))

h.write("\n")

for function in functions:
	args1 = []
	args2 = []
	letter = ord("a")
	for argtype in argtypes[function]:
		args1.append(argtype + " " + chr(letter))
		args2.append(chr(letter))
		letter = letter + 1
	
	ret = ""
	if returntypes[function] != "void":
		ret = "return "
	
	h.write("inline %s %s(%s) { %sp%s(%s); }\n" % (
		returntypes[function],
		function,
		", ".join(args1),
		ret,
		function,
		", ".join(args2)))

h.write("""
#endif
#endif
""")

h.close()




cpp = open("opengl.cpp", "w")

cpp.write("""#include "opengl.h"

#ifndef _WIN32
void setupWinGLFunctions()
{
}
#else

""")

for function in functions:
	cpp.write("PFN%sPROC p%s = NULL;\n" % (function.upper(), function))

cpp.write("""
void setupWinGLFunctions()
{
	static bool alreadySetup = false;
	if (alreadySetup)
		return;
	alreadySetup = true;
	
""")

for function in functions:
	cpp.write("\tp%s = (PFN%sPROC) wglGetProcAddress(\"%s\");\n" % (function, function.upper(), function));

cpp.write("""}

#endif
""")

cpp.close()
functions = [
	"glBindBuffer",
	"glGenBuffers",
	"glBufferData",
	"glDeleteBuffers",
	"glDrawRangeElements",
	"glUseProgram",
	"glAttachShader",
	"glGetShaderInfoLog",
	"glGetShaderiv",
	"glCompileShader",
	"glShaderSource",
	"glIsShader",
	"glLinkProgram",
	"glCreateProgram",
	"glDeleteShader",
	"glIsProgram",
	"glActiveTexture",
	"glUniform1i",
	"glUniform1f",
	"glMultiTexCoord2f",
	
]

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
	h.write("#define %s p%s\n" % (function, function))

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
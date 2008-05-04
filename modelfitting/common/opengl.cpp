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
PFNGLUSEPROGRAMPROC pglUseProgram = NULL;
PFNGLATTACHSHADERPROC pglAttachShader = NULL;
PFNGLGETSHADERINFOLOGPROC pglGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC pglGetShaderiv = NULL;
PFNGLCOMPILESHADERPROC pglCompileShader = NULL;
PFNGLSHADERSOURCEPROC pglShaderSource = NULL;
PFNGLISSHADERPROC pglIsShader = NULL;
PFNGLLINKPROGRAMPROC pglLinkProgram = NULL;
PFNGLCREATEPROGRAMPROC pglCreateProgram = NULL;
PFNGLDELETESHADERPROC pglDeleteShader = NULL;
PFNGLISPROGRAMPROC pglIsProgram = NULL;
PFNGLACTIVETEXTUREPROC pglActiveTexture = NULL;
PFNGLUNIFORM1IPROC pglUniform1i = NULL;
PFNGLUNIFORM1FPROC pglUniform1f = NULL;
PFNGLUNIFORM2FPROC pglUniform2f = NULL;
PFNGLDELETEPROGRAMPROC pglDeleteProgram = NULL;
PFNGLCREATESHADERPROC pglCreateShader = NULL;
PFNGLGETUNIFORMLOCATIONPROC pglGetUniformLocation = NULL;
PFNGLMULTITEXCOORD2FPROC pglMultiTexCoord2f = NULL;

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
	pglUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	pglAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
	pglGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
	pglGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
	pglCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
	pglShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
	pglIsShader = (PFNGLISSHADERPROC) wglGetProcAddress("glIsShader");
	pglLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
	pglCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
	pglDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
	pglIsProgram = (PFNGLISPROGRAMPROC) wglGetProcAddress("glIsProgram");
	pglActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
	pglUniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
	pglUniform1f = (PFNGLUNIFORM1FPROC) wglGetProcAddress("glUniform1f");
	pglUniform2f = (PFNGLUNIFORM2FPROC) wglGetProcAddress("glUniform2f");
	pglDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
	pglCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
	pglGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
	pglMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) wglGetProcAddress("glMultiTexCoord2f");
}

#endif

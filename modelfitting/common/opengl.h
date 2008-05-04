#ifndef OPENGL_H
#define OPENGL_H

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
extern PFNGLUSEPROGRAMPROC pglUseProgram;
extern PFNGLATTACHSHADERPROC pglAttachShader;
extern PFNGLGETSHADERINFOLOGPROC pglGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC pglGetShaderiv;
extern PFNGLCOMPILESHADERPROC pglCompileShader;
extern PFNGLSHADERSOURCEPROC pglShaderSource;
extern PFNGLISSHADERPROC pglIsShader;
extern PFNGLLINKPROGRAMPROC pglLinkProgram;
extern PFNGLCREATEPROGRAMPROC pglCreateProgram;
extern PFNGLDELETESHADERPROC pglDeleteShader;
extern PFNGLISPROGRAMPROC pglIsProgram;
extern PFNGLACTIVETEXTUREPROC pglActiveTexture;
extern PFNGLUNIFORM1IPROC pglUniform1i;
extern PFNGLUNIFORM1FPROC pglUniform1f;
extern PFNGLUNIFORM2FPROC pglUniform2f;
extern PFNGLDELETEPROGRAMPROC pglDeleteProgram;
extern PFNGLCREATESHADERPROC pglCreateShader;
extern PFNGLGETUNIFORMLOCATIONPROC pglGetUniformLocation;
extern PFNGLMULTITEXCOORD2FPROC pglMultiTexCoord2f;

#define glBindBuffer pglBindBuffer
#define glGenBuffers pglGenBuffers
#define glBufferData pglBufferData
#define glDeleteBuffers pglDeleteBuffers
#define glDrawRangeElements pglDrawRangeElements
#define glUseProgram pglUseProgram
#define glAttachShader pglAttachShader
#define glGetShaderInfoLog pglGetShaderInfoLog
#define glGetShaderiv pglGetShaderiv
#define glCompileShader pglCompileShader
#define glShaderSource pglShaderSource
#define glIsShader pglIsShader
#define glLinkProgram pglLinkProgram
#define glCreateProgram pglCreateProgram
#define glDeleteShader pglDeleteShader
#define glIsProgram pglIsProgram
#define glActiveTexture pglActiveTexture
#define glUniform1i pglUniform1i
#define glUniform1f pglUniform1f
#define glUniform2f pglUniform2f
#define glDeleteProgram pglDeleteProgram
#define glCreateShader pglCreateShader
#define glGetUniformLocation pglGetUniformLocation
#define glMultiTexCoord2f pglMultiTexCoord2f

#endif
#endif

#ifndef GLWIN_H
#define GLWIN_H

void setupWinGLFunctions();

#ifdef _WIN32

#define GL_GLEXT_PROTOTYPES 1
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>

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

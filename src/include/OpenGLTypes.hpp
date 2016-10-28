#pragma once

namespace GlProj
{
	namespace Graphics
	{
#if !defined(__gl_h_) && !defined(__GL_H__)
		typedef unsigned int GLenum;
		typedef unsigned char GLboolean;
		typedef unsigned int GLbitfield;
		typedef signed char GLbyte;
		typedef short GLshort;
		typedef int GLint;
		typedef int GLsizei;
		typedef unsigned char GLubyte;
		typedef unsigned short GLushort;
		typedef unsigned int GLuint;
		typedef float GLfloat;
		typedef float GLclampf;
		typedef double GLdouble;
		typedef double GLclampd;
		typedef void GLvoid;
#endif
		//TODO: Move definitions of major opengl-related types here
	}
}

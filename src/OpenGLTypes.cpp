#include "OpenGLTypes.hpp"
#include <cstdint>

using namespace GlProj::Graphics;

void GLFwdDeclCompatTests()
{
	static_assert(sizeof(GLboolean) == sizeof(std::uint8_t), "GLboolean size fail");
	static_assert(sizeof(GLbyte) == sizeof(std::int8_t), "GLbyte size fail");
	static_assert(sizeof(GLubyte) == sizeof(std::uint8_t), "GLubyte size fail");
	static_assert(sizeof(char) == 1, "char size fail");
	static_assert(sizeof(GLshort) == sizeof(std::int16_t), "GLshort size fail");
	static_assert(sizeof(GLushort) == sizeof(std::uint16_t), "GLushort size fail");
	static_assert(sizeof(GLint) == sizeof(std::int32_t), "GLint size fail");
	static_assert(sizeof(GLuint) == sizeof(std::uint32_t), "GLuint size fail");
	static_assert(sizeof(GLsizei) == sizeof(std::int32_t), "GLsizei size fail");
	static_assert(sizeof(GLenum) == sizeof(std::int32_t), "GLenum size fail");
	static_assert(sizeof(GLfloat) == sizeof(float), "GLfloat size fail");
	static_assert(sizeof(GLclampf) == sizeof(float), "GLclampf size fail");
	static_assert(sizeof(GLdouble) == sizeof(double), "GLclampf size fail");
	static_assert(sizeof(GLclampd) == sizeof(double), "GLclampf size fail");
	static_assert(sizeof(GLclampf) == sizeof(float), "GLclampf size fail");
}
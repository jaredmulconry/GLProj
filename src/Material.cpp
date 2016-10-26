#include "Material.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"
#include "Camera.hpp"
#include "ShadingProgram.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "ShadingProgram.hpp"
#include "glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>

template<typename T>
using remove_cvrp = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

static const constexpr bool enable_gl_type_validation = false;

template<typename T>
void ValidateType(const GlProj::Graphics::UniformInformation& t);

template<>
void ValidateType<GLint>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_INT,
		GL_INT_VEC2,
		GL_INT_VEC3,
		GL_INT_VEC4,
		GL_BOOL,
		GL_BOOL_VEC2,
		GL_BOOL_VEC3,
		GL_BOOL_VEC4,
		GL_SAMPLER_1D,
		GL_SAMPLER_2D,
		GL_SAMPLER_3D,
		GL_SAMPLER_CUBE,
		GL_SAMPLER_1D_SHADOW,
		GL_SAMPLER_2D_SHADOW,
		GL_SAMPLER_1D_ARRAY,
		GL_SAMPLER_2D_ARRAY,
		GL_SAMPLER_1D_ARRAY_SHADOW,
		GL_SAMPLER_2D_ARRAY_SHADOW,
		GL_SAMPLER_2D_MULTISAMPLE,
		GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
		GL_SAMPLER_CUBE_SHADOW,
		GL_SAMPLER_BUFFER,
		GL_SAMPLER_2D_RECT,
		GL_SAMPLER_2D_RECT_SHADOW,
		GL_INT_SAMPLER_CUBE,
		GL_INT_SAMPLER_1D_ARRAY,
		GL_INT_SAMPLER_2D_ARRAY,
		GL_INT_SAMPLER_2D_MULTISAMPLE,
		GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		GL_INT_SAMPLER_BUFFER,
		GL_INT_SAMPLER_2D_RECT,
		GL_UNSIGNED_INT_SAMPLER_1D,
		GL_UNSIGNED_INT_SAMPLER_2D,
		GL_UNSIGNED_INT_SAMPLER_3D,
		GL_UNSIGNED_INT_SAMPLER_CUBE,
		GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
		GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
		GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
		GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		GL_UNSIGNED_INT_SAMPLER_BUFFER,
		GL_UNSIGNED_INT_SAMPLER_2D_RECT,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: GLint";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<GLuint>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_UNSIGNED_INT,
		GL_UNSIGNED_INT_VEC2,
		GL_UNSIGNED_INT_VEC3,
		GL_UNSIGNED_INT_VEC4,
		GL_BOOL,
		GL_BOOL_VEC2,
		GL_BOOL_VEC3,
		GL_BOOL_VEC4,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: GLuint";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<GLfloat>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT,
		GL_FLOAT_VEC2,
		GL_FLOAT_VEC3,
		GL_FLOAT_VEC4,
		GL_FLOAT_MAT2,
		GL_FLOAT_MAT3,
		GL_FLOAT_MAT4,
		GL_FLOAT_MAT2x3,
		GL_FLOAT_MAT2x4,
		GL_FLOAT_MAT3x2,
		GL_FLOAT_MAT3x4,
		GL_FLOAT_MAT4x2,
		GL_FLOAT_MAT4x3,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: GLfloat";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec2>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC2,
		GL_FLOAT_MAT2,
		GL_FLOAT_MAT2x3,
		GL_FLOAT_MAT2x4,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::vec2";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec3>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC3,
		GL_FLOAT_MAT3,
		GL_FLOAT_MAT3x2,
		GL_FLOAT_MAT3x4,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::vec3";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec4>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC4,
		GL_FLOAT_MAT4,
		GL_FLOAT_MAT4x2,
		GL_FLOAT_MAT4x3,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::vec4";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat2>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT2,
	};
	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::mat2";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat3>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT3,
	};
	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::mat3";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat4>(const GlProj::Graphics::UniformInformation& t)
{
	static GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT4,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t.type))
	{
		std::string err = "Type mismatch on shader uniform \"" + t.name
			+ "\": \n Actual type: ";
		err += std::to_string(t.type);
		err += "\n Provided type: glm::mat4";
		throw std::logic_error(err);
	}
}

void ValidateBounds(const GlProj::Graphics::UniformInformation& t, int s)
{
	if (t.size < s)
	{
		std::string err = "Uniform access out of bounds.\nUniform name: " + t.name
			+ "\nType: ";
		err += std::to_string(t.type);
		err += "\nSize: ";
		err += std::to_string(t.size);
		err += "\nInput size: ";
		err += std::to_string(s);
		throw std::out_of_range(err);
	}
}

namespace GlProj
{
	namespace Graphics
	{
		Material::Material(const LocalSharedPtr<ShadingProgram>& p)
			:program(p)
		{}
		Material& Material::operator=(const LocalSharedPtr<ShadingProgram>& p)
		{
			program = p;

			return *this;
		}
		void Material::Bind() const
		{
			if (program != nullptr)
			{
				program->Bind();
			}
		}
		const ShadingProgram * Material::GetProgram() const noexcept
		{
			return program.get();
		}
		void Material::SetUniform(const UniformInformation& u, GLint i)
		{
			SetUniform(u, &i, 1);
		}
		void Material::SetUniform(const UniformInformation& u, GLuint ui)
		{
			SetUniform(u, &ui, 1);
		}
		void Material::SetUniform(const UniformInformation& u, GLfloat f)
		{
			SetUniform(u, &f, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec2& v)
		{
			SetUniform(u, &v, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec3& v)
		{
			SetUniform(u, &v, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec4& v)
		{
			SetUniform(u, &v, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat2& m)
		{
			SetUniform(u, &m, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat3& m)
		{
			SetUniform(u, &m, 1);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat4& m)
		{
			SetUniform(u, &m, 1);
		}

		void Material::SetUniform(const UniformInformation& u, const GLint* i, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(i)>>(u);
				ValidateBounds(u, s);
			}

			glUniform1iv(u.location, s, i);
		}
		void Material::SetUniform(const UniformInformation& u, const GLuint* ui, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(ui)>>(u);
				ValidateBounds(u, s);
			}

			glUniform1uiv(u.location, s, ui);
		}
		void Material::SetUniform(const UniformInformation& u, const GLfloat* f, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(f)>>(u);
				ValidateBounds(u, s);
			}

			glUniform1fv(u.location, s, f);
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec2* v, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(v)>>(u);
				ValidateBounds(u, s);
			}
			glUniform2fv(u.location, s, reinterpret_cast<const GLfloat*>(v));
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec3* v, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(v)>>(u);
				ValidateBounds(u, s);
			}
			glUniform3fv(u.location, s, reinterpret_cast<const GLfloat*>(v));
		}
		void Material::SetUniform(const UniformInformation& u, const glm::vec4* v, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(v)>>(u);
				ValidateBounds(u, s);
			}
			glUniform4fv(u.location, s, reinterpret_cast<const GLfloat*>(v));
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat2* m, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(m)>>(u);
				ValidateBounds(u, s);
			}
			glUniformMatrix2fv(u.location, s, GL_FALSE, reinterpret_cast<const GLfloat*>(m));
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat3* m, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(m)>>(u);
				ValidateBounds(u, s);
			}
			glUniformMatrix3fv(u.location, s, GL_FALSE, reinterpret_cast<const GLfloat*>(m));
		}
		void Material::SetUniform(const UniformInformation& u, const glm::mat4* m, int s)
		{
			if (enable_gl_type_validation)
			{
				ValidateType<remove_cvrp<decltype(m)>>(u);
				ValidateBounds(u, s);
			}
			glUniformMatrix4fv(u.location, s, GL_FALSE, reinterpret_cast<const GLfloat*>(m));
		}
		GLint TextureSlotToGL(TextureSlot s)
		{
			return static_cast<GLint>(s);
		}
		bool operator==(const Material& x, const Material& y) noexcept
		{
			return x.program == y.program;
		}
		bool operator!=(const Material& x, const Material& y) noexcept
		{
			return !(x == y);
		}
		bool operator<(const Material& x, const Material& y) noexcept
		{
			if (x == y) return false;
			if (x.program == nullptr) return true;
			if (y.program == nullptr) return false;
			return x.program->GetHandle() < y.program->GetHandle();
		}
		bool operator<=(const Material& x, const Material& y) noexcept
		{
			return !(y < x);
		}
		bool operator>(const Material& x, const Material& y) noexcept
		{
			return y < x;
		}
		bool operator>=(const Material& x, const Material& y) noexcept
		{
			return !(x < y);
		}
	}
}

#include "Material.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"
#include "ShadingProgram.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "glm.hpp"

#include <algorithm>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <string>

template<typename T>
void ValidateType(GLenum t);

template<>
void ValidateType<GLint>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_INT,
		GL_INT_VEC2,
		GL_INT_VEC3,
		GL_INT_VEC4,
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

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: GLint";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<GLfloat>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
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

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: GLfloat";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec2>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC2,
		GL_FLOAT_MAT2,
		GL_FLOAT_MAT2x3,
		GL_FLOAT_MAT2x4,
		GL_FLOAT_MAT3x2,
		GL_FLOAT_MAT4x2,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::vec2";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec3>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC3,
		GL_FLOAT_MAT3,
		GL_FLOAT_MAT2x3,
		GL_FLOAT_MAT3x2,
		GL_FLOAT_MAT3x4,
		GL_FLOAT_MAT4x3,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::vec3";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::vec4>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_VEC4,
		GL_FLOAT_MAT4,
		GL_FLOAT_MAT2x4,
		GL_FLOAT_MAT3x4,
		GL_FLOAT_MAT4x2,
		GL_FLOAT_MAT4x3,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::vec4";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat2>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT2,
		GL_FLOAT_MAT4,
	};
	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::mat2";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat3>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT3,
	};
	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::mat3";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<glm::mat4>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
		GL_FLOAT_MAT4,
	};

	static std::once_flag sortFlg;
	std::call_once(sortFlg, [&]()
	{
		std::sort(std::begin(ValidTypes), std::end(ValidTypes));
	});

	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: glm::mat4";
		throw std::logic_error(err);
	}
}
template<>
void ValidateType<GlProj::Graphics::Texture>(GLenum t)
{
	static const constexpr GLenum ValidTypes[] =
	{
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
	if (!std::binary_search(std::begin(ValidTypes), std::end(ValidTypes), t))
	{
		std::string err = "Type mismatch: \n Actual type: ";
		err += t;
		err += "\n Provided type: Texture";
		throw std::logic_error(err);
	}
}

namespace GlProj
{
	namespace Graphics
	{
		void Material::SetUniform(const UniformInformation& u, GLint i)
		{
			ValidateType<GLint>(u.type);
			
			glUniform1iv(u.location, 1, &i);
		}
	}
}
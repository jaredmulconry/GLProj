#include "ShadingProgram.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"
#include "Mesh.hpp"
#include "Shader.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		struct UniformNameLess
		{
			template<typename T, typename U>
			inline bool operator()(const T& x, const U& y)
			{
				return x.first < y.first;
			}
			template<typename T>
			inline bool operator()(const T& x, const std::string& y)
			{
				return x.first < y;
			}
		};

		bool operator==(const ShadingProgram& x, const ShadingProgram& y) noexcept
		{
			return x.programHandle == y.programHandle;
		}
		bool operator!=(const ShadingProgram& x, const ShadingProgram& y) noexcept
		{
			return !(x == y);
		}
		ShadingProgram::ShadingProgram(ShadingProgram&& x) noexcept
			: programHandle(x.programHandle)
			, attributes(std::move(x.attributes))
			, uniforms(std::move(x.uniforms))
			, transformsAreBatchable(x.transformsAreBatchable)
		{
			x.programHandle = invalidHandle;
		}
		ShadingProgram& ShadingProgram::operator=(ShadingProgram&& x) noexcept
		{
			if (this != &x)
			{
				if (programHandle != invalidHandle)
				{
					glDeleteProgram(programHandle);
				}
				programHandle = x.programHandle;
				attributes = std::move(x.attributes);
				uniforms = std::move(x.uniforms);
				transformsAreBatchable = x.transformsAreBatchable;
				x.programHandle = invalidHandle;
			}

			return *this;
		}
		ShadingProgram::~ShadingProgram()
		{
			if (programHandle != invalidHandle)
			{
				glDeleteProgram(programHandle);
			}
		}
		ShadingProgram::ShadingProgram(GLuint handle) noexcept
			: programHandle(handle)
		{}
		GLuint ShadingProgram::GetHandle() const noexcept
		{
			return programHandle;
		}
		ShadingProgram::VertexAttribConstIterator ShadingProgram::FindAttribute(const std::string& name) const
		{
			return std::find_if(attributes.cbegin(), attributes.cend(), [&name](const auto& x)
			{
				return name == x.name;
			});
		}
		ShadingProgram::VertexAttribConstIterator ShadingProgram::FindAttribute(GLint loc) const
		{
			return std::find_if(attributes.cbegin(), attributes.cend(), [&loc](const auto& x)
			{
				return loc == x.location;
			});
		}
		ShadingProgram::VertexAttribConstIterator ShadingProgram::FindAttribute(MeshSlots slot) const
		{
			return FindAttribute(MeshSlotToGL(slot));
		}
		ShadingProgram::UniformInfoConstIterator ShadingProgram::FindUniform(const std::string& name) const
		{
			auto pos = std::lower_bound(uniformNameRef.cbegin(), uniformNameRef.cend(), name, UniformNameLess{});
			if (pos != uniformNameRef.cend() && pos->first == name)
			{
				auto dist = pos->second - uniforms.data();
				return uniforms.cbegin() + dist;
			}
			return uniforms.cend();
		}
		ShadingProgram::UniformInfoConstIterator ShadingProgram::FindUniform(GLint loc) const
		{
			return std::find_if(uniforms.cbegin(), uniforms.cend(), [loc](const auto& x)
			{
				return loc == x.location;
			});
		}

		ShadingProgram::VertexAttribConstIterator ShadingProgram::AttributesEnd() const
		{
			return attributes.cend();
		}
		ShadingProgram::UniformInfoConstIterator ShadingProgram::UniformsEnd() const
		{
			return uniforms.cend();
		}

		void ShadingProgram::Bind() const noexcept
		{
			glUseProgram(GetHandle());
		}
		void ShadingProgram::FetchProgramInfo()
		{
			attributes.clear();
			uniforms.clear();
			uniformNameRef.clear();

			//In-progress migration from old introspection API to new.
			auto numAttribs = GLint(0);
			glGetProgramInterfaceiv(GetHandle(), GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
			auto maxAttribNameLen = GLint(0);
			glGetProgramInterfaceiv(GetHandle(), GL_PROGRAM_INPUT, GL_MAX_NAME_LENGTH, &maxAttribNameLen);
			attributes.reserve(numAttribs);

			const GLenum attribProperties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };
			std::string attribNameBuf(maxAttribNameLen, '\0');

			for (GLint i = 0; i < numAttribs; ++i)
			{
				GLint attribInfo[sizeof(attribProperties) / sizeof(*attribProperties)];
				glGetProgramResourceiv(GetHandle(), GL_PROGRAM_INPUT, i, sizeof(attribProperties) / sizeof(*attribProperties), attribProperties,
					sizeof(attribProperties) / sizeof(*attribProperties), nullptr, attribInfo);

				attribNameBuf.resize(attribInfo[0], '\0');

				glGetProgramResourceName(GetHandle(), GL_PROGRAM_INPUT, i, attribInfo[0], nullptr, &*(attribNameBuf.begin()));
				attribNameBuf.resize(attribNameBuf.size() - 1);

				attributes.push_back({ attribNameBuf, GLenum(attribInfo[1]), attribInfo[2], attribInfo[3] });
			}

			auto numActiveUniforms = GLint();
			glGetProgramInterfaceiv(GetHandle(), GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
			auto maxUniformNameLen = GLint();
			glGetProgramInterfaceiv(GetHandle(), GL_UNIFORM, GL_MAX_NAME_LENGTH, &maxUniformNameLen);
			uniforms.reserve(numActiveUniforms);

			auto uniformNameBuf = std::string(maxUniformNameLen, '\0');

			const GLenum uniformProperties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };

			for (GLint i = 0; i < numActiveUniforms; ++i)
			{
				GLint uniformInfo[sizeof(uniformProperties) / sizeof(*uniformProperties)];
				glGetProgramResourceiv(GetHandle(), GL_UNIFORM, i, sizeof(uniformProperties) / sizeof(*uniformProperties), uniformProperties,
					sizeof(uniformProperties) / sizeof(*uniformProperties), nullptr, uniformInfo);

				uniformNameBuf.resize(uniformInfo[0], '\0');

				glGetProgramResourceName(GetHandle(), GL_UNIFORM, i, uniformInfo[0], nullptr, &*(uniformNameBuf.begin()));
				uniformNameBuf.resize(uniformNameBuf.size() - 1);

				uniforms.push_back({ uniformNameBuf, GLenum(uniformInfo[1]), uniformInfo[2], uniformInfo[3] });
			}
			
			std::transform(uniforms.begin(), uniforms.end(), std::back_inserter(uniformNameRef), 
				[](auto& x) -> UniformNameBufStorage::value_type
			{
				return{ x.name, &x };
			});
			std::sort(uniformNameRef.begin(), uniformNameRef.end(), UniformNameLess{});
		}
	}
}

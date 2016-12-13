#pragma once
#include "gl_core_4_5.h"
#include "LocalSharedPtr.hpp"
#include <initializer_list>
#include <string>

namespace GlProj
{
	namespace Utilities
	{
		std::string NormalisePath(const std::string& path);
	}
	namespace Graphics
	{
		class ShaderManager;
		class Shader;
		class ShadingProgram;

		using GlProj::Utilities::LocalSharedPtr;

		ShaderManager* GetShaderManager();

		LocalSharedPtr<Shader> LoadShader(ShaderManager*, GLenum, const std::string&, bool = false);
		LocalSharedPtr<Shader> RegisterShader(ShaderManager*, GLenum, GLuint, const std::string&, bool = false);

		LocalSharedPtr<Shader> FindCachedShaderByPath(const ShaderManager*, const std::string&);
		LocalSharedPtr<Shader> FindCachedShaderByName(const ShaderManager*, const std::string&);

		void ReleaseUnused(ShaderManager*);

		LocalSharedPtr<ShadingProgram> GenerateProgram();
		void AttachShader(ShadingProgram*, Shader*);
		void DetachShader(ShadingProgram*, Shader*);
		void LinkProgram(ShadingProgram*);
	}
}
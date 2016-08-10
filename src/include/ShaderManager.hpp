#pragma once
#include "OpenGLTypes.hpp"
#include <initializer_list>
#include <memory>
#include <string>

namespace GlProj
{
	namespace Graphics
	{
		class ShaderManager;
		class Shader;
		class ShadingProgram;

		ShaderManager* GetShaderManager();

		std::shared_ptr<Shader> LoadShader(ShaderManager*, GLenum, const std::string&, bool = false);
		std::shared_ptr<Shader> RegisterShader(ShaderManager*, GLenum, GLuint, const std::string&, bool = false);

		std::shared_ptr<Shader> FindCachedShaderByPath(const ShaderManager*, const std::string&);
		std::shared_ptr<Shader> FindCachedShaderByName(const ShaderManager*, const std::string&);

		void ReleaseUnused(ShaderManager*);

		std::shared_ptr<ShadingProgram> GenerateProgram();
		void AttachShader(ShadingProgram*, Shader*);
		void DetachShader(ShadingProgram*, Shader*);
		void LinkProgram(ShadingProgram*);
	}
}
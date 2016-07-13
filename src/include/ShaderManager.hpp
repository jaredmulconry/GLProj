#pragma once
#include "OpenGLTypes.hpp"
#include <memory>
#include <string>

namespace GlProj
{
	namespace Graphics
	{
		class ShaderManager;
		class Shader;

		ShaderManager* GetShaderManager();

		std::shared_ptr<Shader> LoadShader(ShaderManager*, const std::string&, bool = false);
		std::shared_ptr<Shader> RegisterShader(ShaderManager*, GLenum, GLuint, const std::string&, bool = false);
		std::shared_ptr<Shader> FindCachedShaderByPath(const ShaderManager*, const std::string&);
		std::shared_ptr<Shader> FindCachedShaderByName(const ShaderManager*, const std::string&);
		void ReleaseUnused(ShaderManager*);
	}
}
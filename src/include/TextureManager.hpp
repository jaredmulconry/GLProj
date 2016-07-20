#pragma once
#include "OpenGLTypes.hpp"
#include <memory>
#include <string>

namespace GlProj
{
	namespace Graphics
	{
		class TextureManager;
		class Texture;
		class Sampler;

		TextureManager* GetTextureManager();

		std::shared_ptr<Texture> LoadTexture(TextureManager*, const std::string&, bool = false);
		std::shared_ptr<Texture> RegisterTexture(TextureManager*, GLenum, GLuint, const std::string&, bool = false);
		std::shared_ptr<Texture> FindCachedTextureByPath(const TextureManager*, const std::string&);
		std::shared_ptr<Texture> FindCachedTextureByName(const TextureManager*, const std::string&);
		void ReleaseUnused(TextureManager*);

		std::unique_ptr<Sampler> GenerateSampler();
	}
}
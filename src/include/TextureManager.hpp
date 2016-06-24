#pragma once
#include "OpenGLTypes.hpp"
#include <memory>

namespace GlProj
{
	namespace Graphics
	{
		class TextureManager;
		class Texture;

		TextureManager* GetTextureManager();

		std::shared_ptr<Texture> LoadTexture(TextureManager*, const std::string&);
		std::shared_ptr<Texture> RegisterTexture(TextureManager*, GLuint, const std::string&);
		std::shared_ptr<Texture> FindCachedTextureByPath(TextureManager*, const std::string&);
		std::shared_ptr<Texture> FindCachedTextureByName(TextureManager*, const std::string&);
		void ReleaseUnused(TextureManager*);
	}
}
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

		std::shared_ptr<Texture> LoadTexture(TextureManager*, const char*);
	}
}
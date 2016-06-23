#include "TextureManager.hpp"
#include "Texture.hpp"
#include "stb_image.h"

namespace GlProj
{
	namespace Graphics
	{
		class TextureManager
		{

		};

		TextureManager* GetTextureManager()
		{
			static TextureManager instance;
			return &instance;
		}
		std::shared_ptr<Texture> LoadTexture(TextureManager* manager, const char* path)
		{
			int width, height, components;
			auto pixelData = stbi_load(path, &width, &height, &components, 0);
			return std::shared_ptr<Texture>();
		}
	}
}
#pragma once
#include "gl_core_4_5.h"
#include "LocalSharedPtr.hpp"
#include <memory>
#include <string>

namespace GlProj
{
	namespace Graphics
	{
		class TextureManager;
		class Texture;
		class Sampler;

		using GlProj::Utilities::LocalSharedPtr;

		TextureManager* GetTextureManager();

		LocalSharedPtr<Texture> LoadTexture(TextureManager*, const std::string&, bool = false);
		LocalSharedPtr<Texture> RegisterTexture(TextureManager*, GLenum, GLuint, const std::string&, bool = false);
		LocalSharedPtr<Texture> FindCachedTextureByPath(const TextureManager*, const std::string&);
		LocalSharedPtr<Texture> FindCachedTextureByName(const TextureManager*, const std::string&);
		void ReleaseUnused(TextureManager*);

		LocalSharedPtr<Sampler> GenerateSampler();
	}
}
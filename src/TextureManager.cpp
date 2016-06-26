#include "TextureManager.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"
#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <experimental\filesystem>
#include <stdexcept>
#include <unordered_map>
#include <utility>

using namespace std::experimental::filesystem;

namespace GlProj
{
	namespace Graphics
	{
		struct stbiImageDeleter
		{
			void operator()(stbi_uc* p)
			{
				stbi_image_free(p);
			}
		};

		class TextureManager
		{
			std::unordered_map<std::string, std::weak_ptr<Texture>> registeredTextures;
		public:
			std::shared_ptr<Texture> RegisterTexture(GLuint handle, const std::string& name)
			{
				auto newPtr = std::make_shared<Texture>(handle);
				registeredTextures[name] = newPtr;
				return std::move(newPtr);
			}
			std::shared_ptr<Texture> FindByName(const std::string& name) const
			{
				auto found = registeredTextures.find(name);
				if (found == registeredTextures.end() || found->second.expired())
				{
					return nullptr;
				}

				return found->second.lock();
			}

			void CleanUpDangling()
			{
				auto begin = registeredTextures.begin();

				while (begin != registeredTextures.end())
				{
					if (!begin->second.expired())
					{
						++begin;
						continue;
					}
					begin = registeredTextures.erase(begin);
				}
			}
		};

		using stbiDataPtr = std::unique_ptr<stbi_uc, stbiImageDeleter>;

		TextureManager* GetTextureManager()
		{
			static TextureManager instance;
			return &instance;
		}

		std::shared_ptr<Texture> LoadTexture(TextureManager* manager, const std::string& path, bool replace)
		{
			if(!replace)
			{
				auto ptr = FindCachedTextureByPath(manager, path);
				if (ptr != nullptr)
				{
					return std::move(ptr);
				}
			}

			stbiImageDeleter deleter;
			int width, height, components;
			auto pixelData = stbiDataPtr(stbi_load(path.c_str(), &width, &height, &components, 0), deleter);

			if (pixelData == nullptr)
			{
				std::string err = "Loading of image file failed.\n";
				err += "Path: ";
				err += path;
				err += "\nReason: ";
				err += stbi_failure_reason();
				err += '\n';
				throw std::runtime_error(err);
			}

			GLint internalFormat, externalFormat;

			switch (components)
			{
			default:
				throw std::runtime_error("Invalid number of components in texture.");
			case 1:
				internalFormat = GL_R8;
				externalFormat = GL_RED;
				break;
			case 2:
				internalFormat = GL_RG_INTEGER; 
				externalFormat = GL_RG;
				break;
			case 3:
				internalFormat = GL_RGB_INTEGER;
				externalFormat = GL_RGB;
				break;
			case 4:
				internalFormat = GL_RGBA_INTEGER;
				externalFormat = GL_RGBA;
				break;
			}

			GLuint handle;
			glGenTextures(1, &handle);

			glBindTexture(GL_TEXTURE_2D, handle);

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, pixelData.get());

			return manager->RegisterTexture(handle, canonical(path).u8string());
		}
		std::shared_ptr<Texture> RegisterTexture(TextureManager* manager, GLuint handle, const std::string& name, bool replace)
		{
			if (!replace)
			{
				auto ptr = FindCachedTextureByName(manager, name);
				if (ptr != nullptr)
				{
					return std::move(ptr);
				}
			}

			return manager->RegisterTexture(handle, name);
		}

		std::shared_ptr<Texture> FindCachedTextureByPath(const TextureManager* manager, const std::string& path)
		{
			auto canonicalPath = canonical(path).u8string();
			
			return manager->FindByName(canonicalPath);
		}
		std::shared_ptr<Texture> FindCachedTextureByName(const TextureManager* manager, const std::string& name)
		{
			return manager->FindByName(name);
		}

		void ReleaseUnused(TextureManager* manager)
		{
			manager->CleanUpDangling();
		}
	}
}
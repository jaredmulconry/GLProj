#pragma once
#include <string>

namespace GlProj
{
	namespace Utilities
	{
		class AssetManager;

		AssetManager* OpenProject(const char* projectPath, bool savePrevious = true);
	}
}
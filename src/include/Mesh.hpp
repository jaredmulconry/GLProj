#pragma once
#include "OpenGLTypes.hpp"
#include "MeshDataBuffer.hpp"
#include <vector>

struct aiMesh;

namespace GlProj
{
	namespace Graphics
	{
		class Mesh
		{
			std::vector<MeshDataBuffer> data;

		public:
			explicit Mesh(const aiMesh*);
		};
	}
}
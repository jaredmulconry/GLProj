#include "Mesh.hpp"
#include "assimp\scene.h"
#include <stdexcept>

GlProj::Graphics::Mesh::Mesh(const aiMesh* mesh)
{
	if (!mesh->HasPositions() || !mesh->HasFaces())
	{
		throw std::runtime_error("Provided mesh has no positions or faces.");
	}
}

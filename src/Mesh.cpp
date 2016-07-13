#include "Mesh.hpp"
#include "assimp\scene.h"
#include "assimp\anim.h"
#include <stdexcept>

GlProj::Graphics::Mesh::Mesh(const aiMesh* mesh)
{
	if (!mesh->HasPositions() || !mesh->HasFaces())
	{
		throw std::runtime_error("Provided mesh has no positions or faces.");
	}

	arrayBuffer.Bind();
	indices = MeshIndexBuffer(mesh->mNumFaces, mesh->mFaces);
	indices.Bind();

	vertexData.resize(ReservedVertexSlots);

	vertexData[Mesh::Positions] = MeshDataBuffer(GL_ARRAY_BUFFER, 
												 mesh->mNumVertices * sizeof(*mesh->mVertices), 
												 mesh->mVertices, 
												 GL_STATIC_DRAW);
	if (mesh->HasNormals())
	{
		vertexData[Mesh::Normals] = MeshDataBuffer(GL_ARRAY_BUFFER,
												   mesh->mNumVertices * sizeof(*mesh->mNormals),
												   mesh->mNormals,
												   GL_STATIC_DRAW);
	}
	if (mesh->HasTangentsAndBitangents())
	{
		vertexData[Mesh::Tangents] = MeshDataBuffer(GL_ARRAY_BUFFER,
			mesh->mNumVertices * sizeof(*mesh->mTangents),
			mesh->mTangents,
			GL_STATIC_DRAW);
		vertexData[Mesh::BiTangents] = MeshDataBuffer(GL_ARRAY_BUFFER,
			mesh->mNumVertices * sizeof(*mesh->mBitangents),
			mesh->mBitangents,
			GL_STATIC_DRAW);
	}
	int uvChannelCount = static_cast<int>(mesh->GetNumUVChannels());
	for (int i = 0; i < uvChannelCount; ++i)
	{
		if (mesh->HasTextureCoords(i))
		{
			vertexData[Mesh::TexCoord0 + i] = MeshDataBuffer(GL_ARRAY_BUFFER,
				mesh->mNumVertices * mesh->mNumUVComponents[i],
				mesh->mTextureCoords[i],
				GL_STATIC_DRAW);
		}
	}
	int colourChannelCount = static_cast<int>(mesh->GetNumColorChannels());
	for (int i = 0; i < colourChannelCount; ++i)
	{
		if (mesh->HasVertexColors(i))
		{
			vertexData[Mesh::Colour0 + i] = MeshDataBuffer(GL_ARRAY_BUFFER,
				mesh->mNumVertices * sizeof(*mesh->mColors),
				mesh->mColors[i],
				GL_STATIC_DRAW);
		}
	}
}

bool GlProj::Graphics::operator==(const Mesh& x, const Mesh& y) noexcept
{
	return x.vertexData == y.vertexData && x.indices == y.indices && x.arrayBuffer == y.arrayBuffer;
}

bool GlProj::Graphics::operator!=(const Mesh& x, const Mesh& y) noexcept
{
	return !(x == y);
}

#include "Mesh.hpp"
#include "assimp\scene.h"
#include "assimp\anim.h"
#include <algorithm>
#include <stdexcept>

namespace GlProj
{
	namespace Graphics
	{
		const constexpr int MaxTextureCoordinates(4);
		const constexpr int MaxColourChannels(2);
		const constexpr int MaxColorChannels(2);

		Mesh::Mesh(const aiMesh* mesh)
		{
			if (!mesh->HasPositions() || !mesh->HasFaces())
			{
				throw std::runtime_error("Provided mesh has no positions or faces.");
			}

			arrayBuffer.Bind();
			indices = MeshIndexBuffer(mesh->mNumFaces, mesh->mFaces);

			vertexData.resize(ReservedVertexSlots);

			vertexData[MeshSlots::Positions] = MeshDataBuffer(GL_ARRAY_BUFFER,
				mesh->mNumVertices * sizeof(*mesh->mVertices),
				mesh->mVertices,
				GL_FLOAT,
				3);
			EnableAttribute(MeshSlots::Positions);
			SetAttributePointer(MeshSlots::Positions);

			if (mesh->HasNormals())
			{
				vertexData[MeshSlots::Normals] = MeshDataBuffer(GL_ARRAY_BUFFER,
					mesh->mNumVertices * sizeof(*mesh->mNormals),
					mesh->mNormals,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::Normals);
				SetAttributePointer(MeshSlots::Normals);
			}
			if (mesh->HasTangentsAndBitangents())
			{
				vertexData[MeshSlots::Tangents] = MeshDataBuffer(GL_ARRAY_BUFFER,
					mesh->mNumVertices * sizeof(*mesh->mTangents),
					mesh->mTangents,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::Tangents);
				SetAttributePointer(MeshSlots::Tangents);

				vertexData[MeshSlots::BiTangents] = MeshDataBuffer(GL_ARRAY_BUFFER,
					mesh->mNumVertices * sizeof(*mesh->mBitangents),
					mesh->mBitangents,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::BiTangents);
				SetAttributePointer(MeshSlots::BiTangents);
			}
			int uvChannelCount = std::min(static_cast<int>(mesh->GetNumUVChannels()), MaxTextureCoordinates);
			for (int i = 0; i < uvChannelCount; ++i)
			{
				if (mesh->HasTextureCoords(i))
				{
					vertexData[MeshSlots::TexCoord0 + i] = MeshDataBuffer(GL_ARRAY_BUFFER,
						mesh->mNumVertices * mesh->mNumUVComponents[i],
						mesh->mTextureCoords[i],
						GL_FLOAT,
						mesh->mNumUVComponents[i]);
					EnableAttribute(MeshSlots(MeshSlots::TexCoord0 + i));
					SetAttributePointer(MeshSlots(MeshSlots::TexCoord0 + i));
				}
			}
			int colourChannelCount = std::min(static_cast<int>(mesh->GetNumColorChannels()), MaxColourChannels);
			for (int i = 0; i < colourChannelCount; ++i)
			{
				if (mesh->HasVertexColors(i))
				{
					vertexData[MeshSlots::Colour0 + i] = MeshDataBuffer(GL_ARRAY_BUFFER,
						mesh->mNumVertices * sizeof(*mesh->mColors),
						mesh->mColors[i],
						GL_FLOAT,
						4);
					EnableAttribute(MeshSlots(MeshSlots::Colour0 + i));
					SetAttributePointer(MeshSlots(MeshSlots::Colour0 + i));
				}
			}
		}

		const MeshDataBuffer& Mesh::GetMeshData(MeshSlots s) const
		{
			return vertexData[s];
		}
		void Mesh::Bind() const noexcept
		{
			arrayBuffer.Bind();
		}

		void Mesh::SetAttributePointer(MeshSlots slot)
		{
			auto& meshData = GetMeshData(slot);
			if (meshData.GetHandle() == meshData.invalidHandle)
			{
				return;
			}
			Bind();
			meshData.Bind();

			glVertexAttribPointer(slot, meshData.GetElementsPerVertex(),
				meshData.GetDataType(), GL_FALSE, 0, nullptr);
		}
		void Mesh::EnableAttribute(MeshSlots s)
		{
			glEnableVertexAttribArray(s);
		}
		void Mesh::DisableAttribute(MeshSlots s)
		{
			glDisableVertexAttribArray(s);
		}

		bool operator==(const Mesh& x, const Mesh& y) noexcept
		{
			return x.vertexData == y.vertexData && x.indices == y.indices && x.arrayBuffer == y.arrayBuffer;
		}
		bool operator!=(const Mesh& x, const Mesh& y) noexcept
		{
			return !(x == y);
		}
	}
}

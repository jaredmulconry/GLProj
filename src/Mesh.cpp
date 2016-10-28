#include "Mesh.hpp"
#include "assimp/scene.h"
#include "assimp/anim.h"
#include <algorithm>
#include <stdexcept>

namespace GlProj
{
	namespace Graphics
	{
		Mesh::Mesh(const aiMesh* mesh)
		{
			if (!mesh->HasPositions() || !mesh->HasFaces())
			{
				throw std::runtime_error("Provided mesh has no positions or faces.");
			}
			if ((mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) != 0)
			{
				throw std::runtime_error("Provided mesh has unsupported primitive type.");
			}

			vertsPerPrimitive = 3;
			if ((mesh->mPrimitiveTypes & aiPrimitiveType_POINT) != 0)
			{
				vertsPerPrimitive = 1;
			}
			else if ((mesh->mPrimitiveTypes & aiPrimitiveType_LINE) != 0)
			{
				vertsPerPrimitive = 2;
			}

			auto faceCount = mesh->mNumFaces;
			primitiveCount = mesh->mNumFaces * vertsPerPrimitive;

			Bind();
			indices = MeshIndexBuffer(faceCount, mesh->mFaces);

			vertexData.resize(ReservedVertexSlots);

			auto positionGL = MeshSlotToGL(MeshSlots::Positions);
			vertexData[positionGL] = MeshDataBuffer(BufferType::array,
				mesh->mNumVertices * sizeof(*mesh->mVertices),
				mesh->mVertices,
				GL_FLOAT,
				3);
			EnableAttribute(MeshSlots::Positions);
			SetAttributePointer(MeshSlots::Positions, vertexData[positionGL]);

			if (mesh->HasNormals())
			{
				auto normalGL = MeshSlotToGL(MeshSlots::Normals);
				vertexData[normalGL] = MeshDataBuffer(BufferType::array,
					mesh->mNumVertices * sizeof(*mesh->mNormals),
					mesh->mNormals,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::Normals);
				SetAttributePointer(MeshSlots::Normals, vertexData[normalGL]);
			}
			if (mesh->HasTangentsAndBitangents())
			{
				auto tangentGL = MeshSlotToGL(MeshSlots::Tangents);
				auto bitangentGL = MeshSlotToGL(MeshSlots::BiTangents);
				vertexData[tangentGL] = MeshDataBuffer(BufferType::array,
					mesh->mNumVertices * sizeof(*mesh->mTangents),
					mesh->mTangents,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::Tangents);
				SetAttributePointer(MeshSlots::Tangents, vertexData[tangentGL]);

				vertexData[bitangentGL] = MeshDataBuffer(BufferType::array,
					mesh->mNumVertices * sizeof(*mesh->mBitangents),
					mesh->mBitangents,
					GL_FLOAT,
					3);
				EnableAttribute(MeshSlots::BiTangents);
				SetAttributePointer(MeshSlots::BiTangents, vertexData[bitangentGL]);
			}
			int uvChannelCount = std::min(static_cast<int>(mesh->GetNumUVChannels()), MaxTextureCoordinates);
			for (int i = 0; i < uvChannelCount; ++i)
			{
				if (mesh->HasTextureCoords(i))
				{
					auto uvGL = MeshSlotToGL(MeshSlots::TexCoord0) + i;
					auto uvSlot = MeshSlots(uvGL);
					vertexData[uvGL] = MeshDataBuffer(BufferType::array,
						mesh->mNumVertices * mesh->mNumUVComponents[i],
						mesh->mTextureCoords[i],
						GL_FLOAT,
						mesh->mNumUVComponents[i]);
					EnableAttribute(uvSlot);
					SetAttributePointer(uvSlot, vertexData[uvGL]);
				}
			}
			int colourChannelCount = std::min(static_cast<int>(mesh->GetNumColorChannels()), MaxColourChannels);
			for (int i = 0; i < colourChannelCount; ++i)
			{
				if (mesh->HasVertexColors(i))
				{
					auto colourGL = MeshSlotToGL(MeshSlots::Colour0) + i;
					auto colourSlot = MeshSlots(colourGL);
					vertexData[colourGL] = MeshDataBuffer(BufferType::array,
						mesh->mNumVertices * sizeof(*mesh->mColors),
						mesh->mColors[i],
						GL_FLOAT,
						4);
					EnableAttribute(colourSlot);
					SetAttributePointer(colourSlot, vertexData[colourGL]);
				}
			}

			MeshArrayBuffer::UnBind();
		}

		const MeshDataBuffer& Mesh::GetMeshData(MeshSlots s) const
		{
			return vertexData[MeshSlotToGL(s)];
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
			SetAttributePointer(slot, meshData);
		}
		void Mesh::EnableAttribute(MeshSlots s)
		{
			glEnableVertexAttribArray(MeshSlotToGL(s));
		}
		void Mesh::DisableAttribute(MeshSlots s)
		{
			glDisableVertexAttribArray(MeshSlotToGL(s));
		}

		void Mesh::SetAttributePointer(MeshSlots s, const MeshDataBuffer& b, GLsizei stride, const void* offset)
		{
			glVertexAttribPointer(MeshSlotToGL(s), b.GetElementsPerVertex(),
				b.GetDataType(), GL_FALSE, stride, offset);
		}

		void Mesh::SetAttributeDivisor(MeshSlots s, GLuint u)
		{
			glVertexAttribDivisor(MeshSlotToGL(s), u);
		}

		int Mesh::FindAttributeRange(int size, int offset)
		{
			auto emptyObj = MeshDataBuffer();
			auto pos = std::search_n(vertexData.begin() + int(MeshSlots::User) + offset, vertexData.end(), size, emptyObj);
			if (pos == vertexData.end())
			{
				return -1;
			}

			return int(pos - vertexData.begin());
		}

		GLenum MeshSlotToGL(MeshSlots s)
		{
			return GLenum(s);
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

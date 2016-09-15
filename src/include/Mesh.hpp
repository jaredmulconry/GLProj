#pragma once
#include "OpenGLTypes.hpp"
#include "MeshDataBuffer.hpp"
#include "MeshIndexBuffer.hpp"
#include "MeshArrayBuffer.hpp"
#include <vector>

struct aiMesh;

namespace GlProj
{
	namespace Graphics
	{
		enum class MeshSlots : GLenum
		{
			Positions,
			Normals,
			Tangents,
			BiTangents,
			TexCoord0,
			TexCoord1,
			TexCoord2,
			TexCoord3,
			Colour0,
			Color0 = Colour0,
			Colour1,
			Color1 = Colour1,
			User,
		};

		GLenum MeshSlotToGL(MeshSlots s);

		static const constexpr int MaxTextureCoordinates = 4;
		static const constexpr int MaxColourChannels = 2;
		static const constexpr int MaxColorChannels = MaxColourChannels;

		class Mesh
		{
			std::vector<MeshDataBuffer> vertexData;
			MeshIndexBuffer indices;
			MeshArrayBuffer arrayBuffer;
			unsigned int faceCount;

			static const constexpr int ReservedVertexSlots = 20;

			void SetAttributePointer(MeshSlots);
			void EnableAttribute(MeshSlots);
			void DisableAttribute(MeshSlots);
		public:
			Mesh() = default;
			explicit Mesh(const aiMesh*);
			Mesh(const Mesh&) = delete;
			Mesh(Mesh&&) = default;
			Mesh& operator=(Mesh&&) = default;

			const MeshDataBuffer& GetMeshData(MeshSlots) const;
			void Bind() const noexcept;
			unsigned int FaceCount()const
			{
				return faceCount;
			}

			friend bool operator==(const Mesh&, const Mesh&) noexcept;
			friend bool operator!=(const Mesh&, const Mesh&) noexcept;
		};
	}
}

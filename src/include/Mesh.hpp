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
			Colour0,
			Color0 = Colour0,
			User,
		};

		GLenum MeshSlotToGL(MeshSlots s);

		static const constexpr int MaxTextureCoordinates = 2;
		static const constexpr int MaxColourChannels = 1;
		static const constexpr int MaxColorChannels = MaxColourChannels;

		class Mesh
		{
			std::vector<MeshDataBuffer> vertexData;
			MeshIndexBuffer indices;
			MeshArrayBuffer arrayBuffer;
			unsigned int primitiveCount;
			unsigned int vertsPerPrimitive;

			static const constexpr int ReservedVertexSlots = 16;

			void SetAttributePointer(MeshSlots);
		public:
			Mesh() = default;
			explicit Mesh(const aiMesh*);
			Mesh(const Mesh&) = delete;
			Mesh(Mesh&&) = default;
			Mesh& operator=(Mesh&&) = default;

			static void EnableAttribute(MeshSlots);
			static void DisableAttribute(MeshSlots);
			static void SetAttributePointer(MeshSlots, const MeshDataBuffer&, GLsizei = 0, const void* = nullptr);
			static void SetAttributeDivisor(MeshSlots, GLuint);

			//Searches through the internal vertex data for a range of empty slots
			//into which custom vertex data could be bound. Search begins at
			//the start of User-defined vertex attributes defined by 'User'.
			int FindAttributeRange(int, int = 0);

			const MeshDataBuffer& GetMeshData(MeshSlots) const;
			void Bind() const noexcept;
			unsigned int PrimitiveCount()const noexcept
			{
				return primitiveCount;
			}
			unsigned int VertsPerPrimitive() const noexcept
			{
				return vertsPerPrimitive;
			}

			friend bool operator==(const Mesh&, const Mesh&) noexcept;
			friend bool operator!=(const Mesh&, const Mesh&) noexcept;
		};
	}
}

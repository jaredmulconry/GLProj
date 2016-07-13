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
		class Mesh
		{
			std::vector<MeshDataBuffer> vertexData;
			MeshIndexBuffer indices;
			MeshArrayBuffer arrayBuffer;

			enum MeshSlots
			{
				Positions,
				Normals,
				Tangents,
				BiTangents,
				TexCoord0,
				TexCoord1,
				TexCoord2,
				TexCoord3,
				TexCoord4,
				TexCoord5,
				TexCoord6,
				TexCoord7,
				Colour0,
				Color0 = Colour0,
				Colour1,
				Color1 = Colour1,
				Colour2,
				Color2 = Colour2,
				Colour3,
				Color3 = Colour3,
				Colour4,
				Color4 = Colour4,
				Colour5,
				Color5 = Colour5,
				Colour6,
				Color6 = Colour6,
				Colour7,
				Color7 = Colour7,
			};

			static const constexpr int ReservedVertexSlots = 20;

		public:
			explicit Mesh(const aiMesh*);

			friend bool operator==(const Mesh&, const Mesh&) noexcept;
			friend bool operator!=(const Mesh&, const Mesh&) noexcept;
		};
	}
}
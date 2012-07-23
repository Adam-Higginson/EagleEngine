//A utility class which can build some basic
//geometry to be rendered
#pragma once

#ifndef _GEOMETRY_BUILDER_H_
#define _GEOMETRY_BUILDER_H_

#include <vector>
#include <Windows.h>
#include <xnamath.h>

namespace ee
{
	class GeometryBuilder
	{
	public:
		struct Vertex
		{
			//Create a vertex with no data set
			Vertex() {}
			Vertex(const XMFLOAT3 &pos, const XMFLOAT3 &norm, const XMFLOAT3 &tangent, const XMFLOAT2 &uv)
				: position(pos), normal(norm), tangentU(tangent), texC(uv) {}
			Vertex(
				float posX, float posY, float posZ,
				float normX, float normY, float normZ,
				float tX, float tY, float tZ,
				float texU, float texV)
				: position(posX, posY, posZ), normal(normX, normY, normZ),
				tangentU(tX, tY, tZ), texC(texU, texV) {}


			XMFLOAT3 position;
			XMFLOAT3 normal;
			XMFLOAT3 tangentU;
			XMFLOAT2 texC;

		};

		struct MeshData
		{
			std::vector<Vertex> vertices;
			std::vector<UINT> indices;
		};

		void CreateCube(float length, MeshData &meshData);
		void CreatePlane(float width, float depth, UINT m, UINT n, MeshData &meshData);

	};
}

#endif
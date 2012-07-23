#include "GeometryBuilder.h"

namespace ee
{
	void GeometryBuilder::CreateCube(float length, MeshData &meshData)
	{
		//8 vertices
		meshData.vertices.resize(8);

		float halfLength = length / 2.0f;

		meshData.vertices[0].position = XMFLOAT3(-halfLength, -halfLength, -halfLength);
		meshData.vertices[1].position = XMFLOAT3(-halfLength, halfLength, -halfLength);
		meshData.vertices[2].position = XMFLOAT3(halfLength, halfLength, -halfLength);
		meshData.vertices[3].position = XMFLOAT3(halfLength, -halfLength, -halfLength);
		meshData.vertices[4].position = XMFLOAT3(-halfLength, -halfLength, halfLength);
		meshData.vertices[5].position = XMFLOAT3(-halfLength, halfLength, halfLength);
		meshData.vertices[6].position = XMFLOAT3(halfLength, halfLength, halfLength);
		meshData.vertices[0].position = XMFLOAT3(halfLength, -halfLength, halfLength);

		UINT cubeIndices[] = {
			//front face
			0, 1, 2,
			0, 2, 3,
			//back face
			4, 6, 5,
			4, 7, 6,
			//left face
			4, 5, 1,
			4, 1, 0,
			//right face
			3, 2, 6,
			3, 6, 7,
			//top face
			1, 5, 6,
			1, 6, 2,
			//bottom face
			4, 0, 3,
			4, 3, 7
		};

		meshData.indices.assign(cubeIndices, cubeIndices + 36);
	}

	void GeometryBuilder::CreatePlane(float width, float depth, UINT m, UINT n, MeshData &meshData)
	{
		UINT vertexCount = m * n;
		UINT triCount = (m - 1) * (n - 1) * 2;

		float halfWidth = 0.5f * width;
		float halfDepth = 0.5f * depth;

		float dx = width / (n - 1);
		float dz = depth / (m - 1);

		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		meshData.vertices.resize(vertexCount);
		for (UINT i = 0; i < m; i++)
		{
			float z = halfDepth - i * dz;
			for (UINT j = 0; j < n; j++)
			{
				float x = -halfWidth + j * dx;

				meshData.vertices[i * n + j].position = XMFLOAT3(x, 0.0f, z);

				//Lighting
				meshData.vertices[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				meshData.vertices[i * n + j].tangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

				meshData.vertices[i * n + j].texC.x = j * du;
				meshData.vertices[i * n + j].texC.y = i * dv;
			}
		}

		//Calculate grid indices
		meshData.indices.resize(triCount * 3); //3 indices per face
		UINT k = 0;
		for (UINT i = 0; i < m - 1; i++)
		{
			for (UINT j = 0; j < n - 1; j++)
			{
				meshData.indices[k] = i * n + j;
				meshData.indices[k + 1] = i * n + j + 1;
				meshData.indices[k + 2] = (i + 1) * n + j;
				meshData.indices[k + 3] = (i + 1) * n + j;
				meshData.indices[k + 4] = i * n + j + 1;
				meshData.indices[k + 5] = (i + 1) * n + j + 1;

				k += 6; //for next face
			}
		}
	}
}
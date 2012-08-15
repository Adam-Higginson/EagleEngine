#include "GeometryBuilder.h"

namespace ee
{
	void GeometryBuilder::CreateCube(float length, MeshData &meshData)
	{
		//8 vertices
		meshData.vertices.resize(8);

		float halfLength = length * 0.5f;

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

	void GeometryBuilder::CreateBox(float width, float height, float depth, MeshData &meshData)
	{
		Vertex v[24];

		float w2 = 0.5f*width;
		float h2 = 0.5f*height;
		float d2 = 0.5f*depth;
    
		// Fill in the front face vertex data.
		v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		meshData.vertices.assign(&v[0], &v[24]);
 
		//
		// Create the indices.
		//

		UINT i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7]  = 5; i[8]  = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] =  9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		meshData.indices.assign(&i[0], &i[36]);
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

	void GeometryBuilder::CreateCylinder(float bottomRadius, float topRadius, 
										 float height, UINT sliceCount, 
										 UINT stackCount, MeshData &meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		//Build stacks
		float stackHeight = height / stackCount;

		//Amount to increment radius as we move up each stack level
		float radiusStep = (topRadius - bottomRadius) / stackCount;

		UINT ringCount = stackCount + 1;

		//Compute vertices starting at bottom and moving up
		for (UINT i = 0; i < ringCount; i++)
		{
			float y = -0.5 * height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			//vertices of ring
			float dTheta = 2.0f * XM_PI / sliceCount;
			for (UINT j = 0; j <= sliceCount; j++)
			{
				Vertex vertex;

				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);

				vertex.position = XMFLOAT3(r * c, y, r * s);

				vertex.texC.x = (float) j / sliceCount;
				vertex.texC.y = 1.0f - (float) i / stackCount;

				vertex.tangentU = XMFLOAT3(-s, 0.0f, c);

				float dr = bottomRadius - topRadius;
				XMFLOAT3 bitangent(dr * c, -height, dr * s);

				XMVECTOR T = XMLoadFloat3(&vertex.tangentU);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.normal, N);

				meshData.vertices.push_back(vertex);
			}
		}

		UINT ringVertexCount = sliceCount + 1;

		//Compute indices for each stack
		for (UINT i = 0; i < stackCount; i++)
		{
			for (UINT j = 0; j < sliceCount; j++)
			{
				//Triangle ABC given by (i*n + j, (i + 1) * n + j, (i + 1) * n + j + 1)
				meshData.indices.push_back(i * ringVertexCount + j);
				meshData.indices.push_back((i + 1) * ringVertexCount + j);
				meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);

				//Triangle ACD given by: (i*n + j, (i + 1) * n + j + 1, i * n + j + 1)
				meshData.indices.push_back(i * ringVertexCount + j);
				meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);
				meshData.indices.push_back(i * ringVertexCount + j + 1);
			}
		}

		CreateCylinderTop(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
		CreateCylinderBottom(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	}

	void GeometryBuilder::CreateCylinderTop(float bottomRadius, float topRadius, float height, 
											UINT sliceCount, UINT stackCount, MeshData &meshData)
	{
		UINT baseIndex = (UINT)meshData.vertices.size();

		float y = 0.5f * height;
		float dTheta = 2.0f * XM_PI / sliceCount;

		for (UINT i = 0; i <= sliceCount; i++)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			//Scale down by height to make top cap texture coord proportional to base
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		//Cap center vertex
		meshData.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		//Index of center vertex
		UINT centerIndex = (UINT)meshData.vertices.size() - 1;

		for (UINT i = 0; i < sliceCount; i++)
		{
			meshData.indices.push_back(centerIndex);
			meshData.indices.push_back(baseIndex + i + 1);
			meshData.indices.push_back(baseIndex + i);
		}
	}

	void GeometryBuilder::CreateCylinderBottom(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData &meshData)
	{
			// 
		// Build bottom cap.
		//

		UINT baseIndex = (UINT)meshData.vertices.size();
		float y = -0.5f*height;

		// vertices of ring
		float dTheta = 2.0f*XM_PI/sliceCount;
		for(UINT i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius*cosf(i*dTheta);
			float z = bottomRadius*sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x/height + 0.5f;
			float v = z/height + 0.5f;

			meshData.vertices.push_back( Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
		}

		// Cap center vertex.
		meshData.vertices.push_back( Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

		// Cache the index of center vertex.
		UINT centerIndex = (UINT)meshData.vertices.size()-1;

		for(UINT i = 0; i < sliceCount; ++i)
		{
			meshData.indices.push_back(centerIndex);
			meshData.indices.push_back(baseIndex + i);
			meshData.indices.push_back(baseIndex + i+1);
		}
	}
}
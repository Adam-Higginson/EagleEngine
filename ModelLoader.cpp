#include "ModelLoader.h"

namespace ee
{
	namespace ModelLoader
	{
		bool ModelLoader::LoadModelFromTextFile(const std::string &fileName, MeshData &meshData)
		{
			std::ifstream modelFile(fileName);

			meshData.indices.clear();
			meshData.vertices.clear();

			if (!modelFile)
			{
				MessageBox(NULL, L"Could not open model for reading!", NULL, 0);
				return false;
			}

			UINT vertexCount = 0;
			UINT triCount = 0;
			std::string ignore; //Used to ignore certain data

			modelFile >> ignore >> vertexCount;
			modelFile >> ignore >> triCount;
			modelFile >> ignore >> ignore >> ignore >> ignore;

			float nx, ny, nz;

			meshData.vertices.resize(vertexCount);
			for (UINT i = 0; i < vertexCount; i++)
			{
				modelFile >> meshData.vertices[i].pos.x >> meshData.vertices[i].pos.y >> meshData.vertices[i].pos.z;
				meshData.vertices[i].colour = (const float *)&Colour::WHITE;

				modelFile >> nx >> ny >> nz;
			}

			modelFile >> ignore;
			modelFile >> ignore;
			modelFile >> ignore;

			meshData.indices.resize(3 * triCount);
			for (UINT i = 0; i < triCount; i++)
			{
				modelFile >> meshData.indices[i * 3] >> meshData.indices[i * 3  + 1] >> meshData.indices[i * 3 + 2];
			}

			modelFile.close();

			return true;
		}
	}
}
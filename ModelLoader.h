#pragma once

#ifndef _MODEL_LOADER_H_
#define _MODEL_LOADER_H_

#include "Graphics.h"

namespace ee
{
	namespace ModelLoader
	{
		struct Vertex
		{
			XMFLOAT3 pos;
			XMFLOAT4 colour;
		};

		struct MeshData
		{
			std::vector<Vertex> vertices;
			std::vector<UINT> indices;
		};
		bool LoadModelFromTextFile(const std::string &fileName, MeshData &meshData);
	}
}

#endif
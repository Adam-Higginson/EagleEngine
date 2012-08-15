#pragma once

#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "Graphics.h"
#include "EffectsUtil.h"

namespace ee
{
	namespace Vertex
	{
		//Pos and normal vertex structure
		struct PosNormal
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
		};
		//Pos/normal and tex
		struct PosNormalTex
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT2 tex;
		};
	}

	class InputLayoutDesc
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC posNormalDesc[2];
		static const D3D11_INPUT_ELEMENT_DESC posNormalTexDesc[3];
	};

	class InputLayouts
	{
	public:
		static void Init(ID3D11Device *device);
		static void Release();

		static ID3D11InputLayout *posNormalLayout;
		static ID3D11InputLayout *posNormalTexLayout;
	};
}

#endif

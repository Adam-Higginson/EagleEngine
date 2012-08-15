#include "Vertex.h"
namespace ee
{
	const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::posNormalDesc[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::posNormalTexDesc[3] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ID3D11InputLayout *InputLayouts::posNormalLayout = NULL;
	ID3D11InputLayout *InputLayouts::posNormalTexLayout = NULL;

	void InputLayouts::Init(ID3D11Device *device)
	{
		D3DX11_PASS_DESC passDesc;

		//PosNormal
		/*Effects::basicFX->light1SpotTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(device->CreateInputLayout(InputLayoutDesc::posNormalDesc, 2, passDesc.pIAInputSignature, 
									passDesc.IAInputSignatureSize, &posNormalLayout));*/

		//PosNormalTex
		Effects::basicFX->light1SpotTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(device->CreateInputLayout(InputLayoutDesc::posNormalTexDesc, 3, passDesc.pIAInputSignature, 
									passDesc.IAInputSignatureSize, &posNormalTexLayout));

	}

	void InputLayouts::Release()
	{
		if (posNormalLayout)
		{
			posNormalLayout->Release();
			posNormalLayout = NULL;
		}
		if (posNormalTexLayout)
		{
			posNormalTexLayout->Release();
			posNormalTexLayout = NULL;
		}
	}
}

#include "EffectsUtil.h"

namespace ee
{
#pragma region Effect
	Effect::Effect(ID3D11Device *device, const std::wstring &fileName)
		: m_effect(NULL)
	{
		std::ifstream file(fileName, std::ios::binary);

		file.seekg(0, std::ios_base::end);
		int size = (int)file.tellg();
		file.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);

		file.read(&compiledShader[0], size);
		file.close();

		HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &m_effect));
	}

	Effect::~Effect()
	{
		if (m_effect)
		{
			m_effect->Release();
			m_effect = NULL;
		}
	}

#pragma endregion

#pragma region BasicEffect

	BasicEffect::BasicEffect(ID3D11Device *device, const std::wstring &fileName)
		: Effect(device, fileName)
	{
		light1SpotTech = m_effect->GetTechniqueByName("Light1Spot");
		light1SpotTexTech = m_effect->GetTechniqueByName("Light1SpotTex");

		light1Tech = m_effect->GetTechniqueByName("Light1");
		light2Tech = m_effect->GetTechniqueByName("Light2");
		light3Tech = m_effect->GetTechniqueByName("Light3");

		light0TexTech = m_effect->GetTechniqueByName("Light0Tex");
		light1TexTech = m_effect->GetTechniqueByName("Light1Tex");
		light2TexTech = m_effect->GetTechniqueByName("Light2Tex");
		light3TexTech = m_effect->GetTechniqueByName("Light3Tex");

		worldViewProj		= m_effect->GetVariableByName("gWorldViewProj")->AsMatrix();
		world				= m_effect->GetVariableByName("gWorld")->AsMatrix();
		worldInvTranspose	= m_effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
		texTransform		= m_effect->GetVariableByName("gTexTransform")->AsMatrix();
		eyePosW				= m_effect->GetVariableByName("gEyePosW")->AsVector();
		dirLights			= m_effect->GetVariableByName("gDirLights");
		spotLight			= m_effect->GetVariableByName("gSpotLight");
		staticSpotLight		= m_effect->GetVariableByName("gStaticSpotLight");
		pointLight			= m_effect->GetVariableByName("gPointLight");
		mat					= m_effect->GetVariableByName("gMaterial");
		diffuseMap			= m_effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	}

	BasicEffect::~BasicEffect()
	{
	}

#pragma endregion

#pragma region Effects
	BasicEffect *Effects::basicFX = NULL;

	void Effects::Init(ID3D11Device *device)
	{
		basicFX = new BasicEffect(device, L"BasicShader.fxo");

	}

	void Effects::Release()
	{
		if (basicFX)
		{
			delete basicFX;
			basicFX = NULL;
		}
	}

#pragma endregion

}
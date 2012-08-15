#pragma once

#ifndef _EFFECTS_H_
#define _EFFECTS_H_

#include "Graphics.h"
namespace ee
{
	class Effect
	{
	public:
		Effect(ID3D11Device *device, const std::wstring &fileName);
		virtual ~Effect();

	private:
		Effect(const Effect &rhs);
		Effect& operator=(const Effect &rhs);

	protected:
		ID3DX11Effect *m_effect;
	};

	class BasicEffect : public Effect
	{
	public:
		BasicEffect(ID3D11Device *device, const std::wstring &fileName);
		~BasicEffect();

		void SetWorldViewProj(CXMMATRIX M)					{ worldViewProj->SetMatrix(reinterpret_cast<const float *>(&M)); }
		void SetWorld(CXMMATRIX M)							{ world->SetMatrix(reinterpret_cast<const float *>(&M)); }
		void SetWorldInvTranspose(CXMMATRIX M)				{ worldInvTranspose->SetMatrix(reinterpret_cast<const float *>(&M)); }
		void SetTexTransform(CXMMATRIX M)					{ texTransform->SetMatrix(reinterpret_cast<const float *>(&M)); }
		void SetEyePosW(const XMFLOAT3 &v)					{ eyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetDirLights(const DirectionalLight *lights)	{ dirLights->SetRawValue(lights, 0, sizeof(DirectionalLight)); }
		void SetSpotLight(const SpotLight &light)			{ spotLight->SetRawValue(&light, 0, sizeof(SpotLight)); }
		void SetStaticSpotLight(const SpotLight &light)		{ staticSpotLight->SetRawValue(&light, 0, sizeof(SpotLight)); }
		void SetPointLight(const PointLight &light)			{ pointLight->SetRawValue(&light, 0, sizeof(PointLight)); }
		void SetMaterial(const Material &material)			{ mat->SetRawValue(&material, 0, sizeof(Material)); }
		void SetDiffuseMap(ID3D11ShaderResourceView *tex)	{	diffuseMap->SetResource(tex); }

		ID3DX11EffectTechnique *light1SpotTech;
		ID3DX11EffectTechnique *light1SpotTexTech;

		ID3DX11EffectTechnique *light1Tech;
		ID3DX11EffectTechnique *light2Tech;
		ID3DX11EffectTechnique *light3Tech;

		ID3DX11EffectTechnique *light0TexTech;
		ID3DX11EffectTechnique *light1TexTech;
		ID3DX11EffectTechnique *light2TexTech;
		ID3DX11EffectTechnique *light3TexTech;

		ID3DX11EffectMatrixVariable *worldViewProj;
		ID3DX11EffectMatrixVariable *world;
		ID3DX11EffectMatrixVariable *worldInvTranspose;
		ID3DX11EffectMatrixVariable *texTransform;
		ID3DX11EffectVectorVariable *eyePosW;
		ID3DX11EffectVariable		*dirLights;
		ID3DX11EffectVariable		*spotLight;
		ID3DX11EffectVariable		*staticSpotLight;
		ID3DX11EffectVariable		*pointLight;
		ID3DX11EffectVariable		*mat;

		ID3DX11EffectShaderResourceVariable *diffuseMap;
	};
	
#pragma region Effects
	class Effects
	{
	public:
		static void Init(ID3D11Device *device);
		static void Release();

		static BasicEffect *basicFX;
	};

}
#pragma endregion

#endif
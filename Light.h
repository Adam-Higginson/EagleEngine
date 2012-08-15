#pragma once

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Windows.h>
#include <xnamath.h>

namespace ee
{
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT3 direction;
		float padding; //Pad last float for array of lights
	};

	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		//Packed in 4d vector: (pos, range)
		XMFLOAT3 position;
		float range;

		//Packed into 4d vector: (A0, A1, A2, padding)
		XMFLOAT3 att;
		float padding;
	};

	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		//packed into 4d vector: (pos, range}
		XMFLOAT3 position;
		float range;

		//Packed into 4d vector: dir spot
		XMFLOAT3 direction;
		float spot;

		XMFLOAT3 att;
		float padding;
	};

}

#endif
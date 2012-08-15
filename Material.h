#pragma once

#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include <Windows.h>
#include <xnamath.h>

namespace ee
{
	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular; // w = specPower
		XMFLOAT4 reflect;
	};
};

#endif
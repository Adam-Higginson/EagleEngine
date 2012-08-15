#include "LightUtil.fx"

//Constant buffers per frame
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[1];
	SpotLight		 gSpotLight;
	SpotLight		 gStaticSpotLight;
	PointLight		 gPointLight;
	float3 gEyePosW;

	float gFogStart;
	float gFogRange;
	float4 gFogColour;
};

//Constant buffer object
cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
	 
};

//Cannot be added to buffer
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 posL		: POSITION;
    float3 normalL  : NORMAL;
	float2 tex		: TEXCOORD;
};

struct VertexOut
{
	float4 posH		: SV_POSITION;
    float3 posW		: POSITION;
	float3 normalW  : NORMAL;
	float2 tex		: TEXCOORD;
};

//Vertex Shader. Returns a VertexOut struct
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	//Transofrm to world space
	vout.posW = mul(float4(vin.posL, 1.0f), gWorld).xyz;
	vout.normalW = mul(vin.normalL, (float3x3)gWorldInvTranspose);

	//Transform to homogeneous clip space
	vout.posH = mul(float4(vin.posL, 1.0f), gWorldViewProj);

	//Output vertex attributes for interpolation accross triangle
	vout.tex = mul(float4(vin.tex, 0.0f, 1.0f), gTexTransform).xy;
    
    return vout;
}

//Pixel Shader, returns a float4 object. SV_Target states the
//output must be the same type as specified in the render target
float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexture, uniform bool isSpotlight) : SV_Target
{
	pin.normalW = normalize(pin.normalW);
	float3 toEye = gEyePosW - pin.posW;

	float distToEye = length(toEye);

	//Normalize
	toEye /= distToEye;

	float4 texColour = float4(1, 1, 1, 1);
	if (gUseTexture)
	{
		texColour = gDiffuseMap.Sample(samAnisotropic, pin.tex);
	}



	//lighting
	float4 litColour = texColour;
	if (gLightCount > 0)
	{
		//start with a sum of 0
		float4 ambient	= float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse	= float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec		= float4(0.0f, 0.0f, 0.0f, 0.0f);

		[unroll]
		for (int i = 0; i < 1; ++i)
		{
			float4 A, D, S;
			/*ComputeDirectionalLight(gMaterial, gDirLights[i], pin.normalW, toEye, A, D, S);
			ambient += A;
			diffuse += D;
			spec    += S;*/

			if (isSpotlight)
			{
				ComputeSpotLight(gMaterial, gSpotLight, pin.posW, pin.normalW, toEye, A, D, S);
				ambient += A;
				diffuse += D;
				spec	+= S;
			}

			ComputeSpotLight(gMaterial, gStaticSpotLight, pin.posW, pin.normalW, toEye, A, D, S);
			//ComputePointLight(gMaterial, gPointLight, pin.posW, pin.normalW, toEye, A, D, S);
			ambient += A;
			diffuse += D;
			spec	+= S;


		}

		//Modulate with late add
		litColour = texColour * (ambient + diffuse) + spec;
		litColour.a = gMaterial.diffuse.a * texColour.a;



	}

	//Take alpha from diffuse material and texture
	//litColour.a = gMaterial.diffuse.a * texColour.a;

	return litColour;
}

//Technique. This consists of one or more passes which are used to
//create a specific rendering technique. Each pass renders the geometry
//in a different way
technique11 Light1Spot
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, false, true)));
	}
}

technique11 Light1SpotTex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, true, true)));
	}
}

technique11 Light1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, false, false)));
	}
}

technique11 Light2
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, false, false)));
	}
}

technique11 Light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, false, false)));
	}
}

technique11 Light0Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true, false) ) );
    }
}

technique11 Light1Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true, false) ) );
    }
}

technique11 Light2Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true, false) ) );
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, false) ) );
    }
}
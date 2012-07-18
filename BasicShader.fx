//Constant buffer object
cbuffer cbPerObject
{
	float4x4 gWorldViewProj; 
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

//Vertex Shader. Returns a VertexOut struct
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}

//Pixel Shader, returns a float4 object. SV_Target states the
//output must be the same type as specified in the render target
float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

//Technique. This consists of one or more passes which are used to
//create a specific rendering technique. Each pass renders the geometry
//in a different way
technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float padding;
};

struct PointLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float range;

	float3 att;
	float padding;
};

struct SpotLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float range;

	float3 direction;
	float spot;

	float3 att;
	float padding;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular; //w = specPower
	float4 ref; //reflect
};

//Util functions to compute ambient, diffuse and specular terms in lighting
//equations.

void ComputeDirectionalLight(Material mat, DirectionalLight L,
							 float3 normal, float3 toEye,
							 out float4 ambient,
							 out float4 diffuse,
							 out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Light vector opposite direction of light rays
	float3 lightVec = -L.direction;

	//add ambient term
	ambient = mat.ambient * L.ambient;

	//Add diffuse and spec term, if surface is in line of site
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}
}

void ComputePointLight(Material mat, PointLight L,
							 float3 pos,
							 float3 normal, float3 toEye,
							 out float4 ambient,
							 out float4 diffuse,
							 out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Light vector opposite direction of light rays
	float3 lightVec = L.position - pos;

	float d = length(lightVec);

	if (d > L.range)
		return;

	//Normalize light vector
	lightVec /= d;

	//Ambient term
	ambient = mat.ambient * L.ambient;


	//Add diffuse and spec term, if surface is in line of site
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}

	//Attenuate
	float att = 1.0f / dot(L.att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec	*= att;
}

void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
				  out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Light vector opposite direction of light rays
	float3 lightVec = L.position - pos;

	float d = length(lightVec);

	if (d > L.range)
		return;

	//Normalize light vector
	lightVec /= d;

	//Ambient term
	ambient = mat.ambient * L.ambient;


	//Add diffuse and spec term, if surface is in line of site
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}

	//scale by spotlight factor
	float spot = pow(max(dot(-lightVec, L.direction), 0.0f), L.spot);

	//Attenuate
	float att = spot / dot(L.att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec	*= att;
}
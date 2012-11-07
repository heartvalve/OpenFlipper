
#define SHININESS g_vMaterial.x
#define TRANSPARENCY g_vMaterial.y
#define ALPHA g_vMaterial.y

vec3 LitPointLight(vec3 vPosition,
				   vec3 vNormal,
				   vec3 vLightPos,
				   vec3 cLightAmbient,
				   vec3 cLightDiffuse,
				   vec3 cLightSpecular,
				   vec3 vLightAtten)
{
	vec3 vLightVS = vPosition - vLightPos;
	float fLen2 = dot(vLightVS, vLightVS);
	float fLen = sqrt(fLen2);
	
	vec3 vLightDir = vLightVS / fLen;
	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	cLight += ldotn * cLightDiffuse * g_cDiffuse;
 
	// specular
	vec3 h = normalize(vec3(0, 0, 1) + vLightDir);
	float hdotn = max(dot(h, vNormal), 0.0);
	cLight += (pow(hdotn, SHININESS) * cLightSpecular) * g_cSpecular;
	
	
	// attenuate
	float fAtten = vLightAtten.x + vLightAtten.y * fLen + vLightAtten.z * fLen2;
	
	return cLight / fAtten;
}


vec3 LitDirLight(vec3 vNormal,
				 vec3 vLightDir,
				 vec3 cLightAmbient,
				 vec3 cLightDiffuse,
				 vec3 cLightSpecular)
{	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	cLight += ldotn * cLightDiffuse * g_cDiffuse;

	// specular
	vec3 h = normalize(vec3(0, 0, 1) + vLightDir);
	float hdotn = max(dot(h, vNormal), 0.0);
	cLight += (pow(hdotn, SHININESS) * cLightSpecular) * g_cSpecular;

	return cLight;
}



vec3 LitSpotLight(vec3 vPosition,
				  vec3 vNormal,
				  vec3 vLightPos,
				  vec3 vSpotLightDir,
				  vec3 cLightAmbient,
				  vec3 cLightDiffuse,
				  vec3 cLightSpecular,
				  vec3 vLightAtten,
				  vec2 vLightSpotAngleExp)
{
	vec3 vLightVS = vPosition - vLightPos;
	float fLen2 = dot(vLightVS, vLightVS);
	float fLen = sqrt(fLen2);
	
	vec3 vLightDir = vLightVS / fLen;
	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	cLight += ldotn * cLightDiffuse * g_cDiffuse;

	// specular
	vec3 h = normalize(vec3(0, 0, 1) + vLightDir);
	float hdotn = max(dot(h, vNormal), 0.0);
	cLight += (pow(hdotn, SHININESS) * cLightSpecular) * g_cSpecular;

	
	
	// attenuate
	float fAtten = vLightAtten.x + vLightAtten.y * fLen + vLightAtten.z * fLen2;
	
	// spot angle falloff
	float fSpot = -dot(vLightDir, vSpotLightDir);
	fSpot *= step(vLightSpotAngleExp.x, fSpot);
	fSpot *= pow(fSpot, vLightSpotAngleExp.y);
	
	return cLight * (fSpot / fAtten);
}


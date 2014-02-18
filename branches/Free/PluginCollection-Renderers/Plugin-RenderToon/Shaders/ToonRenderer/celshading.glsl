// cel shading functions:
//  downgrade precision of diffuse and specular lighting to given palette size

#ifndef SHININESS
#define SHININESS g_vMaterial.x
#endif

#ifndef TRANSPARENCY
#define TRANSPARENCY g_vMaterial.y
#endif

#ifndef ALPHA
#define ALPHA g_vMaterial.y
#endif

// downgrade precision of an intensity in [0, 1] range to given paletteSize
float QuantizeDiffuse_Cel(float intensity, float paletteSize)
{
	// linear quantification
	float x = intensity * paletteSize;
	float linQ = trunc(x) / paletteSize;
	
//	return linQ;
	
	// quadratic
//	return linQ * linQ;
	
	// exponential
	return pow(linQ, 1.4);
	
	return sqrt(linQ);
}

// quantifier for specular lighting intensity
float QuantizeSpecular_Cel(float intensity, float paletteSize)
{
	// linear quantification
	float x = intensity * paletteSize;
	float linQ = trunc(x) / paletteSize;
	
	return linQ * linQ;
}

vec3 LitPointLight_Cel(vec3 vPosition,
				   vec3 vNormal,
				   vec3 vLightPos,
				   vec3 cLightAmbient,
				   vec3 cLightDiffuse,
				   vec3 cLightSpecular,
				   vec3 vLightAtten,
				   float paletteSize)
{
	vec3 vLightVS = vPosition - vLightPos;
	float fLen2 = dot(vLightVS, vLightVS);
	float fLen = sqrt(fLen2);
	
	vec3 vLightDir = vLightVS / fLen;
	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	
	// quantification of lighting intensity
	ldotn = QuantizeDiffuse_Cel(ldotn, paletteSize);
	
	cLight += ldotn * cLightDiffuse * g_cDiffuse;
 
	// specular
	vec3 h = normalize(vLightDir - normalize(vPosition)); // half vector between light and view direction
	float hdotn = max(dot(h, vNormal), 0.0);
//	hdotn = QuantizeSpecular_Cel(hdotn, paletteSize);
	cLight += QuantizeSpecular_Cel(pow(hdotn, SHININESS), paletteSize) * cLightSpecular * g_cSpecular;
	
	
	// attenuate
	float fAtten = vLightAtten.x + vLightAtten.y * fLen + vLightAtten.z * fLen2;
	
	return cLight / fAtten;
}


vec3 LitDirLight_Cel(vec3 vPosition,
				 vec3 vNormal,
				 vec3 vLightDir,
				 vec3 cLightAmbient,
				 vec3 cLightDiffuse,
				 vec3 cLightSpecular,
				 float paletteSize)
{	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	ldotn = QuantizeDiffuse_Cel(ldotn, paletteSize);
	cLight += ldotn * cLightDiffuse * g_cDiffuse;

	// specular
	vec3 h = normalize(vLightDir - normalize(vPosition)); // half vector between light and view direction
	float hdotn = max(dot(h, vNormal), 0.0);
//	hdotn = QuantizeSpecular_Cel(hdotn, paletteSize);
	cLight += QuantizeSpecular_Cel(pow(hdotn, SHININESS), paletteSize) * cLightSpecular * g_cSpecular;

	return cLight;
}



vec3 LitSpotLight_Cel(vec3 vPosition,
				  vec3 vNormal,
				  vec3 vLightPos,
				  vec3 vSpotLightDir,
				  vec3 cLightAmbient,
				  vec3 cLightDiffuse,
				  vec3 cLightSpecular,
				  vec3 vLightAtten,
				  vec2 vLightSpotAngleExp,
				  float paletteSize)
{
	vec3 vLightVS = vPosition - vLightPos;
	float fLen2 = dot(vLightVS, vLightVS);
	float fLen = sqrt(fLen2);
	
	vec3 vLightDir = vLightVS / fLen;
	
	// ambient
	vec3 cLight = cLightAmbient * g_cAmbient;
	
	// diffuse
	float ldotn = clamp(dot(vLightDir, vNormal), 0.0, 1.0);
	ldotn = QuantizeDiffuse_Cel(ldotn, paletteSize);
	cLight += ldotn * cLightDiffuse * g_cDiffuse;

	// specular
	vec3 h = normalize(vLightDir - normalize(vPosition)); // half vector between light and view direction
	float hdotn = max(dot(h, vNormal), 0.0);
//	hdotn = QuantizeSpecular_Cel(hdotn, paletteSize);
	cLight += QuantizeSpecular_Cel(pow(hdotn, SHININESS), paletteSize) * cLightSpecular * g_cSpecular;

	
	
	// attenuate
	float fAtten = vLightAtten.x + vLightAtten.y * fLen + vLightAtten.z * fLen2;
	
	// spot angle falloff
	float fSpot = -dot(vLightDir, vSpotLightDir);
	fSpot *= step(vLightSpotAngleExp.x, fSpot);
	fSpot *= pow(fSpot, vLightSpotAngleExp.y);
	
	return cLight * (fSpot / fAtten);
}


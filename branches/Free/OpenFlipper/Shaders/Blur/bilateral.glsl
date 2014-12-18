#version 130

// bilateral filter using the depth buffer
// - separable approximation
// - red channel only

// sigma_s : spatial kernel
// sigma_r : range kernel (linear depth range)

// potential optimization:
//  prelinearize depth

#define BLUR_R 0
#define BLUR_RG 1
#define BLUR_RGB 2
#define BLUR_RGBA 3

#ifndef BLUR_CHANNELS
#define BLUR_CHANNELS BLUR_R
#endif


in vec2 vTexCoord;
out vec4 outColor;

#ifndef BLUR_SAMPLES
#define BLUR_SAMPLES 9
#endif

uniform sampler2D g_InputTex;
uniform sampler2D g_DepthTex;

uniform mat4 g_P; // projection

uniform float g_BlurSigmaRcp2; // -1 / (2 * sigma_r^2))

uniform vec2 g_SampleOffsets[BLUR_SAMPLES];
uniform float g_SpatialKernel[BLUR_SAMPLES];

#include "../Common/depthbuffer.glsl"


float GetDepthVS(vec2 uv)
{
  // sample depth buffer and linearize
  float d = texture(g_DepthTex, uv).x;
  return ProjectedToViewspaceDepth(d, g_P[2][2], g_P[2][3]);
}

void main()
{
  vec4 res = vec4(0,0,0,0);
  float weightSum = 0;
  
  float centerZ = GetDepthVS(vTexCoord);
  
  for (int i = 0; i < BLUR_SAMPLES; ++i)
  {
    vec2 sampleUV = vTexCoord + g_SampleOffsets[i];

	float sampleZ = GetDepthVS(sampleUV);
	
	// weighting based on spatial and range closeness
	// here: range = diff linear depth
	float diffZ = centerZ - sampleZ;
	float d2 = diffZ * diffZ;
    float weight = exp(d2 * g_BlurSigmaRcp2 + g_SpatialKernel[i]);
	
	weightSum += weight;

#if (BLUR_CHANNELS == BLUR_R)
	res.x += weight * texture(g_InputTex, sampleUV).x;
#elif (BLUR_CHANNELS == BLUR_RG)
	res.xy += weight * texture(g_InputTex, sampleUV).xy;
#elif (BLUR_CHANNELS == BLUR_RGB)
    res.xyz += weight * texture(g_InputTex, sampleUV).xyz;
#else
    res += weight * texture(g_InputTex, sampleUV);
#endif
  }
  
  outColor = res / weightSum;
}
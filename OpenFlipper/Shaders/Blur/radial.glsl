#version 130

in vec2 vTexCoord;
out vec4 outColor;

#ifndef BLUR_SAMPLES
#define BLUR_SAMPLES 8
#endif

uniform sampler2D g_InputTex;

uniform vec2 g_BlurCenter;

uniform float g_BlurRadiusRcp2; // 1 / r^2
uniform float g_BlurIntensity;


void main()
{
/*
 // debug, manual params
  float blurRadius = 1.5;
  vec2 blurCenter = g_BlurCenter;
  float blurIntensity = 0.01;
  float blurRadiusRcp2 = 1.0 / (blurRadius * blurRadius);
*/

  float blurRadiusRcp2 = g_BlurRadiusRcp2;
  float blurIntensity = g_BlurIntensity;


  vec2 blurDir = g_BlurCenter - vTexCoord;
  
  // quadratic intensity scale
  float d2 = dot(blurDir, blurDir);
  float blurAmount = clamp(1 - d2 * blurRadiusRcp2, 0, 1) * blurIntensity;
  
  blurDir *= blurAmount;
  
  
  vec4 r = vec4(0,0,0,0);
  for (float i = 0; i < BLUR_SAMPLES; ++i)
    r += texture(g_InputTex, vTexCoord + blurDir * i);

  outColor = r / BLUR_SAMPLES;
}
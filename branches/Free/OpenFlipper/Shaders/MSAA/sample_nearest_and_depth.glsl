#version 150

#include "msaa.glsl"


uniform sampler2DMS inputTex;
uniform sampler2DMS inputDepthTex;
uniform int numSamples;

in vec2 vTexCoord;
out vec4 outFragment;


void main()
{
  ivec2 pixelPos = ivec2( vTexCoord * vec2(textureSize(inputTex).xy) );

  outFragment = ReadMultisampledTexture(inputTex, pixelPos, numSamples);
//  outFragment = vec4(1,1,0,1);
//  outFragment = texelFetch(inputTex, pixelPos, 0);
  
  // high variance in depth at edges, so averaging depth values doesn't make sense
  // -> choose sample closest to viewer as depth
  
  float d = texelFetch(inputDepthTex, pixelPos, 0).x;
  
  for (int i = 1; i < numSamples; ++i)
  {
	vec4 sample = texelFetch(inputDepthTex, pixelPos, i);
	
	d = min(d, sample.x);
  }
  
  gl_FragDepth = d;
}
// methods for resolving multisampled textures


vec4 ReadMultisampledTexture(in sampler2DMS tex, in ivec2 pos, in int numSamples)
{
  vec4 texel = vec4(0,0,0,0);
  for (int i = 0; i < numSamples; ++i)
  {
	vec4 sample = texelFetch(tex, pos, i);
	
	texel += sample;
  }
  
  return texel / float(numSamples);
}

vec4 ReadMultisampledTexture(in sampler2DMS tex, in vec2 texcoord, in int numSamples)
{
  ivec2 pos = ivec2(texcoord * vec2(textureSize(tex)));
  return ReadMultisampledTexture(tex, pos, numSamples);
}


vec4 ReadMultisampledTexture(in sampler2DMS tex, in ivec2 pos, in int numSamples, in samplerBuffer filterWeights)
{
  vec4 texel = vec4(0,0,0,0);
  for (int i = 0; i < numSamples; ++i)
  {
    float filterWeight = texelFetch(filterWeights, i).x;
	vec4 sample = texelFetch(tex, pos, i);
	
	texel += filterWeight * sample;
  }
  
  return texel;
}

vec4 ReadMultisampledTexture(in sampler2DMS tex, in vec2 texcoord, in int numSamples, in samplerBuffer filterWeights)
{
  ivec2 pos = ivec2(texcoord * vec2(textureSize(tex)));
  return ReadMultisampledTexture(tex, pos, numSamples, filterWeights);
}


vec4 FilterMultisampledTextureBilinear(in sampler2DMS tex, in vec2 texcoord, in int numSamples)
{
  ivec2 texSize = textureSize(tex);
  
  // top-left texel position
  vec2 pos00 = floor( texSize * texcoord );
  ivec2 loc00 = ivec2(pos00);
  
  // lerp weights
  vec2 alpha = fract( texSize * texcoord );

  // bilinear filtering

  // lerp in u-direction top
  vec4 sampleX0 = mix(
    ReadMultisampledTexture(tex, loc00, numSamples),
    ReadMultisampledTexture(tex, loc00 + ivec2(1,0), numSamples),
	alpha.x);
			
  // lerp in u-direction bottom
  vec4 sampleX1 = mix(
    ReadMultisampledTexture(tex, loc00 + ivec2(0,1), numSamples),
    ReadMultisampledTexture(tex, loc00 + ivec2(1,1), numSamples),
	alpha.x);
  
  // lerp in v direction
  return mix(sampleX0, sampleX1, alpha.y);
}


vec4 FilterMultisampledTextureBilinear(in sampler2DMS tex, in vec2 texcoord, in int numSamples, in samplerBuffer filterWeights)
{
  ivec2 texSize = textureSize(tex);
  
  // top-left texel position
  vec2 pos00 = floor( texSize * texcoord );
  ivec2 loc00 = ivec2(pos00);
  
  // lerp weights
  vec2 alpha = fract( texSize * texcoord );

  // bilinear filtering

  // lerp in u-direction top
  vec4 sampleX0 = mix(
    ReadMultisampledTexture(tex, loc00, numSamples, filterWeights),
    ReadMultisampledTexture(tex, loc00 + ivec2(1,0), numSamples, filterWeights),
	alpha.x);
			
  // lerp in u-direction bottom
  vec4 sampleX1 = mix(
    ReadMultisampledTexture(tex, loc00 + ivec2(0,1), numSamples, filterWeights),
    ReadMultisampledTexture(tex, loc00 + ivec2(1,1), numSamples, filterWeights),
	alpha.x);
  
  // lerp in v direction
  return mix(sampleX0, sampleX1, alpha.y);
}
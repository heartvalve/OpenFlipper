#version 150

#include "msaa.glsl"


uniform sampler2DMS inputTex;
uniform int numSamples;

in vec2 vTexCoord;
out vec4 outFragment;


void main()
{
  outFragment = FilterMultisampledTextureBilinear(inputTex, vTexCoord, numSamples);
}
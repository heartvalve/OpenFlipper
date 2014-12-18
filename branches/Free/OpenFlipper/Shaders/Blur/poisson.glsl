#version 130

in vec2 vTexCoord;
out vec4 outColor;

#ifndef BLUR_SAMPLES
#define BLUR_SAMPLES 9
#endif

uniform sampler2D g_InputTex;


uniform vec2 g_SampleOffsets[BLUR_SAMPLES];

void main()
{
  vec4 r = vec4(0,0,0,0);
  
  for (int i = 0; i < BLUR_SAMPLES; ++i)
    r += texture(g_InputTex, vTexCoord + g_SampleOffsets[i]);

  outColor = r / BLUR_SAMPLES;
}
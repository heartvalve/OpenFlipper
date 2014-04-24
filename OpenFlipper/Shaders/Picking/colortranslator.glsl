// transcode from 32 bit integer to float4 color (quantized to byte4 color)

vec4 IntToColor(in int i)
{
  // A8B8G8R8?
  i = i + 1;
  int r = (i >> 16) & 0xff;
  int g = (i >> 8) & 0xff;
  int b = i & 0xff;
  int a = (i >> 24) & 0xff;
  
  return vec4( float(r) / 255.0, float(g) / 255.0, float(b) / 255.0, float(a) / 255.0 );
}

// maybe write test for random colors and compare with ColorTranslator.cc

/*
ColorTranslator::index2color(int _idx):

blue_shift = green_shift = red_shift = alpha_shift = 0
blue_round = green_round = red_round = alpha_round = 0x80000000
blue_mask = green_mask = red_mask = alpha_mask = 0xff

blue_bits = green_bits = red_bits = alpha_bits = 8


idx = _idx + 1;

b = idx & 0xff
g = (idx >> 8) & 0xff
r = (idx >> 16) & 0xff
a = (idx >> 24) & 0xff

*/
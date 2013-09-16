#version 150

uniform sampler2D textureSampler;

in vec2 vTexCoord;
out vec4 oColor;

void main() 
{
  vec4 col = texture2D(textureSampler, vTexCoord);
  vec3 conv = vec3(0.299, 0.587, 0.114);
  float lum = dot(col.rgb, conv);
  
  oColor = vec4(lum, lum, lum, 1.0);
//  oColor.xyz = rgbM;
}

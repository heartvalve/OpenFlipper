#version 150

uniform sampler2D textureSampler;

in vec2 vTexCoord;
out vec4 oColor;

void main() 
{
  oColor = texture2D(textureSampler, vTexCoord);
}

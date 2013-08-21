#version 150

uniform sampler2D Tex;

in vec2 vTexCoord;
out vec4 oColor;

void main()
{
	oColor = texture2D(Tex, vTexCoord);
}
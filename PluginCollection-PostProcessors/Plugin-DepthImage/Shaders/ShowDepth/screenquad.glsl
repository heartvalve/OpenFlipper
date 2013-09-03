#version 150

in vec4 inPosition;
out vec2 vTexCoord;

void main()
{
	gl_Position = inPosition;
	vTexCoord = inPosition.xy * 0.5 + vec2(0.5, 0.5);
}
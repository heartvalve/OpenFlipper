#version 150


uniform vec2 offset = vec2(0,0);
uniform vec2 size = vec2(1,1);


in vec4 inPosition;
out vec2 vTexCoord;

void main()
{
	gl_Position = inPosition * vec4(size, 1, 1) + vec4(offset, 0, 0);
	vTexCoord = inPosition.xy * 0.5 + vec2(0.5, 0.5);
}
#version 150

out vec4 oColor;

void main()
{
	oColor = vec2(-gl_FragCoord.z, gl_FragCoord.z);
}
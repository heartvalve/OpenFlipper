#version 150

uniform sampler2D SceneTex;
uniform vec3 BkgColor;

in vec2 vTexCoord;
out vec4 oColor;

void main()
{
	vec4 c = texture2D(SceneTex, vTexCoord);
	oColor = vec4(c.rgb + BkgColor * c.a, 1.0);
}
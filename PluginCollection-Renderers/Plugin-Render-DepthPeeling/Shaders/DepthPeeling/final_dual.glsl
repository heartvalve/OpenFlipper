#version 150

uniform sampler2D FrontSceneTex;
uniform sampler2D BackSceneTex;

in vec2 vTexCoord;
out vec4 oColor;

void main()
{
	vec4 cf = texture2D(FrontSceneTex, vTexCoord);
	vec4 cb = texture2D(BackSceneTex, vTexCoord);

	// combine front and back color
	oColor = vec4(cf.rgb + cb.rbg * (1.0 - cf.a), 1.0);
}
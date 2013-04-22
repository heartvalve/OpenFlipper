#version 150

uniform sampler2D Tex;


in vec2 vTexCoord;
out vec4 oColor;

void main()
{
    vec4 d = texture2D(Tex, vTexCoord);
	oColor = -d.xxxx;
    oColor = d.yyyy;
	oColor = d;
	
//	if (d.a == 0)
//	 oColor = vec4(1.0, 0, 0, 0);
}
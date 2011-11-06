
varying vec2 vTexCoord0;
varying vec4 vTexCoord1;
varying vec4 vTexCoord2;
varying vec4 vTexCoord3;
varying vec4 vTexCoord4;

uniform vec2 SceneTexelSize;

void main(void)
{
	gl_Position = gl_Vertex;
	vec2 uv = gl_Vertex.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
	
	float sc = 1.2215; // use some texture filtering

	vec2 d = SceneTexelSize * sc;
	
	vTexCoord0 = uv;
	vTexCoord1 = vec4(uv + vec2(d.x, 0), uv - vec2(d.x, 0));
	vTexCoord2 = vec4(uv + vec2(0, d.y), uv - vec2(0, d.y));
	vTexCoord3 = vec4(uv + d, uv - d);
	vTexCoord4 = vec4(uv + vec2(d.x, -d.y), uv - vec2(d.x, -d.y));
}
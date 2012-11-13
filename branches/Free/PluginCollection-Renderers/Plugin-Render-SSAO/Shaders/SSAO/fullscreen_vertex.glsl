
varying vec2 vTexCoord;

void main(void)
{
	gl_Position = gl_Vertex;
	vTexCoord = gl_Vertex.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
}
varying vec3 vNormal;
varying vec4 vViewSpace;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vNormal = gl_NormalMatrix * gl_Normal;
	vNormal.z *= -1;
	vViewSpace = gl_ModelViewMatrix * gl_Vertex;
}
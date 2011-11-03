varying vec3 vNormal;
varying vec4 vViewSpace;

void main(void)
{
	gl_FragData[0] = vec4(abs(vViewSpace.z), 0.0, 0.0, 0.0);
	gl_FragData[1] = vec4(vNormal * 0.5 + vec3(0.5, 0.5, 0.5), 0.0);
}
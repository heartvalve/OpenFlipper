varying vec3 normal;
varying vec4 viewingDir;

uniform sampler2D texSamplerTMU0;

void main()
{      

	vec3 viewingDirCut = vec3(viewingDir.x, viewingDir.y, viewingDir.z);
	float angle = dot ( normal , viewingDirCut );

	vec2 coord;
	coord.s = angle/3.14;
	coord.t = angle/3.14;

	gl_FragColor = texture2D (texSamplerTMU0 , coord) ;

}


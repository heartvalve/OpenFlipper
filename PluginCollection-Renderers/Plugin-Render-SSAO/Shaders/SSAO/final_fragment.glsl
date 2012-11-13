// multiplies scene with occlusion value
// render into back buffer

varying vec2 vTexCoord;


uniform sampler2D SceneTex;
uniform sampler2D OcclusionTex;

void main(void)
{
	vec4 Result = texture2D(SceneTex, vTexCoord);
	float Occlusion = clamp(texture2D(OcclusionTex, vTexCoord).x, 0, 1);
	Result.rgb *= Occlusion;
	
	gl_FragData[0] = Result;
}
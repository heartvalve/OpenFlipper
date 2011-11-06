// multiplies scene with occlusion value
// render into back buffer

varying vec2 vTexCoord0;
varying vec4 vTexCoord1;
varying vec4 vTexCoord2;
varying vec4 vTexCoord3;
varying vec4 vTexCoord4;


uniform sampler2D SceneTex;
uniform sampler2D OcclusionTex;

void main(void)
{
	vec4 Center = texture2D(SceneTex, vTexCoord0);
	
	// direct neighbors
	vec4 DirectN = texture2D(SceneTex, vTexCoord1.xy);
	DirectN += texture2D(SceneTex, vTexCoord1.zw);
	DirectN += texture2D(SceneTex, vTexCoord2.xy);
	DirectN += texture2D(SceneTex, vTexCoord2.zw);
	
	// corner neighbors
	vec4 CornerN = texture2D(SceneTex, vTexCoord2.xy);
	CornerN += texture2D(SceneTex, vTexCoord2.zw);
	CornerN += texture2D(SceneTex, vTexCoord3.xy);
	CornerN += texture2D(SceneTex, vTexCoord3.zw);
	
	// using the corner neighbors results in a ~90 instruction shader on HD 5000 series
//	vec4 Result = (Center * 0.5 + DirectN * (0.5 * 0.25))* 0.75 + CornerN * (0.25 * 0.25);
	
	// direct neighbors gives a ~60 instruction shader with neglectable quality loss
	vec4 Result = (Center * 0.5 + DirectN * (0.5 * 0.25));
	
	float Occlusion = clamp(texture2D(OcclusionTex, vTexCoord0).x, 0.0, 1.0);
	Result.rgb *= Occlusion;
	
	gl_FragData[0] = Result;
}
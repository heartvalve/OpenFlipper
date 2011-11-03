
varying vec2 vTexCoord;

uniform sampler2D DepthTex;
uniform sampler2D SceneTex;
uniform sampler2D NormalTex;
uniform sampler2D RandTex;

uniform vec2 TexelSize; // texel size of DepthTex
uniform vec2 ScreenSize;

uniform vec3 Kernel[32];

//--------------------------------------
// change SSAO parameters here:

const float KernelZScale = 0.0018; // increase for less occlusion, decrease for more
const float Radius  = 10.0; // kernel radius in texels
const int NumSamples = 32; // only multiples of 8 allowed
const float RangeCheckScale = 10.4;

//--------------------------------------

// random vector used to generate the orthonormal tangent normal basis
const vec3 TangentShift = vec3(1, -1, 2);


float OcclusionIntensifier(float x)
{
	return x*x*x;
}

mat3 AxisAngleToMatrix(float ux, float uy, float uz, float s, float c)
{
	float c1 = 1.0 - c;
	
	return mat3( vec3( c + ux*ux * c1,  ux*uy*c1 - uz*s, ux*uz*c1 + uy*s),
				 vec3( uy*ux*c1 + uz*s, c + uy*uy*c1,    uy*uz*c1 - ux*s),
				 vec3( uz*ux*c1 - uy*s, uz*uy*c1 + ux*s, c + uz*uz*c1));
}

vec3 ReadNormal(vec2 uv)
{
	return normalize(texture2D(NormalTex, uv).xyz - vec3(0.5, 0.5, 0.5));
}

void main(void)
{	
	float Depth = texture2D(DepthTex, vTexCoord).x;
	
	vec3 Normal = ReadNormal(vTexCoord);
	
	vec2 RandUV = vTexCoord * ScreenSize * 0.25;
	vec4 RandSinCos = texture2D(RandTex, RandUV);
	// convert from axis angle to rotation matrix
	//  axis: normal vector
	//  angle: RandSinCos
	mat3 RotMat = AxisAngleToMatrix(Normal.x, Normal.y, Normal.z, RandSinCos.x, RandSinCos.y);
		
	// generate two tangent vectors
	vec3 Tangent = Normal + TangentShift;
	Tangent -= Normal * dot(Normal, Tangent);
	Tangent = normalize(Tangent);
	
	vec3 Bitangent = cross(Tangent, Normal);
	
		
	float Occlusion = 0.0;
	
	for (int i = 0; i < NumSamples; ++i)
	{
		vec3 Offset = normalize( mat3(Tangent, Bitangent, Normal) * Kernel[i] ) * Radius;
		
		// randomly rotate the kernel
		vec3 RotOffset = RotMat *  Offset;
		vec3 SamplePos = vec3(vTexCoord, Depth);
		
		SamplePos += vec3(RotOffset.xy * TexelSize, RotOffset.z * Depth * KernelZScale);
		
		float SampleDepth = texture2D(DepthTex, SamplePos.xy).x;
		
		// relative depth range
		float Range = clamp( ( Depth - SampleDepth) / SampleDepth * RangeCheckScale, 0.0, 1.0);
		
		// visibility of this sample:
		//  if SampleDepth > SamplePos.z : fragment occluded (0.0),   else visible (1.0)
		float SampleVis = step(SamplePos.z, SampleDepth);
		
		Occlusion += (1.0 - SampleVis) * Range; // add DepthRange if sample is occluded
		Occlusion += SampleVis;	// add 1 if visible
	}
	
	Occlusion /= NumSamples;
	
	Occlusion = OcclusionIntensifier(Occlusion);	
	Occlusion = clamp(Occlusion, 0.0, 1.0);
	
	gl_FragData[0] = vec4(Occlusion,0,0,0);
}
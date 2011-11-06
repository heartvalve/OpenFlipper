// depth preserving gaussian blur filter with 11 samples
// working on the x channel only

varying vec2 vTexCoord;

uniform sampler2D Tex;

// we need a z buffer to avoid color bleeding
uniform sampler2D DepthTex;

// TexelSize controls the direction of this pass
//  example: set TexelSize.y to 0 for a horizontal pass
uniform vec2 TexelSize;
uniform float Kernel[5]; // gaussian distribution in one direction only

const float DepthThreshold = 0.01;

uniform float EdgeBlur; // [0,1]: 0 -> no edge blurring, 1 -> blur everywhere

float GetDepthWeight(vec2 uv, float Depth)
{
	float SampleDepth = texture2D(DepthTex, uv).x;
	float DepthRange = abs(Depth - SampleDepth) / (Depth + 0.001);
	float DepthCheck = DepthRange > DepthThreshold ? EdgeBlur : 1.0;
	
	return DepthCheck;
}

void main(void)
{
	// kernel center:
	float Result = texture2D(Tex, vTexCoord).x * Kernel[0];
	
	float Depth = texture2D(DepthTex, vTexCoord).x;
	
	float WeightSum = Kernel[0]; // not necessarily 1 with enabled depth preserving
	
	// kernel sides:
	vec2 uv0 = vTexCoord, uv1 = vTexCoord;
	for (int i = 1; i < 5; ++i)
	{
		uv0 += TexelSize;
		uv1 -= TexelSize;
		
		float BlurWeight = Kernel[i] * GetDepthWeight(uv0, Depth);
		
		Result += texture2D(Tex, uv0).x * BlurWeight;
		WeightSum += BlurWeight;
		
		BlurWeight = Kernel[i] * GetDepthWeight(uv1, Depth);
		
		Result += texture2D(Tex, uv1).x * BlurWeight;
		WeightSum += BlurWeight;
	}
	
	gl_FragData[0] = vec4(Result / WeightSum, 0, 0, 0);
}
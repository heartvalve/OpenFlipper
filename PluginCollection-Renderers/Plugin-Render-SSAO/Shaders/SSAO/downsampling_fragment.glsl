// 2x2 downsampling, only x values

varying vec2 vTexCoord;

uniform sampler2D Tex;
uniform vec2 TexelSize; // texel size of the bigger texture

void main(void)
{
	// sample direct neighbors and compute average
	float sample = texture2D(Tex, vTexCoord + TexelSize).x;
	sample += texture2D(Tex, vTexCoord + vec2(-TexelSize.x, TexelSize.y)).x;
	sample += texture2D(Tex, vTexCoord + vec2(TexelSize.x, -TexelSize.y)).x;
	sample += texture2D(Tex, vTexCoord - TexelSize).x;
	gl_FragData[0] = vec4(sample * 0.25, 0, 0, 0);
}
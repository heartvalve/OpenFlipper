//--------------------------------------------------------------------------------------
// Order Independent Transparency with Dual Depth Peeling
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

uniform sampler2DRect DepthBlenderTex;
uniform sampler2DRect FrontBlenderTex;
uniform sampler2DRect BackBlenderTex;

uniform vec2 ViewportOffset;

void main(void)
{
	vec4 frontColor = texture2DRect(FrontBlenderTex, gl_FragCoord.xy - ViewportOffset);
	vec3 backColor = texture2DRect(BackBlenderTex, gl_FragCoord.xy - ViewportOffset).rgb;
	float alphaMultiplier = 1.0 - frontColor.w;

	// front + back
	gl_FragColor.rgb = frontColor.rgb + backColor * alphaMultiplier;
//	gl_FragColor = texture2DRect(DepthBlenderTex, gl_FragCoord.xy);
	
	// front blender
	//gl_FragColor.rgb = frontColor + vec3(alphaMultiplier);
	
	// back blender
	//gl_FragColor.rgb = backColor;
}

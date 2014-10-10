#version 130

// resolve left and right stereo buffer

// define ANAGLYPH_CUSTOM for custom color matrix transforms

uniform sampler2D SceneLeft;
uniform sampler2D SceneRight;

uniform mat3 EyeColMatrixLeft;
uniform mat3 EyeColMatrixRight;

in vec2 vTexCoord;
out vec4 outColor;

void main(void)
{
  vec4 colLeft = texture2D( SceneLeft, vTexCoord );
  vec4 colRight = texture2D( SceneRight, vTexCoord );
  
#ifndef ANAGLYPH_CUSTOM
  outColor = vec4(colLeft.x, colRight.yzw);
#else
  outColor = vec4(EyeColMatrixLeft * colLeft.xyz + EyeColMatrixRight * colRight.xyz, 1.0);
#endif
}
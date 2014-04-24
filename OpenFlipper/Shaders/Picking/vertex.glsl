#version 150

uniform mat4 mWVP;

in vec4 inPosition;

void main()
{
  gl_Position = mWVP * vec4(inPosition.xyz, 1);
}
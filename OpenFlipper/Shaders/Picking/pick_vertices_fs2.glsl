// returns picking ids of vertices

#version 150

#include "colortranslator.glsl"

uniform int pickVertexOffset;

out vec4 outPickID;

void main()
{
  int vertexID = gl_PrimitiveID + pickVertexOffset;
  outPickID = IntToColor(vertexID);
}


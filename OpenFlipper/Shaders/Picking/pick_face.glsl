// returns picking ids of faces in a n-poly mesh

#version 150

#include "colortranslator.glsl"

uniform int pickFaceOffset;
uniform isamplerBuffer triToFaceMap; // maps: triangle id in draw buffer -> face id in input buffer

out vec4 outPickID;

void main()
{
  int faceID = texelFetch(triToFaceMap, gl_PrimitiveID).x;
//  faceID = gl_PrimitiveID; // debug mode: skip reading from texture-buffer
  int pickID = pickFaceOffset + faceID;
  outPickID = IntToColor(pickID);
}
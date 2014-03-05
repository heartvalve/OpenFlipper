// hiddenline shader for tri meshes
// emits screen quads for edges
// emits hidden triangle to update depth buffer

#include "createEdgeQuad.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 15) out;

uniform float lineWidth;
uniform vec2 screenSize;

out float outGeometryHidden;

void main()
{
  // draw hidden triangle with background color
  outGeometryHidden = 1.0;

  sg_MapIO(0);
  EmitVertex();
  sg_MapIO(1);
  EmitVertex();
  sg_MapIO(2);
  EmitVertex();
  EndPrimitive();

  // draw edges
  outGeometryHidden = 0.0;

  createEdgeQuad(0,1, screenSize, lineWidth);
  createEdgeQuad(1,2, screenSize, lineWidth);
  createEdgeQuad(2,0, screenSize, lineWidth);
}
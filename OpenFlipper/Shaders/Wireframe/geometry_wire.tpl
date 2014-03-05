// wireframe shader for tri meshes
// emits screen quads for edges

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

uniform float lineWidth;
uniform vec2 screenSize;

#include "createEdgeQuad.glsl"

void main()
{
  createEdgeQuad(0,1, screenSize, lineWidth);
  createEdgeQuad(1,2, screenSize, lineWidth);
  createEdgeQuad(2,0, screenSize, lineWidth);
}

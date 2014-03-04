// modified wireframe shader that works for n-poly faces

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

// stores which triangle edge is a poly edge
// i.e. visible edges of a triangulation
uniform usamplerBuffer polyEdgeBuffer;

uniform float lineWidth;
uniform vec2 screenSize;

#include "createEdgeQuad.glsl"

void main()
{
  int polyEdge = int(texelFetch(polyEdgeBuffer, gl_PrimitiveIDIn/2).x);
  // Each texel in the poly-edge buffer stores edge visibility for two triangles.
  // Address the correct bits in the fetched byte:
  int offsetBits = (gl_PrimitiveIDIn&1) * 3;
  
  // determine visible/hidden edges
  if ((polyEdge & (1 << offsetBits)) != 0)
    createEdgeQuad(0,1, screenSize, lineWidth);
	
  if ((polyEdge & (2 << offsetBits)) != 0)
    createEdgeQuad(1,2, screenSize, lineWidth);
	
  if ((polyEdge & (4 << offsetBits)) != 0)
    createEdgeQuad(2,0, screenSize, lineWidth);
}

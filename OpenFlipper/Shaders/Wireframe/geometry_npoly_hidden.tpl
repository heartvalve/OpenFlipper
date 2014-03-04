// modified hiddenline shader that works for n-poly faces

layout(triangles) in;
layout(triangle_strip, max_vertices = 15) out;

// stores which triangle edge is a poly edge
// i.e. visible edges of a triangulation
uniform usamplerBuffer polyEdgeBuffer;

uniform float lineWidth;
uniform vec2 screenSize;

out float outGeometryHidden;

#include "createEdgeQuad.glsl"

void main()
{
  int polyEdge = int(texelFetch(polyEdgeBuffer, gl_PrimitiveIDIn/2).x);
  // Each texel in the poly-edge buffer stores edge visibility for two triangles.
  // Address the correct bits in the fetched byte:
  int offsetBits = (gl_PrimitiveIDIn&1) * 3;
  
  
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
  
  // determine visible/hidden edges
  if ((polyEdge & (1 << offsetBits)) != 0)
    createEdgeQuad(0,1, screenSize, lineWidth);
	
  if ((polyEdge & (2 << offsetBits)) != 0)
    createEdgeQuad(1,2, screenSize, lineWidth);
	
  if ((polyEdge & (4 << offsetBits)) != 0)
    createEdgeQuad(2,0, screenSize, lineWidth);
}

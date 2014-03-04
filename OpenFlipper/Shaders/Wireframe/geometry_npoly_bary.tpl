// modified wireframe/hiddenline shader that works for n-poly faces
// template file for shader generator

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// stores which triangle edge is a poly edge
uniform usamplerBuffer polyEdgeBuffer;

// send barycentric coords to fragment
out vec3 outGeometryBary;

// mark outer/inner edges of the n-poly triangulation
out vec3 outGeometryPolyEdge;

void main()
{
  int polyEdge = int(texelFetch(polyEdgeBuffer, gl_PrimitiveIDIn/2).x);
  // Each texel in the poly-edge buffer stores edge visibility for two triangles.
  // Address the correct bits in the fetched byte:
  int offsetBits = (gl_PrimitiveIDIn&1) * 3;
  
  float e01 = 0;
  float e12 = 0;
  float e20 = 0;

  // determine visible/hidden edges:
  if ((polyEdge & (1 << offsetBits)) != 0) e01 = 1;
  if ((polyEdge & (2 << offsetBits)) != 0) e12 = 1;
  if ((polyEdge & (4 << offsetBits)) != 0) e20 = 1;
  
  
  outGeometryPolyEdge = vec3(e01, e12, e20);
  
  sg_MapIO(0); // IO mapping provided by ShaderGen, send vertex 0
  outGeometryBary = vec3(1, 0, 0);
  EmitVertex();
  
  sg_MapIO(1); // IO mapping provided by ShaderGen, send vertex 1
  outGeometryBary = vec3(0, 1, 0);
  EmitVertex();
    
  sg_MapIO(2); // IO mapping provided by ShaderGen, send vertex 2
  outGeometryBary = vec3(0, 0, 1);
  EmitVertex();
  
  EndPrimitive();
}

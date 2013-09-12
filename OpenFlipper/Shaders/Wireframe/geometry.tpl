// template file for shader generator

// send barycentric coords to fragment
out vec3 outGeometryBary;

void main()
{

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
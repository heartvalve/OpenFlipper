// template file for shader generator

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;


out vec2 outGeometryQuadPos;

uniform float pointSize;
uniform vec2 screenSize;

void main()
{
  // center of point
  vec4 p0 = gl_in[0].gl_Position;
  
  // convert to screen space
  p0.xy = p0.xy / p0.w * screenSize;
  
  
  // create screen-aligned quad
  vec2 offsetX = vec2(pointSize, 0.0);
  vec2 offsetY = vec2(0.0, pointSize);

  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0.xy - offsetX + offsetY) * p0.w / screenSize, p0.z, p0.w);
  outGeometryQuadPos = vec2(-1.0, 1.0);
  EmitVertex();
  
  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0.xy - offsetX - offsetY) * p0.w / screenSize, p0.z, p0.w);
  outGeometryQuadPos = vec2(-1.0, -1.0);
  EmitVertex();
  
  
  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0.xy + offsetX + offsetY) * p0.w / screenSize, p0.z, p0.w);
  outGeometryQuadPos = vec2(1.0, 1.0);
  EmitVertex();
  
  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0.xy + offsetX - offsetY) * p0.w / screenSize, p0.z, p0.w);
  outGeometryQuadPos = vec2(1.0, -1.0);
  EmitVertex();

  EndPrimitive();
}

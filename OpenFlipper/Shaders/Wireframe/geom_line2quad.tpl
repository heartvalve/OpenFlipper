// template file for shader generator

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float lineWidth;
uniform vec2 screenSize;

void main()
{
  // start/end points of line
  vec4 p0 = gl_in[0].gl_Position;
  vec4 p1 = gl_in[1].gl_Position;
  
  // convert to screen space
  p0.xy = p0.xy / p0.w * screenSize;
  p1.xy = p1.xy / p1.w * screenSize;
  
  
  // compute dir and normal
  vec2 lineDir = p1.xy - p0.xy;
  vec2 lineNormal = normalize(vec2(-lineDir.y, lineDir.x));
  
  
  // create screen-aligned quad
  vec2 offset = lineNormal * lineWidth;

  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0 + offset) * p0.w / screenSize, p0.z, p0.w);
  EmitVertex();
  
  sg_MapIO(0); // default IO mapping
  gl_Position = vec4( (p0 - offset) * p0.w / screenSize, p0.z, p0.w);
  EmitVertex();
  
  
  sg_MapIO(1); // default IO mapping
  gl_Position = vec4( (p1 + offset) * p1.w / screenSize, p1.z, p1.w);
  EmitVertex();
  
  sg_MapIO(1); // default IO mapping
  gl_Position = vec4( (p1 - offset) * p1.w / screenSize, p1.z, p1.w);
  EmitVertex();
  
  EndPrimitive();
}
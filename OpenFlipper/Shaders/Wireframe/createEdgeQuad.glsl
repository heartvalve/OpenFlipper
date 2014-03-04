
// declare IO mapping prototype
void sg_MapIO(const int);

// emit a screen aligned quad along the edge v1-v2
void createEdgeQuad(const int v0, const int v1, in vec2 screenSize, in float lineWidth)
{
  vec4 p0 = gl_in[v0].gl_Position;
  vec4 p1 = gl_in[v1].gl_Position;
  
  // convert to screen space
  p0.xy = p0.xy / p0.w * screenSize;
  p1.xy = p1.xy / p1.w * screenSize;
  
  
  // compute dir and normal
  vec2 lineDir = p1.xy - p0.xy;
  vec2 lineNormal = normalize(vec2(-lineDir.y, lineDir.x));
  
  
  // create screen-aligned quad
  vec2 offset = lineNormal * lineWidth;

  sg_MapIO(v0); // default IO mapping
  gl_Position = vec4( (p0.xy + offset) * p0.w / screenSize, p0.z, p0.w);
  EmitVertex();
  
  sg_MapIO(v0); // default IO mapping
  gl_Position = vec4( (p0.xy - offset) * p0.w / screenSize, p0.z, p0.w);
  EmitVertex();
  
  
  sg_MapIO(v1); // default IO mapping
  gl_Position = vec4( (p1.xy + offset) * p1.w / screenSize, p1.z, p1.w);
  EmitVertex();
  
  sg_MapIO(v1); // default IO mapping
  gl_Position = vec4( (p1.xy - offset) * p1.w / screenSize, p1.z, p1.w);
  EmitVertex();
  
  EndPrimitive();
}
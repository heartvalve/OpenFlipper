// template file for shader generator

in vec3 outGeometryBary;

uniform float lineWidth;
uniform vec4 bkColor;

float edgeFactor()
{
  // compute smoothed distance to edge, source:
  // http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
  
  vec3 d = fwidth(outGeometryBary);
  vec3 a3 = smoothstep(vec3(0.0), d*lineWidth, outGeometryBary);
  return min(min(a3.x, a3.y), a3.z);
}

void main()
{
  SG_FRAGMENT_BEGIN

  sg_cColor.xyz = mix(bkColor.xyz, sg_cColor.xyz, 1-edgeFactor());
    
  SG_FRAGMENT_END
}
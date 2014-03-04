// modified wireframe/hiddenline shader that works for n-poly faces
// template file for shader generator

in float outGeometryHidden;

uniform vec4 bkColor;

void main()
{
  SG_FRAGMENT_BEGIN

  sg_cColor.xyz = mix(sg_cColor.xyz, bkColor.xyz, outGeometryHidden);
  
  SG_FRAGMENT_END
}
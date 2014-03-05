// hiddenline shader for tri meshes

in float outGeometryHidden;

uniform vec4 bkColor;

void main()
{
  SG_FRAGMENT_BEGIN

  sg_cColor.xyz = mix(sg_cColor.xyz, bkColor.xyz, outGeometryHidden);
  
  SG_FRAGMENT_END
}
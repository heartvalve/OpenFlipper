// template file for shader generator

in vec3 outGeometryBary;

uniform float lineWidth;

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
    
  float t = 1-edgeFactor();

  // use alpha blending to fade out
  sg_cColor.a = t;
  
  // without alpha blending:
//  sg_cColor.xyz *= t; //mix(vec3(0.0), sg_cColor.xyz, t);

  gl_FragDepth = gl_FragCoord.z;

  // discard pixels inside face
  if (t < 0.1)
  {
    gl_FragDepth = 1.0;
//    discard; // depth write after discard might depend on gpu driver
  }
  
  SG_FRAGMENT_END  
}
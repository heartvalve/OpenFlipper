// modified wireframe/hiddenline shader that works for n-poly faces
// template file for shader generator


void main()
{
  SG_FRAGMENT_BEGIN

  
  
  SG_FRAGMENT_END

  gl_FragDepth = gl_FragCoord.z * 0.9998;
}

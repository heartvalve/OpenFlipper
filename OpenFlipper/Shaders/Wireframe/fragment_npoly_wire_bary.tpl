// modified wireframe/hiddenline shader that works for n-poly faces
// template file for shader generator

in vec3 outGeometryBary;
in vec3 outGeometryPolyEdge;

uniform float lineWidth;


void main()
{
  SG_FRAGMENT_BEGIN

  // compute smoothed barycentric distance to edge
  vec3 d = fwidth(outGeometryBary);
  vec3 a3 = smoothstep(vec3(0.0), d*lineWidth, outGeometryBary);

  // edge factor
  float t = 1.0;
  
  // barycentric coordinate = 0 <-> pixel located on opposite edge 
  // check if opposite edge should be hidden or visible

  // distance v0 to edge v1-v2
  if (outGeometryPolyEdge.y > 0.5)
    t = min(t, a3.x);
  // distance v1 to edge v2-v0
  if (outGeometryPolyEdge.z > 0.5)
    t = min(t, a3.y);
  // distance v2 to edge v0-v1
  if (outGeometryPolyEdge.x > 0.5)
    t = min(t, a3.z);

  t = 1 - t; // clamp(t, 0.0, 1.0);
  
  
  // use alpha blending to fade out
  sg_cColor.a = t;
  
  // discard pixels inside face
  if (t < 0.1)
    discard;
  
  SG_FRAGMENT_END
}
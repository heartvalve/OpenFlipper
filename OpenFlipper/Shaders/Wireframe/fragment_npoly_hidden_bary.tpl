// modified wireframe/hiddenline shader that works for n-poly faces
// template file for shader generator

in vec3 outGeometryBary;
in vec3 outGeometryPolyEdge;

uniform float lineWidth;
uniform vec4 bkColor;

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
	
	
  vec3 dbg = vec3(0,0,0);
 /*
  if (outGeometryPolyEdge.y < 0.5)
    dbg.x = min(dbg.x, a3.x);
  // distance v1 to edge v2-v0
  if (outGeometryPolyEdge.z > 0.5)
    dbg.y = min(dbg.y, a3.y);
  // distance v2 to edge v0-v1
  if (outGeometryPolyEdge.x > 0.5)
	dbg.z = min(dbg.z, a3.z);
*/	

  if (outGeometryPolyEdge.y < 0.5 && outGeometryPolyEdge.z < 0.5)
    dbg.x = min(1, a3.x);
  dbg.y = dbg.z = dbg.x;


  // distance from vertices
/*  
  if (a3.x < 0.2 && a3.y < 0.02)
    t = min(t,a3.z);
  if (a3.x < 0.2 && a3.z < 0.02)
    t = min(t,a3.y);
  if (a3.y < 0.2 && a3.z < 0.02)
    t = min(t,a3.x);
*/
  t = 1 - t; // clamp(t, 0.0, 1.0);
  
  
  sg_cColor.xyz = mix(bkColor.xyz, sg_cColor.xyz, t);
  sg_cColor.w = 1;
  
//  sg_cColor.xyz = dbg;
  
  SG_FRAGMENT_END
}
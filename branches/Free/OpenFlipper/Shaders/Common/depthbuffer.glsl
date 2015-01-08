/*
Access of depth buffer values:
Convert between projected (non-linear) and linear depth values:

- compute depth value (gl_FragDepth) of positions in clip space
- fast depth value computation given a view space position
- inverse functions mapping from gl_FragDepth to clip space depth or view space z-coordinate
- reconstruct view space position from depth

P22 and P23 are the entries in the projection matrix (in zero based row-major)

see http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
*/

/* useful info about the opengl clipping planes:
 zn, zf : near and far clipping planes (positive distance from camera)
 depth buffer range in ndc space: [-1,1]
 P : perspective projection
 
P * vec4(*,*,-zn, 1) = (x,y,-zn,zn)  -> div by w = zn: (x/zn,y/zn, -1,1)
P * vec4(*,*,-zf, 1) = (x,y, zf,zf)  -> div by w = zf: (x/zf,y/zf, +1,1)

Unproject a fragment on the near or far clipping plane to view space:

 vec4 posNearVS = Inverse(P) * vec4( ndc.xy * zn, -zn, zn );  // ndc in [-1,1] range,  (ndc = uv * 2 - 1)
 vec4 posFarVS = Inverse(P) * vec4( ndc.xy * zf, zf, zf );  // ndc in [-1,1] range,  (ndc = uv * 2 - 1)
*/



// linear map from (non-linear) clip space depth in [-1,1] range to gl_DepthRange
float ClipSpaceToDepthValue(in float d);

// compute depth value from a position in clip space, taking the correct depth-range into account
//  example:
//  - in vertex shader: posCS = WorldViewProj * inPosition;
//  - in fragment shader (after clipping): gl_FragDepth = ClipSpaceToDepthValue(posCS);
// can be used for depth buffer manipulation, for instance:
//  - rendering imposters with correct depth
//  - custom depth test
//  - soft particles

float ClipSpaceToDepthValue(in vec4 posCS)
{
  // divide by w in clip space to get the non-linear depth in range [-1,1]:
  
  // posCS.z is a the linearly modified view space z coordinate:  P22 * posVS.z + P23
  // posCS.w is the negative view space z-coordinate: w flips the sign if the viewer direction is along the -z axis 
  
  // division posCS.z / posCS.w yields a non-linear depth -(P22 + P23 / posVS.z)
  //  P22, P23 are chosen such that the non-linear depth is in range [-1,1] for all positions between the near and far clipping planes
  float d = posCS.z / posCS.w; // [-1,1] (ndc depth)
  
  // map to depth-range
  return ClipSpaceToDepthValue(d);
}

float ClipSpaceToDepthValue(in float d)
{
  // linear mapping from [-1,1] to [gl_DepthRange.near, gl_DepthRange.far] (which should be [0,1] in most cases)
  // gl_DepthRange is set by calling glDepthRange()
  // -> http://www.opengl.org/sdk/docs/man/html/glDepthRange.xhtml
  
  return (d * gl_DepthRange.diff + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
  
  // test:
  // -1 is mapped to gl_DepthRange.near: (-1 * (f-n) + n + f) / 2 = (n-f + n +f) / 2 = n
  //  1 is mapped to gl_DepthRange.far: (1 * (f-n) + n + f) / 2 = (f-n + n + f) / 2 = f
}

// convert depth value in gl_DepthRange to clip-space factor (z / w) in range [-1,1]
float DepthValueToClipSpace(in float depth)
{
  return ( depth * 2.0 - (gl_DepthRange.near + gl_DepthRange.far) ) / gl_DepthRange.diff;
}


// convert depth buffer value to viewspace z-coord
float ProjectedToViewspaceDepth(in float depth, in float P22, in float P23)
{
  // convert from [0,1] to [-1,1]
//  float d = depth * 2.0 - 1.0;  // special case gl_DepthRange = [0,1]
  float d = DepthValueToClipSpace(depth); // arbitrary gl_DepthRange

  return -P23 / (d + P22);
}

// convert view space z-coordinate to the depth value written into the depth buffer
float ViewspaceToProjectedDepth(in float zVS, in float P22, in float P23)
{
  // convert to non-linear depth in [-1,1]
  float d = -(P22 + P23 / zVS);
  // convert to [0,1]
//  return d * 0.5 + 0.5;  // special case gl_DepthRange = [0,1]
  return ClipSpaceToDepthValue(d); // arbitrary gl_DepthRange
}



// convert screen space texcoord to view space position
//  uv: screenspace pos in [0,1] range
//  zVS: z coordinate in view space
//  P00, P11: (0,0) and (1,1) entries of projection matrix
vec3 ScreenspaceToViewspace(vec2 uv, float zVS, float P00, float P11)
{
/*
  Reconstructing the position of a pixel based on its z-coord in viewspace
  
  1. Compute the clip-space position of the pixel:
    - extent texcoord from range [0,1] to [-1,1]
  
  2. Undo perspective projection of xy-coordinate:
  
    perspective projection matrix:  http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
	
	f/aspect  0  0  0
	0  f  0  0
	0  0   *  *
	0  0  -1  0
	
	(where f = cotangent(fovy/2),   aspect = width/height)
	
	projection of viewspace position:
	xPS = f/aspect * xVS
	yPS = f * yVS
	zPS = ..
	wPS = -zVS
	
	post perspective homogenization in clip-space:
	
	xCS = xPS / wPS = xPS / -zVS
	yCS = yPS / wPS = yPS / -zVS
	

	Revert perspective projection to get xVS and yVS:
	
	xVS = xCS * (-zVS) / (f/aspect)
	yVS = yCS * (-zVS) / f
  */


  // compute xy-position in clip space (position on near clipping plane)
  vec2 vPosCS = uv * 2 - vec2(1,1);

  // revert perspective projection
  vec3 vPos;
  vPos.xy = (vPosCS * (-zVS)) / vec2(P00, P11);
  vPos.z = zVS;
  
  // 8 ALUs
  return vPos;
}


// convert screen space texcoord to view space position (optimized, less ALUs)
//  uv: screenspace pos in [0,1] range
//  zVS: z coordinate in view space
//  scale: -2 / vec2(P00, P11)
//  shift: -vec2(1,1) / vec2(P00, P11)
vec3 ScreenspaceToViewspace(vec2 uv, float zVS, vec2 scale, vec2 shift)
{
  vec3 vPos;
  vPos.xy = uv * zVS;
  vPos.xy = vPos.xy * scale + shift;
  vPos.z = zVS;
  
  // 5 ALUs
  return vPos;
}



// screen space uv to view space position
//  depthTex : nonlinear depth buffer texture
//  uv : screenspace pos in [0,1] range
//  proj : projection matrix
vec3 ReconstructViewspace(in sampler2D depthTex, in vec2 uv, in mat4 proj)
{
  float nonlinearDepth = texture(depthTex, uv).x;
  float linearDepthVS = ProjectedToViewspaceDepth(nonlinearDepth, proj[2][2], proj[2][3]);
  return ScreenspaceToViewspace(uv, linearDepthVS, proj[0][0], proj[1][1]);
}
// template file for shader generator

in vec2 outGeometryQuadPos;

uniform float pointSize;
uniform vec2 screenSize;

void main()
{
    
  float r2 = dot(outGeometryQuadPos, outGeometryQuadPos);

  // discard pixels outside of radius
  if (r2 > 1.0)
    discard;
	
	
  SG_FRAGMENT_BEGIN
  
  SG_FRAGMENT_END
}

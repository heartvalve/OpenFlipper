varying vec3 normal;

void main(void) 
{
  normal      = normalize( gl_Normal);
  gl_Position = ftransform();
}


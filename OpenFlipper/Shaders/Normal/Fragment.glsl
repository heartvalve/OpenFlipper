varying vec3 normal;

void main(void)
{
  vec4 color = vec4( normal[0] / 2.0 + 0.5,normal[1]/ 2.0 + 0.5,normal[2]/ 2.0 + 0.5, 1.0 );
  gl_FragColor = color;
}


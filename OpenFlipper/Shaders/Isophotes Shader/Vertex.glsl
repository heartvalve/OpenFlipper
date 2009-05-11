varying vec3 normal;
varying vec4 viewingDir;
       
void main()
{       
      /* first transform the normal into eye space and 
      normalize the result */
      normal = normalize(gl_NormalMatrix * gl_Normal);
               
      vec4 viewingDirBase = vec4( 0.0,0.0,-1.0,0.0 );
      viewingDir = viewingDirBase;
 
      gl_Position = ftransform();
} 

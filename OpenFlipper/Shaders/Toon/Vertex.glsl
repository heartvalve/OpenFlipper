varying vec3 lightDir;
varying vec3 normal;
varying vec4 vertexPosition;

void main()
{
   normal = normalize(gl_NormalMatrix * gl_Normal);
   lightDir = normalize( vec3( gl_LightSource[0].position ) );
   vertexPosition =  gl_ModelViewMatrix * gl_Vertex;
   gl_Position = gl_ProjectionMatrix * vertexPosition; 
}

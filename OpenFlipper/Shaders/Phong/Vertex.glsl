varying vec4 diffuse_color;
varying vec3 normal;
varying vec4 vertexPosition;

void main()
{
   normal = normalize(gl_NormalMatrix * gl_Normal);
   vertexPosition = gl_ModelViewMatrix * gl_Vertex;
   gl_Position = gl_ProjectionMatrix * vertexPosition;
   diffuse_color = gl_FrontMaterial.diffuse;
}

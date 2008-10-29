varying vec4 diffuse_color;
varying vec3 normal;
varying vec4 vertexPosition;
uniform float ward_specular;
uniform float ward_diffuse;
uniform float ward_alpha;

void main()
{
   normal = normalize(gl_NormalMatrix * gl_Normal);
   vertexPosition = gl_ModelViewMatrix * gl_Vertex;
   gl_Position = gl_ProjectionMatrix * vertexPosition;
   diffuse_color = gl_FrontMaterial.diffuse;
}

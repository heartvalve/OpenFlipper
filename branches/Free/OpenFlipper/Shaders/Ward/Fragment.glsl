varying vec4 diffuse;
varying vec3 normal;
varying vec4 vertexPosition;
uniform float ward_specular;
uniform float ward_diffuse;
uniform float ward_alpha;

const float pi = 3.141592653589793238;
const float pi_rec = 0.3183098861837906715;

/** \brief Isotropic Ward BRDF.
 */
vec4 ward_direct(int lightSource)
{
    vec3 light_pos = vec3(gl_LightSource[lightSource].position);
    vec3 vtx_pos = vec3(vertexPosition / vertexPosition.w);
    vec3 i = vec3(light_pos - vtx_pos);
    float lightDistance = length(i);
    i /= lightDistance;

    vec4 color = vec4(0.0);
    vec3 n;

    n = normalize(normal);

    float NdotI = dot(n, i);
    float IdotD = dot(-i, gl_LightSource[lightSource].spotDirection);

    if (NdotI > 0.0 && IdotD > 0.0)
    {
        vec3 o = normalize(-vtx_pos);
        vec3 h = normalize(i + o);
        float NdotO = dot(n, o);
        float G = (IdotD * NdotI);
        float HdotN = dot(h, n);

        vec4 f_R = ward_diffuse * pi_rec + (ward_specular * exp((HdotN * HdotN - 1) / (HdotN * HdotN * ward_alpha * ward_alpha)))
                                            / (4 * pi * ward_alpha * ward_alpha * sqrt(NdotI * NdotO));

        color = gl_LightSource[lightSource].diffuse *  f_R * G;
    }
    return color;
}

void  main() {
  gl_FragColor = ward_direct(0);
}

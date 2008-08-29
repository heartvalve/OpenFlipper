varying vec4 diffuse;
varying vec3 normal;
varying vec4 vertexPosition;

vec4 phong_direct(int lightSource) {
    vec3 light_pos = vec3(gl_LightSource[lightSource].position);
    vec3 eye_pos = vec3(vertexPosition / vertexPosition.w);
    vec3 lightVector = vec3(light_pos - eye_pos);
    float lightDistance = length(lightVector);
    lightVector /= lightDistance;

    vec4 color = vec4(0.0);
    vec3 n;
    float NdotL, NdotHV;
    
    n = normalize(normal);
    
    NdotL = dot(n, lightVector);
    float LdotD = dot(lightVector, gl_LightSource[lightSource].spotDirection);

    if (NdotL > 0.0 && LdotD < 0.0) {
        color = gl_LightSource[lightSource].diffuse * -LdotD * NdotL;
    }
    return color;
}

void  main() {
  gl_FragColor = phong_direct(0);
}
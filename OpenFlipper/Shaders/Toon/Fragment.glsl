varying vec4 lightDir;
varying vec3 normal;
varying vec4 vertexPosition;

vec4 toon(int lightSource) {
    vec4 color = vec4(0.0);

    // Position of light source
    vec3 light_pos = vec3( gl_LightSource[lightSource].position );

    vec3 eye_pos = vec3( vertexPosition / vertexPosition.w );
    vec3 lightVector = vec3(light_pos - eye_pos );

    float lightDistance = length(lightVector);
    lightVector /= lightDistance;

    vec3 n;
    
    n = normalize(normal);

    float light = dot ( lightVector, n );
    
    if ( light > 0.99 )
       color = vec4( 0.9,0.9,1.0,1.0 );
    else if ( light > 0.95 )
       color = vec4( 0.7,0.7,1.0,1.0 );
    else if ( light > 0.5 )               
       color = vec4( 0.3,0.3,0.6,1.0 );
    else if ( light > 0.25 )               
       color = vec4( 0.2,0.2,0.4,1.0 );
    else
       color = vec4( 0.1,0.1,0.2,1.0 );
    
    return color;
}

void  main() {
  gl_FragColor = toon(0);
}


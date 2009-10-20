
uniform sampler2D       Texture;


void main(void)
{

 vec4 depthColor;
 depthColor.x = texture2D( Texture, gl_TexCoord[0].st ).a;
 depthColor.y = depthColor.x;
 depthColor.z = depthColor.x;
 depthColor.a = 1.0;

// gl_FragColor = depthColor;
 gl_FragColor = texture2D( Texture, gl_TexCoord[0].st );

 //gl_FragColor = gl_Color;
 //gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}


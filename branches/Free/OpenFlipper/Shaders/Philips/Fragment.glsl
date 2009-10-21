
uniform sampler2D       ColorTexture;
uniform sampler2D       DepthStencil;


void main(void)
{
 
 // Left  side is the color image seen by the user
 // Right side is the the depth map of the current view
 // Texture coordinates have to be scaled
 // and for the left image they also have to be shifted
 if ( gl_TexCoord[0].s < 0.5 ) {
   gl_TexCoord[0].s = gl_TexCoord[0].s * 2.0; 
   gl_FragColor = texture2D( ColorTexture, gl_TexCoord[0].st );
 } else {
   gl_TexCoord[0].s = (gl_TexCoord[0].s - 0.5) * 2.0;
   gl_FragColor = texture2D( DepthStencil, gl_TexCoord[0].st );
 }

}


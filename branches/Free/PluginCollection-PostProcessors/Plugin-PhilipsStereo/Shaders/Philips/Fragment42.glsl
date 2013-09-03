#version 150

uniform sampler2D       ColorTexture;
uniform sampler2D       DepthStencil;


in vec2 vTexCoord;
out vec4 oColor;

void main(void)
{
 
 // Left  side is the color image seen by the user
 // Right side is the the depth map of the current view
 // Texture coordinates have to be scaled
 // and for the left image they also have to be shifted
 if ( vTexCoord.x < 0.5 ) {
   vTexCoord.x = vTexCoord.x *  2.0; 
   oColor = texture2D( ColorTexture, vTexCoord );
 } else {
   vTexCoord.x = (vTexCoord.x - 0.5) * 2.0;
   vec4 convert = texture2D( DepthStencil, vTexCoord );
   
   // This conversion is specified in the docs for a 42'' Display
   convert.rgba = (-1960.37 *( 1.0 - (7.655192 / ( convert.r - 0.467481 + 7.655192)) ) + 127.5) / 255.0;
   oColor = convert;
 }

}


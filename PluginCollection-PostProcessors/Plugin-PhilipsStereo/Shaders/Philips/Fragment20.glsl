#version 150

uniform sampler2D       ColorTexture;
uniform sampler2D       DepthStencil;


in vec2 vTexCoord;
out vec4 oColor;

vec2 coordinate;

void main(void)
{

 coordinate = vTexCoord;
 
 // Left  side is the color image seen by the user
 // Right side is the the depth map of the current view
 // Texture coordinates have to be scaled
 // and for the left image they also have to be shifted
 if ( vTexCoord.x < 0.5 ) {
   coordinate.x = vTexCoord.x *  2.0; 
   oColor = texture2D( ColorTexture, coordinate );
 } else {
   coordinate.x = (vTexCoord.x - 0.5) * 2.0;
   vec4 convert = texture2D( DepthStencil, coordinate );
   
   // This conversion is specified in the docs for a 20'' Display
   convert = vec4((-1586.34 *( 1.0 - (6.180772 / ( convert.r - 0.459813 + 6.180772)) ) + 127.5) / 255.0);
   oColor = convert;
 }

}


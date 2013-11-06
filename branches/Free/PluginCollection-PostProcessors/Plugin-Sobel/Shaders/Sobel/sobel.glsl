#version 150

uniform sampler2D textureSampler;

in vec2 vTexCoord;
out vec4 oColor;

// The inverse of the texture dimensions along X and Y
uniform vec2 texcoordOffset;

void main() 
{
  vec2 coord_tl = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y - texcoordOffset.y );
  vec2 coord_t  = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y                    );
  vec2 coord_tr = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y + texcoordOffset.y );
  
  vec2 coord_cl = vec2( vTexCoord.x,vTexCoord.y-texcoordOffset.y );
  vec2 coord_cr = vec2( vTexCoord.x,vTexCoord.y+texcoordOffset.y );
  
  vec2 coord_bl = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y-texcoordOffset.y );
  vec2 coord_b  = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y );
  vec2 coord_br = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y+texcoordOffset.y );
  
  
  vec4 color_horizontal  = abs(texture2D(textureSampler, coord_tl)       - texture2D(textureSampler, coord_tr));
       color_horizontal += abs(2.0 * texture2D(textureSampler, coord_cl) - 2.0 * texture2D(textureSampler, coord_cr));
       color_horizontal += abs(texture2D(textureSampler, coord_bl) - texture2D(textureSampler, coord_br));
       
  vec4 color_vertical    = abs(texture2D(textureSampler, coord_tl)      - texture2D(textureSampler, coord_bl));
       color_vertical   += abs(2.0 * texture2D(textureSampler, coord_t) - 2.0 * texture2D(textureSampler, coord_b));
       color_vertical   += abs(texture2D(textureSampler, coord_tr)      - texture2D(textureSampler, coord_br));     
       
  oColor = color_horizontal + color_vertical;
  oColor.a = 1.0;
}

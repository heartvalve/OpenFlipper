#version 150

uniform sampler2D samplerScene;
uniform sampler2D samplerDepth;

uniform vec2 clipPlanes;
uniform vec3 outlineColor;

in vec2 vTexCoord;
out vec4 oColor;

// The inverse of the texture dimensions along X and Y
uniform vec2 texcoordOffset;

void main() 
{
  // detect edges in depth buffer with sobel filter
  vec2 coord_tl = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y - texcoordOffset.y );
  vec2 coord_t  = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y                    );
  vec2 coord_tr = vec2( vTexCoord.x + texcoordOffset.x , vTexCoord.y + texcoordOffset.y );
  
  vec2 coord_cl = vec2( vTexCoord.x,vTexCoord.y-texcoordOffset.y );
  vec2 coord_cr = vec2( vTexCoord.x,vTexCoord.y+texcoordOffset.y );
  
  vec2 coord_bl = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y-texcoordOffset.y );
  vec2 coord_b  = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y );
  vec2 coord_br = vec2( vTexCoord.x-texcoordOffset.x,vTexCoord.y+texcoordOffset.y );
  
  
  float depth_horizontal  = abs(texture2D(samplerDepth, coord_tl).x       - texture2D(samplerDepth, coord_tr).x);
        depth_horizontal += abs(2.0 * texture2D(samplerDepth, coord_cl).x - 2.0 * texture2D(samplerDepth, coord_cr).x);
        depth_horizontal += abs(texture2D(samplerDepth, coord_bl).x - texture2D(samplerDepth, coord_br).x);
       
  float depth_vertical    = abs(texture2D(samplerDepth, coord_tl).x      - texture2D(samplerDepth, coord_bl).x);
        depth_vertical   += abs(2.0 * texture2D(samplerDepth, coord_t).x - 2.0 * texture2D(samplerDepth, coord_b).x);
        depth_vertical   += abs(texture2D(samplerDepth, coord_tr).x      - texture2D(samplerDepth, coord_br).x);     


  // compute edge factor
  float edge = (depth_horizontal + depth_vertical); // / clipPlanes.y;
  float edge_factor = min(edge, 1.0);
  edge_factor = 1.0 - step(0.1, edge_factor * edge_factor);
  
  // outline edges
  vec4 color_scene = texture2D(samplerScene, vTexCoord);
  
  
  oColor = vec4(mix(outlineColor.xyz, color_scene.xyz, edge_factor), color_scene.w);
  
//  oColor = vec4(edge_factor, edge_factor, edge_factor, 1.0);
  
//  oColor = color_scene;
  
//  oColor = edge;
//  vec4 fragD = texture2D(samplerDepth, vTexCoord);
//  fragD.x *= fragD.x;
  
//  oColor = -fragD.xxxx;

  // write depth in case any future post-processors require scene depth information
  gl_FragDepth = texture2D(samplerDepth, vTexCoord).x;
}

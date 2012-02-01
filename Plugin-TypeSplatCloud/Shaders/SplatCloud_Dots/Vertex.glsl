////////////////////////////////////////////////////////////////////////////////////////////
/**/                                                                                    /**/
/**/  // WARNING: When modifying this file, modify file "PickVertex.glsl" accordingly!  /**/
/**/                                                                                    /**/
////////////////////////////////////////////////////////////////////////////////////////////

#version 120

uniform float pointsizeScale  = 1.0;
uniform bool  backfaceCulling = false;
uniform float modelviewScale;
uniform float viewportScaleFov_y;

void main()
{
    // cull if normal is pointing away from eye
    if( backfaceCulling && dot( vec3( gl_Vertex - gl_ModelViewMatrixInverse[3] ), gl_Normal ) > 0.0 ) // second part is evaluated only if backfaceCulling is true
    {
        // discard vertex
        gl_Position.w = 0.0;
    }
    else
    {
        // output vertex in clip-coordinates
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

        // calculate pointsize (gl_MultiTexCoord0.x is the pointsize in model-coordinates)
        float ecPointsize  = modelviewScale * pointsizeScale * gl_MultiTexCoord0.x;

        // output pointsize in window-coordinates
        //   - divided by gl_Position.w to shrink size by distance
        //   - dot is always parallel to near-plane so we do not have to divide like in the vertex-shader for splats
        //   - multiply by viewportScaleFov_y to get window coordinates
        gl_PointSize = max( ecPointsize * viewportScaleFov_y / gl_Position.w, 1.0 );

//////////////////////////////////////////////////////////////
/**/                                                      /**/
/**/    // pass secondary color to fragment-shader        /**/
/**/    // (if selected, pass selection-color instead)    /**/
/**/    if( gl_MultiTexCoord1.x != 0.0 )                  /**/
/**/    {                                                 /**/
/**/        gl_FrontColor = vec4( 1.0, 0.0, 0.0, 1.0 );   /**/
/**/    }                                                 /**/
/**/    else                                              /**/
/**/    {                                                 /**/
/**/        gl_FrontColor.rgb = gl_SecondaryColor.rgb;    /**/
/**/        gl_FrontColor.a   = 1.0;                      /**/
/**/                                                      /**/
/**/    }                                                 /**/
/**/                                                      /**/
//////////////////////////////////////////////////////////////

    }
}

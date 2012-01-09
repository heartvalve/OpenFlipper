////////////////////////////////////////////////////////////////////////////////////////////
/**/                                                                                    /**/
/**/  // WARNING: When modifying this file, modify file "PickVertex.glsl" accordingly!  /**/
/**/                                                                                    /**/
////////////////////////////////////////////////////////////////////////////////////////////

#version 120

uniform bool backfaceCulling = false;

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

////////////////////////////////////////////////////////////////////////////////////////////
/**/                                                                                    /**/
/**/  // WARNING: When modifying this file, modify file "Vertex.glsl" accordingly!      /**/
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
/**/    // pass primary color to fragment-shader          /**/
/**/    gl_FrontColor = gl_Color;                         /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
/**/                                                      /**/
//////////////////////////////////////////////////////////////

    }
}

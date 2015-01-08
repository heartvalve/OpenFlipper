in vec4 inModelView0;
in vec4 inModelView1;
in vec4 inModelView2;

in vec3 inModelViewIT0;
in vec3 inModelViewIT1;
in vec3 inModelViewIT2;

void main()
{
  SG_VERTEX_BEGIN;
  
  // transform with instance matrix
  sg_vPosVS.x = dot(inModelView0, inPosition);
  sg_vPosVS.y = dot(inModelView1, inPosition);
  sg_vPosVS.z = dot(inModelView2, inPosition);
  sg_vPosVS.w = 1.0;
  
  sg_vPosPS = g_mP * sg_vPosVS;
#ifdef SG_INPUT_NORMALOS
  sg_vNormalVS.x = dot(inModelViewIT0, SG_INPUT_NORMALOS.xyz);
  sg_vNormalVS.y = dot(inModelViewIT1, SG_INPUT_NORMALOS.xyz);
  sg_vNormalVS.z = dot(inModelViewIT2, SG_INPUT_NORMALOS.xyz);

  sg_vNormalVS = normalize(sg_vNormalVS);
#endif

  SG_VERTEX_END;
}
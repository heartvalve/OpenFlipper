in vec4 inModelView0;
in vec4 inModelView1;
in vec4 inModelView2;

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
  sg_vNormalVS.x = dot(inModelView0.xyz, SG_INPUT_NORMALOS.xyz);
  sg_vNormalVS.y = dot(inModelView1.xyz, SG_INPUT_NORMALOS.xyz);
  sg_vNormalVS.z = dot(inModelView2.xyz, SG_INPUT_NORMALOS.xyz);
  
  sg_vNormalVS = normalize(sg_vNormalVS);
#endif

  SG_VERTEX_END;
}
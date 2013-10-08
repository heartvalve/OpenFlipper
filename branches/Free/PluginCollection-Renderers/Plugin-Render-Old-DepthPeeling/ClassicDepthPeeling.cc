/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "ClassicDepthPeeling.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ObjectTypes/Light/LightNode.hh>

#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL


// this define enables a shader export of the generated peel shader for debugging purpose
//#define DEPTHPEELING_SHADER_EXPORT


DepthPeelingPlugin::DepthPeelingPlugin() :
        blendQueryID_(0),
        glStateTmp_(0),
        numLights_(0)
{

  for (unsigned int i = 0; i < 16; ++i)
    lightTypes_[i] = LIGHTTYPE_POINT;

  for (unsigned int i = 0; i < PEEL_NUM_COMBINATIONS; ++i)
    peelProgs_[i] = 0;

  for (unsigned int i = 0; i < PEEL_NUM_COMBINATIONS*3; ++i)
    peelShaders_[i] = 0;

  for (unsigned int i = 0; i < 8; ++i)
    blendShaders_[i] = 0;

  for (unsigned int i = 0; i < 4; ++i)
    blendDualPeelProg_[i] = 0;


}

DepthPeelingPlugin::~DepthPeelingPlugin() {

}


DepthPeelingPlugin::ViewerResources::ViewerResources()
{
  rtWidth_ = rtHeight_ = 0;

  glWidth_ = glHeight_ = 0;

  memset(blendDualPeelTexID_, 0, 7*4);

  blendDualPeelFbo_ = 0;
}

void DepthPeelingPlugin::initializePlugin()
{
  memset(blendShaders_, 0, sizeof(blendShaders_));
  memset(peelShaders_, 0, sizeof(peelShaders_));

  memset(blendDualPeelProg_, 0, sizeof(blendDualPeelProg_));
  memset(peelProgs_, 0, sizeof(peelProgs_));

  blendQueryID_ = 0;
  numLights_    = 0;

  ACG::glCheckErrors();
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::exit()
{
  destroyResources();
}

//////////////////////////////////////////////////////////////////////////

QString DepthPeelingPlugin::rendererName() {
  return QString("Classical Depth Peeling Renderer");
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {
  _mode =  ACG::SceneGraph::DrawModes::DEFAULT;
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::reloadResources(int _viewerId) {
  // called in constructor and resizeEvent()

  ACG::GLState::syncFromGL();

  ViewerResources* p = &viewerRes_[_viewerId];

  if (!p->glWidth_ || !p->glHeight_) return;

  destroyResources(_viewerId);

  // dual depth peeling rt's
  glGenTextures(7, p->blendDualPeelTexID_);
  GLint DualPeelIntFmt[] = {GL_RG32F, GL_RG32F, // depth0, depth1
                 GL_RGBA, GL_RGBA, // front blender0, ..._1
                 GL_RGBA, GL_RGBA, // back_temp_tex0, ..._1,
                 GL_RGB};   // back_blender

  for (int i = 0; i < 7; ++i)
  {
    GLint fmt = GL_RGB; // fmt for depth textures

    if (i >= 2) fmt = GL_RGBA; // fmt for front_blender01 and back_temp01
    if (i == 6) fmt = GL_RGB; // fmt for back_blender

    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i]);
    // texture access: clamped
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // filter: none
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, DualPeelIntFmt[i], p->glWidth_, p->glHeight_, 0, fmt, GL_FLOAT, 0);




    ACG::glCheckErrors();
  }

  p->rtWidth_ = p->glWidth_;
  p->rtHeight_ = p->glHeight_;

  // dual depth peeling fbo's
  glGenFramebuffersEXT(1, &p->blendDualPeelFbo_);
  {
    // layer_peel fbo
    ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, p->blendDualPeelFbo_);

    // color_attachment order:
    // depth0, front_blend0, back_temp0,   depth1, front_blend1, back_temp1,    back_blender_tex_id

    for (int i = 0; i < 6; ++i)
    {
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,
        GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i < 3 ? 2*i : 2*(i-3) +1], 0);
    }

    // back_blender_tex_id
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE_EXT,
                                p->blendDualPeelTexID_[6], 0);
  }

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, 0);


  // load shaders

  const char* ShaderFiles[] = {"Blending/dual_peeling_init_vertex.glsl",
    "Blending/dual_peeling_init_fragment.glsl",
    "Blending/dual_peeling_blend_vertex.glsl",
    "Blending/dual_peeling_blend_fragment.glsl",
    "Blending/dual_peeling_final_vertex.glsl",
    "Blending/dual_peeling_final_fragment.glsl"};

  for (int i = 0; i < 6; ++i)
  {
    QString shaderFile = OpenFlipper::Options::shaderDirStr() + QDir::separator() + QString(ShaderFiles[i]);

    if (blendShaders_[i]) continue;

    if (i & 1) // alternating vertex/fragment shader
      blendShaders_[i] = GLSL::loadFragmentShader(shaderFile.toUtf8());
    else
      blendShaders_[i] = GLSL::loadVertexShader(shaderFile.toUtf8());

    if (!blendShaders_[i]) {
      log(LOGERR, QString(ShaderFiles[i]) + QString(" could not be loaded and compiled"));
      return;
    }
  }

  // dual depth peeling programs

  if (!blendDualPeelProg_[0])
  {
    for (int i = 0; i < 4; ++i)
    {
      if (i != 1)
        blendDualPeelProg_[i] = new GLSL::Program();
    }

    // init shaders
    blendDualPeelProg_[0]->attach(blendShaders_[0]);
    blendDualPeelProg_[0]->attach(blendShaders_[1]);
    blendDualPeelProg_[0]->link();

    // blend shaders
    blendDualPeelProg_[2]->attach(blendShaders_[2]);
    blendDualPeelProg_[2]->attach(blendShaders_[3]);
    blendDualPeelProg_[2]->link();

    // final shaders
    blendDualPeelProg_[3]->attach(blendShaders_[4]);
    blendDualPeelProg_[3]->attach(blendShaders_[5]);
    blendDualPeelProg_[3]->link();
  }

  if (!blendQueryID_)
    glGenQueries(1, &blendQueryID_);

  ACG::glCheckErrors();
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::destroyResources() {
  // called in destructor and reloadBlendingTech()

  if (blendQueryID_)
    glDeleteQueries(1, &blendQueryID_);
  blendQueryID_ = 0;

  for (int i = 0; i < 4; ++i)
  {
    delete blendDualPeelProg_[i]; blendDualPeelProg_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(blendShaders_) / sizeof(blendShaders_[0]); ++i)
  {
    delete blendShaders_[i];
    blendShaders_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(peelShaders_) / sizeof(peelShaders_[0]); ++i)
  {
    delete peelShaders_[i]; peelShaders_[i] = 0;
  }

  for (unsigned int i = 0; i < sizeof(peelProgs_) / sizeof(peelProgs_[0]); ++i)
  {
    delete peelProgs_[i]; peelProgs_[i] = 0;
  }

  // free all viewer specific resources
  std::map<int, ViewerResources>::iterator resIt = viewerRes_.begin();
  for (; resIt != viewerRes_.end(); ++resIt)
    destroyResources(resIt->first);
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::destroyResources(int _viewerId)
{
  ViewerResources* p = &viewerRes_[_viewerId];

  if (p->blendDualPeelFbo_) glDeleteFramebuffersEXT(1, &p->blendDualPeelFbo_);
  p->blendDualPeelFbo_ = 0;

  if (p->blendDualPeelTexID_) glDeleteTextures(7, p->blendDualPeelTexID_);
  memset(p->blendDualPeelTexID_, 0, sizeof(p->blendDualPeelTexID_));
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::drawQuadProj(float x0, float y0, float w, float h)
{
  // quad in projection space
  // here only position are passed to GL
  // tex-coords can be generated in a vertex-shader as follows
  //  uv = pos * (.5, -.5) + (.5, .5)

  glBegin(GL_QUADS);
  {
    glVertex2f(x0,  y0);
    glVertex2f(x0, y0-h);
    glVertex2f(x0+w, y0-h);
    glVertex2f(x0+w, y0);
  }
  glEnd();
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::traverseLightNodes( BaseNode* _node)
{

  if (_node)
  {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != BaseNode::HideSubtree)
    {

      if (_node->status() != BaseNode::HideNode)
      {
        ACG::SceneGraph::LightNode* lnode = dynamic_cast<ACG::SceneGraph::LightNode*>(_node);
        if (lnode)
        {
          ACG::SceneGraph::LightSource light;
          lnode->getLightSource(&light);

//          GLenum lightID = lnode->getGLLightID();
          GLenum lightID = numLights_ + GL_LIGHT0;

          if (lightID != GL_INVALID_ENUM)
          {
            int id = lightID - GL_LIGHT0;

            if (numLights_ < 8)
            {
              if (light.directional())
                lightTypes_[numLights_] = LIGHTTYPE_DIRECTIONAL;
              else if (light.spotCutoff() > 179.5f)
                lightTypes_[numLights_] = LIGHTTYPE_POINT;
              else
                lightTypes_[numLights_] = LIGHTTYPE_SPOT;

              glLightIDs_[numLights_] = id;
              
              ++numLights_;
            }

          }

        }
      }

      if (process_children)
      {

        BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children which are not second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseLightNodes(*cIt);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseLightNodes(*cIt);

      }

    }
  }
}

// depth peeling shader generation

void DepthPeelingPlugin::generatePeelingShaders(GLSL::StringList* _strVertexShaderOut,
                                                GLSL::StringList* _strFragmentShaderOut,
                                                GLSL::StringList* _strGeometryShaderOut,
                                                bool _textured,
                                                bool _flatShaded,
                                                bool _phong,
                                                bool _vertexColor,
                                                bool _wireFrame)
{
  if (_flatShaded) _phong = false;
  if (_vertexColor) _phong = false;

  std::string strColor = "color";
  std::string strNormal = "normal";
  std::string strFragPos = "vPosVS";

  if (_flatShaded)
    strFragPos = "VPosVS";

  if (_wireFrame)
    _flatShaded = _vertexColor = _textured = _phong = false;

  std::string strCode = "";

  for (unsigned int i = 0; i < numLights_ && (!_wireFrame); ++i)
  {
    // new code block for this light
    strCode += "{\n";

    std::string strLight = "gl_LightSource[";
    std::string strMaterial = "gl_FrontLightProduct[";
    {
      char szTmp[8];
      sprintf(szTmp, "%d]", glLightIDs_[i]);

      strLight += szTmp;
      strMaterial += szTmp;
    }

    // light vector
    if (lightTypes_[i] == LIGHTTYPE_DIRECTIONAL)
      strCode += "vec3 vLightVS = " + strLight + ".position.xyz;\n";
    else
      strCode += "vec3 vLightVS = " + strLight + ".position.xyz - " + strFragPos + ".xyz;\n";

    strCode += "vec3 vLightDir = normalize(vLightVS);\n";

    // ambient
    strCode += "vec4 colLight = " + strMaterial + ".ambient;\n";

    // diffuse
    strCode += "float ldotn = dot(vLightDir, " + strNormal + ");\n";
    strCode += "ldotn = clamp(ldotn, 0.0, 1.0);\n";
    strCode += "colLight += ldotn * ";
    if (_textured && _phong)
      strCode += "diffColor * ";

    if (_vertexColor)
    {
      if (_flatShaded)
        strCode += "vColor[2];\n";
      else
        strCode += "gl_Color;\n";
    }
    else
    strCode += strMaterial + ".diffuse;\n";

    //specular
    strCode += "colLight += floor(ldotn + 0.9) * pow(ldotn, gl_FrontMaterial.shininess) * " + strMaterial + ".specular;\n";


    if (lightTypes_[i] == LIGHTTYPE_DIRECTIONAL)
      strCode += strColor + ".rgb += colLight.rgb;\n";
    else 
    {
      // attenuation
      strCode += "float fLenSq = dot(vLightVS, vLightVS);\n";
      strCode += "float atten = " + strLight + ".constantAttenuation + " +
        strLight + ".linearAttenuation * sqrt(fLenSq) + " +
        strLight + ".quadraticAttenuation * fLenSq;\n";

      if (lightTypes_[i] == LIGHTTYPE_POINT)
        strCode += strColor + ".rgb += colLight.rgb / atten;\n";
      else
      {
        // spotlight
        strCode += "float spot = -dot(vLightDir, " + strLight + ".spotDirection);\n";
        strCode += "spot *= step(" + strLight + ".spotCosCutoff, spot);\n";
        strCode += "spot *= pow(spot, " + strLight + ".spotExponent);\n";

        strCode += strColor + ".rgb += " + "(spot / atten) * colLight.rgb;\n";
      }
    }

    strCode += "}\n";
  }

  const char* szVertexShader[] = {"varying vec2 vTexCoord;",
    "varying vec3 vNormal;  // normal: view space",
    "varying vec4 vPosVS; // position in view space",
    "varying vec4 vColor; // used in gouraud / flat shading only",
    "",
    "void main(void)",
    "{",
    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;",
    "  ",
    "  vNormal = gl_NormalMatrix * gl_Normal; // gl_NormalMatrix : upper left 3x3 of WorldViewIT",
    "  vPosVS = (gl_ModelViewMatrix * gl_Vertex);",
    "  vTexCoord = gl_MultiTexCoord0.xy;",
    ""};
  // NOTE: this vertex shader code misses the ending '}' to append the lighting code later

  // geometry shader for flat shading only
  const char* szGeometryShaderStart[] = {
    "#version 120",
    "#extension GL_ARB_geometry_shader4 : enable",
    "",
    "varying vec2 vTexCoord[3];",
    "varying vec4 vPosVS[3];",
    "varying vec3 vNormal[3];",
    "varying vec4 vColor[3];",
    "",
    "varying vec2 VTexCoord;",
    "varying vec3 VNormal;",
    "varying vec4 VPosVS;",
    "varying vec4 VColor;",
    "",
    "",
    "void main()",
    "{",
    "   // recompute triangle normal",
//    "   vec3 normal = cross(vNormal[2].xyz - vNormal[0].xyz, vNormal[1].xyz - vNormal[0].xyz);",
    "   vec3 normal = cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz);",
    "   normal = normalize(normal);",
    "   normal = gl_NormalMatrix * normal;",
//    "   normal = vNormal[2].xyz;",
    "   ",
    "   VPosVS = (gl_PositionIn[0] + gl_PositionIn[1] + gl_PositionIn[2]) / 3.0;",
    "   VPosVS = gl_ModelViewMatrix * VPosVS; // triangle center in view space needed for point and spot lights",
    "   vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.diffuse.a);",
    ""};

  const char* szGeometryShaderEnd[] = {
    "",
    "   for(int i = 0; i < 3; i++)",
    "   {",
    "     gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];",
    "	 VTexCoord = vTexCoord[i];",
    "	 VNormal = normal;",
    "	 VPosVS = vPosVS[i];",
//    "	 VColor = (normal+vec3(0.5,0.5,0.5)).xyzz*0.5;",
    "	 VColor = color;",
    "     EmitVertex();",
    "   }",
    "   EndPrimitive();",
    "   ",
    "}"};

  const char* szFragmentShaderStart[] = {"varying vec2 vTexCoord;",
    "varying vec3 vNormal; ",
    "varying vec4 vPosVS;",
    "varying vec4 vColor;",
    "",
    "uniform sampler2D DiffuseTex;",
    "uniform sampler2DRect DepthBlenderTex;",
    "uniform sampler2DRect FrontBlenderTex;",
    "",
    "#define MAX_DEPTH 1.0",
    "",
    "",
    "void main(void)",
    "{",
    "  // window-space depth interpolated linearly in screen space",
    "  float fragDepth = gl_FragCoord.z;",
    "",
    "  vec2 depthBlender = texture2DRect(DepthBlenderTex, gl_FragCoord.xy).xy;",
    "  vec4 forwardTemp = texture2DRect(FrontBlenderTex, gl_FragCoord.xy);",
    "  ",
    "  // Depths and 1.0-alphaMult always increase",
    "  // so we can use pass-through by default with MAX blending",
    "  gl_FragData[0].xy = depthBlender;",
    "  ",
    "  // Front colors always increase (DST += SRC*ALPHA_MULT)",
    "  // so we can use pass-through by default with MAX blending",
    "  gl_FragData[1] = forwardTemp;",
    "  ",
    "  // Because over blending makes color increase or decrease,",
    "  // we cannot pass-through by default.",
    "  // Each pass, only one fragment writes a color greater than 0",
    "  gl_FragData[2] = vec4(0.0);",
    "",
    "  float nearestDepth = -depthBlender.x;",
    "  float farthestDepth = depthBlender.y;",
    "  float alphaMultiplier = 1.0 - forwardTemp.w;",
    "",
    "  if (fragDepth < nearestDepth || fragDepth > farthestDepth) {",
    "    // Skip this depth in the peeling algorithm",
    "    gl_FragData[0].xy = vec2(-MAX_DEPTH);",
    "    return;",
    "  }",
    "  ",
    "  if (fragDepth > nearestDepth && fragDepth < farthestDepth) {",
    "    // This fragment needs to be peeled again",
    "    gl_FragData[0].xy = vec2(-fragDepth, fragDepth);",
    "    return;",
    "  }",
    "  ",
    "  // If we made it here, this fragment is on the peeled layer from last pass",
    "  // therefore, we need to shade it, and make sure it is not peeled any farther",
    "",};

  const char* szFragmentShaderEnd[] = {"",
    "  gl_FragData[0].xy = vec2(-MAX_DEPTH);",
    "  ",
    "  if (fragDepth == nearestDepth) {",
    "    gl_FragData[1].xyz += color.rgb * color.a * alphaMultiplier;",
    "    gl_FragData[1].w = 1.0 - alphaMultiplier * (1.0 - color.a);",
    "  } else {",
    "    gl_FragData[2] += color;",
    "  }",
    "}"};

  // assemble final shaders:

  // vertex shader
  for (unsigned int i = 0; i < sizeof(szVertexShader) / sizeof(char*); ++i)
  {
    std::string str = szVertexShader[i];
    if (_phong)
    {
      if (!strncmp("varying vec4 vColor", str.c_str(), strlen("varying vec4 vColor")))
      {
//        str.replace(str.begin(), str.begin() + 7, "flat out");
        str += "\n";
        continue;
      }
    }
    str += "\n";
    _strVertexShaderOut->push_back(str);
  }

  // finalize vertex shader
  if (!_phong && !_flatShaded && !_wireFrame)
  {
    // lighting code:
    _strVertexShaderOut->push_back("\nvec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.diffuse.a);");
    _strVertexShaderOut->push_back("\nvec3 normal = vNormal;\n");

    std::string::size_type curPos = 0;
    std::string::size_type newLinePos = strCode.find_first_of('\n');
    while (newLinePos != std::string::npos)
    {
      _strVertexShaderOut->push_back(strCode.substr(curPos, newLinePos - curPos + 1));
      curPos = newLinePos + 1; 
      newLinePos = strCode.find_first_of('\n', curPos);
    }
    _strVertexShaderOut->push_back("\nvColor = color;\n");
  }
  if (_flatShaded)
  {
    if (_vertexColor)
      _strVertexShaderOut->push_back("vColor = gl_Color;\n");
    _strVertexShaderOut->push_back("gl_Position = gl_Vertex;\n");
  }

  _strVertexShaderOut->push_back("\n}");
  
  // fragment shader:
  for (unsigned int i = 0; i < sizeof(szFragmentShaderStart) / sizeof(char*); ++i)
  {
    std::string str = szFragmentShaderStart[i];
    if (_phong)
    {
      if (!strncmp("varying vec4 vColor", str.c_str(), strlen("varying vec4 vColor")))
      {
//        str.replace(str.begin(), str.begin() + 7, "flat in");
          str += "\n";
          continue;
      }
    }
    if (_flatShaded)
    {
      if (!strncmp("varying vec2 vTexCoord", str.c_str(), strlen("varying vec2 vTexCoord")))
        str = "varying vec2 VTexCoord;";
      if (!strncmp("varying vec3 vNormal", str.c_str(), strlen("varying vec3 vNormal")))
        str = "varying vec3 VNormal;";
      if (!strncmp("varying vec4 vPosVS", str.c_str(), strlen("varying vec4 vPosVS")))
        str = "varying vec4 VPosVS;";
      if (!strncmp("varying vec4 vColor", str.c_str(), strlen("varying vec4 vColor")))
        str = "varying vec4 VColor;";
    }
    str += "\n";
    _strFragmentShaderOut->push_back(str);
  }

  if (!_wireFrame)
  {
    if (!_flatShaded)
    {
      _strFragmentShaderOut->push_back("  vec3 normal = normalize(vNormal);\n");
//      _strFragmentShaderOut->push_back("  vec3 normal = normalize(cross(dFdx(vPosVS.xyz), dFdy(vPosVS.xyz)));\n");
      _strFragmentShaderOut->push_back("  vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.diffuse.a);\n");
      if (_textured)
        _strFragmentShaderOut->push_back(" vec4 diffColor = texture2D(DiffuseTex, vTexCoord); color.a *= diffColor.a;\n");
    }
    else
    {
      _strFragmentShaderOut->push_back("  vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.diffuse.a);\n");
      _strFragmentShaderOut->push_back("  vec4 vColor = VColor;\n");
      if (_textured)
        _strFragmentShaderOut->push_back(" vec4 diffColor = texture2D(DiffuseTex, VTexCoord); color.a *= diffColor.a;\n");
    }
    
    if (!_phong)
    {
      if (_textured)
        _strFragmentShaderOut->push_back(" color = diffColor * vColor;\n");
      else
        _strFragmentShaderOut->push_back(" color = vColor;\n");
    }
    else
    {
      // lighting code:
      std::string::size_type curPos = 0;
      std::string::size_type newLinePos = strCode.find_first_of('\n');
      while (newLinePos != std::string::npos)
      {
        _strFragmentShaderOut->push_back(strCode.substr(curPos, newLinePos - curPos + 1));
        curPos = newLinePos + 1; 
        newLinePos = strCode.find_first_of('\n', curPos);
      }
    }
  }
  else // wireframe:
    _strFragmentShaderOut->push_back("  vec4 color = VColor;\n");

  for (unsigned int i = 0; i < sizeof(szFragmentShaderEnd) / sizeof(char*); ++i)
  {
    std::string str = szFragmentShaderEnd[i];
    str += "\n";
    _strFragmentShaderOut->push_back(str);
  }


  // geometry shader:
  if (_flatShaded)
  {
    for (unsigned int i = 0; i < sizeof(szGeometryShaderStart) / sizeof(char*); ++i)
    {
      std::string str = szGeometryShaderStart[i];
      str += "\n";
      _strGeometryShaderOut->push_back(str);
    }

    // lighting code:
    std::string::size_type curPos = 0;
    std::string::size_type newLinePos = strCode.find_first_of('\n');
    while (newLinePos != std::string::npos)
    {
      _strGeometryShaderOut->push_back(strCode.substr(curPos, newLinePos - curPos + 1));
      curPos = newLinePos + 1; 
      newLinePos = strCode.find_first_of('\n', curPos);
    }

    for (unsigned int i = 0; i < sizeof(szGeometryShaderEnd) / sizeof(char*); ++i)
    {
      std::string str = szGeometryShaderEnd[i];
      str += "\n";
      _strGeometryShaderOut->push_back(str);
    }
  }


  // debug output:
#ifdef DEPTHPEELING_SHADER_EXPORT
  FILE* pShaderOut = fopen("peel_vertex.glsl", "wt");
  for (GLSL::StringList::iterator it = _strVertexShaderOut->begin(); it != _strVertexShaderOut->end(); ++it)
    fprintf(pShaderOut, it->c_str());
  fclose(pShaderOut);


  pShaderOut = fopen("peel_frag.glsl", "wt");
  for (GLSL::StringList::iterator it = _strFragmentShaderOut->begin(); it != _strFragmentShaderOut->end(); ++it)
    fprintf(pShaderOut, it->c_str());
  fclose(pShaderOut);

  if (_flatShaded)
  {
    pShaderOut = fopen("peel_geom.glsl", "wt");
    for (GLSL::StringList::iterator it = _strGeometryShaderOut->begin(); it != _strGeometryShaderOut->end(); ++it)
      fprintf(pShaderOut, it->c_str());
    fclose(pShaderOut);
  }
#endif
}

//////////////////////////////////////////////////////////////////////////

template <class Action>
bool
DepthPeelingPlugin::traverseDrawApplyAction( BaseNode* _node, Action& _action, ACG::SceneGraph::DrawModes::DrawMode _globalDrawMode, int _pass, int _peelPass)
{
  bool ret = true; // process_children flag returned

  // use nodes drawmode to grab a peeler program with correct shading
  ACG::SceneGraph::DrawModes::DrawMode dm, dmSave = _node->drawMode();
  dm = dmSave;

  if (!dm) return ret;

  if (dm & ACG::SceneGraph::DrawModes::DEFAULT)
    dm = _globalDrawMode;
/*
  if (dm & ACG::SceneGraph::DrawModes::HIDDENLINE)
  {
    // hiddenline is accomplished with wireframe in first peel layer only
    dm &= ~ACG::SceneGraph::DrawModes::HIDDENLINE;
    dm |= ACG::SceneGraph::DrawModes::WIREFRAME;
    if (_peelPass > 2)
      return ret;
    _node->drawMode(dm);
  }
*/
  if (dm & ACG::SceneGraph::DrawModes::WIREFRAME || dm & ACG::SceneGraph::DrawModes::HIDDENLINE)
  {
    ACG::SceneGraph::DrawModes::DrawMode dmShaded = dm;
    dmShaded &= ~ACG::SceneGraph::DrawModes::WIREFRAME;
    dmShaded &= ~ACG::SceneGraph::DrawModes::HIDDENLINE;

    // polygon only draw
    if (dmShaded)
    {
      _node->drawMode(dmShaded); // evil method: change nodes drawmode here, restore it later
      ret &= traverseDrawApplyAction(_node, _action, _globalDrawMode, _pass, _peelPass);
    }

    // wireframe only follows
    dm &= (~dmShaded);
    _node->drawMode(dm);
  }

  unsigned int shaderIndex = getPeelShaderIndex(dm);

  // do hiddenline algorithmus manually here
  GLenum prev_depth = glStateTmp_->depthFunc();
  if (dm & ACG::SceneGraph::DrawModes::HIDDENLINE)
  {
    // manual hiddenline

    // First:
    // Render all faces in background color to initialize z-buffer
    _node->drawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);

    GLSL::Program* peelProg = peelProgs_[PEEL_SHADER_HIDDENLINE];
    peelProg->use();
    peelProg->setUniform("DepthBlenderTex", 4);
    peelProg->setUniform("FrontBlenderTex", 5);
    peelProg->setUniform("ObjectColor", glStateTmp_->clear_color());

    ACG::GLState::depthRange(0.01, 1.0);
    ACG::GLState::lockDepthRange();
    ACG::GLState::lockProgram();
    ret &= _action(_node);
    ACG::GLState::unlockProgram();
    ACG::GLState::unlockDepthRange();
    ACG::GLState::depthRange(0.0, 1.0);

    // Second
    // Render the lines. All lines not on the front will be skipped in z-test
    _node->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);

    ACG::GLState::depthFunc(GL_LEQUAL);
    ACG::GLState::lockDepthFunc();

    // use wireframe shader now
    shaderIndex = PEEL_SHADER_WIREFRAME;
  }

  GLSL::Program* peelProg = peelProgs_[shaderIndex];
  peelProg->use();
  peelProg->setUniform("DepthBlenderTex", 4);
  peelProg->setUniform("FrontBlenderTex", 5);

  if (shaderIndex & PEEL_SHADER_FLAT)
  {
    // set geomtry shader constants
    peelProg->setGeometryInputType(GL_TRIANGLES);
    peelProg->setGeometryOutputType(GL_TRIANGLE_STRIP);
    peelProg->setGeometryVertexCount(3);
  }
  peelProg->setUniform("DiffuseTex", 0);

  ACG::GLState::lockProgram();
  ret &= _action(_node);
  ACG::GLState::unlockProgram();

  // restore state
  _node->drawMode(dmSave);

  if (dm & ACG::SceneGraph::DrawModes::HIDDENLINE)
  {
    ACG::GLState::unlockDepthFunc();

    //restore depth buffer comparison function
    ACG::GLState::depthFunc(prev_depth);
  }

  return ret;
}

//////////////////////////////////////////////////////////////////////////

template <class Action>
void
DepthPeelingPlugin::traverseDraw( BaseNode* _node, Action& _action, ACG::SceneGraph::DrawModes::DrawMode _globalDrawMode, int _pass, int _peelPass)
{

  // Process node if it exists
  if (_node) {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != BaseNode::HideSubtree) {

      // Executes this nodes enter function (if available and active in multipass)
      if ( _node->multipassStatusActive(_pass) ) {
        if_has_enter(_action, _node);
      }

      // If the node itself is hidden, don't call the action on it.
      // Additionally check if rendering order is node first. otherwise, we will call it after the children.
      // And check if it should be called in this rendering pass.
      if ( (_node->status() != BaseNode::HideNode )  && ( _node->traverseMode() & BaseNode::NodeFirst ) && _node->multipassNodeActive(_pass))
        process_children &= traverseDrawApplyAction(_node, _action, _globalDrawMode, _pass, _peelPass);

      if (process_children) {

        BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseDraw(*cIt, _action, _globalDrawMode, _pass, _peelPass);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & BaseNode::SecondPass)
            traverseDraw(*cIt, _action, _globalDrawMode, _pass, _peelPass);

      }


      // If we are in childrenfirst node, the children have been painted andwe now check, if we can draw this node.
      // If its hidden, ignore it.
      // If it should not be rendered in this pass, ignore it too.
      if ( (_node->traverseMode() & BaseNode::ChildrenFirst ) && (_node->status() != BaseNode::HideNode) && _node->multipassNodeActive(_pass) )
        traverseDrawApplyAction(_node, _action, _globalDrawMode, _pass, _peelPass);

      // Call the leave function of the node (if available and active in multipass).
      if ( _node->multipassStatusActive(_pass) )
        if_has_leave(_action, _node);

    } // if (status != BaseNode::HideSubtree)
  } // if(node_)
}

void DepthPeelingPlugin::drawScenePeelPass(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, BaseNode* _sceneGraphRoot, int _peelPass)
{
  ACG::SceneGraph::DrawAction action(_drawMode, *_glState, false);
//  traverseDraw(_sceneGraphRoot, action, *_glState, _drawMode);

  // Reset render pass counter
  _glState->reset_render_pass();

  // Get max render passes
  unsigned int max_passes = _glState->max_render_passes();

  // Render all passes
  for(unsigned int pass = BaseNode::PASS_1; pass <= (BaseNode::PASS_1 + max_passes); ++pass) {

    // Traverse scenegraph
    traverseDraw (_sceneGraphRoot, action, _drawMode, pass, _peelPass);
    // Increment render pass counter by 1
    _glState->next_render_pass();
  }

  // Reset render pass counter
  _glState->reset_render_pass();
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::drawScenePass(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, BaseNode* _sceneGraphRoot)
{
  ACG::SceneGraph::DrawAction action(_drawMode, *_glState, false);
  traverse_multipass(_sceneGraphRoot, action, *_glState, _drawMode);
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::updatePeelingShaderSet()
{
  bool rebuildShaders = false;

  BaseNode* sceneGraphRoot = PluginFunctions::getSceneGraphRootNode();

  // peel shader is rebuilt, if a new light is added or a light type changed

  LightType prevLights[8];
  unsigned int prevIDs[8];
  unsigned int prevNum = numLights_;

  memcpy(prevLights, lightTypes_, sizeof(LightType) * numLights_);
  memcpy(prevIDs, glLightIDs_, sizeof(unsigned int) * numLights_);

  numLights_ = 0;
  traverseLightNodes(sceneGraphRoot);

  // test for necessary shader recompilation
  if (numLights_ == prevNum)
  {
    // look for any light node changes
    if (memcmp(prevLights, lightTypes_, sizeof(LightType) * numLights_) ||
      memcmp(prevIDs, glLightIDs_, sizeof(unsigned int) * numLights_))
      rebuildShaders = true;
  }
  else rebuildShaders = true;

  if (rebuildShaders)
  {
    // delete old shaders and programs
    for (unsigned int i = 0; i < sizeof(peelShaders_) / sizeof(peelShaders_[0]); ++i)
    {
      delete peelShaders_[i]; peelShaders_[i] = 0;
    }

    for (unsigned int i = 0; i < sizeof(peelProgs_) / sizeof(peelProgs_[0]); ++i)
    {
      delete peelProgs_[i]; peelProgs_[i] = 0;
    }

    for (unsigned int i = 0; i < PEEL_NUM_COMBINATIONS; ++i)
    {
      GLuint texturedDrawMode = 0, flatDrawMode = 0, phongDrawMode = 0, vertexColorDrawMode = 0, gouraudDrawMode = 0;

      texturedDrawMode = i & PEEL_SHADER_TEXTURED;
      flatDrawMode = i & PEEL_SHADER_FLAT;
      phongDrawMode = i & PEEL_SHADER_PHONG;
      vertexColorDrawMode = i & PEEL_SHADER_VERTEXCOLORS;
      gouraudDrawMode = i & PEEL_SHADER_GOURAUD;

      if (i != PEEL_SHADER_WIREFRAME)
      {
        // filter nonsense
        if (flatDrawMode && phongDrawMode) continue;
        if (flatDrawMode && gouraudDrawMode) continue;
        if (phongDrawMode && gouraudDrawMode) continue;

        if (phongDrawMode + flatDrawMode + gouraudDrawMode == 0) continue;
      }

      if (flatDrawMode)
      {
        // replace flat shading with gouraud, if geometry shaders are not supported
        if (!ACG::checkExtensionSupported("GL_ARB_geometry_shader4") && !ACG::checkExtensionSupported("GL_EXT_geometry_shader4"))
        {
          flatDrawMode = 0;
          gouraudDrawMode = 1;
        }
      }

      GLSL::StringList strVertexShader, strFragmentShader, strGeometryShader;
      generatePeelingShaders(&strVertexShader, &strFragmentShader, &strGeometryShader, texturedDrawMode != 0, flatDrawMode != 0, phongDrawMode != 0, vertexColorDrawMode != 0, i == PEEL_SHADER_WIREFRAME);

      peelProgs_[i] = new GLSL::Program();
      

      GLSL::VertexShader* pVertexSh = new GLSL::VertexShader();
      pVertexSh->setSource(strVertexShader);
      pVertexSh->compile();
      peelShaders_[i*3 + 0] = pVertexSh;
      peelProgs_[i]->attach(pVertexSh);

      if (flatDrawMode)
      {
        GLSL::GeometryShader* pGeomSh = new GLSL::GeometryShader();
        pGeomSh->setSource(strGeometryShader);
        pGeomSh->compile();
        peelShaders_[i*3 + 1] = pGeomSh;
        peelProgs_[i]->attach(pGeomSh);
      }

      GLSL::FragmentShader* pFragSh = new GLSL::FragmentShader();
      pFragSh->setSource(strFragmentShader);
      pFragSh->compile();
      peelShaders_[i*3 + 2] = pFragSh;
      peelProgs_[i]->attach(pFragSh);

      peelProgs_[i]->link();
      ACG::glCheckErrors();


#ifdef DEPTHPEELING_SHADER_EXPORT
      char szFileName[256];
      sprintf(szFileName, "peel_vertex_%02u.glsl", i);
      FILE* pShaderOut = fopen(szFileName, "wt");
      for (GLSL::StringList::iterator it = strVertexShader.begin(); it != strVertexShader.end(); ++it)
        fprintf(pShaderOut, it->c_str());
      fclose(pShaderOut);


      sprintf(szFileName, "peel_frag%02u.glsl", i);
      pShaderOut = fopen(szFileName, "wt");
      for (GLSL::StringList::iterator it = strFragmentShader.begin(); it != strFragmentShader.end(); ++it)
        fprintf(pShaderOut, it->c_str());
      fclose(pShaderOut);

      if (flatDrawMode)
      {
        sprintf(szFileName, "peel_geom%02u.glsl", i);
        pShaderOut = fopen(szFileName, "wt");
        for (GLSL::StringList::iterator it = strGeometryShader.begin(); it != strGeometryShader.end(); ++it)
          fprintf(pShaderOut, it->c_str());
        fclose(pShaderOut);
      }
#endif
    }
  }


  // create a special shader for hiddenline
  // hiddenline = wireframe + early z cull
  // the special shader is needed for the z buffer pass

  const char* szVertexShader[] = {
    "void main(void)",
    "{",
    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;",
    "}"};

  const char* szFragmentShader[] = {
    "uniform vec4 ObjectColor;",
    "uniform sampler2DRect DepthBlenderTex;",
    "uniform sampler2DRect FrontBlenderTex;",
    "",
    "#define MAX_DEPTH 1.0",
    "",
    "",
    "void main(void)",
    "{",
    "  // window-space depth interpolated linearly in screen space",
    "  float fragDepth = gl_FragCoord.z;",
    "",
    "  vec2 depthBlender = texture2DRect(DepthBlenderTex, gl_FragCoord.xy).xy;",
    "  vec4 forwardTemp = texture2DRect(FrontBlenderTex, gl_FragCoord.xy);",
    "  ",
    "  // Depths and 1.0-alphaMult always increase",
    "  // so we can use pass-through by default with MAX blending",
    "  gl_FragData[0].xy = depthBlender;",
    "  ",
    "  // Front colors always increase (DST += SRC*ALPHA_MULT)",
    "  // so we can use pass-through by default with MAX blending",
    "  gl_FragData[1] = forwardTemp;",
    "  ",
    "  // Because over blending makes color increase or decrease,",
    "  // we cannot pass-through by default.",
    "  // Each pass, only one fragment writes a color greater than 0",
    "  gl_FragData[2] = vec4(0.0);",
    "",
    "  float nearestDepth = -depthBlender.x;",
    "  float farthestDepth = depthBlender.y;",
    "  float alphaMultiplier = 1.0 - forwardTemp.w;",
    "",
    "  if (fragDepth < nearestDepth || fragDepth > farthestDepth) {",
    "    // Skip this depth in the peeling algorithm",
    "    gl_FragData[0].xy = vec2(-MAX_DEPTH);",
    "    return;",
    "  }",
    "  ",
    "  if (fragDepth > nearestDepth && fragDepth < farthestDepth) {",
    "    // This fragment needs to be peeled again",
    "    gl_FragData[0].xy = vec2(-fragDepth, fragDepth);",
    "    return;",
    "  }",
    "  ",
    "  // If we made it here, this fragment is on the peeled layer from last pass",
    "  // therefore, we need to shade it, and make sure it is not peeled any farther",
    "  vec4 color = ObjectColor;",
    "  gl_FragData[0].xy = vec2(-MAX_DEPTH);",
    "  ",
    "  if (fragDepth == nearestDepth) {",
    "    gl_FragData[1].xyz += color.rgb * color.a * alphaMultiplier;",
    "    gl_FragData[1].w = 1.0 - alphaMultiplier * (1.0 - color.a);",
    "  } else {",
    "    gl_FragData[2] += color;",
    "  }",
    "}"};

    GLSL::StringList strVertexShader, strFragmentShader;
    for (unsigned int i = 0; i < sizeof(szVertexShader) / sizeof(char*); ++i)
    {
      std::string str = szVertexShader[i];
      str += "\n";
      strVertexShader.push_back(str);
    }

    for (unsigned int i = 0; i < sizeof(szFragmentShader) / sizeof(char*); ++i)
    {
      std::string str = szFragmentShader[i];
      str += "\n";
      strFragmentShader.push_back(str);
    }

    GLSL::Program* peelHiddenLine = peelProgs_[PEEL_SHADER_HIDDENLINE] = new GLSL::Program();

    GLSL::VertexShader* pVertexSh = new GLSL::VertexShader();
    pVertexSh->setSource(strVertexShader);
    pVertexSh->compile();
    peelShaders_[PEEL_SHADER_HIDDENLINE*3 + 0] = pVertexSh;
    peelHiddenLine->attach(pVertexSh);

    GLSL::FragmentShader* pFragSh = new GLSL::FragmentShader();
    pFragSh->setSource(strFragmentShader);
    pFragSh->compile();
    peelShaders_[PEEL_SHADER_HIDDENLINE*3 + 2] = pFragSh;
    peelHiddenLine->attach(pFragSh);

    peelHiddenLine->link();
    ACG::glCheckErrors();
}

//////////////////////////////////////////////////////////////////////////

unsigned int DepthPeelingPlugin::getPeelShaderIndex(ACG::SceneGraph::DrawModes::DrawMode _drawMode)
{
  if (_drawMode & ACG::SceneGraph::DrawModes::WIREFRAME ||
    _drawMode & ACG::SceneGraph::DrawModes::HIDDENLINE)
    return PEEL_SHADER_WIREFRAME;

  bool textured = _drawMode & ACG::SceneGraph::DrawModes::SOLID_TEXTURED || _drawMode & ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED;
  bool flat = _drawMode & ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED;
  bool phong = _drawMode & ACG::SceneGraph::DrawModes::SOLID_PHONG_SHADED;
  bool vertexColor = _drawMode & ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED || _drawMode & ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED_FLAT_SHADED ||
    _drawMode & ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED || _drawMode & ACG::SceneGraph::DrawModes::POINTS_COLORED || _drawMode & ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED;

  bool gouraud = _drawMode & ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED;

  // fix illegal combinations
  if (phong && flat) flat = false;
  if (flat && gouraud) gouraud = false;
  if (gouraud && phong) phong = false;

  // wireframe, point, etc use gouraud shading
  if ((!phong) && (!gouraud) && (!flat)) gouraud = true;

  unsigned int idx = 0;

  if (flat) idx |= PEEL_SHADER_FLAT;
  if (gouraud) idx |= PEEL_SHADER_GOURAUD;
  if (phong) idx |= PEEL_SHADER_PHONG;
  if (vertexColor) idx |= PEEL_SHADER_VERTEXCOLORS;
  if (textured) idx |= PEEL_SHADER_TEXTURED;

  return idx;
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  glStateTmp_ = _glState;

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  const GLuint targetFbo = ACG::GLState::getFramebufferDraw();

  int viewerId = _properties.viewerId();

  ViewerResources* pViewer = &viewerRes_[viewerId];
  pViewer->glWidth_ = _glState->viewport_width();
  pViewer->glHeight_ = _glState->viewport_height();

  if (pViewer->glWidth_ != pViewer->rtWidth_ || pViewer->glHeight_ != pViewer->rtHeight_)
    reloadResources(viewerId);
  
//  updatePeelingShaderSet(viewerId, _properties.drawMode());
  updatePeelingShaderSet();
  BaseNode* sceneGraphRoot = PluginFunctions::getSceneGraphRootNode();

  ACG::GLState::depthFunc(GL_LESS);


  ACG::GLState::disable(GL_CULL_FACE);
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_NORMALIZE);

  ACG::GLState::lockDepthFunc();
  ACG::GLState::lockState(GL_CULL_FACE);
  ACG::GLState::lockState(GL_LIGHTING);
  ACG::GLState::lockState(GL_NORMALIZE);

  // from nvidia demo code:
  //  needed some minor adjustments

  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0_EXT,
    GL_COLOR_ATTACHMENT1_EXT,
    GL_COLOR_ATTACHMENT2_EXT,
    GL_COLOR_ATTACHMENT3_EXT,
    GL_COLOR_ATTACHMENT4_EXT,
    GL_COLOR_ATTACHMENT5_EXT,
    GL_COLOR_ATTACHMENT6_EXT};

  // the farthest depth value possible in the depth buffer
  const float maxDepth = 1.0f;

  ACG::GLState::disable(GL_DEPTH_TEST);
  ACG::GLState::enable(GL_BLEND);

  ACG::GLState::lockState(GL_DEPTH_TEST);
  ACG::GLState::lockState(GL_BLEND);

  
  
  /* FIXED: VIEWPORT BUG
  
  log-window causes viewport shift by 16 units upward in window's y axis
  therefore the scene gets rendered only in the upper part of the viewport: 
    ->  glViewport(0, 16, glWidht, glHeight_)
  
  glHeight_() is NOT the height of the back buffer (i.e. glViewer window), 
    but the height of scene target view
    -> glHeight_() is 16 units less than the back buffer
  
  -> all render targets are 16 units less in height than back buffer
  
  since the scene has to use the full render targets size,
  use glViewport(0, 0, glWidth_, glHeight_) for all offscreen rendering
  
  in the final pass, shift the viewport up by 16 units and use the shift amount
  in the shader (uniform g_Offset), to finally get the correct sampling coordinates
  
  
  note: shift amount is not hardcoded, but fetched from glGetIntegerv(GL_VIEWPORT)
  */

  
  GLint old_viewport[4];
  glGetIntegerv(GL_VIEWPORT, old_viewport);
  glViewport(0, 0, pViewer->glWidth_, pViewer->glHeight_);

  for (int i = 0; i < 6; ++i)
  {
    ACG::GLState::activeTexture(GL_TEXTURE0 + i);
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
  }
  

  // ---------------------------------------------------------------------
  // 1. Initialize Min-Max Depth Buffer
  // ---------------------------------------------------------------------

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, pViewer->blendDualPeelFbo_);

  // Render targets 1 and 2 store the front and back colors
  // Clear to 0.0 and use MAX blending to filter written color
  // At most one front color and one back color can be written every pass
  ACG::GLState::drawBuffers(2, &drawBuffers[1]);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
  ACG::GLState::drawBuffer(drawBuffers[0]);
  glClearColor(-maxDepth, -maxDepth, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  ACG::GLState::blendEquation(GL_MAX_EXT);
  ACG::GLState::lockBlendEquation();

  blendDualPeelProg_[0]->use();
  ACG::GLState::lockProgram();
  drawScenePass(_glState, _properties.drawMode(), sceneGraphRoot);
  ACG::GLState::unlockProgram();



  int currId = 0;

  // ---------------------------------------------------------------------
  // 2. Dual Depth Peeling + Blending
  // ---------------------------------------------------------------------

  // Since we cannot blend the back colors in the geometry passes,
  // we use another render target to do the alpha blending
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualBackBlenderFboId);
  ACG::GLState::drawBuffer(drawBuffers[6]);
  glClearColor(_glState->clear_color()[0], _glState->clear_color()[1], _glState->clear_color()[2], 0);

  glClear(GL_COLOR_BUFFER_BIT);

  // Geometry layers are peeled until the sample query returns 0
  GLuint sampleCount = 1;
  for (int pass = 1; sampleCount; ++pass)
  {
    currId = pass % 2;
    int prevId = 1 - currId;
    int bufId = currId * 3;

    ACG::GLState::drawBuffers(2, &drawBuffers[bufId+1]);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ACG::GLState::drawBuffer(drawBuffers[bufId+0]);
    glClearColor(-maxDepth, -maxDepth, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render target 0: RG32F MAX blending
    // Render target 1: RGBA MAX blending
    // Render target 2: RGBA MAX blending
    ACG::GLState::drawBuffers(3, &drawBuffers[bufId+0]);
    ACG::GLState::unlockBlendEquation();
    ACG::GLState::blendEquation(GL_MAX_EXT);
    ACG::GLState::lockBlendEquation();

    ACG::GLState::activeTexture(GL_TEXTURE5); // front_blender_tex  base_offset: 2
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + prevId]);

    ACG::GLState::activeTexture(GL_TEXTURE4); // depth_tex  base_offset: 0
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[0 + prevId]);


    // scene geometry peeling pass
    //  note that the peel shader is set right before rendering in the traverser, based on a node's drawmode
    ACG::GLState::activeTexture(GL_TEXTURE0);

    ACG::GLState::shadeModel(GL_SMOOTH); // flat shading is emulated in Geometry Shader, which only works with interpolated vertex shader output
    ACG::GLState::lockShadeModel();
    drawScenePeelPass(_glState, _properties.drawMode(), sceneGraphRoot, pass);
    ACG::GLState::unlockShadeModel();


    // Full screen pass to alpha-blend the back color
    ACG::GLState::drawBuffer(drawBuffers[6]);

    ACG::GLState::unlockBlendEquation();
    ACG::GLState::blendEquation(GL_FUNC_ADD);
    ACG::GLState::lockBlendEquation();
    ACG::GLState::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // start samples counter query
    glBeginQuery(GL_SAMPLES_PASSED_ARB, blendQueryID_);

    blendDualPeelProg_[2]->use();
    blendDualPeelProg_[2]->setUniform("TempTex", 4);

    ACG::GLState::activeTexture(GL_TEXTURE4); // back_temp_tex  base_offset: 4
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[4 + currId]);

    drawQuadProj(); // full screen quad, already projected

    glEndQuery(GL_SAMPLES_PASSED_ARB);
    glGetQueryObjectuiv(blendQueryID_, GL_QUERY_RESULT_ARB, &sampleCount);
  }

  ACG::GLState::unlockBlendEquation();

  ACG::GLState::unlockState(GL_BLEND);
  ACG::GLState::disable(GL_BLEND);



  // ---------------------------------------------------------------------
  // 3. Final Pass
  //  operates on screen pixels only
  // ---------------------------------------------------------------------

  // enable back buffer
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, targetFbo);
  ACG::GLState::drawBuffer(GL_BACK);

  // program id 3
  blendDualPeelProg_[3]->use();
  blendDualPeelProg_[3]->setUniform("FrontBlenderTex", 4);
  blendDualPeelProg_[3]->setUniform("BackBlenderTex", 5);

  // bugfix for multiple viewports:
  //  gl_FragCoord represents the screen space coordinate of a pixel into the back buffer
  //  this has to be back-shifted by the upper left viewport coordinate to get correct texture coordinates
  blendDualPeelProg_[3]->setUniform("ViewportOffset", ACG::Vec2f(old_viewport[0], old_viewport[1]));

  ACG::GLState::activeTexture(GL_TEXTURE5); // back_blender:  offset 6
  ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[6]);


  ACG::GLState::activeTexture(GL_TEXTURE4); // front_blender_tex  base_offset: 2
  ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + currId]);

  
  glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
  drawQuadProj(-1.0f, 1.0f, 2.0f, 2.0f);


  blendDualPeelProg_[3]->disable();



  ACG::glCheckErrors();


  // unlock states
  ACG::GLState::unlockDepthFunc();
  ACG::GLState::unlockState(GL_CULL_FACE);
  ACG::GLState::unlockState(GL_LIGHTING);
  ACG::GLState::unlockState(GL_NORMALIZE);

  ACG::GLState::unlockState(GL_DEPTH_TEST);
  ACG::GLState::unlockState(GL_BLEND);


  glPopAttrib();
}

QString DepthPeelingPlugin::checkOpenGL() {

  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( ! flags.testFlag(QGLFormat::OpenGL_Version_2_0) )
    return QString("Insufficient OpenGL Version! OpenGL 2.0 or higher required");

  //Get OpenGL extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));

  // Collect missing extension
  QString missing = "";

  if ( !glExtensions.contains("GL_ARB_geometry_shader4") )
    missing += "Missing Extension GL_ARB_geometry_shader4\n";

  if ( !glExtensions.contains("GL_ARB_vertex_program") )
    missing += "Missing Extension GL_ARB_vertex_program\n";

  return missing;

}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( depthpeelingplugin , DepthPeelingPlugin );
#endif


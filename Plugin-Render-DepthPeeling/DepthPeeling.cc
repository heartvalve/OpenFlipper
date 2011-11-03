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

#include "DepthPeeling.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ObjectTypes/Light/LightNode.hh>


// this define enables a shader export of the generated peel shader for debugging purpose
//#define DEPTHPEELING_SHADER_EXPORT


DepthPeelingPlugin::ViewerResources::ViewerResources()
{
  rtWidth_ = rtHeight_ = 0;

  glWidth_ = glHeight_ = 0;

  memset(blendDualPeelTexID_, 0, 7*4);

  peelProg_ = 0;
  peelVertexShader_ = 0;
  peelFragmentShader_ = 0;
  peelGeometryShader_ = 0;

  blendDualPeelFbo_ = 0;
}

void DepthPeelingPlugin::initializePlugin()
{
  memset(blendShaders_, 0, 8 * sizeof(GLSL::Shader*));

  memset(blendDualPeelProg_, 0, 4 * sizeof(GLSL::Program*));

  blendQueryID_ = 0;
  numLights_ = 0;

  ACG::glCheckErrors();
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::exit()
{
  destroyResources();
}

//////////////////////////////////////////////////////////////////////////

QString DepthPeelingPlugin::rendererName() {
  return QString("Depth Peeling Renderer");
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

#ifdef ARCH_DARWIN
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i]);
    // texture access: clamped
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // filter: none
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, DualPeelIntFmt[i], p->glWidth_, p->glHeight_, 0, fmt, GL_FLOAT, 0);
#else
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, p->blendDualPeelTexID_[i]);
    // texture access: clamped
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // filter: none
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, DualPeelIntFmt[i], p->glWidth_, p->glHeight_, 0, fmt, GL_FLOAT, 0);
#endif




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
      #ifdef ARCH_DARWIN
        GL_TEXTURE_RECTANGLE_EXT, p->blendDualPeelTexID_[i < 3 ? 2*i : 2*(i-3) +1], 0);
      #else
        GL_TEXTURE_RECTANGLE, p->blendDualPeelTexID_[i < 3 ? 2*i : 2*(i-3) +1], 0);
      #endif
    }

    // back_blender_tex_id
    #ifdef ARCH_DARWIN
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE_EXT,
                                p->blendDualPeelTexID_[6], 0);
    #else
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE,
                                p->blendDualPeelTexID_[6], 0);
    #endif
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

  delete p->peelVertexShader_; p->peelVertexShader_ = 0;
  delete p->peelFragmentShader_; p->peelFragmentShader_ = 0;
  delete p->peelGeometryShader_; p->peelGeometryShader_ = 0;
  delete p->peelProg_; p->peelProg_ = 0;
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
  std::vector<int> pLightTypes;

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
                                                bool _phong)
{
  if (_flatShaded) _phong = false;

  std::string strColor = "color";
  std::string strNormal = "normal";
  std::string strFragPos = "vPosVS";

  if (_flatShaded)
    strFragPos = "VPosVS";

  std::string strCode = "";

  for (unsigned int i = 0; i < numLights_; ++i)
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
    "   vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.ambient.a);",
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
  if (!_phong && !_flatShaded)
  {
    // lighting code:
    _strVertexShaderOut->push_back("\nvec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.ambient.a);");
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
    _strVertexShaderOut->push_back("gl_Position = gl_Vertex;\n");

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

  if (!_flatShaded)
  {
    _strFragmentShaderOut->push_back("  vec3 normal = normalize(vNormal);\n");
    _strFragmentShaderOut->push_back("  vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.ambient.a);\n");
  }
  else
  {
    _strFragmentShaderOut->push_back("  vec4 color = vec4(gl_FrontMaterial.emission.rgb, gl_FrontMaterial.ambient.a);\n");
    _strFragmentShaderOut->push_back("  vec4 vColor = VColor;\n");
  }

  if (_textured)
    _strFragmentShaderOut->push_back(" vec4 diffColor = texture2D(DiffuseTex, vTexCoord); color.a *= diffColor.a;\n");

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

void DepthPeelingPlugin::drawScenePass(ACG::GLState* _glState, Viewer::ViewerProperties& _properties, BaseNode* _sceneGraphRoot)
{
  ACG::SceneGraph::DrawAction action(_properties.drawMode(), *_glState, false);
  ACG::SceneGraph::traverse_multipass(_sceneGraphRoot, action, *_glState, _properties.drawMode());
}

//////////////////////////////////////////////////////////////////////////

void DepthPeelingPlugin::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  int viewerId = _properties.viewerId();

  viewerRes_[viewerId].glWidth_ = _glState->viewport_width();
  viewerRes_[viewerId].glHeight_ = _glState->viewport_height();
  ViewerResources* pViewer = &viewerRes_[viewerId];

  bool rebuildShaders = false;

  if (pViewer->glWidth_ != pViewer->rtWidth_ || pViewer->glHeight_ != pViewer->rtHeight_) {
    reloadResources(viewerId);
    rebuildShaders = true;
  }

  BaseNode* sceneGraphRoot = PluginFunctions::getSceneGraphRootNode();

  {
    // peel shader is rebuilt, if a new light is added or a light type changed
    // or a viewer DrawMode change

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

    // check drawmode
    bool texturedDrawMode = _properties.drawMode() & ACG::SceneGraph::DrawModes::SOLID_TEXTURED || _properties.drawMode() & ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED;
    bool flatDrawMode = _properties.drawMode() & ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED || _properties.drawMode() & ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED;
    bool phongDrawMode = _properties.drawMode() & ACG::SceneGraph::DrawModes::SOLID_PHONG_SHADED;
    if (flatDrawMode) phongDrawMode = false;

    if ( !pViewer->peelProg_ || ! pViewer->peelVertexShader_ || ! pViewer->peelFragmentShader_ ||
      pViewer->peelShaderTextured_ != texturedDrawMode || 
      pViewer->peelShaderFlat_ != flatDrawMode || 
      pViewer->peelShaderPhong_ != phongDrawMode)
    {
      rebuildShaders = true;
      pViewer->peelShaderTextured_ = texturedDrawMode;
      pViewer->peelShaderFlat_ = flatDrawMode;
      pViewer->peelShaderPhong_ = phongDrawMode;
    }

    if (rebuildShaders)
    {
      GLSL::StringList strVertexShader, strFragmentShader, strGeometryShader;

      generatePeelingShaders(&strVertexShader, &strFragmentShader, &strGeometryShader, texturedDrawMode, flatDrawMode, phongDrawMode);

      delete pViewer->peelProg_;

      // recompile

      pViewer->peelProg_ = new GLSL::Program();

      delete pViewer->peelVertexShader_;
      delete pViewer->peelFragmentShader_;
      delete pViewer->peelGeometryShader_;

      pViewer->peelVertexShader_ = GLSL::PtrVertexShader(new GLSL::VertexShader());
      pViewer->peelVertexShader_->setSource(strVertexShader);
      pViewer->peelVertexShader_->compile();

      pViewer->peelFragmentShader_ = GLSL::PtrFragmentShader(new GLSL::FragmentShader());
      pViewer->peelFragmentShader_->setSource(strFragmentShader);
      pViewer->peelFragmentShader_->compile();

      pViewer->peelProg_->attach(pViewer->peelVertexShader_);
      pViewer->peelProg_->attach(pViewer->peelFragmentShader_);

      pViewer->peelGeometryShader_ = 0;
      if (flatDrawMode)
      {
        pViewer->peelGeometryShader_ = GLSL::PtrGeometryShader(new GLSL::GeometryShader());
        pViewer->peelGeometryShader_->setSource(strGeometryShader);
        pViewer->peelFragmentShader_->compile();
        pViewer->peelProg_->attach(pViewer->peelGeometryShader_);
      }

      pViewer->peelProg_->link();


      ACG::glCheckErrors();

    }
  }
  
  
  
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

    #ifdef ARCH_DARWIN
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
    #else
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, 0);
    #endif

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
  drawScenePass(_glState, _properties, sceneGraphRoot);
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


    pViewer->peelProg_->use();
    pViewer->peelProg_->setUniform("DepthBlenderTex", 4);
    pViewer->peelProg_->setUniform("FrontBlenderTex", 5);

    if (pViewer->peelShaderFlat_)
    {
      // set geomtry shader constants
      pViewer->peelProg_->setGeometryInputType(GL_TRIANGLES);
      pViewer->peelProg_->setGeometryOutputType(GL_TRIANGLE_STRIP);
      pViewer->peelProg_->setGeometryVertexCount(3);
    }

    ACG::GLState::activeTexture(GL_TEXTURE5); // front_blender_tex  base_offset: 2
    #ifdef ARCH_DARWIN
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + prevId]);
    #else
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, pViewer->blendDualPeelTexID_[2 + prevId]);
    #endif

    ACG::GLState::activeTexture(GL_TEXTURE4); // depth_tex  base_offset: 0
    #ifdef ARCH_DARWIN
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[0 + prevId]);
    #else
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, pViewer->blendDualPeelTexID_[0 + prevId]);
    #endif


    // scene geometry peeling pass
    ACG::GLState::activeTexture(GL_TEXTURE0);
    pViewer->peelProg_->setUniform("DiffuseTex", 0);

    ACG::GLState::lockProgram();
    ACG::GLState::shadeModel(GL_SMOOTH); // flat shading is emulated in Geometry Shader, which only works with interpolated vertex shader output
    ACG::GLState::lockShadeModel();
    drawScenePass(_glState, _properties, sceneGraphRoot);
    ACG::GLState::unlockShadeModel();
    ACG::GLState::unlockProgram();


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
    #ifdef ARCH_DARWIN
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[4 + currId]);
    #else
      ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, pViewer->blendDualPeelTexID_[4 + currId]);
    #endif

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
  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
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
  #ifdef ARCH_DARWIN
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[6]);
  #else
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, pViewer->blendDualPeelTexID_[6]);
  #endif


  ACG::GLState::activeTexture(GL_TEXTURE4); // front_blender_tex  base_offset: 2
  #ifdef ARCH_DARWIN
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE_EXT, pViewer->blendDualPeelTexID_[2 + currId]);
  #else
    ACG::GLState::bindTexture(GL_TEXTURE_RECTANGLE, pViewer->blendDualPeelTexID_[2 + currId]);
  #endif


  
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

Q_EXPORT_PLUGIN2( depthpeelingplugin , DepthPeelingPlugin );


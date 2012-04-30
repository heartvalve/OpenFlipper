#include "ShaderGenerator.hh"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>

// sprintf_s warning
//#pragma warning(disable : 4996)

// stringlist iterator
#define FOR_EACH_STRING(lst, it) for(StringList::iterator it=lst.begin();it!=lst.end();++it)
#define FOR_EACH_STRING_CONST(lst, it) for(StringList::const_iterator it=lst.begin();it!=lst.end();++it)


#define LIGHTING_CODE_FILE "../SG_LIGHTING.GLSL"



ShaderGenerator::ShaderGenerator()
{
  version_ = "#version 150"; 
}

ShaderGenerator::~ShaderGenerator()
{

}


void ShaderGenerator::initVertexShaderIO(const ShaderGenDesc* _desc)
{
  addInput("vec4 inPosition");

  if (_desc->shadeMode != SG_SHADE_UNLIT)
    addInput("vec3 inNormal");

  if (_desc->textured)
  {
    addInput("vec2 inTexCoord");
    addOutput("vec2 outTexCoord");
  }
  if (_desc->vertexColors)
    addInput("vec4 inColor");


  if (_desc->shadeMode == SG_SHADE_PHONG)
  {
    addOutput("vec3 outNormal");
    addOutput("vec4 outPosVS");
  }


  std::string strColorOut = "";

  if (_desc->shadeMode == SG_SHADE_FLAT)
    strColorOut = "flat out vec4 outColor;";
  else
  {
    if (_desc->shadeMode == SG_SHADE_GOURAUD ||
      _desc->vertexColors)
      strColorOut = "vec4 outColor";
  }

  if (strColorOut.size())
    addOutput(strColorOut.c_str());
}



void ShaderGenerator::initFragmentShaderIO(const ShaderGenDesc* _desc)
{
  if (_desc->textured)
    addInput("vec2 outTexCoord");


  std::string strColorOut = "";

  if (_desc->shadeMode == SG_SHADE_FLAT)
    strColorOut = "flat in vec4 outColor;";
  else
  {
    if (_desc->shadeMode == SG_SHADE_GOURAUD ||
      _desc->vertexColors)
      strColorOut = "vec4 outColor";
  }

  if (strColorOut.size())
    addInput(strColorOut.c_str());


  if (_desc->shadeMode == SG_SHADE_PHONG)
  {
    addInput("vec3 outNormal");
    addInput("vec4 outPosVS");
  }

  addOutput("vec4 outFragment");
}




void ShaderGenerator::initDefaultUniforms()
{
  addUniform("mat4 g_mWVP");
  addUniform("mat4 g_mWV");
  addUniform("mat3 g_mWVIT");
  addUniform("mat4 g_mP");

  addUniform("vec3 g_vCamPos");
  addUniform("vec3 g_vCamDir");

  addUniform("vec3 g_cDiffuse");
  addUniform("vec3 g_cAmbient");
  addUniform("vec3 g_cEmissive");
  addUniform("vec3 g_cSpecular");
  addUniform("vec4 g_vMaterial");
}


#define ADDLIGHT(x) (sprintf(sz, x"_%d", lightIndex_), addUniform(sz))

void ShaderGenerator::addLight(int lightIndex_, ShaderGenLightType _light)
{
  char sz[0x100];
// 
//   sprintf(sz, "g_vLightPos""_%d", lightIndex_);
//   addUniform("uniform vec3");


  ADDLIGHT("vec3 g_cLightDiffuse");
  ADDLIGHT("vec3 g_cLightAmbient");
  ADDLIGHT("vec3 g_cLightSpecular");

  if (_light == SG_LIGHT_POINT ||
    _light == SG_LIGHT_SPOT)
  {
    ADDLIGHT("vec3 g_vLightPos");
    ADDLIGHT("vec3 g_vLightAtten");
  }

  if (_light == SG_LIGHT_DIRECTIONAL ||
    _light == SG_LIGHT_SPOT)
    ADDLIGHT("vec3 g_vLightDir");


  if (_light == SG_LIGHT_SPOT)
    ADDLIGHT("vec2 g_vLightAngleExp");
}






void ShaderGenerator::addStringToList(const char* _str, 
                                       StringList* _arr,
                                       const char* _prefix,
                                       const char* _postfix)
{
  std::string tmp;


  if (_prefix)
  {
    // set prefix, if wanted
    if (!strstr(_str, _prefix))
      tmp += _prefix;

    tmp += _str;
  }

  if (_postfix)
  {
    if (!strstr(_str, _postfix))
      tmp += _postfix;
  }

  // normalize string
  //  remove tabs, double whitespace

  std::replace(tmp.begin(), tmp.end(), '\t', ' ');
  
  int cpos = tmp.find("  ");
  while (cpos != (int)std::string::npos)
  {
    tmp.erase(cpos);
    cpos = tmp.find("  ");
  }

  // remove whitespace at first or last pos
  if (tmp.length() && tmp[0] == ' ')
    tmp.erase(tmp.begin());

  if (tmp.length() && tmp[tmp.length()-1] == ' ')
    tmp.erase(tmp.end()-1);


  // avoid duplicates
  FOR_EACH_STRING((*_arr), it)
  {
    if (!strcmp(tmp.c_str(), it->c_str()))
      return;
  }

  _arr->push_back(tmp);
}


void ShaderGenerator::addInput(const char* _input)
{
  addStringToList(_input, &inputs_, "in ", ";");
}



void ShaderGenerator::addOutput(const char* _output)
{
  addStringToList(_output, &outputs_, "out ", ";");
}


void ShaderGenerator::addDefine(const char* _def)
{
  addStringToList(_def, &genDefines_, "#define ");
}




void ShaderGenerator::addUniform(const char* _uniform)
{
  addStringToList(_uniform, &uniforms_, "uniform ", ";");
}



void ShaderGenerator::addIOToCode(const StringList& _cmds)
{
  FOR_EACH_STRING_CONST(_cmds, it)
  {
    code_.push_back(*it);
    // append ; eventually

    if (!strstr(it->c_str(), ";"))
      code_.back().append(";");
  }
}



void ShaderGenerator::buildShaderCode(StringList* _pMainCode)
{
  code_.push_back(version_);

  // provide defines
  FOR_EACH_STRING(genDefines_, it)
    code_.push_back(*it);

  // provide imports
  FOR_EACH_STRING(imports_, it)
    code_.push_back(*it);

  // io

  addIOToCode(inputs_);
  addIOToCode(outputs_);
  addIOToCode(uniforms_);

  // main function

  FOR_EACH_STRING((*_pMainCode), it)
    code_.push_back(*it);


}



void ShaderGenerator::addIncludeFile(const char *_fileName)
{
  FILE* pFile = fopen(_fileName, "rt");


  char tmpLineBuf[0x400];

  while (!feof(pFile))
  {
    fgets(tmpLineBuf, 0x400, pFile);

    // remove newline char
    for (int i = 0; tmpLineBuf[i]; ++i)
    {
      if (tmpLineBuf[i] == '\n')
      {
        tmpLineBuf[i] = 0;
        break;
      }
    }

    imports_.push_back(tmpLineBuf);
  }

  fclose(pFile);
}



void ShaderGenerator::saveToFile(const char* _fileName)
{
  FILE* file = fopen(_fileName, "wt");

  if (!file) return;

  FOR_EACH_STRING(code_, it)
    fprintf(file, "%s\n", it->c_str());

  fclose(file);
}



const StringList& ShaderGenerator::getShaderCode()
{
  return code_;
}











StringList ShaderProgGenerator::lightingCode_;


ShaderProgGenerator::ShaderProgGenerator(ShaderGenDesc* _desc)
: vertex_(0), fragment_(0)
{
  memcpy(&desc_, _desc, sizeof(ShaderGenDesc));

  loadLightingFunctions();
}

ShaderProgGenerator::~ShaderProgGenerator(void)
{
  delete vertex_;
  delete fragment_;
}



void ShaderProgGenerator::loadStringListFromFile(const char* _fileName, StringList* _out)
{
  FILE* pFile = fopen(_fileName, "rt");

  if (!pFile)
    std::cout << "error: file missing -> \"" << _fileName << "\"" << std::endl;

  else
  {
    char szLine[0x200];

    while (!feof(pFile))
    {
      fgets(szLine, 0x200, pFile);

      // remove '\n'
      int len = strlen(szLine);

      char* nl = strstr(szLine, "\n");
      if (nl)
        *nl = 0;

      _out->push_back(szLine);
    }

    fclose(pFile);
  }
}


void ShaderProgGenerator::loadLightingFunctions()
{
  if (lightingCode_.size()) return;

  // load shader code from file
  loadStringListFromFile(LIGHTING_CODE_FILE, &lightingCode_);
}



void ShaderProgGenerator::initGenDefines(ShaderGenerator* _gen)
{
  switch (desc_.shadeMode)
  {
  case SG_SHADE_GOURAUD:
    _gen->addDefine("SG_GOURAUD 1"); break;
  case SG_SHADE_FLAT:
    _gen->addDefine("SG_FLAT 1"); break;
  case SG_SHADE_UNLIT:
    _gen->addDefine("SG_UNLIT 1"); break;
  case SG_SHADE_PHONG:
    _gen->addDefine("SG_PHONG 1"); break;

  default:
    std::cout << __FUNCTION__ << " -> unknown shade mode: " << desc_.shadeMode << std::endl;
  }

  if (desc_.textured)
    _gen->addDefine("SG_TEXTURED 1");

  if (desc_.vertexColors)
    _gen->addDefine("SG_VERTEX_COLOR 1");

  // # lights define
  char szNumLights[0x100];
  sprintf(szNumLights, "SG_NUM_LIGHTS %d", desc_.numLights);
  _gen->addDefine(szNumLights);

  // light types define
  const char* lightTypeNames[] = {"SG_LIGHT_DIRECTIONAL",
    "SG_LIGHT_POINT", "SG_LIGHT_SPOT"};

  for (int i = 0; i < 3; ++i)
    _gen->addDefine(lightTypeNames[i]);


  for (int i = 0; i < desc_.numLights; ++i)
  {
    char szLighType[0x100];
    sprintf(szLighType, "SG_LIGHT_TYPE_%d %s", i, lightTypeNames[desc_.lightTypes[i]]);
    _gen->addDefine(szLighType);
  }
}




void ShaderProgGenerator::buildVertexShader()
{
  delete vertex_;

  vertex_  = new ShaderGenerator();


//  vertex_->initDefaultVertexShaderIO();
  vertex_->initVertexShaderIO(&desc_);
  
  vertex_->initDefaultUniforms();


  initGenDefines(vertex_);



  // io

  // when to use vertex lights
  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT)
  {
    for (int i = 0; i < desc_.numLights; ++i)
      vertex_->addLight(i, desc_.lightTypes[i]);
  }




  // assemble main function
  StringList mainCode;

  addLightingFunctions(&mainCode);

  if (!vertexTemplate_.size())
  {
    mainCode.push_back("void main()");
    mainCode.push_back("{");

    addVertexBeginCode(&mainCode);
    addVertexEndCode(&mainCode);

    mainCode.push_back("}");
  }
  else
  {
    // interpret loaded shader template:
    //  import #includes and replace SG_VERTEX_BEGIN/END markers

    FOR_EACH_STRING(vertexTemplate_, it)
    {
      if (!checkForIncludes(it->c_str(), vertex_, getPathName(vertexShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        const char* szMarker = strstr(it->c_str(), "SG_VERTEX_BEGIN");

        if (szMarker)
          addVertexBeginCode(&mainCode);
        else
        {
          szMarker = strstr(it->c_str(), "SG_VERTEX_END");

          if (szMarker)
            addVertexEndCode(&mainCode);
          else
          {
            // no SG marker
            mainCode.push_back(*it);
          }
        }

      }
    }

  }



  vertex_->buildShaderCode(&mainCode);

}


void ShaderProgGenerator::addVertexBeginCode(StringList* _code)
{
  _code->push_back("vec4 sg_vPosPS = g_mWVP * inPosition;");
  _code->push_back("vec4 sg_vPosVS = g_mWV * inPosition;");
  _code->push_back("vec3 sg_vNormalVS = vec3(0.0, 1.0, 0.0);");
  _code->push_back("vec2 sg_vTexCoord = vec2(0.0, 0.0);");
  _code->push_back("vec4 sg_cColor = vec4(1.0, 1.0, 1.0, 1.0);");

  if (desc_.shadeMode != SG_SHADE_UNLIT)
    _code->push_back("sg_vNormalVS = g_mWVIT * inNormal;");

  if (desc_.textured)
    _code->push_back("sg_vTexCoord = inTexCoord;");

  if (desc_.vertexColors)
    _code->push_back("sg_cColor = inColor;");

  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT)
  {
    // add lighting code here

    addLightingCode(_code);
  }


}


void ShaderProgGenerator::addVertexEndCode(StringList* _code)
{
  _code->push_back("gl_Position = sg_vPosPS;");

  if (desc_.textured)
    _code->push_back("outTexCoord = sg_vTexCoord;");

  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT ||
    desc_.vertexColors)
    _code->push_back("outColor = sg_cColor;");

  if (desc_.shadeMode == SG_SHADE_PHONG)
  {
    _code->push_back("outNormal = sg_vNormalVS;");
    _code->push_back("outPosVS = sg_vPosVS;");
  }
}


int ShaderProgGenerator::checkForIncludes(const char* _str, ShaderGenerator* _gen, std::string _includePath)
{
  const char* szInclude = strstr(_str, "#include");
  if (szInclude)
  {
    const char* szIncludeFile = strstr(szInclude, "\"");

    if (!szIncludeFile)
      std::cout << "wrong include syntax: " << _str << std::endl;
    else
    {
      ++szIncludeFile; // skip first "
      char buf[0x100];

      // read filename between quotation marks
      for (int i = 0; i < (int)strlen(szIncludeFile); ++i)
      {
        buf[i] = szIncludeFile[i];
        if (buf[i] == '\"')
        {
          buf[i] = 0;
          break;
        }
      }


      std::string fullPathToIncludeFile = _includePath;
      fullPathToIncludeFile += "/";
      fullPathToIncludeFile += buf;

      _gen->addIncludeFile(fullPathToIncludeFile.c_str());
    }

    return 1;
  }

  return 0;
}

void ShaderProgGenerator::buildFragmentShader()
{
  delete fragment_;

  fragment_  = new ShaderGenerator();


  fragment_->initFragmentShaderIO(&desc_);

  fragment_->initDefaultUniforms();


  // texture sampler id
  if (desc_.textured)
    fragment_->addUniform("sampler2D g_Texture0");


  initGenDefines(fragment_);



  // io

  // when to use fragment lights
  if (desc_.shadeMode == SG_SHADE_PHONG)
  {
    for (int i = 0; i < desc_.numLights; ++i)
      fragment_->addLight(i, desc_.lightTypes[i]);
  }




  // assemble main function
  StringList mainCode;

  addLightingFunctions(&mainCode);


  if (!fragmentTemplate_.size())
  {
    mainCode.push_back("void main()");
    mainCode.push_back("{");

    addFragmentBeginCode(&mainCode);
    addFragmentEndCode(&mainCode);

    mainCode.push_back("}");
  }
  else
  {
    // interpret loaded shader template:
    //  import #includes and replace SG_VERTEX_BEGIN/END markers

    FOR_EACH_STRING(fragmentTemplate_, it)
    {
      if (!checkForIncludes(it->c_str(), fragment_, getPathName(fragmentShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        const char* szMarker = strstr(it->c_str(), "SG_FRAGMENT_BEGIN");

        if (szMarker)
          addFragmentBeginCode(&mainCode);
        else
        {
          szMarker = strstr(it->c_str(), "SG_FRAGMENT_END");

          if (szMarker)
            addFragmentEndCode(&mainCode);
          else
          {
            // no SG marker
            mainCode.push_back(*it);
          }
        }

      }
    }

  }



  fragment_->buildShaderCode(&mainCode);
}


void ShaderProgGenerator::addFragmentBeginCode(StringList* _code)
{
  _code->push_back("vec4 sg_cColor = vec4(0.0, 0.0, 0.0, 0.0);");

  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT ||
    desc_.vertexColors)
    _code->push_back("sg_cColor = outColor;");


  if (desc_.shadeMode == SG_SHADE_PHONG)
  {
    _code->push_back("vec4 sg_vPosVS = outPosVS;");
    _code->push_back("vec3 sg_vNormalVS = outNormal;");

    addLightingCode(_code);
  }

  if (desc_.textured)
  {
    _code->push_back("vec4 sg_cTex = texture(g_Texture0, outTexCoord);");
    _code->push_back("sg_cColor *= sg_cTex;");
  }
}

void ShaderProgGenerator::addFragmentEndCode(StringList* _code)
{
  _code->push_back("outFragment = sg_cColor;");
}



void ShaderProgGenerator::addLightingCode(StringList* _code)
{
  char buf[0x100];

  for (int i = 0; i < desc_.numLights; ++i)
  {
    ShaderGenLightType lgt = desc_.lightTypes[i];


    switch (lgt)
    {
    case SG_LIGHT_DIRECTIONAL:
      sprintf(buf, "sg_cColor.xyz += LitDirLight(sg_vNormalVS, g_vLightDir_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d);", i, i, i, i);
      break;

    case SG_LIGHT_POINT:
      sprintf(buf, "sg_cColor.xyz += LitPointLight(sg_vPosVS.xyz, sg_vNormalVS,  g_vLightPos_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d,  g_vLightAtten_%d);", i, i, i, i, i);
      break;

    case SG_LIGHT_SPOT:
      sprintf(buf, "sg_cColor.xyz += LitSpotLight(sg_vPosVS.xyz,  sg_vNormalVS,  g_vLightPos_%d,  g_vLightDir_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d,  g_vLightAtten_%d,  g_vLightAngleExp_%d);", i, i, i, i, i, i, i);
      break;

    default: break;
    }

    _code->push_back(buf);
  }

}




void ShaderProgGenerator::addLightingFunctions(StringList* _code)
{
  FOR_EACH_STRING(lightingCode_, it)
    _code->push_back(*it);
}



void ShaderProgGenerator::generateShaders()
{
  buildVertexShader();
  buildFragmentShader();
}




void ShaderProgGenerator::saveVertexShToFile(const char* _fileName)
{
  vertex_->saveToFile(_fileName);
}

void ShaderProgGenerator::saveFragmentShToFile(const char* _fileName)
{
  fragment_->saveToFile(_fileName);
}


void ShaderProgGenerator::loadShaderTemplateFromFile(const char *_vertexShaderFile, 
                                                     const char *_fragmentShaderFile)
{
  loadStringListFromFile(_vertexShaderFile, &vertexTemplate_);
  loadStringListFromFile(_fragmentShaderFile, &fragmentTemplate_);

  vertexShaderFile_ = _vertexShaderFile;
  fragmentShaderFile_ = _fragmentShaderFile;
}

std::string ShaderProgGenerator::getPathName(std::string _strFileName)
{
  std::string strRet = _strFileName;

  // find last slash
  int lpos = (int)_strFileName.rfind('/');

  if (lpos == (int)std::string::npos)
    lpos = _strFileName.rfind('\\');

  // erase last sequence, including slash
  if (lpos != (int)std::string::npos)
    strRet.erase(strRet.begin() + lpos, strRet.end());

  return strRet;
}

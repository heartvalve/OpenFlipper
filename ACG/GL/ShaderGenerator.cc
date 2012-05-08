#include "ShaderGenerator.hh"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>

#include <QFile>
#include <QTextStream>

// sprintf_s warning
//#pragma warning(disable : 4996)

// stringlist iterator
#define FOR_EACH_STRING(lst, it) for(QStringList::iterator it=lst.begin();it!=lst.end();++it)
#define FOR_EACH_STRING_CONST(lst, it) for(QStringList::const_iterator it=lst.begin();it!=lst.end();++it)


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


#define ADDLIGHT(x) (sz.sprintf(x"_%d", lightIndex_), addUniform(sz))

void ShaderGenerator::addLight(int lightIndex_, ShaderGenLightType _light)
{
  QString sz;
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






void ShaderGenerator::addStringToList(QString _str, 
                                       QStringList* _arr,
                                       const char* _prefix,
                                       const char* _postfix)
{
  QString tmp;


  if (_prefix)
  {
    // set prefix, if wanted
    if (!_str.contains(_prefix))
      tmp += _prefix;

    tmp += _str;
  }

  if (_postfix)
  {
    if (!_str.contains(_postfix))
      tmp += _postfix;
  }

  // normalize string
  //  remove tabs, double whitespace

  tmp = tmp.simplified();



  // avoid duplicates
  if (!_arr->contains(tmp))
    _arr->push_back(tmp);

}


void ShaderGenerator::addInput(QString _input)
{
  addStringToList(_input, &inputs_, "in ", ";");
}



void ShaderGenerator::addOutput(QString _output)
{
  addStringToList(_output, &outputs_, "out ", ";");
}


void ShaderGenerator::addDefine(QString _def)
{
  addStringToList(_def, &genDefines_, "#define ");
}




void ShaderGenerator::addUniform(QString _uniform)
{
  addStringToList(_uniform, &uniforms_, "uniform ", ";");
}



void ShaderGenerator::addIOToCode(const QStringList& _cmds)
{
  FOR_EACH_STRING_CONST(_cmds, it)
  {
    code_.push_back(*it);
    // append ; eventually

    if (!it->contains(";"))
      code_.back().append(";");
  }
}



void ShaderGenerator::buildShaderCode(QStringList* _pMainCode)
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



void ShaderGenerator::addIncludeFile(QString _fileName)
{
  QFile file(_fileName);

  QTextStream fileStream(&file);

  while (!fileStream.atEnd())
  {
    QString tmpLine = fileStream.readLine();

    imports_.push_back(tmpLine.simplified());
  }

}



void ShaderGenerator::saveToFile(const char* _fileName)
{
  QFile file(_fileName);
  QTextStream fileStream(&file);


  FOR_EACH_STRING(code_, it)
    fileStream << *it << '\n';
}



const QStringList& ShaderGenerator::getShaderCode()
{
  return code_;
}











QStringList ShaderProgGenerator::lightingCode_;


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



void ShaderProgGenerator::loadStringListFromFile(const char* _fileName, QStringList* _out)
{
  QFile file(_fileName);

  file.open(QIODevice::ReadOnly | QIODevice::Text);

  if (!file.isReadable())
    std::cout << "error: file missing -> \"" << _fileName << "\"" << std::endl;

  else
  {
    QTextStream filestream(&file);

    while (!filestream.atEnd())
    {
      QString szLine = filestream.readLine();
      _out->push_back(szLine.trimmed());
    }
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
  QString strNumLights;
  strNumLights.sprintf("SG_NUM_LIGHTS %d", desc_.numLights);
  _gen->addDefine(strNumLights);

  // light types define
  const char* lightTypeNames[] = {"SG_LIGHT_DIRECTIONAL",
    "SG_LIGHT_POINT", "SG_LIGHT_SPOT"};

  for (int i = 0; i < 3; ++i)
    _gen->addDefine(lightTypeNames[i]);


  for (int i = 0; i < desc_.numLights; ++i)
  {
    QString strLightType;
    strLightType.sprintf("SG_LIGHT_TYPE_%d %s", i, lightTypeNames[desc_.lightTypes[i]]);
    _gen->addDefine(strLightType);
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
  QStringList mainCode;

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
      if (!checkForIncludes(*it, vertex_, getPathName(vertexShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        if (it->contains("SG_VERTEX_BEGIN"))
          addVertexBeginCode(&mainCode);
        else
        {
          if (it->contains("SG_VERTEX_END"))
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


void ShaderProgGenerator::addVertexBeginCode(QStringList* _code)
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


void ShaderProgGenerator::addVertexEndCode(QStringList* _code)
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


int ShaderProgGenerator::checkForIncludes(QString _str, ShaderGenerator* _gen, QString _includePath)
{
  if (_str.contains("#include "))
  {
    QString strIncludeFile = _str.remove("#include ").remove('\"').remove('<').remove('>').trimmed();

    if (strIncludeFile.isEmpty())
      std::cout << "wrong include syntax: " << (const char*)_str.toAscii() << std::endl;
    else
    {
      QString fullPathToIncludeFile = _includePath;
      fullPathToIncludeFile += "/";
      fullPathToIncludeFile += strIncludeFile;

      _gen->addIncludeFile(fullPathToIncludeFile);
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
  QStringList mainCode;

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
      if (!checkForIncludes(*it, fragment_, getPathName(fragmentShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        if (it->contains("SG_FRAGMENT_BEGIN"))
          addFragmentBeginCode(&mainCode);
        else
        {
          if (it->contains("SG_FRAGMENT_END"))
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


void ShaderProgGenerator::addFragmentBeginCode(QStringList* _code)
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

void ShaderProgGenerator::addFragmentEndCode(QStringList* _code)
{
  _code->push_back("outFragment = sg_cColor;");
}



void ShaderProgGenerator::addLightingCode(QStringList* _code)
{
  QString buf;

  for (int i = 0; i < desc_.numLights; ++i)
  {
    ShaderGenLightType lgt = desc_.lightTypes[i];


    switch (lgt)
    {
    case SG_LIGHT_DIRECTIONAL:
      buf.sprintf("sg_cColor.xyz += LitDirLight(sg_vNormalVS, g_vLightDir_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d);", i, i, i, i);
      break;

    case SG_LIGHT_POINT:
      buf.sprintf("sg_cColor.xyz += LitPointLight(sg_vPosVS.xyz, sg_vNormalVS,  g_vLightPos_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d,  g_vLightAtten_%d);", i, i, i, i, i);
      break;

    case SG_LIGHT_SPOT:
      buf.sprintf("sg_cColor.xyz += LitSpotLight(sg_vPosVS.xyz,  sg_vNormalVS,  g_vLightPos_%d,  g_vLightDir_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d,  g_vLightAtten_%d,  g_vLightAngleExp_%d);", i, i, i, i, i, i, i);
      break;

    default: break;
    }

    _code->push_back(buf);
  }

}




void ShaderProgGenerator::addLightingFunctions(QStringList* _code)
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

QString ShaderProgGenerator::getPathName(QString _strFileName)
{
  QString ret = _strFileName;

  // find last slash
  int lpos = _strFileName.lastIndexOf('/');

  if (lpos < 0)
    lpos = _strFileName.lastIndexOf('\\');

  // erase last sequence, including slash
  if (lpos != (int)std::string::npos)
    ret.remove(lpos, ret.length());

  return ret;
}

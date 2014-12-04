/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "ShaderGenerator.hh"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QHash>

namespace ACG
{

#define LIGHTING_CODE_FILE "ShaderGen/SG_LIGHTING.GLSL"



// space naming
// OS : object space
// VS : view space
// CS : clip space

// attribute request keywords
#define SG_REQUEST_POSVS "#define SG_REQUEST_POSVS"
#define SG_REQUEST_POSOS "#define SG_REQUEST_POSOS"
#define SG_REQUEST_TEXCOORD "#define SG_REQUEST_TEXCOORD"
#define SG_REQUEST_VERTEXCOLOR "#define SG_REQUEST_VERTEXCOLOR"
#define SG_REQUEST_NORMALVS "#define SG_REQUEST_NORMALVS"
#define SG_REQUEST_NORMALOS "#define SG_REQUEST_NORMALOS"

// renormalize normal-vec before lighting in fragment shader
#define SG_REQUEST_RENOMARLIZE "#define SG_REQUEST_RENORMARLIZE"

// generic default attribute input keywords
//  these are extended by the correct input name by the generator for each stage
#define SG_INPUT_POSVS "SG_INPUT_POSVS"
#define SG_INPUT_POSOS "SG_INPUT_POSOS"
#define SG_INPUT_POSCS "SG_INPUT_POSCS"
#define SG_INPUT_NORMALVS "SG_INPUT_NORMALVS"
#define SG_INPUT_NORMALOS "SG_INPUT_NORMALOS"
#define SG_INPUT_TEXCOORD "SG_INPUT_TEXCOORD"
#define SG_INPUT_VERTEXCOLOR "SG_INPUT_VERTEXCOLOR"

#define SG_OUTPUT_POSVS "SG_OUTPUT_POSVS"
#define SG_OUTPUT_POSOS "SG_OUTPUT_POSOS"
#define SG_OUTPUT_POSCS "SG_OUTPUT_POSCS"
#define SG_OUTPUT_NORMALVS "SG_OUTPUT_NORMALVS"
#define SG_OUTPUT_NORMALOS "SG_OUTPUT_NORMALOS"
#define SG_OUTPUT_TEXCOORD "SG_OUTPUT_TEXCOORD"
#define SG_OUTPUT_VERTEXCOLOR "SG_OUTPUT_VERTEXCOLOR"


int ShaderProgGenerator::numRegisteredModifiers_ = 0;
std::vector<ShaderModifier*> ShaderProgGenerator::registeredModifiers_;



ShaderGenerator::ShaderGenerator()
  : version_(150), inputArrays_(false), outputArrays_(false)
{
}

ShaderGenerator::~ShaderGenerator()
{

}


void ShaderGenerator::initVertexShaderIO(const ShaderGenDesc* _desc, const DefaultIODesc* _iodesc)
{
  // set type of IO
  inputArrays_ = false;
  outputArrays_ = false;
  inputPrefix_ = "in";         // inputs: inPosition, inTexCoord...
  outputPrefix_ = "outVertex"; // outputs: outVertexPosition, outVertexTexCoord..

  addInput("vec4 inPosition");
  addOutput("vec4 outVertexPosCS");


  if (_iodesc->inputNormal_)
    addInput("vec3 inNormal");

  if (_desc->textured())
  {

    std::map<size_t,ShaderGenDesc::TextureType>::const_iterator iter = _desc->textureTypes().begin();

    /// TODO Setup for multiple texture coordinates as input
    if (iter->second.type == GL_TEXTURE_3D) {
      addInput("vec3 inTexCoord");
      addOutput("vec3 outVertexTexCoord");
    } else {
      addInput("vec2 inTexCoord");
      addOutput("vec2 outVertexTexCoord");
    }
  }


  if (_iodesc->inputColor_)
    addInput("vec4 inColor");


  if (_iodesc->passNormalVS_)
    addOutput("vec3 outVertexNormal");

  if (_iodesc->passNormalOS_)
    addOutput("vec3 outVertexNormalOS");



  std::string strColorOut = "";


  if (_desc->shadeMode == SG_SHADE_FLAT)
    if (!_desc->geometryTemplateFile.isEmpty())
      strColorOut = "vec4 outVertexColor";
    else {
      // Bypass the output setter, as we have to set that directly with the flat.
      addStringToList("vec4 outVertexColor", &outputs_, "flat out ", ";");
    }
  else {
    if (_desc->shadeMode == SG_SHADE_GOURAUD || _desc->vertexColors || _iodesc->inputColor_)
      strColorOut = "vec4 outVertexColor";
  }

  if (strColorOut.size())
    addOutput(strColorOut.c_str());


  // handle other requests: normals, positions, texcoords

  if (_iodesc->passPosVS_)
    addOutput("vec4 outVertexPosVS");

  if (_iodesc->passPosOS_)
    addOutput("vec4 outVertexPosOS");

  if (_iodesc->passTexCoord_ && !_desc->textured())
  {
    // assume 2d texcoords
    addInput("vec2 inTexCoord");
    addOutput("vec2 outVertexTexCoord");
  }


  defineIOAbstraction(_iodesc, true, false);
}

void ShaderGenerator::initTessControlShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc) 
{
  // set type of IO
  inputArrays_ = true;
  outputArrays_ = true;
  inputPrefix_ = _prevStage->outputPrefix_;
  outputPrefix_ = "outTc"; // outputs: outTcPosition, outTcTexCoord..

  matchInputs(_prevStage, true, inputPrefix_, outputPrefix_);

  defineIOAbstraction(_iodesc, false, false);
}

void ShaderGenerator::initTessEvalShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc) 
{
  // set type of IO
  inputArrays_ = true;
  outputArrays_ = false;
  inputPrefix_ = _prevStage->outputPrefix_;
  outputPrefix_ = "outTe"; // outputs: outTePosition, outTeTexCoord..

  matchInputs(_prevStage, true, inputPrefix_, outputPrefix_);

  defineIOAbstraction(_iodesc, false, false);
}

void ShaderGenerator::initGeometryShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc) 
{
  // set type of IO
  inputArrays_ = true;
  outputArrays_ = false;
  inputPrefix_ = _prevStage->outputPrefix_;
  outputPrefix_ = "outGeometry"; // outputs: outGeometryPosition, outGeometryTexCoord..

  matchInputs(_prevStage, true, inputPrefix_, outputPrefix_);

  return;

  addInput("vec4 outVertexPosCS[]");
  addOutput("vec4 outGeometryPosCS");

  if (_desc->textured()) {

    std::map<size_t,ShaderGenDesc::TextureType>::const_iterator iter = _desc->textureTypes().begin();

    /// TODO Setup for multiple texture coordinates as input
    if (iter->second.type == GL_TEXTURE_3D)
    {
      addInput("vec3 outVertexTexCoord[]");
      addOutput("vec3 outGeometryTexCoord");
    } else {
      addInput("vec2 outVertexTexCoord[]");
      addOutput("vec2 outGeometryTexCoord");
    }

  }


  if (_desc->shadeMode == SG_SHADE_PHONG)
  {
    addInput("vec3 outVertexNormal[]");
    addInput("vec4 outVertexPosVS[]");

    addOutput("vec3 outGeometryNormal");
    addOutput("vec4 outGeometryPosVS");
  }

  QString strColorOut = "";

  if (_desc->shadeMode == SG_SHADE_FLAT || _desc->shadeMode == SG_SHADE_GOURAUD || _desc->vertexColors) {
    addInput("vec4 outVertexColor[]");


    if (_desc->shadeMode == SG_SHADE_FLAT)
      addStringToList("vec4 outGeometryColor", &outputs_, "flat out ", ";");
    else {
      if (_desc->shadeMode == SG_SHADE_GOURAUD || _desc->vertexColors)
        strColorOut = "vec4 outGeometryColor";
    }

    if ( !strColorOut.isEmpty() )
      addOutput(strColorOut);
  }

  defineIOAbstraction(_iodesc, false, false);
}



void ShaderGenerator::initFragmentShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc)
{
  // set type of IO
  inputArrays_ = false;
  outputArrays_ = false;
  inputPrefix_ = _prevStage->outputPrefix_;
  outputPrefix_ = "outFragment";

  matchInputs(_prevStage, false);
  addOutput("vec4 outFragment");

  defineIOAbstraction(_iodesc, false, true);
}


void ShaderGenerator::defineIOAbstraction( const DefaultIODesc* _iodesc, bool _vs, bool _fs )
{
  if (_vs)
  {
    // input name abstraction

    addDefine(SG_INPUT_POSOS " inPosition");

    if (_iodesc->inputTexCoord_)
      addDefine(SG_INPUT_TEXCOORD " inTexCoord");

    if (_iodesc->inputNormal_)
      addDefine(SG_INPUT_NORMALOS " inNormal");

    if (_iodesc->inputColor_)
      addDefine(SG_INPUT_VERTEXCOLOR "inColor");



    // output name abstraction

    addDefine(SG_OUTPUT_POSCS " outVertexPosCS");

    if (_iodesc->passPosVS_)
      addDefine(SG_OUTPUT_POSVS " outVertexPosVS");

    if (_iodesc->passPosOS_)
      addDefine(SG_OUTPUT_POSOS " outVertexPosOS");

    if (_iodesc->passTexCoord_)
      addDefine(SG_OUTPUT_TEXCOORD " outVertexTexCoord");

    if (_iodesc->passNormalVS_)
      addDefine(SG_OUTPUT_NORMALVS " outVertexNormal");

    if (_iodesc->passNormalOS_)
      addDefine(SG_OUTPUT_NORMALOS " outVertexNormalOS");

    if (_iodesc->passColor_)
      addDefine(SG_OUTPUT_VERTEXCOLOR " outVertexColor");
  }
  else
  {
    if (_iodesc->passPosVS_)
    {
      addDefine(QString(SG_INPUT_POSVS) + QString(" ") + inputPrefix_ + QString("PosVS"));
      if (!_fs)
        addDefine(QString(SG_OUTPUT_POSVS) + QString(" ") + inputPrefix_ + QString("PosVS"));
    }

    if (_iodesc->passPosOS_)
    {
      addDefine(QString(SG_INPUT_POSOS) + QString(" ") + inputPrefix_ + QString("PosOS"));
      if (!_fs)
        addDefine(QString(SG_OUTPUT_POSOS) + QString(" ") + inputPrefix_ + QString("PosOS"));
    }

    addDefine(QString(SG_INPUT_POSCS) + QString(" ") + inputPrefix_ + QString("PosCS"));
    if (!_fs)
      addDefine(QString(SG_OUTPUT_POSCS) + QString(" ") + inputPrefix_ + QString("PosCS"));

    if (_iodesc->passNormalVS_)
    {
      addDefine(QString(SG_INPUT_NORMALVS) + QString(" ") + inputPrefix_ + QString("Normal"));
      if (!_fs)
        addDefine(QString(SG_OUTPUT_NORMALVS) + QString(" ") + outputPrefix_ + QString("Normal"));
    }

    if (_iodesc->passNormalOS_)
    {
      addDefine(QString(SG_INPUT_NORMALOS) + QString(" ") + inputPrefix_ + QString("NormalOS"));
      if (!_fs)
        addDefine(QString(SG_OUTPUT_NORMALOS) + QString(" ") + outputPrefix_ + QString("NormalOS"));
    }

    if (_iodesc->passTexCoord_)
    {
      addDefine(QString(SG_INPUT_TEXCOORD) + QString(" ") + inputPrefix_ + QString("TexCoord"));
      if (!_fs)
        addDefine(QString(SG_OUTPUT_TEXCOORD) + QString(" ") + outputPrefix_ + QString("TexCoord"));
    }

    if (_iodesc->passColor_)
    {
      addDefine(QString(SG_INPUT_VERTEXCOLOR) + QString(" ") + inputPrefix_ + QString("Color"));
      if (!_fs)
        addDefine(QString(SG_INPUT_VERTEXCOLOR) + QString(" ") + outputPrefix_ + QString("Color"));
    }
  }

  
}



void ShaderGenerator::initDefaultUniforms()
{
  addUniform("mat4 g_mWVP" , "  // Projection * Modelview");       // Transforms directly from Object space to NDC
  addUniform("mat4 g_mWV" , "   // Modelview matrix");             // Modelview transforms from Object to World to View coordinates
  addUniform("mat3 g_mWVIT" , " // Modelview inverse transposed"); // Modelview inverse transposed, transforms from view across World into Object coordinates
  addUniform("mat4 g_mP", "     // Projection matrix");            // Projection Matrix

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
                                      QString _prefix,
                                      QString _postfix)
{
  // Construct the whole string
  QString tmp = _str;

  if (!_str.startsWith(_prefix))
    tmp = _prefix + tmp;

  if (!_str.endsWith(_postfix))
     tmp += _postfix;

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

bool ShaderGenerator::hasDefine(QString _define) const
{
  if (genDefines_.contains(_define))
    return true;

  // check trimmed strings and with startsWith()

  QString trimmedDef = _define.trimmed();

  for (QStringList::const_iterator it = genDefines_.constBegin(); it != genDefines_.constEnd(); ++it)
  {
    QString trimmedRef = it->trimmed();

    if (trimmedRef.startsWith(trimmedDef))
      return true;
  }

  // also check raw io blocks
  for (QStringList::const_iterator it = rawIO_.constBegin(); it != rawIO_.constEnd(); ++it)
  {
    QString trimmedRef = it->trimmed();

    if (trimmedRef.startsWith(trimmedDef))
      return true;
  }

  return false;
}

void ShaderGenerator::addLayout(QString _def)
{
  addStringToList(_def, &layouts_);
}


void ShaderGenerator::addUniform(QString _uniform, QString _comment)
{
  QString prefix = "";
  if (!_uniform.startsWith("uniform ") && !_uniform.contains(" uniform "))
    prefix = "uniform ";

  addStringToList(_uniform, &uniforms_, prefix, "; " + _comment );
}



void ShaderGenerator::addIOToCode(const QStringList& _cmds)
{
  QString it;
  foreach(it, _cmds)
  {
    code_.push_back(it);
    // append ; eventually

    if (!it.contains(";"))
      code_.back().append(";");
  }
}



void ShaderGenerator::buildShaderCode(QStringList* _pMainCode, const QStringList& _defaultLightingFunctions)
{
  QString glslversion;
  glslversion.sprintf("#version %d", version_);

  code_.push_back(glslversion);

  // provide defines
  QString it;

  foreach(it, genDefines_)
    code_.push_back(it);

  // layouts
  foreach(it, layouts_)
    code_.push_back(it);

  // IO
  addIOToCode(inputs_);
  addIOToCode(outputs_);
  addIOToCode(uniforms_);

  // eventually attach lighting functions if required
  bool requiresLightingCode = false;

  // search for references in imports
  foreach(it, imports_)
  {
    if (it.contains("LitDirLight") || it.contains("LitPointLight") || it.contains("LitSpotLight"))
      requiresLightingCode = true;
  }

  if (requiresLightingCode)
  {
    foreach(it, _defaultLightingFunctions)
      code_.push_back(it);
  }

  // provide imports
  foreach(it, imports_)
    code_.push_back(it);


  // search for lighting references in main code

  if (!requiresLightingCode)
  {
    foreach(it, (*_pMainCode))
    {
      if (it.contains("LitDirLight") || it.contains("LitPointLight") || it.contains("LitSpotLight"))
        requiresLightingCode = true;
    }

    if (requiresLightingCode)
    {
      foreach(it, _defaultLightingFunctions)
        code_.push_back(it);
    }
  }


  // add raw IO code block
  code_.append(rawIO_);


  // main function
  foreach(it, (*_pMainCode))
    code_.push_back(it);
}



void ShaderGenerator::addIncludeFile(QString _fileName)
{
  QFile file(_fileName);

  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream fileStream(&file);
    
    // track source of include files in shader comment
    
    imports_.push_back("// ==============================================================================");
    imports_.push_back(QString("// ShaderGenerator - begin of imported file: ") + _fileName);
    

    while (!fileStream.atEnd())
    {
      QString tmpLine = fileStream.readLine();

      imports_.push_back(tmpLine.simplified());
    }
    
    
    // mark end of include file in comment
    
    imports_.push_back(QString("// ShaderGenerator - end of imported file #include \"") + _fileName);
    imports_.push_back("// ==============================================================================");
    
  }

}



void ShaderGenerator::saveToFile(const char* _fileName)
{
  QFile file(_fileName);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream fileStream(&file);

    QString it;
    foreach(it, code_)
      fileStream << it << '\n';
  }
}



const QStringList& ShaderGenerator::getShaderCode()
{
  return code_;
}

void ShaderGenerator::setGLSLVersion( int _version )
{
  version_ = _version;
}

void ShaderGenerator::matchInputs(const ShaderGenerator* _previousShaderStage,
  bool _passToNextStage,
  QString _inputPrefix, 
  QString _outputPrefix)
{
  if (!_previousShaderStage)
  {
    std::cout << "error: ShaderGenerator::matchInputs called without providing input stage" << std::endl;
    return;
  }

  QString it;
  foreach(it, _previousShaderStage->outputs_)
  {
    QString input = it;

    QString outKeyword = "out ";
    QString inKeyword = "in  ";

    // replace first occurrence of "out" with "in"
    input.replace(input.indexOf(outKeyword), outKeyword.size(), inKeyword);

    // special case for array IO

    if (inputArrays_ && !_previousShaderStage->outputArrays_)
    {
      QRegExp alphaNum("[a-zA-Z0-9]");
      int lastNameChar = input.lastIndexOf(alphaNum);
      input.insert(lastNameChar+1, "[]");
//      input.insert(lastNameChar+1, "[gl_in.length()]");
    }


    // add to input list with duplicate check
    addStringToList(input, &inputs_);

    if (_passToNextStage)
    {
      // replace prefixes of in/outputs to avoid name collision

      QString output = input;
      output.replace(output.indexOf(_inputPrefix), _inputPrefix.size(), _outputPrefix);
      output.replace(output.indexOf(inKeyword), inKeyword.size(), outKeyword);

      // take care of arrays
      if (inputArrays_ && !outputArrays_)
      {
        int bracketStart = output.indexOf("[");
        int bracketEnd = output.indexOf("]");
        output.remove(bracketStart, bracketEnd-bracketStart+1);
      }
      else if (!inputArrays_ && outputArrays_)
      {
        QRegExp alphaNum("[a-zA-Z0-9]");
        int lastNameChar = output.lastIndexOf(alphaNum);
        output.insert(lastNameChar+1, "[]");
//        output.insert(lastNameChar+1, "[gl_in.length()]");
      }


      // add to output list with duplicate check
      addStringToList(output, &outputs_);
    }
  }
}

int ShaderGenerator::getNumOutputs() const
{
  return outputs_.size();
}

QString ShaderGenerator::getOutputName(int _id) const
{
  QString output = outputs_.at(_id);

  output.remove(";");
  output.remove("out ");

  int bracketStart = output.indexOf("[");
  int bracketEnd = output.lastIndexOf("]");

  if (bracketStart >= 0)
    output.remove(bracketStart, bracketEnd-bracketStart+1);

  // decompose output declaration
  QStringList decompOutput = output.split(" ");
  return decompOutput.last();
}

int ShaderGenerator::getNumInputs() const
{
  return inputs_.size();
}

QString ShaderGenerator::getInputName(int _id) const
{
  QString input = inputs_.at(_id);

  input.remove(";");
  input.remove("out ");
  
  int bracketStart = input.indexOf("[");
  int bracketEnd = input.lastIndexOf("]");

  if (bracketStart >= 0)
    input.remove(bracketStart, bracketEnd-bracketStart+1);

  // decompose output declaration
  QStringList decompInput = input.split(" ");
  return decompInput.last();
}

QString ShaderGenerator::getIOMapName(int _inId) const
{
  QString inputName = getInputName(_inId);

  // output name = input name with swapped prefix
  QString outputName = inputName;
  outputName.replace(outputName.indexOf(inputPrefix_), inputPrefix_.size(), outputPrefix_);

  return outputName;
}


ShaderGenerator::DefaultIODesc::DefaultIODesc()
  : inputTexCoord_(false),
  inputColor_(false),
  inputNormal_(false),
  passPosVS_(false), passPosOS_(false), 
  passTexCoord_(false), 
  passColor_(false),
  passNormalVS_(false), passNormalOS_(false)
{
}




QString ShaderProgGenerator::shaderDir_;
QStringList ShaderProgGenerator::lightingCode_;


ShaderProgGenerator::ShaderProgGenerator( const ShaderGenDesc* _desc )
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, (ShaderModifier**)0, 0);
}

ShaderProgGenerator::ShaderProgGenerator( const ShaderGenDesc* _desc, const unsigned int* _modifierIDs, unsigned int _numActiveMods )
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, _modifierIDs, _numActiveMods);
}

ShaderProgGenerator::ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<unsigned int>& _modifierIDs)
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, _modifierIDs.empty() ? 0 : &_modifierIDs[0], (unsigned int)_modifierIDs.size());
}

ShaderProgGenerator::ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<unsigned int>* _modifierIDs)
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, !_modifierIDs || _modifierIDs->empty() ? 0 : &((*_modifierIDs)[0]), (unsigned int)_modifierIDs->size());
}

ShaderProgGenerator::ShaderProgGenerator(const ShaderGenDesc* _desc, ShaderModifier* const* _modifiers, unsigned int _numActiveMods)
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, _modifiers, _numActiveMods);
}

ShaderProgGenerator::ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<ShaderModifier*>& _modifierIDs)
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, _modifierIDs.empty() ? 0 : &(_modifierIDs[0]), (unsigned int)_modifierIDs.size());
}

ShaderProgGenerator::ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<ShaderModifier*>* _modifierIDs)
  : vertex_(0), tessControl_(0), tessEval_(0), geometry_(0), fragment_(0), renormalizeLighting_(false)
{
  init(_desc, !_modifierIDs || _modifierIDs->empty() ? 0 : &((*_modifierIDs)[0]), (unsigned int)_modifierIDs->size());
}


void ShaderProgGenerator::init( const ShaderGenDesc* _desc, const unsigned int* _modifierIDs, unsigned int _numActiveMods )
{
  if (_modifierIDs && _numActiveMods)
  {
    activeMods_.resize(_numActiveMods);

    for (unsigned int i = 0; i < _numActiveMods; ++i)
      activeMods_[i] = registeredModifiers_[ _modifierIDs[i] ];
  }

  init(_desc, (ShaderModifier**)0, 0);
}

void ShaderProgGenerator::init( const ShaderGenDesc* _desc, ShaderModifier* const* _modifiers, unsigned int _numActiveMods )
{
  if (_modifiers && _numActiveMods)
  {
    activeMods_.resize(_numActiveMods);

    for (unsigned int i = 0; i < _numActiveMods; ++i)
      activeMods_[i] = _modifiers[i];
  }


  if (shaderDir_.isEmpty())
    std::cout << "error: call ShaderProgGenerator::setShaderDir() first!" << std::endl;
  else
  {
    desc_ = *_desc;

    // We need at least version 3.2 or higher to support geometry shaders
    if ( !ACG::openGLVersion(3,2) )
    {
      if (!desc_.geometryTemplateFile.isEmpty())
        std::cerr << "Warning: removing geometry shader from ShaderDesc" << std::endl;

      desc_.geometryTemplateFile.clear();
    }

    // We need at least version 4.0 or higher to support tessellation
    if ( !ACG::openGLVersion(4, 0) )
    {
      if (!desc_.tessControlTemplateFile.isEmpty() || !desc_.tessEvaluationTemplateFile.isEmpty())
        std::cerr << "Warning: removing tessellation shader from ShaderDesc" << std::endl;

      desc_.tessControlTemplateFile.clear();
      desc_.tessEvaluationTemplateFile.clear();
    }

    // adjust glsl version to requirement

    if (!desc_.geometryTemplateFile.isEmpty())
      desc_.version = std::max(desc_.version, 150);

    if (!desc_.tessControlTemplateFile.isEmpty() || !desc_.tessEvaluationTemplateFile.isEmpty())
      desc_.version = std::max(desc_.version, 400);


    loadLightingFunctions();

    generateShaders();
  }
}


ShaderProgGenerator::~ShaderProgGenerator(void)
{
  delete vertex_;
  delete fragment_;
  delete geometry_;
  delete tessControl_;
  delete tessEval_;
}



bool ShaderProgGenerator::loadStringListFromFile(QString _fileName, QStringList* _out)
{
  bool success = false;

  QString absFilename = getAbsFilePath(_fileName);


  QFile file(absFilename);

  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    if (!file.isReadable())
      std::cout << "error: unreadable file -> \"" << absFilename.toStdString() << "\"" << std::endl;
    else
    {
      QTextStream filestream(&file);

      while (!filestream.atEnd())
      {
        QString szLine = filestream.readLine();
        _out->push_back(szLine.trimmed());
      }

      success = true;
    }

    file.close();
  }
  else
    std::cout << "error: " << file.errorString().toStdString() << " -> \"" << absFilename.toStdString() << "\"" << std::endl;

  return success;
}


void ShaderProgGenerator::loadLightingFunctions()
{
  if (lightingCode_.size()) return;
  
  QString fileName = shaderDir_ + QDir::separator() + QString(LIGHTING_CODE_FILE);

  lightingCode_.push_back("// ==============================================================================");
  lightingCode_.push_back(QString("// ShaderGenerator - default lighting functions imported from file: ") + fileName);
  
  
  // load shader code from file
  loadStringListFromFile(fileName, &lightingCode_);
  
  lightingCode_.push_back(QString("// ShaderGenerator - end of default lighting functions"));
  lightingCode_.push_back("// ==============================================================================");
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

  if (desc_.textured())
    _gen->addDefine("SG_TEXTURED 1");

  if (desc_.vertexColors)
    _gen->addDefine("SG_VERTEX_COLOR 1");

//  if (desc_.shadeMode != SG_SHADE_UNLIT)
  if (ioDesc_.passNormalVS_)
    _gen->addDefine("SG_NORMALS 1");

  if (ioDesc_.passPosVS_)
    _gen->addDefine("SG_POSVS 1");

  if (ioDesc_.passPosOS_)
    _gen->addDefine("SG_POSOS 1");

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

  _gen->addDefine("SG_ALPHA g_vMaterial.y");
}




void ShaderProgGenerator::buildVertexShader()
{
  delete vertex_;

  vertex_  = new ShaderGenerator();
  vertex_->setGLSLVersion(desc_.version);

  vertex_->initVertexShaderIO(&desc_, &ioDesc_);

  vertex_->initDefaultUniforms();


  // apply i/o modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyVertexIO(vertex_);


  initGenDefines(vertex_);



  // IO

  // when to use vertex lights
  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT)
  {
    for (int i = 0; i < desc_.numLights; ++i)
      vertex_->addLight(i, desc_.lightTypes[i]);
  }


  // assemble main function
  QStringList mainCode;

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

    QString it;
    foreach(it,vertexTemplate_)
    {
      if (!checkForIncludes(it, vertex_, getPathName(vertexShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        if (it.contains("SG_VERTEX_BEGIN"))
          addVertexBeginCode(&mainCode);
        else
        {
          if (it.contains("SG_VERTEX_END"))
            addVertexEndCode(&mainCode);
          else
          {
            // no SG marker
            mainCode.push_back(it);
          }
        }

      }
    }

  }

  vertex_->buildShaderCode(&mainCode, lightingCode_);

}


void ShaderProgGenerator::addVertexBeginCode(QStringList* _code)
{
  // size in pixel of rendered point-lists, set by user via uniform

  _code->push_back("vec4 sg_vPosPS = g_mWVP * inPosition;");
  _code->push_back("vec4 sg_vPosVS = g_mWV * inPosition;");
  _code->push_back("vec3 sg_vNormalVS = vec3(0.0, 1.0, 0.0);");
  _code->push_back("vec3 sg_vNormalOS = vec3(0.0, 1.0, 0.0);");

  /// note: multi-texturing is not supported, as this requires custom texture compositing
  //       this can be done via shader modifiers or templates
  if (ioDesc_.inputTexCoord_)
  {
    if (desc_.textureTypes().begin()->second.type == GL_TEXTURE_3D) {
      _code->push_back("vec3 sg_vTexCoord = inTexCoord;");
    } else {
      _code->push_back("vec2 sg_vTexCoord = inTexCoord;");
    }
  }

  _code->push_back("vec4 sg_cColor = vec4(g_cEmissive, SG_ALPHA);");

  if (ioDesc_.inputNormal_)
  {
    _code->push_back("sg_vNormalVS = normalize(g_mWVIT * inNormal);");
    _code->push_back("sg_vNormalOS = normalize(inNormal);");
  }

  if (ioDesc_.inputColor_)
    _code->push_back("sg_cColor = inColor;");

  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
    desc_.shadeMode == SG_SHADE_FLAT)
  {
    // add lighting code here

    addLightingCode(_code);
  }



  // apply modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyVertexBeginCode(_code);
}


void ShaderProgGenerator::addVertexEndCode(QStringList* _code)
{
  _code->push_back("gl_Position = sg_vPosPS;");
  _code->push_back("outVertexPosCS = sg_vPosPS;");

  if (ioDesc_.passTexCoord_)
    _code->push_back("outVertexTexCoord = sg_vTexCoord;");

  if (ioDesc_.passColor_)
    _code->push_back("outVertexColor = sg_cColor;");

  if (ioDesc_.passNormalVS_)
    _code->push_back("outVertexNormal = sg_vNormalVS;");

  if (ioDesc_.passNormalOS_)
    _code->push_back("outVertexNormalOS = sg_vNormalOS;");

  if (ioDesc_.passPosVS_)
    _code->push_back("outVertexPosVS = sg_vPosVS;");

  if (ioDesc_.passPosOS_)
    _code->push_back("outVertexPosOS = inPosition;");



  // apply modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyVertexEndCode(_code);
}


int ShaderProgGenerator::checkForIncludes(QString _str, ShaderGenerator* _gen, QString _includePath)
{
  if (_str.contains("#include "))
  {
    QString strIncludeFile = _str.remove("#include ").remove('\"').remove('<').remove('>').trimmed();

    if (strIncludeFile.isEmpty())
      std::cout << "wrong include syntax: " << _str.toStdString() << std::endl;
    else
    {
      QString fullPathToIncludeFile = _includePath + QDir::separator() + strIncludeFile;

      _gen->addIncludeFile(fullPathToIncludeFile);
    }

    return 1;
  }

  return 0;
}

int ShaderProgGenerator::checkForIncludes(QString _str, QStringList* _outImport, QString _includePath)
{
  if (_str.contains("#include "))
  {
    QString strIncludeFile = _str.remove("#include ").remove('\"').remove('<').remove('>').trimmed();

    if (strIncludeFile.isEmpty())
      std::cout << "wrong include syntax: " << _str.toStdString() << std::endl;
    else
    {
      QString fullPathToIncludeFile = _includePath + QDir::separator() + strIncludeFile;

      // unify separator chars
      fullPathToIncludeFile.replace('\\', '/');

      // get rid of ".." usage inside shader includes
      QString cleanFilepath = QDir::cleanPath(fullPathToIncludeFile);

      loadStringListFromFile(cleanFilepath, _outImport);
    }

    return 1;
  }

  return 0;
}

void ShaderProgGenerator::buildTessControlShader()
{
  // Only build a tess-control shader if enabled
  if ( desc_.tessControlTemplateFile.isEmpty() )
    return;

  // the generator provides an IO mapping function and adds default uniforms to this stage
  // - template is necessary
  // - combination/modification of tess-control shader is not supported
  // - template may call sg_MapIO(inId, gl_InvocationID) somewhere in code to take care of default IO pass-through
  //         inId can be gl_InvocationID if the patch size is not modified

  delete tessControl_;

  tessControl_  = new ShaderGenerator();
  tessControl_->setGLSLVersion(desc_.version);

  QString it;
  foreach(it, tessControlLayout_)
    tessControl_->addLayout(it);

  // find previous shader stage
  ShaderGenerator* prevStage = vertex_;

  tessControl_->initTessControlShaderIO(&desc_, prevStage, &ioDesc_);

  tessControl_->initDefaultUniforms();


  // apply i/o modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyTessControlIO(tessControl_);

  initGenDefines(tessControl_);



  // assemble main function
  QStringList mainCode; 

  // add simple io passthrough mapper

  {
    mainCode.push_back("void sg_MapIO(const int inIdx, const int outIdx)");
    mainCode.push_back("{");

    // built-in IO
    mainCode.push_back("gl_out[outIdx].gl_Position = gl_in[inIdx].gl_Position;");
    
    // custom IO
    for (int i = 0; i < tessControl_->getNumInputs(); ++i)
    {
      QString inputName = tessControl_->getInputName(i);
      QString outputName = tessControl_->getIOMapName(i);

      QString outputAssignCode = outputName + QString("[outIdx] = ") + inputName + QString("[inIdx];");

      mainCode.push_back(outputAssignCode);
    }

    mainCode.push_back("}");
  }


  // interpret loaded shader template:
  //  import #includes
  foreach(it,tessControlTemplate_)
  {
    if (!checkForIncludes(it, tessControl_, getPathName(tessControlShaderFile_)))
    {
      // str line is no include directive
      mainCode.push_back(it);
    }
  }

  tessControl_->buildShaderCode(&mainCode, lightingCode_);
}

void ShaderProgGenerator::buildTessEvalShader()
{
  // Only build a tess-eval shader if enabled
  if ( desc_.tessEvaluationTemplateFile.isEmpty() )
    return;

  // the generator provides default interpolation functions and adds default uniforms to this stage
  // - template is necessary
  // - combination/modification of tess-eval shader is not supported
  // - template may call sg_MapIOBarycentric() or sg_MapIOBilinear() somewhere in code to take care of default IO pass-through
  //        - barycentric interpolation can be used for triangle patches
  //        - bilinear interpolation can be used for quad patches
  //        - other interpolation schemes have to be user defined

  delete tessEval_;

  tessEval_  = new ShaderGenerator();
  tessEval_->setGLSLVersion(desc_.version);


  // find previous shader stage
  ShaderGenerator* prevStage = tessControl_;

  if (!prevStage)
    prevStage = vertex_;

  tessEval_->initTessEvalShaderIO(&desc_, prevStage, &ioDesc_);

  tessEval_->initDefaultUniforms();

  QString itLayout;
  foreach(itLayout, tessEvalLayout_)
    tessEval_->addLayout(itLayout);

  // apply i/o modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyTessControlIO(tessEval_);

  initGenDefines(tessEval_);


  // assemble main function
  QStringList mainCode; 

  // add simple io passthrough mapper

  {
    // barycentric interpolation

    mainCode.push_back("void sg_MapIOBarycentric()");
    mainCode.push_back("{");

    // built-in IO
    mainCode.push_back("gl_Position = gl_TessCoord.x * gl_in[0].gl_Position + gl_TessCoord.y * gl_in[1].gl_Position + gl_TessCoord.z * gl_in[2].gl_Position;");

    // custom IO
    for (int i = 0; i < tessEval_->getNumInputs(); ++i)
    {
      QString inputName = tessEval_->getInputName(i);
      QString outputName = tessEval_->getIOMapName(i);

      QString outputAssignCode = outputName + QString(" = ") +
        QString("gl_TessCoord.x*") + inputName + QString("[0] + ") +
        QString("gl_TessCoord.y*") + inputName + QString("[1] + ") +
        QString("gl_TessCoord.z*") + inputName + QString("[2];");

      mainCode.push_back(outputAssignCode);
    }

    mainCode.push_back("}");

    
    // bilinear interpolation

    mainCode.push_back("void sg_MapIOBilinear()");
    mainCode.push_back("{");

    // built-in IO
    mainCode.push_back("gl_Position = mix(  mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x), mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x), gl_TessCoord.y);");

    // custom IO
    for (int i = 0; i < tessEval_->getNumInputs(); ++i)
    {
      QString inputName = tessEval_->getInputName(i);
      QString outputName = tessEval_->getIOMapName(i);

      QString outputAssignCode = outputName + QString(" = mix( ") +
        QString("mix(") + inputName + QString("[0], ") + inputName + QString("[1], gl_TessCoord.x), ") + 
        QString("mix(") + inputName + QString("[2], ") + inputName + QString("[3], gl_TessCoord.x), gl_TessCoord.y); ");

      mainCode.push_back(outputAssignCode);
    }

    mainCode.push_back("}");
  }


  // interpret loaded shader template:
  //  replace (SG_INPUT, SG_OUTPUT) with matching io pairs
  QStringList::iterator it;
  for (it = tessEvalTemplate_.begin(); it != tessEvalTemplate_.end(); ++it)
  {
    QString line = *it;

    // replace IO line matching the pattern:
    //  SG_OUTPUT = r_expression(SG_INPUT);
    // the complete expression must be contained in a single line for this to work
    // more complex interpolation code should use #if SG_NORMALS etc.

    if (line.contains("SG_INPUT") || line.contains("SG_OUTPUT"))
    {

      QStringList resolvedCode;

      resolvedCode.push_back("// ----------------------------------------");
      resolvedCode.push_back("// ShaderGen: resolve SG_OUTPUT = expression(SG_INPUT);");

      int numOccurrences = 0;

      for (int i = 0; i < tessEval_->getNumInputs(); ++i)
      {
        QString inputName = tessEval_->getInputName(i);
        QString outputName = tessEval_->getIOMapName(i);

        // replace SG_INPUT, SG_OUTPUT with actual names
        QString resolvedLine = line;

        // avoid confusion with SG_INPUT_NORMALVS etc. naming convention
        //  resolvedLine.replace("SG_INPUT", inputName);
        //  resolvedLine.replace("SG_OUTPUT", outputName);
        // fails to do this
        
        // maybe this can be simplified with regexp
        // ie. replace SG_INPUT with inputName,  but not SG_INPUTN, SG_INPUT_ ..

        for (int k = 0; k < 2; ++k)
        {
          const QString stringToReplace = k ? "SG_OUTPUT" : "SG_INPUT";
          const int lenStringToReplace = stringToReplace.length();
          const QString replacementString = k ? outputName : inputName;

          int linePos = resolvedLine.indexOf(stringToReplace);

          while (linePos >= 0)
          {
            bool replaceOcc = true;

            int nextCharPos = linePos + lenStringToReplace;

            if (nextCharPos >= resolvedLine.size())
              nextCharPos = -1;

            if (nextCharPos > 0)
            {
              QChar nextChar = resolvedLine.at(nextCharPos);

              if (nextChar == '_' || nextChar.isDigit() || nextChar.isLetter())
              {
                // name token continues, this should not be replaced!

                linePos += lenStringToReplace;
                replaceOcc = false;
              }
            }

            // replace

            if (replaceOcc)
            {
              resolvedLine.replace(linePos, lenStringToReplace, replacementString);
              ++numOccurrences;
            }

            linePos = resolvedLine.indexOf(stringToReplace, linePos + 1);
          }
        }


        


        resolvedCode.push_back(resolvedLine);
      }

      resolvedCode.push_back("// ----------------------------------------");

      if (numOccurrences)
        mainCode.append(resolvedCode);
      else
        mainCode.push_back(line); // nothing to replace in this line
    }
    else
      mainCode.push_back(line);
  }

  tessEval_->buildShaderCode(&mainCode, lightingCode_);
}

void ShaderProgGenerator::buildGeometryShader()
{
  // Only build a geometry shader if enabled
  if ( desc_.geometryTemplateFile.isEmpty() )
    return;


  delete geometry_;

  geometry_  = new ShaderGenerator();
  geometry_->setGLSLVersion(desc_.version);


  // find previous shader stage
  ShaderGenerator* prevStage = tessEval_;

  if (!prevStage)
    prevStage = vertex_;

  geometry_->initGeometryShaderIO(&desc_, prevStage, &ioDesc_);

  geometry_->initDefaultUniforms();


  // apply i/o modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyGeometryIO(geometry_);

  initGenDefines(geometry_);


  // assemble main function
  QStringList mainCode; 

  // add simple io passthrough mapper

  {
    mainCode.push_back("void sg_MapIO(const int inIdx)");
    mainCode.push_back("{");

    // built-in IO
    mainCode.push_back("gl_Position = gl_in[inIdx].gl_Position;");
    mainCode.push_back("gl_PrimitiveID = gl_PrimitiveIDIn;");

    // custom IO
    for (int i = 0; i < geometry_->getNumInputs(); ++i)
    {
      QString inputName = geometry_->getInputName(i);
      QString outputName = geometry_->getIOMapName(i);

      QString outputAssignCode = outputName + QString(" = ") + inputName + QString("[inIdx];");

      mainCode.push_back(outputAssignCode);
    }

    mainCode.push_back("}");
  }


  // interpret loaded shader template:
  //  import #includes
  QString it;
  foreach(it,geometryTemplate_)
  {
    if (!checkForIncludes(it, geometry_, getPathName(geometryShaderFile_)))
    {
      // str line is no include directive
      mainCode.push_back(it);
    }
  }

  geometry_->buildShaderCode(&mainCode, lightingCode_);
}


void ShaderProgGenerator::buildFragmentShader()
{
  delete fragment_;

  fragment_  = new ShaderGenerator();
  fragment_->setGLSLVersion(desc_.version);

  // find previous shader stage
  ShaderGenerator* prevStage = geometry_;

  if (!prevStage)
    prevStage = tessEval_;
  if (!prevStage)
    prevStage = tessControl_;
  if (!prevStage)
    prevStage = vertex_;


  fragment_->initFragmentShaderIO(&desc_, prevStage, &ioDesc_);


  fragment_->initDefaultUniforms();


  // texture sampler id
  if (desc_.textured())
  {
    for (std::map<size_t,ShaderGenDesc::TextureType>::const_iterator iter = desc_.textureTypes().begin();
        iter != desc_.textureTypes().end(); ++iter)
    {
      QString name = QString("g_Texture%1").arg(iter->first);
      QString type = "";
      switch (iter->second.type)
      {
        case GL_TEXTURE_1D: type = "sampler1D"; break;
        case GL_TEXTURE_2D: type = "sampler2D"; break;
        case GL_TEXTURE_3D: type = "sampler3D"; break;
        case GL_TEXTURE_CUBE_MAP: type = "samplerCube"; break;
#ifdef GL_ARB_texture_rectangle //ARCH_DARWIN doesn't support all texture defines with all xcode version (xcode 5.0 seems to support all)
        case GL_TEXTURE_RECTANGLE_ARB: type = "sampler2DRect"; break;
#endif
#ifdef GL_ARB_texture_buffer_object
        case GL_TEXTURE_BUFFER_ARB: type = "samplerBuffer"; break;
#endif
#ifdef GL_EXT_texture_array
        case GL_TEXTURE_1D_ARRAY_EXT: type = "sampler1DArray"; break;
        case GL_TEXTURE_2D_ARRAY_EXT: type = "sampler2DArray"; break;
#endif
#ifdef GL_ARB_texture_cube_map_array
        case GL_TEXTURE_CUBE_MAP_ARRAY_ARB: type = "samplerCubeArray"; break;
#endif
#ifdef GL_ARB_texture_multisample
        case GL_TEXTURE_2D_MULTISAMPLE: type = "sampler2DMS"; break;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: type = "sampler2DMSArray"; break;
#endif
        default: std::cerr << "Texture Type not supported "<< iter->second.type << std::endl; break;
      }
      // todo: check if texture type supports shadowtype
      if (iter->second.shadow)
        type += "Shadow";
      fragment_->addUniform(type + " " + name);
    }
  }

  // apply i/o modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyFragmentIO(fragment_);


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
    QString it;
    foreach(it,fragmentTemplate_)
    {
      if (!checkForIncludes(it, fragment_, getPathName(fragmentShaderFile_)))
      {
        // str line is no include directive
        // check for SG_ markers

        if (it.contains("SG_FRAGMENT_BEGIN"))
          addFragmentBeginCode(&mainCode);
        else if (it.contains("SG_FRAGMENT_END"))
          addFragmentEndCode(&mainCode);
        else if (it.contains("SG_FRAGMENT_LIGHTING"))
          addLightingCode(&mainCode);
        else // no SG marker
          mainCode.push_back(it);

      }

      
    }

  }



  fragment_->buildShaderCode(&mainCode, lightingCode_);
}


void ShaderProgGenerator::addFragmentBeginCode(QStringList* _code)
{

  QString inputShader = "Vertex";

  if ( tessEval_ )
    inputShader = "Te";
  if ( geometry_ )
    inputShader = "Geometry";

  // support for projective texture mapping
  _code->push_back("vec4 sg_vPosCS = " SG_INPUT_POSCS ";");
  _code->push_back("vec2 sg_vScreenPos = sg_vPosCS.xy / sg_vPosCS.w * 0.5 + vec2(0.5, 0.5);");

  _code->push_back("#ifdef " SG_INPUT_POSVS);
  _code->push_back("vec4 sg_vPosVS = " SG_INPUT_POSVS ";");
  _code->push_back("#endif");

  _code->push_back("#ifdef " SG_INPUT_NORMALVS);
  _code->push_back("vec3 sg_vNormalVS = " SG_INPUT_NORMALVS ";");
  if (renormalizeLighting_)
    _code->push_back("sg_vNormalVS = normalize(sg_vNormalVS);");
  _code->push_back("#endif");


  _code->push_back("vec4 sg_cColor = vec4(g_cEmissive, SG_ALPHA);");

  if (desc_.shadeMode == SG_SHADE_GOURAUD ||
      desc_.shadeMode == SG_SHADE_FLAT    ||
      desc_.vertexColors)
  {
    _code->push_back("sg_cColor = out" + inputShader + "Color;");
  }


  if (desc_.shadeMode == SG_SHADE_PHONG)
    addLightingCode(_code);

  if (desc_.textured())
  {
    std::map<size_t,ShaderGenDesc::TextureType>::const_iterator iter = desc_.textureTypes().begin();
    _code->push_back("vec4 sg_cTex = texture(g_Texture"+QString::number(iter->first)+", out" + inputShader + "TexCoord);");

    for (++iter; iter != desc_.textureTypes().end(); ++iter)
      _code->push_back("sg_cTex += texture(g_Texture"+QString::number(iter->first)+", out" + inputShader + "TexCoord);");

    if (desc_.textureTypes().size() > 1 && desc_.normalizeTexColors)
      _code->push_back("sg_cTex = sg_cTex * 1.0/" + QString::number(desc_.textureTypes().size()) +".0 ;");

    if (desc_.shadeMode == SG_SHADE_UNLIT)
      _code->push_back("sg_cColor += sg_cTex;");
    else
      _code->push_back("sg_cColor *= sg_cTex;");
  }

  
  // apply modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyFragmentBeginCode(_code);
}

void ShaderProgGenerator::addFragmentEndCode(QStringList* _code)
{
  _code->push_back("outFragment = sg_cColor;");

  // apply modifiers
  for (size_t i = 0; i < activeMods_.size(); ++i)
    activeMods_[i]->modifyFragmentEndCode(_code);
}



void ShaderProgGenerator::addLightingCode(QStringList* _code)
{

  ShaderModifier* lightingModifier = 0;

  // check if any modifier replaces the default lighting function
  for (size_t i = 0; i < activeMods_.size() && !lightingModifier; ++i)
  {
    if (activeMods_[i]->replaceDefaultLightingCode())
        lightingModifier = activeMods_[i];
  }

  if (!lightingModifier)
  {
    // default lighting code:

    QString buf;

    for (int i = 0; i < desc_.numLights; ++i)
    {
      ShaderGenLightType lgt = desc_.lightTypes[i];

      switch (lgt)
      {
      case SG_LIGHT_DIRECTIONAL:
        buf.sprintf("sg_cColor.xyz += LitDirLight(sg_vPosVS.xyz, sg_vNormalVS, g_vLightDir_%d,  g_cLightAmbient_%d,  g_cLightDiffuse_%d,  g_cLightSpecular_%d);", i, i, i, i);
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

    // modify lighting color afterwards

    for (size_t i = 0; i < activeMods_.size(); ++i)
      modifyLightingCode(_code, activeMods_[i]);
  }
  else
  {
    // there exists a lighting modifier that completely replaces the default lighting shader
    modifyLightingCode(_code, lightingModifier);


    // apply remaining modifiers that do not replace the complete lighting code

    for (size_t i = 0; i < activeMods_.size(); ++i)
    {
      if (lightingModifier != activeMods_[i])
        modifyLightingCode(_code, activeMods_[i]);
    }
  }

}

void ShaderProgGenerator::modifyLightingCode( QStringList* _code, ShaderModifier* _modifier )
{
  if (!_modifier) return;

  for (int i = 0; i < desc_.numLights; ++i)
  {
    ShaderGenLightType lgt = desc_.lightTypes[i];

    _modifier->modifyLightingCode(_code, i, lgt);
  }
}


void ShaderProgGenerator::addLightingFunctions(QStringList* _code)
{
  QString it;
  foreach(it,lightingCode_)
    _code->push_back(it);
}

void ShaderProgGenerator::generateShaders()
{
  // import template source from files
  loadShaderTemplateFromFile();

  // check what needs to be passed down from vertex shader

  if (desc_.shadeMode != SG_SHADE_UNLIT)
    ioDesc_.inputNormal_ = true;

  if (desc_.textured())
  {
    ioDesc_.inputTexCoord_ = true;
    ioDesc_.passTexCoord_ = true;
  }

  if (desc_.vertexColors)
    ioDesc_.inputColor_ = true;

  if (desc_.shadeMode == SG_SHADE_PHONG)
  {
    ioDesc_.passNormalVS_ = true;
    ioDesc_.passPosVS_ = true;
  }

  if (desc_.shadeMode == SG_SHADE_FLAT || desc_.shadeMode == SG_SHADE_GOURAUD || desc_.vertexColors)
    ioDesc_.passColor_ = true;


  // scan macros of modifiers for attribute requests,
  // done by adding modifier io to an empty dummy
  ShaderGenerator dummy;

  for (size_t i = 0; i < activeMods_.size(); ++i)
  {
    ShaderModifier* mod = activeMods_[i];

    mod->modifyVertexIO(&dummy);
    mod->modifyTessControlIO(&dummy);
    mod->modifyTessEvalIO(&dummy);
    mod->modifyGeometryIO(&dummy);
    mod->modifyFragmentIO(&dummy);
  }
  // scan requested inputs from modifiers

  if (dummy.hasDefine(SG_REQUEST_POSVS))
    ioDesc_.passPosVS_ = true;
  if (dummy.hasDefine(SG_REQUEST_TEXCOORD))
  {
    ioDesc_.inputTexCoord_ = true;
    ioDesc_.passTexCoord_ = true;
  }
  if (dummy.hasDefine(SG_REQUEST_VERTEXCOLOR))
  {
    ioDesc_.inputColor_ = true;
    ioDesc_.passColor_ = true;
  }
  if (dummy.hasDefine(SG_REQUEST_NORMALVS))
  {
    ioDesc_.inputNormal_ = true;
    ioDesc_.passNormalVS_ = true;
  }
  if (dummy.hasDefine(SG_REQUEST_NORMALOS))
  {
    ioDesc_.inputNormal_ = true;
    ioDesc_.passNormalOS_ = true;
  }
  if (dummy.hasDefine(SG_REQUEST_POSOS))
    ioDesc_.passPosOS_ = true;
  
  if (dummy.hasDefine(SG_REQUEST_RENOMARLIZE))
    renormalizeLighting_ = true;




  // assemble shader codes

  buildVertexShader();
  buildTessControlShader();
  buildTessEvalShader();
  buildGeometryShader();
  buildFragmentShader();
}


const QStringList& ShaderProgGenerator::getVertexShaderCode()
{
  return vertex_->getShaderCode();
}

const QStringList& ShaderProgGenerator::getTessControlShaderCode()
{
  return tessControl_->getShaderCode();
}

const QStringList& ShaderProgGenerator::getTessEvaluationShaderCode()
{
  return tessEval_->getShaderCode();
}

const QStringList& ShaderProgGenerator::getGeometryShaderCode()
{
  return geometry_->getShaderCode();
}

const QStringList& ShaderProgGenerator::getFragmentShaderCode()
{
  return fragment_->getShaderCode();
}


void ShaderProgGenerator::saveVertexShToFile(const char* _fileName)
{
  vertex_->saveToFile(_fileName);
}

void ShaderProgGenerator::saveGeometryShToFile(const char* _fileName)
{
  geometry_->saveToFile(_fileName);
}

void ShaderProgGenerator::saveFragmentShToFile(const char* _fileName)
{
  fragment_->saveToFile(_fileName);
}


void ShaderProgGenerator::loadShaderTemplateFromFile()
{
  if (!desc_.vertexTemplateFile.isEmpty())
  {
    loadStringListFromFile(desc_.vertexTemplateFile, &vertexTemplate_);
    scanShaderTemplate(vertexTemplate_, desc_.vertexTemplateFile);
  }
  if (!desc_.fragmentTemplateFile.isEmpty())
  {
    loadStringListFromFile(desc_.fragmentTemplateFile, &fragmentTemplate_);
    scanShaderTemplate(fragmentTemplate_, desc_.fragmentTemplateFile);
  }
  if (!desc_.geometryTemplateFile.isEmpty())
  {
    loadStringListFromFile(desc_.geometryTemplateFile, &geometryTemplate_);
    scanShaderTemplate(geometryTemplate_, desc_.geometryTemplateFile);
  }
  if (!desc_.tessControlTemplateFile.isEmpty())
  {
    loadStringListFromFile(desc_.tessControlTemplateFile, &tessControlTemplate_);
    scanShaderTemplate(tessControlTemplate_, desc_.tessControlTemplateFile, &tessControlLayout_);
  }
  if (!desc_.tessEvaluationTemplateFile.isEmpty())
  {
    loadStringListFromFile(desc_.tessEvaluationTemplateFile, &tessEvalTemplate_);
    scanShaderTemplate(tessEvalTemplate_, desc_.tessEvaluationTemplateFile, &tessEvalLayout_);
  }


  vertexShaderFile_   = desc_.vertexTemplateFile;
  tessControlShaderFile_ = desc_.tessControlTemplateFile;
  tessEvalShaderFile_ = desc_.tessEvaluationTemplateFile;
  geometryShaderFile_ = desc_.geometryTemplateFile;
  fragmentShaderFile_ = desc_.fragmentTemplateFile;
}

void ShaderProgGenerator::scanShaderTemplate(QStringList& _templateSrc, QString _templateFilename, QStringList* _outLayoutDirectives)
{
  // interpret loaded shader template:
  //  import #includes

  QString filePath = getPathName(_templateFilename);

  QStringList::iterator it;
  for (it = _templateSrc.begin(); it != _templateSrc.end(); ++it)
  {
    QStringList import;

    if (checkForIncludes(*it, &import, filePath))
    {
      // line is include directive

      // remove line from source
      it = _templateSrc.erase(it);

      int offset = it - _templateSrc.begin();

      // insert imported file

      QString importLine;
      foreach(importLine, import)
      {
        it = _templateSrc.insert(it, importLine);
        ++it;
      }

      // included file might recursively include something again
      // -> scan included file
      it = _templateSrc.begin() + offset;
    }
    else
    {
      QString trimmedLine = it->trimmed();

      // scan and adjust glsl version
      QByteArray lineBytes = trimmedLine.toUtf8();

      int templateVersion = 0;
      if (sscanf(lineBytes.constData(), "#version %d", &templateVersion) == 1)
      {
        desc_.version = std::max(templateVersion, desc_.version);

        // remove version line from template since this is added later in the build functions
        it = _templateSrc.erase(it);
      }
      // scan layout() directive
      else if (trimmedLine.startsWith("layout(") || trimmedLine.startsWith("layout ("))
      {
        if (_outLayoutDirectives)
        {
          _outLayoutDirectives->push_back(trimmedLine);
          // layout() will be inserted later at the correct position in the build functions
          // - must be placed before shader IO declaration to make tess-control shaders compilable on ati
          it = _templateSrc.erase(it);
        }
      }
      else
      {
        // scan requested inputs

        if (trimmedLine.startsWith(SG_REQUEST_POSVS))
          ioDesc_.passPosVS_ = true;
        else if (trimmedLine.startsWith(SG_REQUEST_TEXCOORD))
        {
          ioDesc_.inputTexCoord_ = true;
          ioDesc_.passTexCoord_ = true;
        }
        else if (trimmedLine.startsWith(SG_REQUEST_VERTEXCOLOR))
        {
          ioDesc_.inputColor_ = true;
          ioDesc_.passColor_ = true;
        }
        else if (trimmedLine.startsWith(SG_REQUEST_NORMALVS))
        {
          ioDesc_.inputNormal_ = true;
          ioDesc_.passNormalVS_ = true;
        }
        else if (trimmedLine.startsWith(SG_REQUEST_NORMALOS))
        {
          ioDesc_.inputNormal_ = true;
          ioDesc_.passNormalOS_ = true;
        }
        else if (trimmedLine.startsWith(SG_REQUEST_POSOS))
          ioDesc_.passPosOS_ = true;
        else if (trimmedLine.startsWith(SG_REQUEST_RENOMARLIZE))
          renormalizeLighting_ = true;
        else if (trimmedLine.startsWith("SG_FRAGMENT_LIGHTING"))
        {
          // shader template performs lighting in fragment shader
          // -> forced phong shading
          desc_.shadeMode = SG_SHADE_PHONG;
        }
      }

    }
  }

}

QString ShaderProgGenerator::getPathName(QString _strFileName)
{
  QFileInfo fileInfo(getAbsFilePath(_strFileName));
  return fileInfo.absolutePath();
}

QString ShaderProgGenerator::getAbsFilePath(QString _strFileName)
{
  QString absFilename;
  if ( QDir(_strFileName).isRelative() )
    absFilename = getShaderDir() + QDir::separator() + _strFileName;
  else
    absFilename = _strFileName;

  return QDir::cleanPath(absFilename);
}

void ShaderProgGenerator::setShaderDir( QString _dir )
{
  shaderDir_ = _dir;
}

QString ShaderProgGenerator::getShaderDir()
{
  return shaderDir_ + QString("/");
}


unsigned int ShaderProgGenerator::registerModifier( ShaderModifier* _modifier )
{
  if (!_modifier) return 0;

  // redundancy check
  for (int i = 0; i < numRegisteredModifiers_; ++i)
  {
    if (registeredModifiers_[i] == _modifier) 
    {
//      std::cout << "warning: trying to re-register shader modifier " << _modifier->getID() << std::endl;
      return registeredModifiers_[i]->getID();
    }
  }

  _modifier->modifierID_ = (unsigned int)(numRegisteredModifiers_++);

  registeredModifiers_.push_back(_modifier);
  return _modifier->modifierID_;
}

ShaderModifier* ShaderProgGenerator::getActiveModifier( int _i )
{
  if (_i >= 0 && _i <= int(activeMods_.size()))
    return activeMods_[_i];

  // invalid _i
  return 0;
}

int ShaderProgGenerator::getNumActiveModifiers() const
{
  return int(activeMods_.size());
}


bool ShaderProgGenerator::hasGeometryShader() const
{
  return !desc_.geometryTemplateFile.isEmpty();
}

bool ShaderProgGenerator::hasTessControlShader() const
{
  return !desc_.tessControlTemplateFile.isEmpty();
}

bool ShaderProgGenerator::hasTessEvaluationShader() const
{
  return !desc_.tessEvaluationTemplateFile.isEmpty();
}


//=============================================================================

ShaderModifier::ShaderModifier( void )
: modifierID_(0)
{}

ShaderModifier::~ShaderModifier( void )
{}


class ShaderModifierFile : public ShaderModifier
{
public:

  ShaderModifierFile()
    : version_(0) 
  {}

  virtual ~ShaderModifierFile() 
  {}

  void modifyVertexIO(ShaderGenerator* _shader) { modifyIO(0, _shader); }
  void modifyTessControlIO(ShaderGenerator* _shader) { modifyIO(1, _shader); }
  void modifyTessEvalIO(ShaderGenerator* _shader) { modifyIO(2, _shader); }
  void modifyGeometryIO(ShaderGenerator* _shader) { modifyIO(3, _shader); }
  void modifyFragmentIO(ShaderGenerator* _shader) { modifyIO(4, _shader); }


  void modifyVertexBeginCode(QStringList* _code) { _code->append(vertexBeginCode_); }
  void modifyVertexEndCode(QStringList* _code) { _code->append(vertexEndCode_); }
  void modifyFragmentBeginCode(QStringList* _code) { _code->append(fragmentBeginCode_); }
  void modifyFragmentEndCode(QStringList* _code) { _code->append(fragmentEndCode_); }

  const QString& filename() const {return filename_;}
  const QDateTime& filetime() const {return filetime_;}
  void filetime(const QDateTime& _newtime) {filetime_ = _newtime;}

  void clear()
  {
    version_ = 0;

    for (int i = 0; i < 5; ++i)
      io_[i].clear();

    vertexBeginCode_.clear();
    vertexEndCode_.clear();
    fragmentBeginCode_.clear();
    fragmentEndCode_.clear();
  }

  static ShaderModifierFile* loadFromFile(QString _filename)
  {
    ShaderModifierFile* res = 0;

    // get timestamp
    QString absFilename = ShaderProgGenerator::getAbsFilePath(_filename);
    QDateTime lastmod = QFileInfo(absFilename).lastModified();

    // check cache
    QHash<QString, ShaderModifierFile>::iterator cacheEntry = fileCache_.find(_filename);

    bool reload = false;
    bool firstLoad = false;

    if (cacheEntry != fileCache_.end())
    {
      // fetch from cache
      res = &cacheEntry.value();

      if (lastmod != res->filetime())
      {
        res->clear();
        reload = true;
      }
    }
    else
    {
      // load new modifier
      reload = true;
      firstLoad = true;
    }

    if (reload)
    {
      QStringList lines;
      if (ShaderProgGenerator::loadStringListFromFile(_filename, &lines))
      {
        // new cache entry
        if (firstLoad)
          res = &fileCache_[_filename];

        res->loadBlocks(lines);
        res->filetime(lastmod);

        // also register to generator
        if (firstLoad)
          ShaderProgGenerator::registerModifier(res);
      }
    }

    return res;
  }

private:


  void loadBlocks(const QStringList& _lines)
  {
    static const char* markers [] = 
    {
      "VertexIO:",
      "TessControlIO:",
      "TessEvalIO:",
      "GeometryIO:",
      "FragmentIO:",
      "VertexBeginCode:",
      "VertexEndCode:",
      "FragmentBeginCode:",
      "FragmentEndCode:"
    };
    const int numMarkers = sizeof(markers) / sizeof(markers[0]);

    QStringList* blockTargets [] = 
    {
      io_ + 0,
      io_ + 1,
      io_ + 2,
      io_ + 3,
      io_ + 4,
      &vertexBeginCode_,
      &vertexEndCode_,
      &fragmentBeginCode_,
      &fragmentEndCode_
    };

    assert(sizeof(blockTargets) / sizeof(blockTargets[0]) == numMarkers);


    // current block in file, points to one of io_[idx], vertexBeginCode_, ...
    QStringList* curBlock_ = 0;


    int curLine = 0;

    for (QStringList::const_iterator it = _lines.begin(); it != _lines.end(); ++it, ++curLine)
    {
      if (it->isEmpty())
        continue;

      // read glsl version
      if (version_ <= 0 && it->startsWith("#version "))
      {
        const int offset = strlen("#version ");
        version_ = atoi(it->toLatin1().data() + offset);
      }
      else
      {
        // read code blocks

        bool blockMarker = false;

        for (int i = 0; i < numMarkers && !blockMarker; ++i)
        {
          if ( it->startsWith(markers[i]) ) 
          {
            // new block start
            curBlock_ = blockTargets[i];
            blockMarker = true;
          }
        }

        if (!blockMarker)
        {
          if (curBlock_) // code belongs to some block
            curBlock_->push_back(*it);
          else // wrong file structure
            std::cerr << "ShaderModifierFile::loadBlocks - line belongs to unknown block in file " << filename_.toLatin1().data() << " at line " << curLine << std::endl;
        }
      }
    }
  }

  void modifyIO(int _stage, ShaderGenerator* _shader)
  {
    if (version_ > 0)
      _shader->setGLSLVersion(version_);

    _shader->addRawIOBlock(io_[_stage]);
  }

private:

  QString filename_;

  QDateTime filetime_; 

  // glsl version
  int version_;

  // io mods
  QStringList io_[5];

  // code mods
  QStringList vertexBeginCode_,
    vertexEndCode_,
    fragmentBeginCode_,
    fragmentEndCode_;


  // loaded modifiers
  static QHash<QString, ShaderModifierFile> fileCache_;
};

QHash<QString, ShaderModifierFile> ShaderModifierFile::fileCache_;


ShaderModifier* ShaderModifier::loadFromFile(QString _filename)
{
  return ShaderModifierFile::loadFromFile(_filename);
}


//=============================================================================


QString ShaderGenDesc::toString() const
{
  // mapping (int)ShaderGenMode -> string
  const char* shadeModeString[] = 
  {
    "SG_SHADE_UNLIT",
    "SG_SHADE_FLAT",
    "SG_SHADE_GOURAUD",
    "SG_SHADE_PHONG"
  };

  QString res;
  QTextStream resStrm(&res);

  resStrm << "version: " << version;

  resStrm << "shaderDesc.numLights: " << numLights;

  if (numLights)
  {
    resStrm << "\nshaderDesc.lightTypes[]: {";

    for (int i = 0; i < numLights; ++i)
    {
      switch (lightTypes[i]) 
      {
      case SG_LIGHT_DIRECTIONAL: resStrm << "DIRECTIONAL"; break;
      case SG_LIGHT_POINT: resStrm << "POINT"; break;
      case SG_LIGHT_SPOT: resStrm << "SPOT"; break;
      default: resStrm << "UNDEFINED"; break;
      }

      if (i + 1 < numLights)
        resStrm << ", ";
      else
        resStrm << "}";
    }
  }


  resStrm << "\nshaderDesc.shadeMode: " << shadeModeString[shadeMode];
  resStrm << "\nshaderDesc.vertexColors: " << vertexColors;
  resStrm << "\nshaderDesc.textured(): " << textured();
  for (std::map<size_t,TextureType>::const_iterator iter = textureTypes_.begin(); iter != textureTypes_.end();++iter)
  {
    resStrm << "\nTexture stage: " << iter->first;
    resStrm << "\nTexture Type: ";
    switch (iter->second.type)
    {
        case GL_TEXTURE_1D: resStrm << "GL_TEXTURE_1D"; break;
        case GL_TEXTURE_2D: resStrm << "GL_TEXTURE_2D"; break;
        case GL_TEXTURE_3D: resStrm << "GL_TEXTURE_3D"; break;
        case GL_TEXTURE_CUBE_MAP: resStrm << "GL_TEXTURE_CUBE_MAP"; break;
#ifdef GL_ARB_texture_rectangle //ARCH_DARWIN doesn't support all texture defines with all xcode version (xcode 5.0 seems to support all)
        case GL_TEXTURE_RECTANGLE_ARB: resStrm << "GL_TEXTURE_RECTANGLE"; break;
#endif
#ifdef GL_ARB_texture_buffer_object
        case GL_TEXTURE_BUFFER_ARB: resStrm << "GL_TEXTURE_BUFFER"; break;
#endif
#ifdef GL_EXT_texture_array
        case GL_TEXTURE_1D_ARRAY_EXT: resStrm << "GL_TEXTURE_1D_ARRAY"; break;
        case GL_TEXTURE_2D_ARRAY_EXT: resStrm << "GL_TEXTURE_2D_ARRAY"; break;
#endif
#ifdef GL_ARB_texture_cube_map_array
        case GL_TEXTURE_CUBE_MAP_ARRAY_ARB: resStrm << "GL_TEXTURE_CUBE_MAP_ARRAY"; break;
#endif
#ifdef GL_ARB_texture_multisample
        case GL_TEXTURE_2D_MULTISAMPLE: resStrm << "GL_TEXTURE_2D_MULTISAMPLE"; break;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: resStrm << "GL_TEXTURE_2D_MULTISAMPLE_ARRAY"; break;
#endif
        default: std::cerr << "Texture Type with number "<< iter->second.type << " on stage "<< iter->first << " is not supported "  << std::endl; break;
    }

    resStrm  << "\nShadowTexture: " <<  iter->second.shadow;
  }

  if (!vertexTemplateFile.isEmpty())
    resStrm << "\nshaderDesc.vertexTemplateFile: " << vertexTemplateFile;

  if (!tessControlTemplateFile.isEmpty())
    resStrm << "\nshaderDesc.tessControlTemplateFile: " << tessControlTemplateFile;

  if (!tessEvaluationTemplateFile.isEmpty())
    resStrm << "\nshaderDesc.tessEvaluationTemplateFile: " << tessEvaluationTemplateFile;

  if (!geometryTemplateFile.isEmpty())
    resStrm << "\nshaderDesc.geometryTemplateFile: " << geometryTemplateFile;

  if (!fragmentTemplateFile.isEmpty())
    resStrm << "\nshaderDesc.fragmentTemplateFile: " << fragmentTemplateFile;

  return res;
}



} // namespace ACG
//=============================================================================

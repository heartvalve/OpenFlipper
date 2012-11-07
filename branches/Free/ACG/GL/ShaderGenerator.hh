/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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

#pragma once

#include <QStringList>
#include <QString>
#include <string>
#include <list>
#include <ACG/Config/ACGDefines.hh>


namespace ACG
{


// #define MAX_SHADER_INPUT_UNITS 32
// #define MAX_SHADER_CONSTANTS 0x200
// #define MAX_SHADER_GEN_DEFINES 0x100
#define SG_MAX_SHADER_LIGHTS 8

enum ShaderGenLightType
{
  SG_LIGHT_DIRECTIONAL = 0,
  SG_LIGHT_POINT,
  SG_LIGHT_SPOT,
  SG_LIGHT_FORCE_DWORD = 0xFFFFFFFF
};

enum ShaderGenShadeMode
{
  SG_SHADE_UNLIT = 0,
  SG_SHADE_FLAT,
  SG_SHADE_GOURAUD,
  SG_SHADE_PHONG,
  SG_SHADE_FORCE_DWORD = 0xFFFFFFFF
};

struct ShaderGenDesc
{
  int numLights;
  ShaderGenLightType lightTypes[SG_MAX_SHADER_LIGHTS];

  ShaderGenShadeMode shadeMode;

  bool vertexColors;
  bool textured;

  // optionally specify shader template file names
  const char* vertexTemplateFile;
  const char* fragmentTemplateFile;


  /// convert ShaderGenDesc to string format for debugging
  QString toString() const;
};

/**
ShaderGenerator is used to collect shader io, uniforms, defines and includes.

The shader main-function is not generated here and must be provided
as a parameter to the buildShaderCode function.
*/

class ACGDLLEXPORT ShaderGenerator
{
public:

  ShaderGenerator();
  virtual ~ShaderGenerator();

  /** \brief Imports another shader, same as \#include
  */
  void addIncludeFile(QString _fileName);

  /** \brief Adds fitting vertex shader io for a given description
  */
  void initVertexShaderIO(const ShaderGenDesc* _desc);
  
  /** \brief Adds fitting fragment shader io for a given description
  */
  void initFragmentShaderIO(const ShaderGenDesc* _desc);


  /** \brief Adds frequently used uniform parameters
   *
   * Adds frequently used uniform parameters like:
   * - world, view, projection matrices
   * - cam pos, view dir
   * - per object material names: g_cDiffuse, g_cAmbient...
  */
  void initDefaultUniforms();

  /** \brief Add one GLSL input specifier
   *
   * Stores string pointer only
   * Example:
   * \code in vec4 inPosition; \endcode
  */
  void addInput(QString _input);

  /** \brief Add one GLSL output specifier
   *
   * Stores string pointer only
   * Example:
   * \code out vec4 inPosition; \endcode
   */
  void addOutput(QString _output);

  /** \brief Add one GLSL uniform specifier
   *
   * Stores string pointer only
   * Example:
   * \code uniform sampler2D sampAmbient; \endcode
   */
  void addUniform(QString _uniform);

  /** \brief Add one define
   *
   * Example:
   * \code #define SG_GOURAUD 1 \endcode
   */
  void addDefine(QString _uniform);


  /** \brief Add a light description to shader:
  */
  void addLight(int lightIndex_, ShaderGenLightType _light);

  /** \brief Shader assembly function
  */
  void buildShaderCode(QStringList* _pMainCode);

  /** \brief Get result of buildShaderCode
  */
  const QStringList& getShaderCode();

  /** \brief Save generated shader code to text file
   *
   * @param _fileName Where to save
  */
  void saveToFile(const char* _fileName);

private:

  /// aborts, if string already present
  /// prefix, postfix are very primitive, only checks for occurrence disregard locations
  void addStringToList(QString _str, QStringList* _list, const char* _prefix = 0, const char* _postfix = 0);

  /** Adds command lines to the shader code.
  Eventually appends missing ';'
  */
  void addIOToCode(const QStringList& _cmds);

  QStringList code_;


  /// glsl code imports (with \#include )
  QStringList imports_;

  QString version_;

  QStringList inputs_;
  QStringList outputs_;
  QStringList uniforms_;
  QStringList genDefines_;
};

/**
ShaderProgGenerator is responsible for generating a matching pair of vertex and fragment shaders.
*/

// A shader modifier can modify uniforms, in/outputs 
// and glsl code of vertex and fragment shaders.
// This is useful for global effects like shadow mapping
// and depth peeling, where only a little changes in code are necessary.

class ACGDLLEXPORT ShaderModifier
{
  friend class ShaderProgGenerator;

public:
  ShaderModifier(void);
  virtual ~ShaderModifier(void);

  virtual void modifyVertexIO(ShaderGenerator* _shader) {}
  virtual void modifyVertexBeginCode(QStringList* _code) {}
  virtual void modifyVertexEndCode(QStringList* _code) {}

  virtual void modifyFragmentIO(ShaderGenerator* _shader) {}
  virtual void modifyFragmentBeginCode(QStringList* _code) {}
  virtual void modifyFragmentEndCode(QStringList* _code) {}


  unsigned int getID() {return modifierID_;}

  operator unsigned int() const {return modifierID_;}

private:
  unsigned int modifierID_;
};

class ACGDLLEXPORT ShaderProgGenerator
{
public:

  /**
  The shader directory has to be defined first before making use of the generator!
  For example: setShaderDir(OpenFlipper::Options::shaderDirStr())
  */
  static void setShaderDir(QString _dir);

  ShaderProgGenerator(const ShaderGenDesc* _desc, unsigned int _modifierFlags = 0);
  virtual ~ShaderProgGenerator(void);


  void saveVertexShToFile(const char* _fileName);
  void saveFragmentShToFile(const char* _fileName);

  /** \brief Returns generated vertex shader code
  */
  const QStringList& getVertexShaderCode();

  /** \brief Returns generated fragment shader code
  */
  const QStringList& getFragmentShaderCode();



  static unsigned int registerModifier(ShaderModifier* _modifier);

private:

  /** \brief Loads external shader templates
  */
  void loadShaderTemplateFromFile();

  void generateShaders();


  void buildVertexShader();
  void buildFragmentShader();

  void addVertexBeginCode(QStringList* _code);
  void addVertexEndCode(QStringList* _code);

  void addFragmentBeginCode(QStringList* _code);
  void addFragmentEndCode(QStringList* _code);

  /** \brief Adds lighting definition functions
   *
   * LitPointLight() ...
   */
  void addLightingFunctions(QStringList* _code);

  /** \brief Adds lighting function calls to code
   */
  void addLightingCode(QStringList* _code);

  /// returns path to _strFileName without last slash
  QString getPathName(QString _strFileName);

  /// checks if _str is an include directive
  /// eventually imports the included file to the specified generator
  int checkForIncludes(QString _str, ShaderGenerator* _gen, QString _includePath);

  /// provide generated defines to shader
  void initGenDefines(ShaderGenerator* _gen);

  static void loadLightingFunctions();

  static void loadStringListFromFile(QString _fileName, QStringList* _out);

  ShaderGenerator* vertex_;
  ShaderGenerator* fragment_;

  QStringList vertexTemplate_;
  QStringList fragmentTemplate_;

  ShaderGenDesc   desc_;
  unsigned int usage_;


  /// registered shader modifier
  static int numModifiers_;
  static ShaderModifier* modifiers_[32];

  /// path + filename to shader templates
  QString vertexShaderFile_;
  QString fragmentShaderFile_;


  static QString shaderDir_;
  static QStringList lightingCode_;

  
};


//=============================================================================
} // namespace ACG
//=============================================================================

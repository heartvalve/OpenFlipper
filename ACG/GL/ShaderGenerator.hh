#pragma once

#include <QStringList>
#include <QString>
#include <string>
#include <list>

namespace ACG
{


// #define MAX_SHADER_INPUT_UNITS 32
// #define MAX_SHADER_CONSTANTS 0x200
// #define MAX_SHADER_GEN_DEFINES 0x100
#define MAX_SHADER_LIGHTS 8

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
  ShaderGenLightType lightTypes[MAX_SHADER_LIGHTS];

  ShaderGenShadeMode shadeMode;

  bool vertexColors;
  bool textured;

  // optionally specify shader template file names
  QString vertexTemplateFile;
  QString fragmentTemplateFile;
};

//typedef std::list<std::string> StringList;

/**
ShaderGenerator is used to collect shader io, uniforms, defines and includes.

The shader main-function is not generated here and must be provided
as a parameter to the buildShaderCode function.
*/

class ShaderGenerator
{
public:

  ShaderGenerator();
  virtual ~ShaderGenerator();

  /** 
  imports another shader, same as #include
  */
  void addIncludeFile(QString _fileName);

  /**
  adds fitting vertex shader io for a given description
  */
  void initVertexShaderIO(const ShaderGenDesc* _desc);
  
  /**
  adds fitting fragment shader io for a given description
  */
  void initFragmentShaderIO(const ShaderGenDesc* _desc);


  /**
  adds frequently used uniform parameters like:
  - world, view, projection matrices
  - cam pos, view dir
  - per object material names: g_cDiffuse, g_cAmbient...
  */
  void initDefaultUniforms();

  /**
  add one GLSL input specifier:
  stores string pointer only
  example: "in vec4 inPosition;"
  */
  void addInput(QString _input);

  /**
  add one GLSL output specifier:
  stores string pointer only
  example: "out vec4 outPosition;"
  */
  void addOutput(QString _output);

  /**
  add one GLSL uniform specifier
  stores string pointer only
  example: "uniform sampler2D sampAmbient;"
  */
  void addUniform(QString _uniform);

  /**
  add one define
  example: "#define SG_GOURAUD 1"
  */
  void addDefine(QString _uniform);


  /**
  add a light describtion to shader:
  */
  void addLight(int lightIndex_, ShaderGenLightType _light);



  /**
  shader assembly function
  */
  void buildShaderCode(QStringList* _pMainCode);

  /**
  get result of buildShaderCode
  */
  const QStringList& getShaderCode();

  /**
  save generated shader code to text file
  */
  void saveToFile(const char* _fileName);

private:

  /// aborts, if string already present
  /// prefix, postfix are very primitive, only checks for occurrence disregard locations
  void addStringToList(QString _str, QStringList* _list,
    const char* _prefix = 0, const char* _postfix = 0);

  /** Adds command lines to the shader code.
  Eventually appends missing ';'
  */
  void addIOToCode(const QStringList& _cmds);

  QStringList code_;


  // glsl code imports (with #include)
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

class ShaderProgGenerator
{
public:
  ShaderProgGenerator(const ShaderGenDesc* _desc);
  virtual ~ShaderProgGenerator(void);

  void saveVertexShToFile(const char* _fileName);
  void saveFragmentShToFile(const char* _fileName);

  /**
  returns generated vertex shader code
  */
  const QStringList& getVertexShaderCode();

  /**
  returns generated fragment shader code
  */
  const QStringList& getFragmentShaderCode();

private:

  /**
  loads external shader templates 
  */
  void loadShaderTemplateFromFile();

  void generateShaders();


  void buildVertexShader();
  void buildFragmentShader();

  void addVertexBeginCode(QStringList* _code);
  void addVertexEndCode(QStringList* _code);

  void addFragmentBeginCode(QStringList* _code);
  void addFragmentEndCode(QStringList* _code);

  /// adds lighting definition functions:
  /// LitPointLight() ...
  void addLightingFunctions(QStringList* _code);

  /// adds lighting function calls to code
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


  ShaderGenDesc desc_;


  /// path + filename to shader templates
  QString vertexShaderFile_;
  QString fragmentShaderFile_;


  static QStringList lightingCode_;
};




}
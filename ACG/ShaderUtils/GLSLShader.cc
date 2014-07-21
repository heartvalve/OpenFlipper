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

#include <ACG/GL/acg_glew.hh>

#include <QApplication>
#include <QDir>
#include <QString>
#include <QFileInfo>

#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <string>

#include <ACG/GL/gl.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/GLError.hh>

#include "GLSLShader.hh"
#include <ACG/GL/ShaderGenerator.hh>

#ifdef WIN32
  #ifndef __MINGW32__
    #define snprintf sprintf_s
  #endif 
#endif

//==============================================================================

namespace GLSL {

  //--------------------------------------------------------------------------
  // Generic shader
  //--------------------------------------------------------------------------

  /** \brief Creates a new shader.
  *
  * \param[in]  shaderType   Can be one of GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  */
  Shader::Shader(GLenum shaderType)
  : m_shaderId(0)
  {

    if ( !ACG::openGLVersion(2,0) ) {
      std::cerr << "Shaders not supported with OpenGL Version less than 2.0" << std::endl;
      return;
    }

    this->m_shaderId = glCreateShader(shaderType);
    if ( this->m_shaderId == 0 ) {
      std::cerr << "could not create shader" << std::endl;
    }

  }

  /** \brief Deletes the shader object.
  */
  Shader::~Shader() {
    if (this->m_shaderId) {
      glDeleteShader(m_shaderId);
    }
  }

  /** \brief Upload the source of the shader.
  */
  void Shader::setSource(StringList source)  {

    if ( this->m_shaderId == 0 ) {
      std::cerr << "shader not initialized" << std::endl;
      return;
    }

    const char **stringArray = new const char*[source.size()];

    int index = 0;
    for (StringList::const_iterator it = source.begin();it != source.end();++it) {
      stringArray[index] = (*it).c_str();
      ++index;
    }

    glShaderSource(this->m_shaderId, int(source.size()), stringArray, 0);

    delete[] stringArray;
  }


   /** \brief Upload the source of the shader.
  */
  void Shader::setSource(const QStringList& source)  {

    if ( this->m_shaderId == 0 ) {
      std::cerr << "shader not initialized" << std::endl;
      return;
    }

    StringList strlist;

    for (QStringList::const_iterator it = source.begin();it != source.end();++it)
      strlist.push_back(std::string((const char*)it->toLatin1()) + '\n');

    setSource(strlist);
  }

  /** \brief Compile the shader object.
  *
  * \returns True if compilation was successful, or false if it failed. Also
  * it prints out the shader source and the error message.
  */
  bool Shader::compile(bool verbose) {
    if ( this->m_shaderId == 0 ) {
      std::cerr << "shader not initialized" << std::endl;
      return false;
    }

    glCompileShader(this->m_shaderId);

    GLint compileStatus;
    glGetShaderiv(this->m_shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {

      if (verbose) {
        GLchar *errorLog = new GLchar[GLSL_MAX_LOGSIZE];
        GLsizei errorLength;
        char shaderSource[32768];
        glGetShaderSource(this->m_shaderId, 32768, &errorLength, shaderSource);
        std::cout << "shader source: " << std::endl << shaderSource << std::endl;
        glGetShaderInfoLog(this->m_shaderId, GLSL_MAX_LOGSIZE, &errorLength, errorLog);
        std::cout << "GLSL compile error:" << std::endl << errorLog << std::endl;
        delete[] errorLog;
      }

      return false;
    }
    return true;
  }

  //--------------------------------------------------------------------------
  // Vertex shader
  //--------------------------------------------------------------------------

  VertexShader::VertexShader() : Shader(GL_VERTEX_SHADER) {}
  VertexShader::~VertexShader() {}

  //--------------------------------------------------------------------------
  // Fragment shader
  //--------------------------------------------------------------------------

  FragmentShader::FragmentShader() : Shader(GL_FRAGMENT_SHADER) {}
  FragmentShader::~FragmentShader() {}

  //--------------------------------------------------------------------------
  // Geometry shader
  //--------------------------------------------------------------------------

  GeometryShader::GeometryShader() : Shader(GL_GEOMETRY_SHADER_EXT) {}
  GeometryShader::~GeometryShader() {}

  //--------------------------------------------------------------------------
  // Tessellation-Control shader
  //--------------------------------------------------------------------------

#ifdef GL_ARB_tessellation_shader

  TessControlShader::TessControlShader() : Shader(GL_TESS_CONTROL_SHADER) {}
  TessControlShader::~TessControlShader() {}

  //--------------------------------------------------------------------------
  // Tessellation-Evaluation shader
  //--------------------------------------------------------------------------

  TessEvaluationShader::TessEvaluationShader() : Shader(GL_TESS_EVALUATION_SHADER) {}
  TessEvaluationShader::~TessEvaluationShader() {}

#endif // GL_ARB_tessellation_shader

  //--------------------------------------------------------------------------
  // Shader program object
  //--------------------------------------------------------------------------

  /** \brief Creates a new GLSL program object.
  */
  Program::Program() : m_programId(0), m_linkStatus(GL_FALSE) {
    if ( !ACG::openGLVersion(2,0) ) {
      std::cerr << "Shaders not supported with OpenGL Version less than 2.0" << std::endl;
      return;
    }

    this->m_programId = glCreateProgram();

    if ( this->m_programId == 0 ) {
      std::cerr << "could not create GLSL program" << std::endl;
      return;
    }

  }

  /** \brief Deletes the GLSL program object and frees the linked shader objects.
  */
  Program::~Program() {
    if (this->m_programId) {
      glDeleteProgram(this->m_programId);
    }
    // free linked shaders
    this->m_linkedShaders.clear();
  }

  /** \brief Attaches a shader object to the program object.
  */
  void Program::attach(PtrConstShader _shader) {
    if ( this->m_programId == 0 ) {
      std::cerr << "attach invalid program" << std::endl;
      return;
    }

    if ( _shader->m_shaderId == 0 ) {
      std::cerr << "attach invalid shader" << std::endl;
      return;
    }

    glAttachShader(this->m_programId, _shader->m_shaderId);
    m_linkedShaders.push_back(_shader);
  }

  /** \brief Detaches a shader object from the program object.
  */
  void Program::detach(PtrConstShader _shader) {
    if ( this->m_programId == 0 ) {
      std::cerr << "detach invalid program" << std::endl;
      return;
    }

    if ( _shader->m_shaderId == 0 ) {
      std::cerr << "detach invalid shader" << std::endl;
      return;
    }

    glDetachShader(this->m_programId, _shader->m_shaderId);
    m_linkedShaders.remove(_shader);
  }

  /** \brief Links the shader objects to the program.
  */
  void Program::link() {
    glLinkProgram(this->m_programId);
    checkGLError2("link program failed");
    
    GLint status = GL_FALSE;
    glGetProgramiv(this->m_programId, GL_LINK_STATUS, &status);
    if ( !status ){
      GLint InfoLogLength = 0;
      glGetProgramiv(this->m_programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
      std::string errorlog(InfoLogLength,'\0');
      glGetProgramInfoLog(this->m_programId, InfoLogLength, NULL, &errorlog[0]);
      std::cerr << "program link error: " << errorlog << std::endl;
    }

    m_linkStatus = status;
  }

  /** \brief Enables the program object for using.
  */
  void Program::use() {
    if (m_linkStatus)
    {
      ACG::GLState::useProgram(this->m_programId);
      checkGLError2("use program failed");
    }
  }

  /** \brief Resets to standard rendering pipeline
  */
  void Program::disable() {
    ACG::GLState::useProgram(0);
    checkGLError2("shader disable failed");
  }

  /** \brief Returns if the program object is currently active.
  */
  bool Program::isActive() {
    GLint programId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
    return programId == this->m_programId;
  }

  /** \brief Returns if the program object has been succesfully linked.
  */
  bool Program::isLinked() {
    return m_linkStatus != GL_FALSE;
  }

  /** \brief Returns opengl id
  */
  GLuint Program::getProgramId() {
    return (GLuint)m_programId;
  }

  /** \brief Set int uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, GLint _value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform1i(location, _value);
    checkGLError2(_name);
  }

  /** \brief Set Vec2i uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec2i &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform2iv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec3i uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec3i &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform3iv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec4i uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec4i &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform4iv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set uint uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, GLuint _value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform1ui(location, _value);
    checkGLError2(_name);
  }

  /** \brief Set Vec2ui uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec2ui &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform2uiv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec3ui uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec3ui &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform3uiv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec4ui uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec4ui &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform4uiv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set float uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, GLfloat _value) {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform1f(location, _value);
    checkGLError2(_name);
  }

  /** \brief Set Vec2f uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec2f &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform2fv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec3f uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec3f &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform3fv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set Vec4f uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value New value of the uniform
   */
  void Program::setUniform(const char *_name, const ACG::Vec4f &_value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform4fv(location, 1, _value.data());
    checkGLError();
  }

  /** \brief Set int array uniform to specified values
   *
   *  @param _name Name of the uniform to be set
   *  @param _values Pointer to an array with the new values
   *  @param _count Number of values in the given array
   */
  void Program::setUniform(const char *_name, GLint *_values, int _count) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform1iv(location, _count, _values);
    checkGLError();
  }

  /** \brief Set float array uniform to specified values
   *
   *  @param _name Name of the uniform to be set
   *  @param _values Pointer to an array with the new values
   *  @param _count Number of values in the given array
   */
  void Program::setUniform(const char *_name, GLfloat *_values, int _count) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniform1fv(location, _count, _values);
    checkGLError();
  }

  /** \brief Set an entry of a bool uniform array
   *
   *  @param _name Name of the uniform to be set
   *  @param _index Entry in the array
   *  @param _value New value of the entry
   */
  void Program::setUniform(const char *_name, int _index, bool _value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", _name, _index);
    setUniform(varName, (GLint) _value);
  }

  /** \brief Set an entry of a int uniform array
   *
   *  @param _name Name of the uniform to be set
   *  @param _index Entry in the array
   *  @param _value New value of the entry
   */
  void Program::setUniform(const char *_name, int _index, int _value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", _name, _index);
    setUniform(varName, (GLint) _value);
  }

  /** \brief Set an entry of a float uniform array
   *
   *  @param _name Name of the uniform to be set
   *  @param _index Entry in the array
   *  @param _value New value of the entry
   */
  void Program::setUniform(const char *_name, int _index, float _value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", _name, _index);
    setUniform(varName, _value);
  }


  /** \brief Set 4x4fMatrix uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value Matrix to be set
   * @param _transposed Is the matrix transposed?
   */
  void Program::setUniform( const char *_name, const ACG::GLMatrixf &_value, bool _transposed){
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    glUniformMatrix4fv(location, 1, _transposed, _value.data());
    checkGLError();
  }

  /** \brief Set 3x3fMatrix uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value Matrix to be set
   * @param _transposed Is the matrix transposed?
   */
  void Program::setUniformMat3( const char *_name, const ACG::GLMatrixf &_value, bool _transposed){
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);

    float tmp[9];
    for (int i = 0; i < 3; ++i)
      for (int k = 0; k < 3; ++k)
        tmp[i*3+k] = _value.data()[i*4+k];

    glUniformMatrix3fv(location, 1, _transposed, tmp);
    checkGLError();
  }

  /** \brief Bind attribute to name
   *
   * @param _index Index of the attribute to be bound
   * @param _name  Name of the attribute
   */
  void Program::bindAttributeLocation(unsigned int _index, const char *_name) {
    glBindAttribLocation(this->m_programId, _index, _name);
    checkGLError2(_name);
  }

  /** \brief Bind fragment output data to name
   *
   * @param _index Index of the fragment output to be bound
   * @param _name  Name of the fragment output data
   */
  void Program::bindFragDataLocation(unsigned int _index, const char *_name) {
    glBindFragDataLocationEXT(this->m_programId, _index, _name);
    checkGLError2(_name);
  }

  /** \brief Get location of the specified attribute
   *
   * @param _name Name of the attribute
   * @return Attribute location
   */
  int Program::getAttributeLocation(const char *_name) {
    int attributeLocation = glGetAttribLocation(this->m_programId, _name);
    checkGLError2(_name);
    return attributeLocation;
  }

  /** \brief Get location of the specified uniform
   *
   * @param _name Name of the uniform
   * @return Attribute uniform
   */
  int Program::getUniformLocation(const char *_name) {
    int attributeLocation = glGetUniformLocation(this->m_programId, _name);
    checkGLError2(_name);
    return attributeLocation;
  }

  /** \brief Get location of the fragment data output
   *
   * @param _name Name of the fragment data output
   * @return Fragment data location
   */
  int Program::getFragDataLocation(const char *_name) {
    int attributeLocation = glGetFragDataLocationEXT(this->m_programId, _name);
    checkGLError2(_name);
    return attributeLocation;
  }

  /** \brief Set Type of Geometry
   *
   * valid input types: GL_POINTS, GL_LINES, GL_LINES_ADJACENCY_EXT, GL_TRIANGLES, GL_TRIANGLES_ADJACENCY_EXT
   *
   * @param _type Geometry type
   */
  void Program::setGeometryInputType(GLint _type) {
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_INPUT_TYPE_EXT, _type);
  }

  /** \brief Set output type of geometry
   *
   * valid output types: GL_POINTS, GL_LINE_STRIP, GL_TRIANGLE_STRIP
   *
   * @param _type Output geometry type
   */
  void Program::setGeometryOutputType(GLint _type) {
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_OUTPUT_TYPE_EXT, _type);
  }

  /** \brief Sets the maximum vertex output of the geometry shader
   *
   *  Query GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT to get the gpu limitation
   *
   * @param _numVerticesOut Maximal number of vertices
   */
  void Program::setGeometryVertexCount(GLint _numVerticesOut){
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_VERTICES_OUT_EXT, _numVerticesOut);
  }


  /** \brief Loads the shader source
  *
  * The shader is assumed to be placed in ../shader relative to the
  * executable's installation directory, if the path is a relativ
  * one. If it is determined that the path is absolute, the path is
  * taken as is.
  * Macros are inserted directly after the #version directive.
  * According to glsl spec, only comments and white space are allowed before #version.
  */
  GLSL::StringList loadShader(const char *filename, const GLSL::StringList *macros) {
    QString path_file;
    if (QDir(filename).isRelative()) {
      path_file = qApp->applicationDirPath() + QString("/../shader/")
                    + QString(filename);
    } else {
      path_file = QString::fromLatin1(filename);
    }
    std::ifstream iShader(path_file.toLatin1());
    if (!iShader) {
      std::cout << "ERROR: Could not open file " << path_file.toStdString() << std::endl;
      return GLSL::StringList();
    }

    GLSL::StringList shaderSource;

    bool foundVersionDirective = false;

    while (!iShader.eof()) {
      std::string strLine;
      std::getline(iShader, strLine);

      // check for includes
      QString qstrLine = strLine.c_str();
      if (qstrLine.contains("#include")) {

        // try to load included file
        QString strIncludeFile = qstrLine.remove("#include ").remove('\"').remove('<').remove('>').trimmed();
        QFileInfo loadedShaderFile(path_file);
        QString includePath = loadedShaderFile.absolutePath();

        if (strIncludeFile.isEmpty())
          std::cout << "wrong include syntax: " << strLine.c_str() << std::endl;
        else {
          QString fullPathToIncludeFile = includePath + QDir::separator() + strIncludeFile;

          std::ifstream iInclude(fullPathToIncludeFile.toLatin1());
          if (!iInclude) {
            std::cout << "ERROR: Could not open file " << fullPathToIncludeFile.toStdString() << std::endl;
          }  else {
            // append included file
            while (!iInclude.eof()) {
              std::getline(iInclude, strLine);
              strLine += "\n";
              shaderSource.push_back(strLine);
            }
          }
        }

        strLine = "";
      }
      else
      {
        if (macros && !foundVersionDirective && qstrLine.trimmed().startsWith("#version"))
        {
          foundVersionDirective = true;

          // add #version line to shader
          strLine += "\n";
          shaderSource.push_back(strLine);

          // insert preprocessor macros in the next line
          for (GLSL::StringList::const_iterator it = macros->begin(); it != macros->end(); ++it)
            shaderSource.push_back(*it + "\n");

          // prevent writing empty line
          continue;
        }
      }


      strLine += "\n";
      shaderSource.push_back(strLine);
    }
    iShader.close();


    if (macros && !foundVersionDirective)
    {
      // shader did not contain a #version directive
      // add preprocessor macros to beginning of shader
      for (GLSL::StringList::const_reverse_iterator it = macros->rbegin(); it != macros->rend(); ++it)
        shaderSource.push_front(*it + "\n");
    }


    return shaderSource;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrVertexShader loadVertexShader(const char *name, const GLSL::StringList *macros, bool verbose) {
    PtrVertexShader vertexShader = 0;
    StringList sourceVertex = loadShader(name, macros);

    if (!sourceVertex.empty() ) {
      vertexShader = new GLSL::VertexShader();
      vertexShader->setSource(sourceVertex);
      vertexShader->compile(verbose);
    }
    return vertexShader;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrFragmentShader loadFragmentShader(const char *name, const GLSL::StringList *macros, bool verbose) {
    PtrFragmentShader fragmentShader = 0;
    StringList sourceVertex = loadShader(name, macros);

    if ( !sourceVertex.empty() ) {
      fragmentShader = new GLSL::FragmentShader();
      fragmentShader->setSource(sourceVertex);
      if (!fragmentShader->compile(verbose)) {
        delete fragmentShader;
        fragmentShader = 0;
      }
    }
    return fragmentShader;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrGeometryShader loadGeometryShader(const char *name, const GLSL::StringList *macros, bool verbose) {
    PtrGeometryShader geometryShader = 0;
    StringList sourceVertex = loadShader(name, macros);

    if (!sourceVertex.empty()) {
      geometryShader = new GLSL::GeometryShader();
      geometryShader->setSource(sourceVertex);
      if (!geometryShader->compile(verbose)) {
        delete geometryShader;
        geometryShader = 0;
      }
    }
    return geometryShader;
  }


  /** \brief Loads, compiles and installs a new tessellation control shader.
  */
  GLSL::PtrShader loadTessControlShader(const char *name, const GLSL::StringList *macros, bool verbose) {
    GLSL::PtrShader shader = 0;
#ifdef GL_ARB_tessellation_shader
    StringList src = loadShader(name, macros);

    if (!src.empty()) {
      shader = new GLSL::TessControlShader();
      shader->setSource(src);
      if (!shader->compile(verbose)) {
        delete shader;
        shader = 0;
      }
    }
#endif // GL_ARB_tessellation_shader
    return shader;
  }

  /** \brief Loads, compiles and installs a new tessellation evaluation shader.
  */
  GLSL::PtrShader loadTessEvaluationShader(const char *name, const GLSL::StringList *macros, bool verbose) {
    GLSL::PtrShader shader = 0;
#ifdef GL_ARB_tessellation_shader
    StringList src = loadShader(name, macros);

    if (!src.empty()) {
      shader = new GLSL::TessEvaluationShader();
      shader->setSource(src);
      if (!shader->compile(verbose)) {
        delete shader;
        shader = 0;
      }
    }
#endif // GL_ARB_tessellation_shader
    return shader;
  }



  GLSL::PtrProgram loadProgram(const char *vertexShaderFile,
    const char *tessControlShaderFile,
    const char *tessEvaluationShaderFile,
    const char *geometryShaderFile,
    const char *fragmentShaderFile, 
    const GLSL::StringList *macros,
    bool verbose){

      GLSL::Program* result = 0;

      const int numShaders = 5;
      const char* ShaderFiles[numShaders] = {vertexShaderFile, tessControlShaderFile, tessEvaluationShaderFile, geometryShaderFile, fragmentShaderFile};
      GLSL::Shader* tempShaders[numShaders] = {0};

      for (int i = 0; i < numShaders; ++i) {
        QString shaderFile = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(ShaderFiles[i]);

        if (i == 0) // vertex shader
          tempShaders[i] = GLSL::loadVertexShader(shaderFile.toUtf8(), macros, verbose);
        else if (i == 1 && tessControlShaderFile) // tesscontrol shader
          tempShaders[i] = GLSL::loadTessControlShader(shaderFile.toUtf8(), macros, verbose);
        else if (i == 2 && tessEvaluationShaderFile) // tesseval shader
          tempShaders[i] = GLSL::loadTessEvaluationShader(shaderFile.toUtf8(), macros, verbose);
        else if (i == 3 && geometryShaderFile) // geometry shader
          tempShaders[i] = GLSL::loadGeometryShader(shaderFile.toUtf8(), macros, verbose);
        else if (i == 4) // fragment shader
          tempShaders[i] = GLSL::loadFragmentShader(shaderFile.toUtf8(), macros, verbose);

        if (!tempShaders[i] && ShaderFiles[i]) {
          if (verbose)
            std::cerr << ShaderFiles[i] << " could not be loaded and compiled" << std::endl;

          // avoid memleak
          for (int k = 0; k < numShaders; ++k)
            delete tempShaders[k];

          return 0;
        }
      }

      // create program

      result = new GLSL::Program();
      for (int i = 0; i < numShaders; ++i)
        if (tempShaders[i])
          result->attach(tempShaders[i]);
      result->link();

      for (int i = 0; i < numShaders; ++i)
        delete tempShaders[i];

      if (verbose)
        ACG::glCheckErrors();


      return result;
  }

  GLSL::PtrProgram loadProgram(const char *vertexShaderFile, const char *geometryShaderFile, const char *fragmentShaderFile, const GLSL::StringList *macros, bool verbose){

    return loadProgram(vertexShaderFile, 0, 0, geometryShaderFile, fragmentShaderFile, macros, verbose);
  }


  GLSL::PtrProgram loadProgram(const char *vertexShaderFile, const char *fragmentShaderFile, const GLSL::StringList *macros, bool verbose){
    return loadProgram(vertexShaderFile, 0, fragmentShaderFile, macros, verbose);
  }
}


//==============================================================================

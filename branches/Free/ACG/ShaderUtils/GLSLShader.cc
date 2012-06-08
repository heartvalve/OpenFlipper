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

/*******************************************************************************
 * GLSLShader.hh
 *
 * Utility classes for GLSL shaders.
 *
 * Lehrstuhl I8 RWTH-Aachen, http://www-i8.informatik.rwth-aachen.de
 *
 ******************************************************************************/

//==============================================================================

#include <QApplication>
#include <QDir>
#include <QString>

#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>

#include <ACG/GL/gl.hh>
#include <ACG/GL/GLState.hh>

#include "gldebug.h"
#include "GLSLShader.hh"

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

    glShaderSource(this->m_shaderId, source.size(), stringArray, 0);

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
      strlist.push_back(std::string((const char*)it->toAscii()) + '\n');

    setSource(strlist);
  }

  /** \brief Compile the shader object.
  *
  * \returns True if compilation was successful, or false if it failed. Also
  * it prints out the shader source and the error message.
  */
  bool Shader::compile() {
    if ( this->m_shaderId == 0 ) {
      std::cerr << "shader not initialized" << std::endl;
      return false;
    }

    glCompileShader(this->m_shaderId);

    GLint compileStatus;
    glGetShaderiv(this->m_shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
      GLchar *errorLog = new GLchar[GLSL_MAX_LOGSIZE];
      GLsizei errorLength;
      char shaderSource[32768];
      glGetShaderSource(this->m_shaderId, 32768, &errorLength, shaderSource);
      std::cout << "shader source: " << std::endl << shaderSource << std::endl;
      glGetShaderInfoLog(this->m_shaderId, GLSL_MAX_LOGSIZE, &errorLength, errorLog);
      std::cout << "GLSL compile error:" << std::endl << errorLog << std::endl;
      delete[] errorLog;

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
  // Shader program object
  //--------------------------------------------------------------------------

  /** \brief Creates a new GLSL program object.
  */
  Program::Program() {
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
  void Program::attach(PtrConstShader shader) {
    if ( this->m_programId == 0 ) {
      std::cerr << "attach invalid program" << std::endl;
      return;
    }

    if ( shader->m_shaderId == 0 ) {
      std::cerr << "attach invalid shader" << std::endl;
      return;
    }

    glAttachShader(this->m_programId, shader->m_shaderId);
    m_linkedShaders.push_back(shader);
  }

  /** \brief Detaches a shader object from the program object.
  */
  void Program::detach(PtrConstShader shader) {
    if ( this->m_programId == 0 ) {
      std::cerr << "detach invalid program" << std::endl;
      return;
    }

    if ( shader->m_shaderId == 0 ) {
      std::cerr << "detach invalid shader" << std::endl;
      return;
    }

    glDetachShader(this->m_programId, shader->m_shaderId);
    m_linkedShaders.remove(shader);
  }

  /** \brief Links the shader objects to the program.
  */
  void Program::link() {
    glLinkProgram(this->m_programId);
    checkGLError2("link program failed");
  }

  /** \brief Enables the program object for using.
  */
  void Program::use() {
    ACG::GLState::useProgram(this->m_programId);
    checkGLError2("use program failed");
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

  /** \brief Sets a uniform variable for the program.
  */
  void Program::setUniform(const char *name, GLint value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform1i(location, value);
    checkGLError2(name);
  }

  /** \brief Sets a uniform variable for the program.
  */
  void Program::setUniform(const char *name, GLfloat value) {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform1f(location, value);
    checkGLError2(name);
  }

  /** \brief Sets a uniform variable for the program.
  */
  void Program::setUniform(const char *name, const ACG::Vec2f &value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform2fv(location, 1, value.data());
    checkGLError();
  }

  /** \brief Sets a uniform variable for the program.
  */
  void Program::setUniform(const char *name, const ACG::Vec3f &value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform3fv(location, 1, value.data());
    checkGLError();
  }

  /** \brief Sets a uniform variable for the program.
  */
  void Program::setUniform(const char *name, const ACG::Vec4f &value) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform4fv(location, 1, value.data());
    checkGLError();
  }

  void Program::setUniform(const char *name, GLint *values, int count) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform1iv(location, count, values);
    checkGLError();
  }

  void Program::setUniform(const char *name, GLfloat *values, int count) {
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniform1fv(location, count, values);
    checkGLError();
  }

  void Program::setUniform(const char *name, int index, bool value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", name, index);
    setUniform(varName, (GLint) value);
  }

  void Program::setUniform(const char *name, int index, int value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", name, index);
    setUniform(varName, (GLint) value);
  }

  void Program::setUniform(const char *name, int index, float value) {
    char varName[1024];
    snprintf(varName, 1024, "%s[%d]", name, index);
    setUniform(varName, value);
  }

  void Program::setUniform( const char *name, const ACG::GLMatrixf &value, bool transposed){
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    glUniformMatrix4fv(location, 1, transposed, value.data());
    checkGLError();
  }

  void Program::setUniformMat3( const char *name, const ACG::GLMatrixf &value, bool transposed){
    checkGLError();
    GLint location = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);

    float tmp[9];
    for (int i = 0; i < 3; ++i)
      for (int k = 0; k < 3; ++k)
        tmp[i*3+k] = value.data()[i*4+k];

    glUniformMatrix3fv(location, 1, transposed, tmp);
    checkGLError();
  }

  void Program::bindAttributeLocation(unsigned int index, const char *name) {
    glBindAttribLocation(this->m_programId, index, name);
    checkGLError2(name);
  }

  int Program::getAttributeLocation(const char *name) {
    int attributeLocation = glGetAttribLocation(this->m_programId, name);
    checkGLError2(name);
    return attributeLocation;
  }

  int Program::getUniformLocation(const char *name) {
    int attributeLocation = glGetUniformLocation(this->m_programId, name);
    checkGLError2(name);
    return attributeLocation;
  }

  void Program::setGeometryInputType(GLint type) {
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_INPUT_TYPE_EXT, type);
  }

  void Program::setGeometryOutputType(GLint type) {
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_OUTPUT_TYPE_EXT, type);
  }

  void Program::setGeometryVertexCount(GLint numVerticesOut){
    glProgramParameteriEXT(this->m_programId, GL_GEOMETRY_VERTICES_OUT_EXT, numVerticesOut);
  }


  /** \brief Loads the shader source
  *
  * The shader is assumed to be placed in ../shader relative to the
  * executable's installation directory, if the path is a relativ
  * one. If it is determined that the path is absolute, the path is
  * taken as is.
  */
  GLSL::StringList loadShader(const char *filename) {
    QString path_file;
    if (QDir(filename).isRelative()) {
      path_file = qApp->applicationDirPath() + QString("/../shader/")
                    + QString(filename);
    } else {
      path_file = QString::fromAscii(filename);
    }
    std::ifstream iShader(path_file.toAscii());
    if (!iShader) {
      std::cout << "ERROR: Could not open file " << path_file.toStdString() << std::endl;
      return GLSL::StringList();
    }

    GLSL::StringList shaderSource;

    while (!iShader.eof()) {
      std::string strLine;
      std::getline(iShader, strLine);
      strLine += "\n";
      shaderSource.push_back(strLine);
    }
    iShader.close();
    return shaderSource;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrVertexShader loadVertexShader(const char *name) {
    PtrVertexShader vertexShader = 0;
    StringList sourceVertex = loadShader(name);

    if (!sourceVertex.empty() ) {
      vertexShader = GLSL::PtrVertexShader(new GLSL::VertexShader());
      vertexShader->setSource(sourceVertex);
      vertexShader->compile();
    }
    return vertexShader;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrFragmentShader loadFragmentShader(const char *name) {
    PtrFragmentShader fragmentShader = 0;
    StringList sourceVertex = loadShader(name);

    if ( !sourceVertex.empty() ) {
      fragmentShader = GLSL::PtrFragmentShader(new GLSL::FragmentShader());
      fragmentShader->setSource(sourceVertex);
      fragmentShader->compile();
    }
    return fragmentShader;
  }

  /** \brief Loads, compiles and installs a new vertex shader.
  */
  GLSL::PtrGeometryShader loadGeometryShader(const char *name) {
    PtrGeometryShader geometryShader = 0;
    StringList sourceVertex = loadShader(name);

    if (!sourceVertex.empty()) {
      geometryShader = GLSL::PtrGeometryShader(new GLSL::GeometryShader());
      geometryShader->setSource(sourceVertex);
      geometryShader->compile();
    }
    return geometryShader;
  }

}


//==============================================================================

// Local Variables:
// mode: C++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:

//==============================================================================

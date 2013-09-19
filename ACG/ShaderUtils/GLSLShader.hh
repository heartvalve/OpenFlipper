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

#ifndef GLSLSHADER_H
#define GLSLSHADER_H

//==============================================================================

#include <ACG/Config/ACGDefines.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/GL/gl.hh>

#include <list>
#include <string>
#include <QStringList>

//==============================================================================

/** \brief This namespace contains all the classes and functions for handling
 * GLSL shader and program objects.
 */
namespace GLSL {

#define GLSL_MAX_LOGSIZE 16384

  typedef std::list<std::string> StringList;

  /** \brief A generic shader base class
  */
  class ACGDLLEXPORT Shader {

    public:
      Shader(GLenum shaderType);
      virtual ~Shader();
      void setSource(StringList source);
      void setSource(const QStringList& source);

      // FIXME implement StringList getSource();
      bool compile();

    protected:
      GLuint m_shaderId;

      friend class Program;
  };

  typedef Shader* PtrShader;
  typedef const Shader* PtrConstShader;

  //--------------------------------------------------------------------------

  /** \brief GLSL vertex shader.
  */
  class ACGDLLEXPORT VertexShader : public Shader {

    public:
      VertexShader();
      virtual ~VertexShader();

  };

  typedef VertexShader* PtrVertexShader;
  typedef const VertexShader* PtrVertexConstShader;

  //--------------------------------------------------------------------------

  /** \brief GLSL fragment shader.
  */
  class ACGDLLEXPORT FragmentShader : public Shader {

    public:
      FragmentShader();
      virtual ~FragmentShader();
  };

  typedef FragmentShader* PtrFragmentShader;
  typedef const FragmentShader* PtrConstFragmentShader;

  //--------------------------------------------------------------------------

  /** \brief GLSL geometry shader.
  */
  class ACGDLLEXPORT GeometryShader : public Shader {

    public:
      GeometryShader();
      virtual ~GeometryShader();
  };

  typedef GeometryShader* PtrGeometryShader;
  typedef const GeometryShader* PtrConstGeometryShader;

  //--------------------------------------------------------------------------

  /** \brief GLSL program class.
  *
  * A GLSL program links together the vertex and fragment shaders.
  */
  class ACGDLLEXPORT Program {

    public:
      Program();
      virtual ~Program();



      //===========================================================================
      /** @name Compilation/Linking
       *
       * @{ */
      //===========================================================================

      void attach(PtrConstShader _shader);
      void detach(PtrConstShader _shader);
      void link();

      /** @} */

      //===========================================================================
      /** @name Localizations
       *
       * @{ */
      //===========================================================================

      int getAttributeLocation(const char *_name);
      int getUniformLocation(const char *_name);
      int getFragDataLocation(const char* _name);

      void bindAttributeLocation(unsigned int _index, const char *_name);
      void bindFragDataLocation(unsigned int _index, const char *_name);

      /** @} */

      //===========================================================================
       /** @name Uniform setters
        *
        * @{ */
       //===========================================================================

       void setUniform(const char *_name, GLint _value);
       void setUniform(const char *_name, GLfloat _value);
       void setUniform(const char *_name, const ACG::Vec2f &_value);
       void setUniform(const char *_name, const ACG::Vec3f &_value);
       void setUniform(const char *_name, const ACG::Vec4f &_value);


       void setUniform(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);
       void setUniformMat3(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);



       void setUniform(const char *_name, GLint *_values, int _count);
       void setUniform(const char *_name, GLfloat *_values, int _count);
       void setUniform(const char *_name, int _index, bool _value);

       void setUniform(const char *_name, int _index, int _value);
       void setUniform(const char *_name, int _index, float _value);

       /** @} */

      //===========================================================================
       /** @name Geometry shader parameters
        *
        * @{ */
      //===========================================================================

      void setGeometryInputType(GLint _type);
      void setGeometryOutputType(GLint _type);
      void setGeometryVertexCount(GLint _numVerticesOut);

      /** @} */

      //===========================================================================
      /** @name Enable/disable functions
       *
       * @{ */
      //===========================================================================

      void use();
      void disable();
      bool isActive();

      /** @} */

      GLuint getProgramId();

    private:

      std::list<PtrConstShader> m_linkedShaders;
      GLint m_programId;
  };

  typedef Program* PtrProgram;
  typedef const Program* PtrConstProgram;

  //--------------------------------------------------------------------------

  GLSL::StringList ACGDLLEXPORT loadShader(const char *filename);

  GLSL::PtrVertexShader ACGDLLEXPORT loadVertexShader(const char *name);
  GLSL::PtrFragmentShader ACGDLLEXPORT loadFragmentShader(const char *name);
  GLSL::PtrGeometryShader ACGDLLEXPORT loadGeometryShader(const char *name);


  /** load shaders and create GLSL program if successful
   *
   * Shader file paths for this function are assumed to be relative
   * to the "Shader" directory as specified in   ShaderProgGenerator::getShaderDir()
  */
  GLSL::PtrProgram ACGDLLEXPORT loadProgram(const char *vertexShaderFile,
                                          const char *fragmentShaderFile);

}

#endif // GLSLSHADER_H

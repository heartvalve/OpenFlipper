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
 * Lehrstuhl I8 RWTH-Aachen, http://www.graphics.rwth-aachen.de
 *
 ******************************************************************************/

#ifndef GLSLSHADER_H
#define GLSLSHADER_H

//==============================================================================

#include "../Config/ACGDefines.hh"
#include <ACG/Math/VectorT.hh>
#include <ACG/Math/GLMatrixT.hh>

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

      void attach(PtrConstShader shader);
      void detach(PtrConstShader shader);
      void link();

      void setUniform(const char *name, GLint value);
      void setUniform(const char *name, GLfloat value);
      void setUniform(const char *name, const ACG::Vec2f &value);
      void setUniform(const char *name, const ACG::Vec3f &value);
      void setUniform(const char *name, const ACG::Vec4f &value);

      void setUniform(const char *name, const ACG::GLMatrixf &value, bool transposed = false);
      void setUniformMat3(const char *name, const ACG::GLMatrixf &value, bool transposed = false);

      void setUniform(const char *name, GLint *value, int count);
      void setUniform(const char *name, GLfloat *value, int count);

      // set element of array
      void setUniform(const char *name, int index, bool value);
      void setUniform(const char *name, int index, int value);
      void setUniform(const char *name, int index, float value);

      void bindAttributeLocation(unsigned int index, const char *name);
      int getAttributeLocation(const char *name);
      int getUniformLocation(const char *name);

      // geometry shader params

      // valid input types: GL_POINTS, GL_LINES, GL_LINES_ADJACENCY_EXT, GL_TRIANGLES, GL_TRIANGLES_ADJACENCY_EXT
      void setGeometryInputType(GLint type);
      
      // valid output types: GL_POINTS, GL_LINE_STRIP, GL_TRIANGLE_STRIP
      void setGeometryOutputType(GLint type);

      // Sets the maximum vertex output of the geometry shader. 
      //  query GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT to get the gpu limitation
      void setGeometryVertexCount(GLint numVerticesOut);


      void use();

      void disable();

      bool isActive();

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

}


//==============================================================================

// Local Variables:
// mode: C++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:

//==============================================================================

// GLSLSHADER_H
#endif

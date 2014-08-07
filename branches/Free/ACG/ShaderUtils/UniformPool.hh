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
 *   $Revision: 17546 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2013-09-17 10:04:29 +0200 (Di, 17. Sep 2013) $                   *
 *                                                                           *
\*===========================================================================*/

#pragma once

//==============================================================================

#include <ACG/Config/ACGDefines.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <list>
#include <string>


//==============================================================================

namespace GLSL {

  /** \brief GLSL uniform pool
  *
  * A uniform pool collects values for shader uniforms
  */
  class ACGDLLEXPORT UniformPool {

  public:
    /** \brief Constructor
    */
    UniformPool();

    /** \brief Copy Constructor
    */
    UniformPool(const UniformPool& _pool);

    virtual ~UniformPool();

    void bind(PtrProgram _prog) const;
    void bind(GLuint _prog) const;

    void setUniform(const char *_name, GLint _value);
    void setUniform(const char *_name, const ACG::Vec2i &_value);
    void setUniform(const char *_name, const ACG::Vec3i &_value);
    void setUniform(const char *_name, const ACG::Vec4i &_value);

    void setUniform(const char *_name, GLuint _value);
    void setUniform(const char *_name, const ACG::Vec2ui &_value);
    void setUniform(const char *_name, const ACG::Vec3ui &_value);
    void setUniform(const char *_name, const ACG::Vec4ui &_value);

    void setUniform(const char *_name, GLfloat _value);
    void setUniform(const char *_name, const ACG::Vec2f &_value);
    void setUniform(const char *_name, const ACG::Vec3f &_value);
    void setUniform(const char *_name, const ACG::Vec4f &_value);


    void setUniform(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);
    void setUniformMat3(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);


    void setUniform(const char *_name, GLint *_values, int _count);
    void setUniform(const char *_name, GLfloat *_values, int _count);


    void addPool(const UniformPool& _src);

    /** \brief Clear the pool
     *
     */
    void clear();

    /** \brief returns if the pool is empty
     *
     * @return empty pool?
     */
    bool empty() const;

  private:
    struct UniformBase {
      std::string id;


      UniformBase() {}
      virtual ~UniformBase() {}

      virtual void bind(GLuint _progID) const {}
    };

    struct UniformVecf : public UniformBase {
      ACG::Vec4f val;
      int size;

      void bind(GLuint _progID) const;
    };

    // separate float int vector because sizeof(int) != sizeof(float) for some compilers
    struct UniformVeci : public UniformBase {
      ACG::Vec4i val;
      int size;

      void bind(GLuint _progID) const;
    };

    struct UniformVecui : public UniformBase {
      ACG::Vec4ui val;
      int size;

      void bind(GLuint _progID) const;
    };


    struct UniformMat : UniformBase {
      ACG::Matrix4x4f val;

      bool transposed;
      int size;

      void bind(GLuint _progID) const;
    };

    struct UniformBuf : public UniformBase {
      float* val;
      
      bool integer;
      int size;
      
      UniformBuf();
      ~UniformBuf();

      void bind(GLuint _progID) const;
    };


    typedef std::list<UniformBase*> UniformList;
    typedef UniformList::iterator UniformListIt;
    
    /// list of uniform params
    UniformList pool_;

  private:

    UniformListIt findEntry(std::string _name);

    void addVecf(const UniformVecf& _vec);
    void addVeci(const UniformVeci& _vec);
    void addVecui(const UniformVecui& _vec);
    void addMatrix(const UniformMat& _mat);
    void addBuf(const char *_name, void *_values, int _count, bool _integer);
  };

  typedef UniformPool* PtrUniformPool;
  typedef const UniformPool* PtrConstUniformPool;

}


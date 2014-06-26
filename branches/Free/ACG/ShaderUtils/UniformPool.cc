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

#include <iostream>

#include <GL/glew.h>
#include <ACG/GL/GLError.hh>
#include "UniformPool.hh"

#ifdef WIN32
  #ifndef __MINGW32__
    #define snprintf sprintf_s
  #endif 
#endif

//==============================================================================

namespace GLSL {

  //--------------------------------------------------------------------------
  // Uniform Pool
  //--------------------------------------------------------------------------


  UniformPool::UniformPool(){
  }


  UniformPool::UniformPool(const UniformPool& _pool) {
    addPool(_pool);
  }


  /** \brief Destructor
  */
  UniformPool::~UniformPool(){
    clear();
  }

  void UniformPool::clear() {
    // Delete the uniforms in that pool
    for (UniformListIt it = pool_.begin(); it != pool_.end(); ++it)
      delete (*it);

    // Clear the pool
    pool_.clear();
  }

  bool UniformPool::empty() const {
    return pool_.empty();
  }

  /** \brief Send all stored uniforms to program
   *
   *  @param _prog receiving GLSL program 
   */
  void UniformPool::bind( PtrProgram _prog ) const {
    bind(_prog->getProgramId());
  }

  /** \brief Send all stored uniforms to program
   *
   *  @param _prog opengl program id
   */
  void UniformPool::bind( GLuint _prog ) const {
    for (UniformList::const_iterator it = pool_.begin(); it != pool_.end(); ++it) {
      (*it)->bind(_prog);
    }
  }

  /** \brief Search the pool for an existing value for a uniform name
  *
  * @param _name  Name of the uniform
  * @return iterator of uniform entry
  */
  UniformPool::UniformListIt UniformPool::findEntry( std::string _name ) {

    for (UniformListIt it = pool_.begin(); it != pool_.end(); ++it){
      if ((*it)->id.compare(_name) == 0)
        return it;
    }

    return pool_.end();
  }

  /** \brief Add all uniforms of a pool to this pool
   *
   *  @param _src source uniform pool
   */
  void UniformPool::addPool( const UniformPool& _src ){

    for (UniformList::const_iterator it = _src.pool_.begin(); it != _src.pool_.end(); ++it){

      // determine type
      const UniformVecf* pVecf = dynamic_cast<const UniformVecf*>(*it);
      const UniformVeci* pVeci = dynamic_cast<const UniformVeci*>(*it);
      const UniformMat* pMat = dynamic_cast<const UniformMat*>(*it);
      const UniformBuf* pBuf = dynamic_cast<const UniformBuf*>(*it);

      // add to our list
      if (pVecf)
        addVecf(*pVecf);

      if (pVeci)
        addVeci(*pVeci);

      else if (pMat)
        addMatrix(*pMat);

      else if (pBuf)
        addBuf(pBuf->id.c_str(), pBuf->val, pBuf->size, pBuf->integer);
    }
  }


  /** \brief Bind uniform float vector to shader
  *
  * @param _progID  GL Program ID
  */
  void UniformPool::UniformVecf::bind( GLuint _progID ) const {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(_progID, id.c_str());
    checkGLError2(id.c_str());

    switch (size){
      case 1:
        glUniform1fv(location, 1, val.data());
        break;
      case 2:
        glUniform2fv(location, 1, val.data());
        break;
      case 3:
        glUniform3fv(location, 1, val.data());
        break;
      case 4:
        glUniform4fv(location, 1, val.data());
        break;

      default:
        std::cerr << "UniformPool::UniformVecf : invalid size "  << size << std::endl;
        break;
    }

    checkGLError2(id.c_str());
  }

  /** \brief Bind uniform int vector to shader
  *
  * @param _progID  GL Program ID
  */
  void UniformPool::UniformVeci::bind( GLuint _progID ) const {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(_progID, id.c_str());
    checkGLError2(id.c_str());

    switch (size){
        case 1:
          glUniform1iv(location, 1, (GLint*)val.data());
          break;
        case 2:
          glUniform2iv(location, 1, (GLint*)val.data());
          break;
        case 3:
          glUniform3iv(location, 1, (GLint*)val.data());
          break;
        case 4:
          glUniform4iv(location, 1, (GLint*)val.data());
          break;

        default:
          std::cerr << "UniformPool::UniformVeci : invalid size "  << size << std::endl;
          break;
    }

    checkGLError2(id.c_str());
  }

  /** \brief Bind uniform matrix to shader
  *
  * @param _progID  GL Program ID
  */
  void UniformPool::UniformMat::bind( GLuint _progID ) const {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(_progID, id.c_str());
    checkGLError2(id.c_str());

    switch (size){
      case 2: {
          float tmp[4];
          for (int i = 0; i < 2; ++i)
            for (int k = 0; k < 2; ++k)
              tmp[i*2+k] = val.data()[i*4+k];
          glUniformMatrix2fv(location, 1, transposed, tmp);
        } break;

      case 3: {
        float tmp[9];
        for (int i = 0; i < 3; ++i)
          for (int k = 0; k < 3; ++k)
            tmp[i*3+k] = val.data()[i*4+k];
          glUniformMatrix3fv(location, 1, transposed, tmp);
        } break;

      case 4: glUniformMatrix4fv(location, 1, transposed, val.data()); break;

      default:
        std::cerr << "UniformPool::UniformMat : invalid size "  << size << std::endl;
        break;
    }

    checkGLError2(id.c_str());
  }

  /** \brief Bind uniform array to shader
  *
  * @param _progID  GL Program ID
  */
  void UniformPool::UniformBuf::bind( GLuint _progID ) const {
    checkGLError2("prev opengl error");
    GLint location = glGetUniformLocation(_progID, id.c_str());
    checkGLError2(id.c_str());

    if (integer){
      glUniform1iv(location, size, (GLint*)val);
    }
    else{
      glUniform1fv(location, size, val);
    }

    checkGLError2(id.c_str());
  }


  /** \brief Creates a copy of input data
  */
  UniformPool::UniformBuf::UniformBuf()
    : val(0), integer(false), size(0)
  {
  }

  /** \brief Free data
  */
  UniformPool::UniformBuf::~UniformBuf() {
    delete [] val;
  }

  
  /** \brief Add or update a vector type uniform in pool
  *
  * @param _vec uniform specifier
  */
  void UniformPool::addVecf( const UniformVecf& _vec ) {
    // look for existing uniform in pool
    UniformListIt it = findEntry(_vec.id);

    // storage address of uniform
    UniformVecf* dst = 0;

    if ( it == pool_.end() ){
      // create new entry
      dst = new UniformVecf;
      pool_.push_back(dst);
    }
    else{
      // use existing entry
      dst = dynamic_cast<UniformVecf*>( *it );

      if (!dst)
        std::cerr << "UniformPool::addVecf type of " << _vec.id << " incorrect." << std::endl;
    }

    if (dst) {
      // update data
      dst->id = _vec.id;
      dst->size = _vec.size;
      dst->val = _vec.val;
    }

  }

  /** \brief Add or update a vector type uniform in pool
  *
  * @param _vec uniform specifier
  */
  void UniformPool::addVeci( const UniformVeci& _vec ) {
    // look for existing uniform in pool
    UniformListIt it = findEntry(_vec.id);

    // storage address of uniform
    UniformVeci* dst = 0;

    if ( it == pool_.end() ){
      // create new entry
      dst = new UniformVeci;
      pool_.push_back(dst);
    }
    else{
      // use existing entry
      dst = dynamic_cast<UniformVeci*>( *it );

      if (!dst)
        std::cerr << "UniformPool::addVeci type of " << _vec.id << " incorrect." << std::endl;
    }

    if (dst) {
      // update data
      dst->id = _vec.id;
      dst->size = _vec.size;
      dst->val = _vec.val;
    }

  }

  /** \brief Add or update a matrix type uniform in pool
  *
  * @param _mat uniform specifier
  */
  void UniformPool::addMatrix( const UniformMat& _mat ) {
    // look for existing uniform in pool
    UniformListIt it = findEntry(_mat.id);

    // storage address of uniform
    UniformMat* dst = 0;

    if ( it == pool_.end() ){
      // create new entry
      dst = new UniformMat;
      pool_.push_back(dst);
    }
    else{
      // use existing entry
      dst = dynamic_cast<UniformMat*>( *it );

      if (!dst)
        std::cerr << "UniformPool::addMatrix type of " << _mat.id << " incorrect." << std::endl;
    }

    if (dst) {
      // update data
      dst->id = _mat.id;
      dst->size = _mat.size;
      dst->transposed = _mat.transposed;
      dst->val = _mat.val;
    }
  }

  /** \brief Add or update an array type uniform in pool
  *
  * @param _name Uniform name
  * @param _values array data
  * @param _count array size (in dwords)
  * @param _integer integer/float array
  */
  void UniformPool::addBuf( const char* _name, void* _values, int _count, bool _integer ) {
    // look for existing uniform in pool
    UniformListIt it = findEntry(_name);

    // storage address of uniform
    UniformBuf* dst = 0;

    if ( it == pool_.end() ){
      // create new entry
      dst = new UniformBuf();
      pool_.push_back(dst);
    }
    else{
      // use existing entry
      dst = dynamic_cast<UniformBuf*>( *it );

      if (!dst)
        std::cerr << "UniformPool::addBuf type of " << _name << " incorrect." << std::endl;
    }

    if (dst) {
      // update data
      dst->id = _name;

      if (dst->size < _count)
      {
        // resize
        delete [] dst->val;
        dst->val = new float[_count];
      }

      dst->size = _count;

      if (_values)
        memcpy(dst->val, _values, _count * sizeof(float));
    }
  }


  /** \brief Set int uniform to specified value
  *
  * @param _name  Name of the uniform
  * @param _value New value of the uniform
  */
  void UniformPool::setUniform( const char *_name, GLint _value ) {
    // create uniform descriptor
    UniformVeci tmp;
    tmp.id = _name;
    tmp.size = 1;
    tmp.val[0] = _value;

    // add/update in pool
    addVeci(tmp);
  }

  /** \brief Set float uniform to specified value
  *
  * @param _name  Name of the uniform
  * @param _value New value of the uniform
  */
  void UniformPool::setUniform( const char *_name, GLfloat _value ) {
    // create uniform descriptor
    UniformVecf tmp;
    tmp.id = _name;
    tmp.size = 1;
    tmp.val[0] = _value;

    // add/update in pool
    addVecf(tmp);
  }

  /** \brief Set vec2 uniform to specified value
  *
  * @param _name  Name of the uniform
  * @param _value New value of the uniform
  */
  void UniformPool::setUniform( const char *_name, const ACG::Vec2f &_value ) {
    // create uniform descriptor
    UniformVecf tmp;
    tmp.id = _name;
    tmp.size = 2;
    tmp.val[0] = _value[0];
    tmp.val[1] = _value[1];

    // add/update in pool
    addVecf(tmp);
  }

  /** \brief Set vec3 uniform to specified value
  *
  * @param _name  Name of the uniform
  * @param _value New value of the uniform
  */
  void UniformPool::setUniform( const char *_name, const ACG::Vec3f &_value ) {
    // create uniform descriptor
    UniformVecf tmp;
    tmp.id = _name;
    tmp.size = 3;
    tmp.val[0] = _value[0];
    tmp.val[1] = _value[1];
    tmp.val[2] = _value[2];

    // add/update in pool
    addVecf(tmp);
  }

  /** \brief Set vec4 uniform to specified value
  *
  * @param _name  Name of the uniform
  * @param _value New value of the uniform
  */
  void UniformPool::setUniform( const char *_name, const ACG::Vec4f &_value ) {
    // create uniform descriptor
    UniformVecf tmp;
    tmp.id = _name;
    tmp.size = 4;
    tmp.val = _value;

    // add/update in pool
    addVecf(tmp);
  }

  /** \brief Set 4x4fMatrix uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value Matrix to be set
   * @param _transposed Is the matrix transposed?
   */
  void UniformPool::setUniform( const char *_name, const ACG::GLMatrixf &_value, bool _transposed ) {
    // create uniform descriptor
    UniformMat tmp;
    tmp.id = _name;
    tmp.transposed = _transposed;
    tmp.size = 4;
    tmp.val = _value;

    // add/update in pool
    addMatrix(tmp);
  }

  /** \brief Set 3x3fMatrix uniform to specified value
   *
   * @param _name  Name of the uniform
   * @param _value Matrix to be set
   * @param _transposed Is the matrix transposed?
   */
  void UniformPool::setUniformMat3( const char *_name, const ACG::GLMatrixf &_value, bool _transposed ) {
    // create uniform descriptor
    UniformMat tmp;
    tmp.id = _name;
    tmp.transposed = _transposed;
    tmp.size = 3;
    tmp.val = _value;

    // add/update in pool
    addMatrix(tmp);
  }

  /** \brief Set int array uniform to specified values
   *
   *  @param _name Name of the uniform to be set
   *  @param _values Pointer to an array with the new values
   *  @param _count Number of values in the given array
   */
  void UniformPool::setUniform( const char *_name, GLint *_values, int _count ) {
    // add/update in pool
    addBuf(_name, _values, _count, true);
  }

  /** \brief Set float array uniform to specified values
   *
   *  @param _name Name of the uniform to be set
   *  @param _values Pointer to an array with the new values
   *  @param _count Number of values in the given array
   */
  void UniformPool::setUniform( const char *_name, GLfloat *_values, int _count ) {
    // add/update in pool
    addBuf(_name, _values, _count, false);
  }

}


//==============================================================================

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


#include <list>
#include <ACG/Config/ACGDefines.hh>
#include <ACG/GL/ShaderGenerator.hh>

// forward declaration
namespace GLSL
{
  class Program;
}


namespace ACG {


/** \brief Cache for shaders
 *
 *
 * This class provides a cache for shaders. You can query a GLSL::Program via getProgram
 * by specifying a ShaderGenDesc. If the program has already been compiled, it is returned
 * from the cache. Otherwise it will be compiled and linked, added to the cache and returned.
 * This ensures, that the shader is not compiled and linked every time, it is used.
 *
 * \note This class is a singleton
 *
*/
class ACGDLLEXPORT ShaderCache
{
public:

  /// Destructor
  virtual ~ShaderCache();

  /** \brief Return instance of the ShaderCache singleton
   *
   * @return Instance
   */
  static ShaderCache* getInstance();


  /** \brief Query a program from cache
   *
   * @param _desc  Shader description
   * @param _usage Additional usage identifier
   * @return The program (Either from cache or newly compiled and linked)
   */
  GLSL::Program* getProgram(const ShaderGenDesc* _desc, unsigned int _usage = 0);

protected:
  ShaderCache();



  struct CacheEntry
  {
    ShaderGenDesc desc;
    unsigned int  usage;
  };

  int compareShaderGenDescs(const CacheEntry* _a, const CacheEntry* _b);


  typedef std::list<std::pair<CacheEntry, GLSL::Program*> > CacheList;
  CacheList cache_;
};



//=============================================================================
} // namespace ACG
//=============================================================================

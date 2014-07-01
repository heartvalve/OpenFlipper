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

#pragma once


#include <list>
#include <QDateTime>
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


  /** \brief Query a dynamically generated program from cache
   *
   * @param _desc  Shader description
   * @param _usage Additional usage identifier (combination of shader modifier ids)
   * @return The program (Either from cache or newly compiled and linked)
   */
  GLSL::Program* getProgram(const ShaderGenDesc* _desc, unsigned int _usage = 0);

  /** \brief Query a static shader program from cache
   *
   * Can be used to load a shader and have external changes automatically applied by the timestamp watchdog.
   * However, the program should be queried every time before using the program as it may have been deleted and recompiled in the meantime.
   *
   * @param _vertexShaderFile relative (from shader directory) or absolute filename of vertex shader
   * @param _fragmentShaderFile relative (from shader directory) or absolute filename of vertex shader
   * @return The program (Either from cache or newly compiled and linked)
   */
  GLSL::Program* getProgram(const char* _vertexShaderFile, const char* _fragmentShaderFile);
  
  /** \brief Delete all cached shaders
   */
  void clearCache();

  /** \brief enable or disable checking of the time step of each file
   *
   */
  void setTimeCheck(bool _on){timeCheck_ = _on;}
  bool getTimeCheck(){return timeCheck_;}

protected:
  ShaderCache();

  struct CacheEntry
  {
    ShaderGenDesc desc;
    unsigned int  usage;

    // string-pointer in ShaderGenDesc may not be permanent,
    // so copy string data here
    QString strVertexTemplate;
    QString strTessControlTemplate;
    QString strTessEvaluationTemplate;
    QString strGeometryTemplate;
    QString strFragmentTemplate;

    QDateTime vertexFileLastMod;
    QDateTime tessControlFileLastMod;
    QDateTime tessEvaluationFileLastMod;
    QDateTime geometryFileLastMod;
    QDateTime fragmentFileLastMod;
  };

  /// \brief Returns true, if the shaders have the timestamp
  bool compareTimeStamp(const CacheEntry* _a, const CacheEntry* _b);

  /// \brief Returns true, if the shaders are not equal
  int compareShaderGenDescs(const CacheEntry* _a, const CacheEntry* _b);


  typedef std::list<std::pair<CacheEntry, GLSL::Program*> > CacheList;
  
  /// cache containing dynamic shaders from ShaderProgGenerator
  CacheList cache_;

  /// cache containing static shaders loaded from files (separate from dynamic cache to reduce access time)
  CacheList cacheStatic_;
  bool timeCheck_;
};



//=============================================================================
} // namespace ACG
//=============================================================================

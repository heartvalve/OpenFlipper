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


#include "ShaderCache.hh"
#include "ShaderGenerator.hh"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>


#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include <ACG/GL/gl.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

namespace ACG
{

#define SG_DEBUG_OUTPUT

ShaderCache::ShaderCache():
    cache_(),
    timeCheck_(false)
{
}

ShaderCache::~ShaderCache()
{
  // free all glsl programs in cache
  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
    delete it->second;
}

ShaderCache* ACG::ShaderCache::getInstance()
{
  static ShaderCache singleton;
  return &singleton;
}


//***********************************************************************
// TODO implement binary search eventually
// - modify compareShaderGenDescs s.t. it defines an order
// or generate a hash key from ShaderGenDesc

GLSL::Program* ACG::ShaderCache::getProgram( const ShaderGenDesc* _desc, unsigned int _usage )
{
  CacheEntry newEntry;
  newEntry.desc = *_desc;
  newEntry.usage = _usage;

  if (!_desc->fragmentTemplateFile.isEmpty())
  {
    newEntry.strFragmentTemplate = _desc->fragmentTemplateFile;
    newEntry.fragmentFileLastMod = QFileInfo(newEntry.strFragmentTemplate).lastModified();
  }

  if (!_desc->geometryTemplateFile.isEmpty())
  {
      newEntry.strGeometryTemplate = _desc->geometryTemplateFile;
      newEntry.geometryFileLastMod = QFileInfo(newEntry.strGeometryTemplate).lastModified();
  }

  if (!_desc->vertexTemplateFile.isEmpty())
  {
    newEntry.strVertexTemplate = _desc->vertexTemplateFile;
    newEntry.vertexFileLastMod = QFileInfo(newEntry.strVertexTemplate).lastModified();
  }

  CacheList::iterator oldCache = cache_.end();

  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
  {
    // If the shaders are equal, we return the cached entry
    if (!compareShaderGenDescs(&it->first, &newEntry))
    {
      if ( timeCheck_ && !compareTimeStamp(&it->first, &newEntry))
        oldCache = it;
      else
        return it->second;
    }
  }

  // glsl program not in cache, generate shaders
  ShaderProgGenerator progGen(_desc, _usage);

#ifdef SG_DEBUG_OUTPUT
  {
    static int counter = 0;

    char fileName[0x100];
    sprintf(fileName, "../../shader_%02d.glsl", counter++);

    QFile fileOut(fileName);
    if (fileOut.open(QFile::WriteOnly | QFile::Truncate))
    {
      QTextStream outStrm(&fileOut);

      outStrm << _desc->toString();
      outStrm << "\nusage: " << _usage << "\n";


      outStrm << "\n---------------------vertex-shader--------------------\n\n";
      
      for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
        outStrm << progGen.getVertexShaderCode()[i] << "\n";

      outStrm << "\n---------------------geometry-shader--------------------\n\n";

      if (progGen.hasGeometryShader() )
        for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
          outStrm << progGen.getGeometryShaderCode()[i] << "\n";

      outStrm << "\n---------------------fragment-shader--------------------\n\n";

      for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
        outStrm << progGen.getFragmentShaderCode()[i] << "\n";


      fileOut.close();
    }
  }
#endif

  // TODO: Build geometry shader only if supported!

  GLSL::FragmentShader* fragShader = new GLSL::FragmentShader();
  GLSL::VertexShader* vertShader   = new GLSL::VertexShader();

  vertShader->setSource(progGen.getVertexShaderCode());
  fragShader->setSource(progGen.getFragmentShaderCode());

  vertShader->compile();
  fragShader->compile();

  GLSL::Program* prog = new GLSL::Program();
  prog->attach(vertShader);
  prog->attach(fragShader);

  // Check if we have a geometry shader and if we have support for it, enable it here
  if ( progGen.hasGeometryShader() ) {
    GLSL::GeometryShader* geomShader = new GLSL::GeometryShader();
    geomShader->setSource(progGen.getGeometryShaderCode());
    geomShader->compile();
    prog->attach(geomShader);
  }

  prog->link();
  glCheckErrors();

  if (oldCache != cache_.end())
  {
    if (!prog->isLinked())
    {
      return oldCache->second;
    }
    else
    {
      cache_.erase(oldCache);
    }
  }

  cache_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

  return prog;
}

bool ACG::ShaderCache::compareTimeStamp(const CacheEntry* _a, const CacheEntry* _b)
{
  if (_a->vertexFileLastMod != _b->vertexFileLastMod)
    return false;

  if (_a->geometryFileLastMod != _b->geometryFileLastMod)
    return false;

  if (_a->fragmentFileLastMod != _b->fragmentFileLastMod)
    return false;
  return true;
}

int ACG::ShaderCache::compareShaderGenDescs( const CacheEntry* _a, const CacheEntry* _b)
{
  if (_a->usage != _b->usage)
    return -1;

  const ShaderGenDesc* a = &_a->desc;
  const ShaderGenDesc* b = &_b->desc;

  if (a->numLights != b->numLights)
    return -1;

  if (a->shadeMode != b->shadeMode)
    return -1;

  if (a->vertexColors != b->vertexColors)
    return -1;

  if (a->textured() != b->textured())
    return -1;

  if (_a->strFragmentTemplate != _b->strFragmentTemplate)
    return -1;

  if (_a->strGeometryTemplate != _b->strGeometryTemplate)
      return -1;

  if (_a->strVertexTemplate != _b->strVertexTemplate)
    return -1;


  if (a->numLights)
    return memcmp(a->lightTypes, b->lightTypes, a->numLights * sizeof(ShaderGenLightType));

  return 0; // false
}


void ACG::ShaderCache::clearCache()
{
  cache_.clear();
}


//=============================================================================
} // namespace ACG
//=============================================================================

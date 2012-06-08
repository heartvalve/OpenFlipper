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


#include "ShaderCache.hh"
#include "ShaderGenerator.hh"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include <ACG/GL/gl.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

namespace ACG
{

//#define SG_DEBUG_OUTPUT

ShaderCache::ShaderCache()
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

GLSL::Program* ACG::ShaderCache::getProgram( const ShaderGenDesc* _desc )
{
  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
  {
    if (!compareShaderGenDescs(&it->first, _desc))
      return it->second;
  }

  // glsl program not in cache, generate shaders

  ShaderProgGenerator progGen(_desc);

#ifdef SG_DEBUG_OUTPUT
  progGen.saveFragmentShToFile("../../../dbg_frag.glsl");
  progGen.saveVertexShToFile("../../../dbg_vert.glsl");
#endif

  GLSL::FragmentShader* fragShader = new GLSL::FragmentShader();
  GLSL::VertexShader* vertShader = new GLSL::VertexShader();

  vertShader->setSource(progGen.getVertexShaderCode());
  fragShader->setSource(progGen.getFragmentShaderCode());

  vertShader->compile();
  fragShader->compile();


  GLSL::Program* prog = new GLSL::Program();

  prog->attach(vertShader);
  prog->attach(fragShader);

  prog->link();
  glCheckErrors();


  cache_.push_back(std::pair<ShaderGenDesc, GLSL::Program*>(*_desc, prog));

  return prog;
}



int ACG::ShaderCache::compareShaderGenDescs( const ShaderGenDesc* _a, const ShaderGenDesc* _b )
{
  if (_a->numLights != _b->numLights)
    return -1;

  if (_a->shadeMode != _b->shadeMode)
    return -1;

  if (_a->vertexColors != _b->vertexColors)
    return -1;

  if (_a->textured != _b->textured)
    return -1;

  if (_a->vertexTemplateFile != _b->vertexTemplateFile)
    return -1;

  if (_a->fragmentTemplateFile != _b->fragmentTemplateFile)
    return -1;

  if (_a->numLights)
    return memcmp(_a->lightTypes, _b->lightTypes, _a->numLights * sizeof(ShaderGenLightType));

  return 0;
}


//=============================================================================
} // namespace ACG
//=============================================================================

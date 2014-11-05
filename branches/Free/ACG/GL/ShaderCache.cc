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
#include <fstream>


#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include <ACG/GL/gl.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>


namespace ACG
{

ShaderCache::ShaderCache():
    cache_(),
    cacheStatic_(),
    timeCheck_(false)
{
}

ShaderCache::~ShaderCache()
{
  // free all glsl programs in cache
  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
    delete it->second;

  for (CacheList::iterator it = cacheStatic_.begin(); it != cacheStatic_.end();  ++it)
    delete it->second;

  for (CacheList::iterator it = cacheComputeShaders_.begin(); it != cacheComputeShaders_.end();  ++it)
    delete it->second;
}

ShaderCache* ACG::ShaderCache::getInstance()
{
  static ShaderCache singleton;
  return &singleton;
}


//***********************************************************************
// TODO implement binary search eventually (if cache access is getting too slow)
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

  if (!_desc->tessControlTemplateFile.isEmpty())
  {
    newEntry.strTessControlTemplate = _desc->tessControlTemplateFile;
    newEntry.tessControlFileLastMod = QFileInfo(newEntry.strTessControlTemplate).lastModified();
  }

  if (!_desc->tessEvaluationTemplateFile.isEmpty())
  {
    newEntry.strTessEvaluationTemplate = _desc->tessEvaluationTemplateFile;
    newEntry.tessEvaluationFileLastMod = QFileInfo(newEntry.strTessEvaluationTemplate).lastModified();
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

  if (!dbgOutputDir_.isEmpty())
  {
    static int counter = 0;

    QString fileName;
    fileName.sprintf("shader_%02d.glsl", counter++);
    fileName = dbgOutputDir_ + QDir::separator() + fileName;

    QFile fileOut(fileName);
    if (fileOut.open(QFile::WriteOnly | QFile::Truncate))
    {
      QTextStream outStrm(&fileOut);

      outStrm << _desc->toString();
      outStrm << "\nusage: " << _usage << "\n";


      outStrm << "\n---------------------vertex-shader--------------------\n\n";
      
      for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
        outStrm << progGen.getVertexShaderCode()[i] << "\n";

      if (progGen.hasTessControlShader())
      {
        outStrm << "\n---------------------tesscontrol-shader--------------------\n\n";

        for (int i = 0; i < progGen.getTessControlShaderCode().size(); ++i)
          outStrm << progGen.getTessControlShaderCode()[i] << "\n";
      }

      if (progGen.hasTessEvaluationShader())
      {
        outStrm << "\n---------------------tesseval-shader--------------------\n\n";

        for (int i = 0; i < progGen.getTessEvaluationShaderCode().size(); ++i)
          outStrm << progGen.getTessEvaluationShaderCode()[i] << "\n";
      }

      if (progGen.hasGeometryShader())
      {
        outStrm << "\n---------------------geometry-shader--------------------\n\n";

        for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
          outStrm << progGen.getGeometryShaderCode()[i] << "\n";
      }

      outStrm << "\n---------------------fragment-shader--------------------\n\n";

      for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
        outStrm << progGen.getFragmentShaderCode()[i] << "\n";


      fileOut.close();
    }
  }

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

  // Check if we have tessellation shaders and if we have support for it, enable it here
  if ( progGen.hasTessControlShader() || progGen.hasTessEvaluationShader() ) {
    GLSL::Shader* tessControlShader = 0, *tessEvalShader = 0;

#ifdef GL_ARB_tessellation_shader
    tessControlShader = new GLSL::TessControlShader();
    tessEvalShader = new GLSL::TessEvaluationShader();
#endif // GL_ARB_tessellation_shader

    if (tessControlShader && progGen.hasTessControlShader())
    {
      tessControlShader->setSource(progGen.getTessControlShaderCode());
      tessControlShader->compile();
      prog->attach(tessControlShader);
    }

    if (tessEvalShader && progGen.hasTessEvaluationShader())
    {
      tessEvalShader->setSource(progGen.getTessEvaluationShaderCode());
      tessEvalShader->compile();
      prog->attach(tessEvalShader);
    }
    
  }

  prog->link();
  glCheckErrors();

  if (oldCache != cache_.end())
  {
    if (!prog || !prog->isLinked())
    {
      delete prog;
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

GLSL::Program* ACG::ShaderCache::getProgram( const char* _vertexShaderFile, 
  const char* _tessControlShaderFile,
  const char* _tessEvalShaderFile,
  const char* _geometryShaderFile,
  const char* _fragmentShaderFile,
  QStringList* _macros, bool _verbose )
{
  CacheEntry newEntry;
  newEntry.usage = 0;


  // store filenames and timestamps in new entry

  // fragment shader
  QFileInfo fileInfo(_fragmentShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_fragmentShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strFragmentTemplate = absFilename;
    newEntry.fragmentFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strFragmentTemplate = _fragmentShaderFile;
    newEntry.fragmentFileLastMod = fileInfo.lastModified();
  }

  // vertex shader
  fileInfo = QFileInfo(_vertexShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_vertexShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strVertexTemplate = absFilename;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strVertexTemplate = _vertexShaderFile;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }


  // geometry shader
  if (_geometryShaderFile)
  {
    fileInfo = QFileInfo(_geometryShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_geometryShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strGeometryTemplate = absFilename;
      newEntry.geometryFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strGeometryTemplate = _geometryShaderFile;
      newEntry.geometryFileLastMod = fileInfo.lastModified();
    }
  }

  // tess-ctrl shader
  if (_tessControlShaderFile)
  {
    fileInfo = QFileInfo(_tessControlShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_tessControlShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strTessControlTemplate = absFilename;
      newEntry.tessControlFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strTessControlTemplate = _tessControlShaderFile;
      newEntry.tessControlFileLastMod = fileInfo.lastModified();
    }
  }

  // tess-eval shader
  if (_tessEvalShaderFile)
  {
    fileInfo = QFileInfo(_tessEvalShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_tessEvalShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strTessEvaluationTemplate = absFilename;
      newEntry.tessEvaluationFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strTessEvaluationTemplate = _tessEvalShaderFile;
      newEntry.tessEvaluationFileLastMod = fileInfo.lastModified();
    }
  }



  if (_macros)
    newEntry.macros = *_macros;

  CacheList::iterator oldCache = cacheStatic_.end();

  for (CacheList::iterator it = cacheStatic_.begin(); it != cacheStatic_.end();  ++it)
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


  // convert QStringList to GLSL::StringList

  GLSL::StringList glslMacros;

  if (_macros)
  {
    for (QStringList::const_iterator it = _macros->constBegin(); it != _macros->constEnd(); ++it)
      glslMacros.push_back(it->toStdString());
  }


  GLSL::Program* prog = GLSL::loadProgram(_vertexShaderFile, _tessControlShaderFile, _tessEvalShaderFile, _geometryShaderFile, _fragmentShaderFile, &glslMacros, _verbose);
  glCheckErrors();

  if (oldCache != cacheStatic_.end())
  {
    if (!prog || !prog->isLinked())
    {
      delete prog;
      return oldCache->second;
    }
    else
    {
      cacheStatic_.erase(oldCache);
    }
  }

  cacheStatic_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

  return prog;
}

GLSL::Program* ACG::ShaderCache::getProgram( const char* _vertexShaderFile, const char* _fragmentShaderFile, QStringList* _macros, bool _verbose )
{
  return getProgram(_vertexShaderFile, 0, 0, 0, _fragmentShaderFile, _macros, _verbose);
}

GLSL::Program* ACG::ShaderCache::getComputeProgram(const char* _computeShaderFile, QStringList* _macros /* = 0 */, bool _verbose /* = true */)
{
  CacheEntry newEntry;
  newEntry.usage = 0;

  // store filenames and timestamps in new entry
  // use vertex shader filename as compute shader
  QFileInfo fileInfo(_computeShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_computeShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strVertexTemplate = absFilename;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strVertexTemplate = _computeShaderFile;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }

  if (_macros)
    newEntry.macros = *_macros;

  CacheList::iterator oldCache = cacheComputeShaders_.end();

  for (CacheList::iterator it = cacheComputeShaders_.begin(); it != cacheComputeShaders_.end();  ++it)
  {
    // If the shaders are equal, we return the cached entry
    if (!compareShaderGenDescs(&it->first, &newEntry))
    {
      if ( ( timeCheck_ && !compareTimeStamp(&it->first, &newEntry)) || !it->second)
        oldCache = it;
      else
        return it->second;
    }
  }


  // convert QStringList to GLSL::StringList

  GLSL::StringList glslMacros;

  if (_macros)
  {
    for (QStringList::const_iterator it = _macros->constBegin(); it != _macros->constEnd(); ++it)
      glslMacros.push_back(it->toStdString());
  }


  GLSL::Program* prog = GLSL::loadComputeProgram(newEntry.strVertexTemplate.toUtf8(), &glslMacros, _verbose);
  glCheckErrors();

  if (oldCache != cacheComputeShaders_.end())
  {
    if (!prog || !prog->isLinked())
    {
      delete prog;


      // dump shader source including macros to debug output
      if (!glslMacros.empty() && !dbgOutputDir_.isEmpty())
      {
        GLSL::StringList shaderSrc = GLSL::loadShader(newEntry.strVertexTemplate.toUtf8(), &glslMacros);

        // compute FNV hash of macros

        unsigned int fnv_prime = 16777619;
        unsigned int fnv_hash = 2166136261;

        for (GLSL::StringList::iterator it = shaderSrc.begin(); it != shaderSrc.end(); ++it)
        {
          for (size_t i = 0; i < it->length(); ++i)
          {
            fnv_hash *= fnv_prime;
            fnv_hash ^= static_cast<unsigned char>((*it)[i]);
          }
        }

        QString fnv_string;
        fnv_string.sprintf("%X", fnv_hash);

        std::string dumpFilename = QString(dbgOutputDir_ + QDir::separator() + fileInfo.fileName() + fnv_string).toStdString();

        std::ofstream dumpStream(dumpFilename.c_str());
        if (dumpStream.is_open())
        {
          for (GLSL::StringList::iterator it = shaderSrc.begin(); it != shaderSrc.end(); ++it)
            dumpStream << *it;
          dumpStream.close();
        }
      }

      return oldCache->second;
    }
    else
    {
      cacheComputeShaders_.erase(oldCache);
    }
  }

  cacheComputeShaders_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

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

  if (_a->tessControlFileLastMod != _b->tessControlFileLastMod)
    return false;

  if (_a->tessEvaluationFileLastMod != _b->tessEvaluationFileLastMod)
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

  if (_a->strTessControlTemplate != _b->strTessControlTemplate)
    return -1;

  if (_a->strTessEvaluationTemplate != _b->strTessEvaluationTemplate)
    return -1;

  if (_a->macros != _b->macros)
    return -1;


  if (a->numLights)
    return memcmp(a->lightTypes, b->lightTypes, a->numLights * sizeof(ShaderGenLightType));

  return 0; // false
}


void ACG::ShaderCache::clearCache()
{
  cache_.clear();
  cacheStatic_.clear();
  cacheComputeShaders_.clear();
}

void ACG::ShaderCache::setDebugOutputDir(const char* _outputDir)
{
  dbgOutputDir_ = _outputDir;
}

//=============================================================================
} // namespace ACG
//=============================================================================

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

#include <ACG/GL/acg_glew.hh>
#include <ACG/GL/gl.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <cassert>
#include <iostream>
#include <QTextStream>

#include "VertexDeclaration.hh"


namespace ACG
{

VertexElement::VertexElement()
  : type_(0), numElements_(0), usage_(VERTEX_USAGE_SHADER_INPUT), shaderInputName_(0), pointer_(0), divisor_(0), vbo_(0)
{
}


void VertexElement::setByteOffset(unsigned int _offset)
{
  // union cast instead of reinterpret_cast for cross-platform compatibility
  union ptr2uint
  {
    unsigned long u;
    const void* p;
  } offset;

  offset.u = static_cast<unsigned long>(_offset);

  pointer_ = offset.p;
}

unsigned int VertexElement::getByteOffset() const
{
  // union cast instead of reinterpret_cast for cross-platform compatibility
  union ptr2uint
  {
    unsigned long u;
    const void* p;
  } offset;

  offset.p = pointer_;

  return static_cast<unsigned int>(offset.u);
}

VertexDeclaration::VertexDeclaration()
: vertexStride_(0), strideUserDefined_(0)
{

}


VertexDeclaration::~VertexDeclaration()
{

}


void VertexDeclaration::addElement(const VertexElement* _pElement)
{
  addElements(1, _pElement);
}

void VertexDeclaration::addElement(unsigned int _type,
                                   unsigned int _numElements,
                                   VERTEX_USAGE _usage,
                                   const void* _pointer,
                                   const char* _shaderInputName,
                                   unsigned int _divisor,
                                   unsigned int _vbo)
{
  VertexElement* ve = new VertexElement();

  ve->type_ = _type;
  ve->numElements_ = _numElements;
  ve->usage_ = _usage;
  ve->shaderInputName_ = _shaderInputName;
  ve->pointer_ = _pointer;
  ve->divisor_ = _divisor;
  ve->vbo_ = _vbo;
  addElement(ve);

  delete ve;
}

void VertexDeclaration::addElement(unsigned int _type,
                                   unsigned int _numElements,
                                   VERTEX_USAGE _usage, 
                                   size_t _byteOffset,
                                   const char* _shaderInputName,
                                   unsigned int _divisor,
                                   unsigned int _vbo)
{
  VertexElement* ve = new VertexElement();

  ve->type_ = _type;
  ve->numElements_ = _numElements;
  ve->usage_ = _usage;
  ve->shaderInputName_ = _shaderInputName;
  ve->pointer_ = (void*)_byteOffset;
  ve->divisor_ = _divisor;
  ve->vbo_ = _vbo;

  addElement(ve);

  delete ve;
}


void VertexDeclaration::addElements(unsigned int _numElements, const VertexElement* _pElements)
{
  elements_.reserve(elements_.size() + _numElements);

  for (unsigned int i = 0; i < _numElements; ++i)
  {
    VertexElement tmp = _pElements[i];
    updateShaderInputName(&tmp);
    elements_.push_back(tmp);
  }

  updateOffsets();

  if (!strideUserDefined_)
  {
    // recompute vertex stride from declaration (based on last element)

    unsigned int n = getNumElements();

    if (n)
    {
      // stride = offset of last element + sizeof last element - offset of first element

      // union instead of reinterpret_cast for cross-platform compatibility
      union ptr2uint
      {
        unsigned long u;
        const void* p;
      } lastOffset, firstOffset;



      std::map<unsigned int, VertexElement*> vboFirstElements;
      std::map<unsigned int, VertexElement*> vboLastElements;

      for (unsigned int i = 0; i < n; ++i)
      {
        if (vboFirstElements.find(elements_[i].vbo_) == vboFirstElements.end())
          vboFirstElements[elements_[i].vbo_] = &elements_[i];

        vboLastElements[elements_[i].vbo_] = &elements_[i];
      }


      for (std::map<unsigned int, VertexElement*>::iterator it = vboFirstElements.begin(); it != vboFirstElements.end(); ++it)
      {
        VertexElement* lastElement = vboLastElements[it->first];
        firstOffset.p = it->second->pointer_;
        lastOffset.p = lastElement->pointer_;

        vertexStridesVBO_[it->first] = static_cast<unsigned int>(lastOffset.u + getElementSize( lastElement ) - firstOffset.u);
      }

      vertexStride_ = vertexStridesVBO_.begin()->second;
    }
  }

}


// union instead of reinterpret_cast for cross-platform compatibility, must be global for use in std::map
union VertexDeclaration_ptr2uint
{
  unsigned long u;
  const void* p;
};

void VertexDeclaration::updateOffsets()
{
  unsigned int numElements = getNumElements();

  if (!numElements) return;


  // separate offsets for each vbo

  std::map<unsigned int, VertexDeclaration_ptr2uint> vboOffsets;
  std::map<unsigned int, VertexElement*> vboPrevElements;

  for (unsigned int i = 0; i < numElements; ++i)
  {
    if (vboOffsets.find(elements_[i].vbo_) == vboOffsets.end())
    {
      vboOffsets[elements_[i].vbo_].p = elements_[i].pointer_;
      vboPrevElements[elements_[i].vbo_] = &elements_[i];
    }
  }

  for (unsigned int i = 0; i < numElements; ++i)
  {
    VertexElement* el = &elements_[i];

    bool updateOffset = false;

    if (el->pointer_)
    {
      vboOffsets[el->vbo_].p = el->pointer_;
      vboPrevElements[el->vbo_] = el;
    }
    else
    {
      VertexElement* prevEl = vboPrevElements[el->vbo_];
      if (prevEl != el)
      {
        updateOffset = true;
        vboOffsets[el->vbo_].u += getElementSize(prevEl);
      }
      vboPrevElements[el->vbo_] = el;
    }

    if (updateOffset)
      el->pointer_ = vboOffsets[el->vbo_].p;
  }
}


void VertexDeclaration::updateShaderInputName(VertexElement* _pElem)
{
  if (!_pElem->shaderInputName_)
  {
    assert(_pElem->usage_ != VERTEX_USAGE_SHADER_INPUT);

    const char* sz = "";

    switch (_pElem->usage_)
    {
    case VERTEX_USAGE_POSITION:     sz = "inPosition"; break;
    case VERTEX_USAGE_NORMAL:       sz = "inNormal"; break;
    case VERTEX_USAGE_TEXCOORD:     sz = "inTexCoord"; break;
    case VERTEX_USAGE_COLOR:        sz = "inColor"; break;
    case VERTEX_USAGE_BLENDWEIGHTS: sz = "inBlendWeights"; break;
    case VERTEX_USAGE_BLENDINDICES: sz = "inBlendIndices"; break;

    default:
      std::cerr << "VertexDeclaration::updateShaderInputName - unknown vertex usage - " << _pElem->usage_ << std::endl;
      break;
    }

    _pElem->shaderInputName_ = sz;
  }
}


unsigned int VertexDeclaration::getNumElements() const
{
  return elements_.size();
}


unsigned int VertexDeclaration::getGLTypeSize(unsigned int _type)
{
  unsigned int size = 0;

  switch (_type)
  {
  case GL_DOUBLE:
    size = 8; break;

  case GL_FLOAT:
  case GL_UNSIGNED_INT:
  case GL_INT:
    size = 4; break;

//  case GL_HALF_FLOAT_ARB:
  case 0x140B: // = GL_HALF_FLOAT_ARB
  case GL_SHORT:
  case GL_UNSIGNED_SHORT:
    size = 2; break;

  case GL_BYTE:
  case GL_UNSIGNED_BYTE:
    size = 1; break;

  default:
    std::cerr << "VertexDeclaration::getElementSize - unknown type - " << _type << std::endl;
    break;
  }

  return size;
}



unsigned int VertexDeclaration::getElementSize(const VertexElement* _pElement)
{
  return _pElement ? getGLTypeSize(_pElement->type_) * _pElement->numElements_ : 0;
}








void VertexDeclaration::activateFixedFunction() const
{
  unsigned int numElements = getNumElements();
  unsigned int vertexStride = getVertexStride();

  for (unsigned int i = 0; i < numElements; ++i)
  {
    const VertexElement* pElem = getElement(i);

    switch (pElem->usage_)
    {
    case VERTEX_USAGE_POSITION:
      {
        ACG::GLState::vertexPointer(pElem->numElements_, pElem->type_, vertexStride, pElem->pointer_);
        ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      } break;

    case VERTEX_USAGE_COLOR:
      {
        ACG::GLState::colorPointer(pElem->numElements_, pElem->type_, vertexStride, pElem->pointer_);
        ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      } break;

    case VERTEX_USAGE_TEXCOORD:
      {
        glClientActiveTexture(GL_TEXTURE0);
        ACG::GLState::texcoordPointer(pElem->numElements_, pElem->type_, vertexStride, pElem->pointer_);
        ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);
      } break;

    case VERTEX_USAGE_NORMAL:
      {
        assert(pElem->numElements_ == 3);

        ACG::GLState::normalPointer(pElem->type_, vertexStride, pElem->pointer_);
        ACG::GLState::enableClientState(GL_NORMAL_ARRAY);
      } break;

    default: break;
    }

    
  }
}


void VertexDeclaration::deactivateFixedFunction() const
{
  unsigned int numElements = getNumElements();

  for (unsigned int i = 0; i < numElements; ++i)
  {
    const VertexElement* pElem = getElement(i);

    switch (pElem->usage_)
    {
    case VERTEX_USAGE_POSITION: ACG::GLState::disableClientState(GL_VERTEX_ARRAY); break;
    case VERTEX_USAGE_COLOR: ACG::GLState::disableClientState(GL_COLOR_ARRAY); break;
    case VERTEX_USAGE_TEXCOORD: ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY); break;
    case VERTEX_USAGE_NORMAL: ACG::GLState::disableClientState(GL_NORMAL_ARRAY); break;

    default: break;
    }
  }
}



void VertexDeclaration::activateShaderPipeline(GLSL::Program* _prog) const
{
  assert(_prog);

  // setup correct attribute locations as specified

  unsigned int numElements = getNumElements();

  for (unsigned int i = 0; i < numElements; ++i)
  {
    unsigned int vertexStride = getVertexStride(i);

    const VertexElement* pElem = &elements_[i];

    int loc = _prog->getAttributeLocation(pElem->shaderInputName_);

    if (loc != -1)
    {
      // default: map integers to [-1, 1] or [0, 1] range
      // exception: blend indices for gpu skinning eventually
      GLboolean normalizeElem = GL_TRUE;

      if (pElem->usage_ == VERTEX_USAGE_BLENDINDICES)
        normalizeElem = GL_FALSE;

      GLint curVBO = 0;
      if (pElem->vbo_)
      {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &curVBO);
        glBindBuffer(GL_ARRAY_BUFFER, pElem->vbo_);
      }

      glVertexAttribPointer(loc, pElem->numElements_,  pElem->type_, normalizeElem, vertexStride, pElem->pointer_);

      if (supportsInstancedArrays())
      {
#ifdef GL_ARB_instanced_arrays
        glVertexAttribDivisor(loc, pElem->divisor_);
#endif
      }
      else if (pElem->divisor_)
        std::cerr << "error: VertexDeclaration::activateShaderPipeline - instanced arrays not supported by gpu!" << std::endl;

      glEnableVertexAttribArray(loc);

      if (curVBO)
        glBindBuffer(GL_ARRAY_BUFFER, curVBO);
    }
  }
}


void VertexDeclaration::deactivateShaderPipeline( GLSL::Program* _prog ) const
{
  assert(_prog);

  unsigned int numElements = getNumElements();

  for (unsigned int i = 0; i < numElements; ++i)
  {
    const VertexElement* pElem = &elements_[i];

    int loc = _prog->getAttributeLocation(pElem->shaderInputName_);

    if (loc != -1)
      glDisableVertexAttribArray(loc);

  }
}



const VertexElement* VertexDeclaration::getElement(unsigned int i) const
{
  return &elements_[i];
}


int VertexDeclaration::findElementIdByUsage(VERTEX_USAGE _usage) const
{
  for (size_t i = 0; i < elements_.size(); ++i)
    if (elements_[i].usage_ == _usage)
      return int(i);
  
  return -1;
}

const VertexElement* VertexDeclaration::findElementByUsage(VERTEX_USAGE _usage) const
{
  int eid = findElementIdByUsage(_usage);

  if (eid >= 0)
    return getElement((unsigned int)eid);

  return 0;
}


unsigned int VertexDeclaration::getVertexStride(unsigned int i) const
{
  unsigned int vbo = getElement(i)->vbo_;
  std::map<unsigned int, unsigned int>::const_iterator it = vertexStridesVBO_.find(vbo);

  return (it != vertexStridesVBO_.end()) ? it->second : vertexStride_;
//  return vertexStride_;
}


void VertexDeclaration::setVertexStride(unsigned int _stride)
{
  strideUserDefined_ = 1;
  vertexStride_ = _stride;
}

void VertexDeclaration::clear()
{
  strideUserDefined_ = 0;
  vertexStride_ = 0;

  elements_.clear();
}


QString VertexDeclaration::toString() const
{
  // maps VERTEX_USAGE -> string
  const char* usageStrings[] = 
  {
    "POSITION",
    "NORMAL",
    "TEXCOORD",
    "COLOR",
    "BLENDWEIGHTS",
    "BLENDINDICES"
  };

  QString result;

  QTextStream resultStrm(&result);
  resultStrm << "stride = " << getVertexStride() << "\n";


  for (unsigned int i = 0; i < getNumElements(); ++i)
  {
    const VertexElement* el = getElement(i);

    // convert element-type GLEnum to string
    const char* typeString = "";

    switch (el->type_)
    {
    case GL_FLOAT: typeString = "GL_FLOAT"; break;
    case GL_DOUBLE: typeString = "GL_DOUBLE"; break;

    case GL_INT: typeString = "GL_INT"; break;
    case GL_UNSIGNED_INT: typeString = "GL_UNSIGNED_INT"; break;

    case GL_SHORT: typeString = "GL_SHORT"; break;
    case GL_UNSIGNED_SHORT: typeString = "GL_UNSIGNED_SHORT"; break;

    case GL_BYTE: typeString = "GL_BYTE"; break;
    case GL_UNSIGNED_BYTE: typeString = "GL_UNSIGNED_BYTE"; break;
    default: typeString = "unknown"; break;
    }

    // get usage in string form
    const char* usage = (el->usage_ < VERTEX_USAGE_SHADER_INPUT) ? usageStrings[el->usage_] : el->shaderInputName_;

    resultStrm << "element " << i
               << " - [type: " << typeString
               << ", count: " << el->numElements_
               << ", usage: " << usage
               << ", shader-input: " << el->shaderInputName_
               << ", offset: " << el->pointer_ 
               << ", divisor: " << el->divisor_ 
               << ", vbo: " << el->vbo_
               << ", stride: " << getVertexStride(i)
               << "]\n";
  }

  return result;
}

bool VertexDeclaration::supportsInstancedArrays()
{
  static int status_ = -1;

#ifdef GL_ARB_instanced_arrays
  if (status_ < 0) // core in 3.3
    status_ = (checkExtensionSupported("GL_ARB_instanced_arrays") || openGLVersion(3,3)) ? 1 : 0;
#endif

  return status_ > 0;
}

//=============================================================================
} // namespace ACG
//=============================================================================

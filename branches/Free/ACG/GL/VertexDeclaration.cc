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

#include <ACG/GL/gl.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <cassert>
#include <iostream>
#include <QTextStream>

#include "VertexDeclaration.hh"


namespace ACG
{


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
                                   const char* _shaderInputName)
{
  VertexElement* ve = new VertexElement();

  ve->type_ = _type;
  ve->numElements_ = _numElements;
  ve->usage_ = _usage;
  ve->shaderInputName_ = _shaderInputName;
  ve->pointer_ = _pointer;
  addElement(ve);

  delete ve;
}

void VertexDeclaration::addElement(unsigned int _type,
                                   unsigned int _numElements,
                                   VERTEX_USAGE _usage, 
                                   size_t _byteOffset,
                                   const char* _shaderInputName)
{
  VertexElement* ve = new VertexElement();

  ve->type_ = _type;
  ve->numElements_ = _numElements;
  ve->usage_ = _usage;
  ve->shaderInputName_ = _shaderInputName;
  ve->pointer_ = (void*)_byteOffset;

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

      firstOffset.p = getElement(0)->pointer_;
      
      const VertexElement* pLastElement = getElement(n-1);
      lastOffset.p = pLastElement->pointer_;

      vertexStride_ = static_cast<unsigned int>(lastOffset.u + getElementSize( pLastElement ) - firstOffset.u);
    }
  }

}



void VertexDeclaration::updateOffsets()
{
  unsigned int numElements = getNumElements();

  if (!numElements) return;


  // union instead of reinterpret_cast for cross-platform compatibility
  union ptr2uint
  {
    unsigned long u;
    const void* p;
  };

  ptr2uint curOffset;
  curOffset.p = elements_[0].pointer_;

  for (unsigned int i = 1; i < numElements; ++i)
  {
    if (elements_[i].pointer_)
      curOffset.p = elements_[i].pointer_;

    else
      curOffset.u += getElementSize(&elements_[i-1]);

    elements_[i].pointer_ = curOffset.p;
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
  return getGLTypeSize(_pElement->type_) * _pElement->numElements_;
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
  unsigned int vertexStride = getVertexStride();

  for (unsigned int i = 0; i < numElements; ++i)
  {
    const VertexElement* pElem = &elements_[i];

    int loc = _prog->getAttributeLocation(pElem->shaderInputName_);

    if (loc != -1)
    {
      // default: map integers to [-1, 1] or [0, 1] range
      // exception: blend indices for gpu skinning eventually
      GLboolean normalizeElem = GL_TRUE;

      if (pElem->usage_ == VERTEX_USAGE_BLENDINDICES)
        normalizeElem = GL_FALSE;

      glVertexAttribPointer(loc, pElem->numElements_,  pElem->type_, normalizeElem, vertexStride, pElem->pointer_);
      glEnableVertexAttribArray(loc);

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


unsigned int VertexDeclaration::getVertexStride() const
{
  return vertexStride_;
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
               << ", offset: " << el->pointer_ << "]\n";
  }

  return result;
}

//=============================================================================
} // namespace ACG
//=============================================================================

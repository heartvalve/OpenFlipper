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

#include <vector>
#include <list>
#include <ACG/Config/ACGDefines.hh>

namespace GLSL
{
  class Program;
}

namespace ACG
{

/// ----  input name in vertex shader -------
enum VERTEX_USAGE
{                               
  VERTEX_USAGE_POSITION = 0,    //!< "inPosition"
  VERTEX_USAGE_NORMAL,          //!< "inNormal"
  VERTEX_USAGE_TEXCOORD,        //!< "inTexCoord"
  VERTEX_USAGE_COLOR,           //!< "inColor" 
  VERTEX_USAGE_BLENDWEIGHTS,    //!< "inBlendWeights"
  VERTEX_USAGE_BLENDINDICES,    //!< "inBlendIndices"
  VERTEX_USAGE_SHADER_INPUT,    //!< defined by user via VertexElement::shaderInputName_
  VERTEX_USAGE_FORCE_DWORD = 0xFFFFFFFF
};

/** \brief Description of one vertex element
 *
 */
struct VertexElement
{
  unsigned int type_;           //!< GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, ...
  unsigned int numElements_;    //!< how many elements of type_
  VERTEX_USAGE usage_;          //!< position, normal, shader input ..
  const char* shaderInputName_; //!< set shader input name, if usage_ = VERTEX_USAGE_USER_DEFINED otherwise this is set automatically, if usage_ != VERTEX_USAGE_USER_DEFINED
  union
  {
    /// offset in bytes to the first instance of this element in vertex buffer (data padding allowed)
    unsigned int byteOffset_;      

    /// if vertex buffer data is in separate memory chunks, this is the pointer to the first vertex element
    const void* pDataSrc_;
  };
  
};

/** \brief Class to define the layout of a vertex element
 *
 * This class is used to specify how the elements (normals,positions,...) of a vertex element are setup.
 *
 * A vertex declaration is needed to describe the data layout of a vertex buffer:
 * - number of vertex elements
 * - element usage (position, normal, ... )
 * - element types (float, int, ...)
 * - memory alignment
 * - data pointers (only in case of system memory buffers)
 *
 *
 * Example usage:
 *
 * Suppose you have an interleaved vertex buffer without memory alignment:
 * \code
 * Vertex
 * {
 *   float4 pos
 *   float3 normal
 *   float2 texcoord
 *   ubyte4 color
 *   float4 tangent
 *   float2 texcoord2
 * }
 * \endcode
 *
 * You would create the following VertexElements:
 * \code
 * VertexElement elemArray[] = { {GL_FLOAT, 4, VERTEX_USAGE_POSITION, 0, {0} },
 *                               {GL_FLOAT, 3, VERTEX_USAGE_NORMAL, 0, {0} },
 *                               {GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD, 0, {0} },
 *                               {GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 0, {0} },
 *                               {GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, "inTangent", {0} },
 *                               {GL_FLOAT, 2, VERTEX_USAGE_SHADER_INPUT, "inTexcoord2", {0}}
 *                             };
 * \endcode
 *
 * equivalent to
 * \code
 * VertexElement elemArray[] = { {GL_FLOAT, 4, VERTEX_USAGE_POSITION, 0, {0} },
 *                               {GL_FLOAT, 3, VERTEX_USAGE_NORMAL, 0, {4*4} },
 *                               {GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD, 0, {4*4 + 4*3} },
 *                               {GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 0, {4*4 + 4*3 + 4*2} },
 *                               ..
 *                              };
 * \endcode
 *
 * in the vertex shader:
 * \code
 * in vec4 inPosition;
 * ...
 * in vec4 inTangent;
 * in vec2 inTexcoord2;
 * \endcode
 *
 * VERTEX_USAGE_SHADER_INPUT is ignored in fixed function pipeline
 *
 *
 * ---------------------------------------------------------------------------------
 *
 * Example use:  multiple buffer source in system memory
 *
 * Suppose you have the following arrays in system memory
 *
 * \code
 *  float4* pVertexPositions;
 *  ubyte4* pVertexColors;
 *  float3* pVertexNormals;
 * \endcode
 *
 * important is, that you define a vertex stride of 0 and define the data pointer in the vertex elements instead
 *
 * The correct vertex declaration to use is:
 *
 * \code
 * VertexElement elemArray[] = { {GL_FLOAT, 4, VERTEX_USAGE_POSITION, 0, pVertexPositions},
 *                               {GL_FLOAT, 3, VERTEX_USAGE_NORMAL, 0, pVertexNormals},
 *                               {GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, 0, pVertexColors}
 *                             };
 *
 * VertexDeclaration* pDecl = new VertexDeclaration();
 *
 * pDecl->addElements(3, elemArray);
 * pDecl->setVertexStride(0);
 * \endcode
 *
 * Note:
 * - vertex stride = 0
 * - assign data address in vertex elements and don't change the data location later
 * - order of elements is not important in this case
 *
 */
class ACGDLLEXPORT VertexDeclaration
{
public:

  VertexDeclaration();
  ~VertexDeclaration();

  /*! append one element to declaration
  */
  void addElement(const VertexElement* _pElement);

  /*! append one element to declarations, direct method
  */
  void addElement(unsigned int _type, unsigned int _numElements, VERTEX_USAGE _usage,  const char* _shaderInputName = 0, unsigned int _byteOffset = 0);
  
  /*! append multiple element declarations
  */
  void addElements(unsigned int _numElements, const VertexElement* _pElements);


  /*! prepare OpenGL to use a vertex buffer with this declaration
  -> uses the fixed function pointers (glVertexPointer, glColorPointer...)
  */
  void activateFixedFunction();

  /*! prepare OpenGL to use a vertex buffer with this declaration
  -> uses shader attribute pointers ( glVertexAttribPointer )
  */
  void activateShaderPipeline(GLSL::Program* _prog);


  /*! disables this vertex declaration to prevent any draw related crashes
  -> calls glDisableVertexAttribArray
  */
  void deactivateShaderPipeline(GLSL::Program* _prog) const;


  /*! set vertex stride manually, otherwise it is computed
  from the vertex elements
  */
  void setVertexStride(unsigned int _stride);

  /*! get vertex size
  */
  unsigned int getVertexStride();

  /*! get num of vertex elements
  */
  unsigned int getNumElements() const;

  /*! get the i'th vertex element desc
  */
  const VertexElement* getElement(unsigned int i);

  /*! get size of GL_FLOAT, GL_INT ...
  */
  static unsigned int getGLTypeSize(unsigned int _type);

  /*! get size of one vertex element
  */
  static unsigned int getElementSize(const VertexElement* _pElement);

private:

  /*!
  automatically computes byte offset for interleaved vertex elements
  and computes vertex stride
  */
  void updateOffsets();

  /*! 
  automatically sets shader input name on passed element, if not provided
  */
  void updateShaderInputName(VertexElement* _pElem);


  std::vector<VertexElement> elements_;

  /// Offset in bytes between each vertex
  unsigned int vertexStride_;

  /// Flag that indicates, whether the stride was set by user or derived automatically
  int strideUserDefined_;
};


//=============================================================================
} // namespace ACG
//=============================================================================

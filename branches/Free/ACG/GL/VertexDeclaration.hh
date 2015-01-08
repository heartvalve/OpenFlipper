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


#ifndef ACG_VERTEXDECLARATION_HH
#define ACG_VERTEXDECLARATION_HH

#include <vector>
#include <list>
#include <map>
#include <QString>
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
struct ACGDLLEXPORT VertexElement
{
  VertexElement();

  unsigned int type_;           //!< GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, ...
  unsigned int numElements_;    //!< how many elements of type_
  VERTEX_USAGE usage_;          //!< position, normal, shader input ..
  const char* shaderInputName_; //!< set shader input name, if usage_ = VERTEX_USAGE_USER_DEFINED otherwise this is set automatically, if usage_ != VERTEX_USAGE_USER_DEFINED
  const void* pointer_;         //!< Offset in bytes to the first occurrence of this element in vertex buffer; Or address to vertex data in system memory
  unsigned int divisor_;        //!< For instanced rendering: Step rate describing how many instances are drawn before advancing to the next element. Must be 0 if this element is not per instance data.
  unsigned int vbo_;            //!< Explicit vbo source of this element, set to 0 if the buffer bound at the time of activating the declaration should be used instead.

  /*! interpret pointer_ as byte offset
  */
  void setByteOffset(unsigned int _offset);

  /*! interpret pointer_ as byte offset
  */
  unsigned int getByteOffset() const;
};

/** \brief Class to define the vertex input layout
 *
 * This class is used to specify vertex data layout (normals,positions,...).
 *
 * The layout of a vertex buffer typically includes:
 * - number of vertex elements
 * - element usage (position, normal, ... )
 * - element types (float, int, ...)
 * - memory alignment
 * - data pointers (only in case of system memory buffers)
 * - source vertex buffers
 * - divisor (step rate) for instance data
 * - vertex stride
 * 
 *
 * Example usage: Interleaved vertex data
 *
 * Create a vertex declaration given the following vertex data:
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
 * The accompanying vertex declaration is created as follows:
 * \code
 * VertexDeclaration decl;
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_POSITION);
 * decl.addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL);
 * decl.addElement(GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD);
 * decl.addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR);
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, 0U, "inTangent");
 * decl.addElement(GL_FLOAT, 2, VERTEX_USAGE_SHADER_INPUT, 0U, "inTexcoord2");
 *
 * \endcode
 *
 * Note that the element offsets are automatically computed, but it is possible to provide them manually of course.
 *
 * These elements are then available in a vertex shader with the following input semantics:
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
 * Example usage:  Multiple source buffers in system memory
 *
 * Given are system memory buffers as vertex data source for drawing:
 * \code
 *  float4* pVertexPositions;
 *  ubyte4* pVertexColors;
 *  float3* pVertexNormals;
 * \endcode
 *
 * It is important that the vertex stride is manually set to 0!! and that the element data pointers are set accordingly:
 *
 * \code
 * VertexDeclaration decl;
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_POSITION, pVertexPositions);
 * decl.addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL, pVertexNormals);
 * decl.addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, pVertexColors);
 * decl.setVertexStride(0);
 * \endcode
 *
 *
 * Note:
 * - vertex stride = 0
 * - assign data address in vertex elements and don't change the data location later
 * - order of elements is not important in this case
 * - mixing interleaved vbo and system memory elements in the same declaration is forbidden
 *
 * ---------------------------------------------------------------------------------
 *
 * Example usage:  Instancing from vbo
 *
 * Geometry instancing can be implemented in several ways:
 * - compute instance transform procedurally from gl_InstanceID in shader
 * - store per instance data in a texture and use vertex texture fetch via gl_InstanceID
 * - store per instance data in a vbo and stream these as input to a vertex shader (ARB_instanced_arrays).
 *
 * The first two methods can be implemented without modifying the vertex declaration,
 * so this example focuses on the last method, in which instance data is streamed from a vbo.
 *
 * Given a mesh with the following vertex elements:
 * \code
 * Vertex
 * {
 *   float4 pos
 *   float3 normal
 *   float2 texcoord
 * }
 * \endcode
 *
 * Each instance of this mesh should have a different world transform and a different color.
 * So the per instance data is:
 * \code
 * InstanceData
 * {
 *   mat4 modelview
 *   uint color
 * }
 * \endcode
 *
 * This per instance data is stored in a separate GL_ARRAY_BUFFER vbo.
 *
 * So there are two vbos involved in the draw call:
 *   geometryVBO - stores static Vertex data of the mesh
 *   instanceVBO - stores InstanceData for each instance
 *
 * The vertex declaration for the instanced draw call is initialized as follows:
 *
 * \code
 * VertexDeclaration decl;
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_POSITION, 0U, 0, 0, geometryVBO.id()); // mesh vertex data layout
 * decl.addElement(GL_FLOAT, 3, VERTEX_USAGE_NORMAL, 0U, 0, 0, geometryVBO.id());
 * decl.addElement(GL_FLOAT, 2, VERTEX_USAGE_TEXCOORD, 0U, 0, 0, geometryVBO.id());
 *
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, 0U, "inModelView0", 1, instanceVBO.id()); // instance data layout
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, 0U, "inModelView1", 1, instanceVBO.id());
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, 0U, "inModelView2", 1, instanceVBO.id());
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, 0U, "inModelView3", 1, instanceVBO.id());
 * decl.addElement(GL_FLOAT, 4, VERTEX_USAGE_COLOR, 0U, 0, 1, instanceVBO.id());
 * \endcode
 *
 * The 4x4 modelview matrix has to be split up into float4 elements.
 *
 * Using this setup, a call to decl.activateShaderPipeline() is enough to prepare an instanced draw call.
 *
 * Finally, instancing requires a customized vertex shader that makes use of the per instance data:
 * \code
 * ..
 * in vec4 inModelView0;
 * in vec4 inModelView1;
 * in vec4 inModelView2;
 * in vec4 inModelView3;
 * ..
 *
 * void main()
 * {
 *   ..
 *   vec4 posVS;
 *   posVS.x = dot(inModelView0, inPosition);
 *   posVS.y = dot(inModelView1, inPosition);
 *   ..
 *   gl_Position = projection * posVS;
 * }
 * \endcode
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
  void addElement(unsigned int _type, unsigned int _numElements, VERTEX_USAGE _usage, const void* _pointer, const char* _shaderInputName = 0, unsigned int _divisor = 0, unsigned int _vbo = 0);
  
  /*! append one element to declarations, direct method
  */
  void addElement(unsigned int _type, unsigned int _numElements, VERTEX_USAGE _usage, size_t _byteOffset = 0, const char* _shaderInputName = 0, unsigned int _divisor = 0, unsigned int _vbo = 0);

  /*! append multiple element declarations
  */
  void addElements(unsigned int _numElements, const VertexElement* _pElements);

  /*! remove all vertex elements, also clears the user defined stride flag
  */
  void clear();

  /*! prepare OpenGL to use a vertex buffer with this declaration
  -> uses the fixed function pointers (glVertexPointer, glColorPointer...)
  */
  void activateFixedFunction() const;

  /*! disables vertex attributes to prevent any draw related crashes
  */
  void deactivateFixedFunction() const;


  /*! prepare OpenGL to use a vertex buffer with this declaration
  -> uses shader attribute pointers ( glVertexAttribPointer )
  */
  void activateShaderPipeline(GLSL::Program* _prog) const;


  /*! disables this vertex declaration to prevent any draw related crashes
  -> calls glDisableVertexAttribArray
  */
  void deactivateShaderPipeline(GLSL::Program* _prog) const;


  /*! set vertex stride manually, otherwise it is computed
  from the vertex elements
  */
  void setVertexStride(unsigned int _stride);

  /*! get vertex size (for element i, for multi vbo support)
  */
  unsigned int getVertexStride(unsigned int i = 0) const;

  /*! get num of vertex elements
  */
  unsigned int getNumElements() const;

  /*! get the i'th vertex element desc
  */
  const VertexElement* getElement(unsigned int i) const;

  /*! find element id by usage, return -1 if not found
  */
  int findElementIdByUsage(VERTEX_USAGE _usage) const;

  /*! find element ptr by usage, return 0 if not found
  */
  const VertexElement* findElementByUsage(VERTEX_USAGE _usage) const;

  /*! get size of GL_FLOAT, GL_INT ...
  */
  static unsigned int getGLTypeSize(unsigned int _type);

  /*! get size of one vertex element
  */
  static unsigned int getElementSize(const VertexElement* _pElement);

  /*! Check hw support for streaming instance data from vbo
  */
  static bool supportsInstancedArrays();

  /*! Returns a string describing the vertex format for debugging purpose.
  */
  QString toString() const;

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

  /// Map vbo to offset in bytes between each vertex in that vbo
  std::map<unsigned int, unsigned int> vertexStridesVBO_;

  /// Flag that indicates, whether the stride was set by user or derived automatically
  int strideUserDefined_;
};


//=============================================================================
} // namespace ACG
//=============================================================================



#endif // ACG_VERTEXDECLARATION_HH defined

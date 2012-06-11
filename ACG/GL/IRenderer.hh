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


#include <ACG/GL/gl.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/GL/ShaderGenerator.hh>

namespace ACG
{

class VertexDeclaration;
class GLState;

/** \brief Interface class between scenegraph and renderer
 *
 * RenderObject is the primary interface between scenegraph and renderer.
 *
 * Scenegraph nodes have to declare any renderable geometry objects by providing:
 * - geometry type (triangles, lines, points, triangle strips, ...)
 * - geometry buffers (vertex/index buffers,  vbo, ibo, sysmem and non-indexed supported )
 * - vertex buffer layout ( via vertex declaration, eventually declare sysmem vertex buffer here )
 * - draw mode
 * - OpenGL render states
 * - material properties
 * - texture
 *
 * Quick initialization for default values is recommended.
 * RenderObject::initFromState() grabs transforms, material and render states from a GLState.
 *
 * An OpenGL style interface for geometry setup is available via:
 * RenderObject::glBindBuffer()
 * RenderObject::glDrawArrays()
 * RenderObject::glDrawArrayElements()
 *
 * You still have to create the VertexDeclaration on your own first though.
 *
 * Note that each RenderObject corresponds to exactly one deferred draw call.
*/
struct ACGDLLEXPORT RenderObject
{
  /** \brief Priority to allow sorting of objects
   *
   * The renderer sorts objects based on priority from high to low before rendering.
   *
   * \note negative values allowed
   */
  int priority;


  /// Modelview transform
  GLMatrixf modelview;

  /// Projection transform
  GLMatrixf proj;


  //===========================================================================
  /** @name Geometry definition
   *
   * @{ */
  //===========================================================================

  /// VBO, IBO ids
  GLuint vertexBuffer,
         indexBuffer;

  /** \brief Use system memory index buffer
   *
   * If you don't want to use an IBO, simply assign your sysmem indexbuffer address here.
   * If both indexBuffer and sysmemIndexBuffer are 0, the renderer will treat this
   * RenderObject like an unfolded vertex buffer (e.g. 3 * nTris vertex buffer for GL_TRIANGLES)
   *
   * \note  It is also possible to specify a sysmem vertex buffer by setting up the VertexDeclaration accordingly use numIndices
   */
  const void* sysmemIndexBuffer;

  /** \brief Primitive type
   *
   *  PrimitiveType must be one of the following:
   *  GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP,
   *  GL_TRIANGLE_FAN, GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, GL_POLYGON
   */
  GLenum primitiveMode;

  /// Number indices to render
  unsigned int numIndices;

  /// Offset to first index in the index buffer or vertex buffer respectively
  unsigned int indexOffset;

  /** \brief Index element type
   *
   * Has to be GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
   */
  GLenum indexType;


  /// Defines the vertex buffer layout
  const VertexDeclaration* vertexDecl;

  /** @} */



  /** \brief Drawmode and other shader params
   *
   * - setup the shade mode :                         shaderDesc.shadeMode = ..  (necessary)
   * - enable lighting with all lights in the scene:  shaderDesc.numLights =  0  (recommended)
   * - disable lighting(only removes lighting code):  shaderDesc.numLights = -1
   * - manual lighting setup:                         shaderDesc.numLights = n
   *                                                  shaderDesc.lightTypes[i] = ..
   * - enable / disable texturing :                   shaderDesc.textured = ..
   * ...
   *
   */
  ShaderGenDesc shaderDesc;

  // opengl states
  //  queried from glState in initFromState()
  bool culling;
  bool blending;
  bool alphaTest;
  bool depthTest;
  bool depthWrite;

//  GLenum shadeModel; // GL_FACE, GL_SMOOTH   obsolute in shader pipeline
  GLenum depthFunc;  //!< GL_LESS, GL_LEQUAL, GL_GREATER ..

  GLenum blendSrc, blendDest; //!< glBlendFunc: GL_SRC_ALPHA, GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA ...

  Vec2f depthRange; //!< glDepthRange: (znear, zmax)

  // ---------------------------
  /// material definitions
  Vec3f diffuse,
        ambient,
        specular,
        emissive;

  float alpha,
        shininess;



  /** \brief Texture to be used
   *
   * eventually a more flexible texture system with user defined:
   * - texture stage binding slot (0 .. 16)
   * - texture type (1D, 2D, 3D, rect, cube)
   * - array of textures
   * assumes binding slot 0 and 2D for now
   */
  GLuint texture;





  // opengl style helper function interface: 
  // provided for easier setup of RenderObjects,
  //  usage is not necessary
  void glBindBuffer(GLenum target, GLuint buffer)
  {
    switch (target)
    {
    case GL_ARRAY_BUFFER: vertexBuffer = buffer; break;
    case GL_ELEMENT_ARRAY_BUFFER: indexBuffer = buffer; break;
    }
  }
  void glDrawArrays(GLenum mode, GLint first, GLsizei count)
  {
    indexBuffer = 0;
    sysmemIndexBuffer = 0;


    primitiveMode = mode;
    indexOffset = first;
    numIndices = count;
  }
  void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
  {
    primitiveMode = mode;
    numIndices = count;
    indexType = type;

    sysmemIndexBuffer = indices;
  }

  
  /** \brief Initializes a RenderObject instance.
   *
   * Grabs material and transforms automatically if a GLState is provided.
  */
  void initFromState(GLState* _glState);
};



class IRenderer
{
public:
  IRenderer(){}
  virtual ~IRenderer(){}


  /** \brief Adds a render object to the current list.
   * Creates another RenderObject instance internally, safe for temporary local RenderObject instances.
   * However, the VertexDeclaration address must be permanently valid.
   * This function gets called from within the Scenegraph nodes.
  */
  virtual void addRenderObject(RenderObject* _renderObject) = 0;

protected:

};

/** \page NonFixedPipelinePage The non-fixed OpenGL rendering pipeline
On this page we will explain how to use the non-fixed (modern) OpenGL rendering pipeline in OpenFlipper.
First let us introduce the necessary classes.

\section baseNode The BaseNode in the Scenegraph

\section iRenderer The IRenderer interface
The IRenderer interface can be derived from and used to attach a RenderObject via the addRenderObject function from within each node within the Scenegraph that is
to be rendered.

\subsection renderObject Defining RenderObjects
The RenderObject class can be used to define the vertex buffer objects and set further information for the scene nodes.

\section vertexDeclaration Defining the layout of a vertex element
The VertexDeclaration class is used to specify the layout of a vertex element (VertexElement)

\section shaderGenerator Generating GLSL shaders

The ShaderGenerator class can be used to generate default GLSL shader code.
\note The main function is not generate and must therefore be provided via the buildShaderCode function

Via shader specific information stored in the ShaderGenDesc struct in the RenderObject attached to a scene node, we can specify the shader input and output
with the functions ShaderGenerator::initVertexShaderIO and ShaderGenerator::initFragmentShaderIO.

\subsection shaderCache The Shader Cache
The ShaderCache singleton.

\subsection glslShader The GLSL Shader
The Shader class is a helper class for building and using GLSL programs
*/

//=============================================================================
} // namespace ACG
//=============================================================================

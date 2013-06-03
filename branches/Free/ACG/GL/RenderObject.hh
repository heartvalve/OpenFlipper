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
 *   $Revision: 15868 $                                                       *
 *   $Author: tenter $                                                      *
 *   $Date: 2012-11-26 12:37:58 +0100 (Mo, 26 Nov 2012) $                   *
 *                                                                           *
\*===========================================================================*/

#pragma once


#include <ACG/GL/gl.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/GL/ShaderGenerator.hh>

#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/Scenegraph/MaterialNode.hh>

#include <map>


namespace GLSL{
  class Program;
}

namespace ACG
{

// forward declaration
class VertexDeclaration;
class GLState;

namespace SceneGraph {
  namespace DrawModes {
    class DrawModeProperties;
  }
  class Material;
}


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
  /** default constructor
   *   set all members to 0
   *   keep renderobject a POD to avoid possible problems
   */
  RenderObject();

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
   *  GL_TRIANGLE_FAN, GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, GL_POLYGON,
   *  GL_TRIANGLES_ADJACENCY, GL_LINES_ADJACENCY
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

  GLenum fillMode; // GL_POINT, GL_LINE, GL_FILL,  default: GL_FILL

  GLboolean colorWriteMask[4]; // {r,g,b,a},  default: all true

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
  struct Texture
  {
    GLuint id;
    GLenum type;
    bool shadow;
    Texture():
      id(0),
      type(GL_TEXTURE_2D),
      shadow(false){}
  };


  /// adds a texture to stage RenderObjects::numTextures()
  void addTexture(const Texture& _t)
  {
    addTexture(_t,numTextures());
  }

  /**
   * adds a texture to an specific stage and enables texture support in shaderDesc
   */
  void addTexture(const Texture& _t,const unsigned int _stage)
  {
    if (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS < numTextures())
    {
      std::cerr << "Texturestage " << _stage << " is too big. Allowed stages: "<< GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;
      return;
    }
    textures_[_stage] = _t;
    shaderDesc.addTextureType(_t.type,_t.shadow,_stage);
  }

  ///clear all textures. Also affected on shaderDesc
  void clearTextures() {textures_.clear(); shaderDesc.clearTextures();}

  const std::map<unsigned,Texture>& textures(){return textures_;}

  size_t numTextures()  {return textures_.size();}

private:
  /// holds the textures (second) and the stage id (first)
  std::map<unsigned,Texture> textures_;
public:


  /// used internally for renderer debugging
  int debugID;
  const char* debugName;

  /// may be used internally by the renderer
  unsigned int internalFlags_;


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

    setGeometryShaderInputFromDrawCall(mode);
  }

  void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
  {
    primitiveMode = mode;
    numIndices = count;
    indexType = type;

    sysmemIndexBuffer = indices;
    setGeometryShaderInputFromDrawCall(mode);
  }

  void glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
  {
    colorWriteMask[0] = r; colorWriteMask[1] = g; colorWriteMask[2] = b; colorWriteMask[3] = a;
  }
  
  /** \brief Initializes a RenderObject instance.
   *
   * Grabs material and transforms automatically if a GLState is provided.
  */
  void initFromState(GLState* _glState);

  void setMaterial(const SceneGraph::Material* _mat);

  /** \brief Fills out ShaderGenDesc parameters based on Drawmode properties
  */
  void setupShaderGenFromDrawmode(const SceneGraph::DrawModes::DrawModeProperties* _props);


  /** \brief Whenever the need for glBegin() glEnd() immediate mode arises,
       this can be implemented by a deriving class of RenderObject.
       Also it gets called only if numIndices is set to 0.
       glBegin and glEnd have to be called in here.
  */
  virtual void executeImmediateMode();

  /** Returns a text representation of the RenderObject for debugging purposes.
  */
  QString toString() const;

  /** Used by the draw functions. Sets the correct input primitive type for the geometry shader.
   *
   */
  void setGeometryShaderInputFromDrawCall(GLenum _mode);

  /** \brief set values for uniforms
   *
   * @param _uniformName Name of uniform in shader
   * @param _dataType    Type of value (GL_INT, GL_FLOAT)
   * @param _value       QVariant encapsuled value of the type
   *
   */
  void addUniformValue(QString _uniformName, unsigned int _dataType, QVariant _value) {
    additionalUniforms_[_uniformName] = QPair<unsigned int, QVariant>( _dataType, _value );
  }

  QMap<QString, QPair<unsigned int, QVariant> > additionalUniforms_;
};

//=============================================================================
} // namespace ACG
//=============================================================================

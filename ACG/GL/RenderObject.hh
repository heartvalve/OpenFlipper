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
 *   $Revision: 15868 $                                                       *
 *   $Author: tenter $                                                      *
 *   $Date: 2012-11-26 12:37:58 +0100 (Mo, 26 Nov 2012) $                   *
 *                                                                           *
\*===========================================================================*/

#pragma once


#include <ACG/GL/gl.hh>
#include <ACG/Math/GLMatrixT.hh>
#include <ACG/GL/ShaderGenerator.hh>
#include <ACG/ShaderUtils/UniformPool.hh>

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
  friend class IRenderer;

  /** default constructor
   *   set all members to OpenGL default values
   *   keep renderobject a POD to avoid possible problems (update: it's not anymore, nevermind then..)
   */
  RenderObject();

  ~RenderObject();


  /** \brief Priority to allow sorting of objects
   *
   * The renderer sorts objects based on priority in ascending order before rendering.
   *
   * \note negative values allowed
   */
  int priority;

  /** \brief Layer based rendering
   *
   * The renderer currently supports two layers:
   *  - scene layer
   *  - overlay layer
   *
   * Usually a render plugin operates on the scene layer only and
   * overlayed objects are rendered on top of the result.
   * For instance, meshes are rendered in the scene layer while coordsys objects are overlayed.
   *
   * \note default = false
   */
  bool overlay;

  /// Modelview transform
  GLMatrixd modelview;

  /// Projection transform
  GLMatrixd proj;


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

//  GLenum shadeModel; // GL_FACE, GL_SMOOTH   obsolete in shader pipeline
  GLenum depthFunc;  //!< GL_LESS, GL_LEQUAL, GL_GREATER ..

  // alpha testing function
  GLenum alphaFunc; //!< GL_LESS, GL_LEQUAL, GL_GREATER ..
  float alphaRef; // reference value for alpha function

  // alpha blending
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


  /** \brief Specify whether this object should be rendered in a z-prepass
   *
   * The renderer might do a z-prepass for some rendering techniques.
   * You can control whether an object should be taken into account for a z-prepass or not.
   * For instance, scene objects should be rendered in the z-prepass, but overlays (coordsys, selection stuff..) should not.
   *
   * default: true
   */
  bool inZPrePass;

  /** \brief Uniform name of the depth map in the used shader
   *
   * If a shader used by this object requires a depth map of the scene, you can specify the name of the texture sampler uniform used here.
   * This depth map is automatically computed in a z-prepass of the scene later in the renderer and assigned to the shader.
   * It will be a 2D texture storing the values of the depth buffer (gl_FragCoord.z) in GL_TEXTURE_2D, GL_R32F format.
   * Should be set to 0 if the used shader does not require a depth map.
   *
   * default: 0
   */
  const char* depthMapUniformName;


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
  void addTexture(const Texture& _t,const size_t _stage, bool _addToShaderGen = true)
  {
    if (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS < numTextures())
    {
      std::cerr << "Texturestage " << _stage << " is too big. Allowed stages: "<< GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;
      return;
    }
    textures_[_stage] = _t;
    if (_addToShaderGen)
      shaderDesc.addTextureType(_t.type,_t.shadow,_stage);
  }

  ///clear all textures. Also affected on shaderDesc
  void clearTextures() {textures_.clear(); shaderDesc.clearTextures();}

  const std::map<size_t,Texture>& textures(){return textures_;}

  size_t numTextures()  {return textures_.size();}

private:
  /// holds the textures (second) and the stage id (first)
  std::map<size_t,Texture> textures_;
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

  }

  void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
  {
    primitiveMode = mode;
    numIndices = count;
    indexType = type;

    sysmemIndexBuffer = indices;
  }

  void glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
  {
    colorWriteMask[0] = r; colorWriteMask[1] = g; colorWriteMask[2] = b; colorWriteMask[3] = a;
  }

  void glAlphaFunc(GLenum func, float ref)
  {
    alphaFunc = func;
    alphaRef = ref;
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


  /** \brief Setup rendering of thick lines
   *
   * Two default rendering methods for line thickness are available:
   *  - quad extrusion in geometry shader   (anti-aliasing,  clipping issue due to depth testing with scene)
   *  - manual rasterization via shader image load/store  (no anti-aliasing,  no clipping issue,  requires gl4.2)
   * The method used depends on renderer settings.
   *
   * Also, this will overwrite the geometry and fragment shader template.
   * Requires support for geometry shaders with glsl 150
  */
  void setupLineRendering(float _lineWidth, const Vec2f& _screenSize);

  /// Test if the object is rendered with one of the default line thickness methods
  bool isDefaultLineObject() const;

  /** \brief Setup rendering of circle points
   *
   * Use default quad extrusion shader.
   * This will overwrite geometry and fragment shader template.
   * Requires support for geometry shaders with glsl 150
  */
  void setupPointRendering(float _pointSize, const Vec2f& _screenSize);

  /// Test if the object is rendered with one of the default point extension methods
  bool isDefaultPointObject() const;



  /** Returns a text representation of the RenderObject for debugging purposes.
  */
  QString toString() const;

  /** \brief set values for int uniforms
   *
   * @param _name        Name of uniform in shader
   * @param _value       value of the type
   *
   */
  void setUniform(const char *_name, GLint _value);

  /** \brief set values for float uniforms
     *
     * @param _name        Name of uniform in shader
     * @param _value       value of the type
     *
     */
  void setUniform(const char *_name, GLfloat _value);

  /** \brief set values for Vec2f uniforms
   *
   * @param _name        Name of uniform in shader
   * @param _value       value of the type
   *
   */
  void setUniform(const char *_name, const ACG::Vec2f &_value);

  /** \brief set values for Vec3f uniforms
   *
   * @param _name        Name of uniform in shader
   * @param _value       value of the type
   *
   */
  void setUniform(const char *_name, const ACG::Vec3f &_value);

  /** \brief set values for Vec4f uniforms
   *
   * @param _name        Name of uniform in shader
   * @param _value       value of the type
   *
   */
  void setUniform(const char *_name, const ACG::Vec4f &_value);

  void setUniform(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);
  void setUniformMat3(const char *_name, const ACG::GLMatrixf &_value, bool _transposed = false);


  void setUniform(const char *_name, GLint *_values, int _count);
  void setUniform(const char *_name, GLfloat *_values, int _count);


  /** \brief add all uniforms from a pool
   *
   * @param _pool input pool
   *
   */
  void addUniformPool(const GLSL::UniformPool& _pool);

private:
  GLSL::UniformPool uniformPool_;
};


/** \brief Interface for modifying render objects
 *
 * This class has to be implemented by a user, 
 * and could be set to nodes that allow modification of render-objects.
 * The modifier is then applied directly before adding an object to a renderer.
 * It allows low-level access to all settings in a render-objects.
*/
class ACGDLLEXPORT RenderObjectModifier
{
public:
  RenderObjectModifier() {}
  virtual ~RenderObjectModifier() {}

  /** \brief apply the modifier
   *
   * @param _obj       modifiable render-object
   *
   */
  virtual void apply(RenderObject* _obj) = 0;

};



//=============================================================================
} // namespace ACG
//=============================================================================

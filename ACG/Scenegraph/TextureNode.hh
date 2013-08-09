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




//=============================================================================
//
//  CLASS TextureNode
//
//=============================================================================


#ifndef ACG_TEXTURE_NODE_HH
#define ACG_TEXTURE_NODE_HH


//== INCLUDES =================================================================


#include "BaseNode.hh"

#include <string>
#include <QImage>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class TextureNode TextureNode.hh <ACG/Scenegraph/TextureNode.hh>

    Set Texture for this node and all its
    children.  All changes will be done in the enter() method undone
    in the leave() method.
    Enable Environment Mapping if you need it. To use Blending or
    AlphaTest use a MaterialNode.
**/

class ACGDLLEXPORT TextureNode : public BaseNode
{
public:

  /// Default constructor. Applies all properties.
  TextureNode( BaseNode*           _parent = 0,
	            const std::string&  _name = "<TextureNode>",
               bool                _texture_repeat = true,
               GLint               _texture_filter = GL_LINEAR );

  /// Destructor.
  virtual ~TextureNode();

  /// set class name
  ACG_CLASSNAME(TextureNode);

  /// get transparency status
  bool alpha() { return alpha_; }

  /// set transparency status
  void set_alpha(bool _status) { alpha_ = _status; }

  /// get texture repeat status
  bool repeat() { return texture_repeat_; }

  /// set texture repeat status
  void set_repeat(bool _status) { texture_repeat_ = _status; }

  /// get texture filter
  GLint filter() { return texture_filter_; }

  /// set texture filter
  void set_filter( GLint _filter ) { texture_filter_ = _filter; }

  void set_texture_mode( GLenum _mode) { tex_mode_ = _mode; }
  
  /// Enable mipmapping
  void enable_mipmapping();
  
  /// Disable mipmapping
  void disable_mipmapping();
  
  /// Get mipmapping status
  bool mipmapping() const { return mipmapping_; }


//===========================================================================
/** @name Todo
  * @{ */
//===========================================================================

/** @} */

//===========================================================================
/** @name Setting textures
  * @{ */
//===========================================================================

public:


  /** \brief Add a texture to this node
   *
   */
  GLuint add_texture(const QImage& _image);

  private:
    /** Apply the internal texture properties to the currently active texture
     */
    void applyTextureParameters( int _id );

    /** Set basic gl settings
     */
    void applyGLSettings(  );

/** @} */

//===========================================================================
/** @name Change active texture
  * @{ */
//===========================================================================

  public :

  /** \brief Set active Texture
   *
   * Sets the active texture of this node to the given id
   *
   */
  bool activateTexture(GLuint _id);

  /** \brief Get active Texture
   *
   * Gets the currently active texture of this node
   *
   */
  GLuint activeTexture();

  /** \brief Uses a QImage to load the texture from the given file.
  *
  * This function will change the active texture.\n
  *
  */
  bool read(const char* _filename);

  /** \brief Uses a QImage to set the texture.
  *
  * This function will change the active texture.\n
  *
  */
  void set_texture(const QImage& _image);

  /** \brief Uses a float buffer to set the texture.
  *
  * This function will change the active texture.\n
  *
  * Assumptions: Buffer is in RGBA format, and either power of two size or
  * using a graphics card that supports arbitrary textures.
  */
  void set_texture(const float * _image, int _width, int _height);


  /** \brief Uses a byte buffer to set the texture.
  *
  * This function will change the active texture.\n
  *
  * Assumptions: Buffer is in RGBA format, and either power of two size or
  * using a graphics card that supports arbitrary textures.
  */
  void set_texture(const unsigned char * _image, int _width, int _height);

/** @} */

//===========================================================================
/** @name Change specific texture
  * @{ */
//===========================================================================

  public :

  /** \brief Uses a QImage to load the texture from the given file.
  *
  * This function will change the given texture if available.\n
  *
  */
  bool read(const char* _filename, GLuint _id );

  /** \brief Uses a QImage to set the texture.
  *
  * This function will change the given texture if available.\n
  *
  */
  void set_texture(const QImage& _image, GLuint _id);

  /** \brief Uses a float buffer to set the texture.
  *
  * This function will change the given texture if available.\n
  *
  * Assumptions: Buffer is in RGBA format, and either power of two size or
  * using a graphics card that supports arbitrary textures.
  */
  void set_texture(const float * _image, int _width, int _height, GLuint _id);


  /** \brief Uses a byte buffer to set the texture.
  *
  * This function will change the given texture if available.\n
  *
  * Assumptions: Buffer is in RGBA format, and either power of two size or
  * using a graphics card that supports arbitrary textures.
  */
  void set_texture(const unsigned char * _image, int _width, int _height, GLuint _id);

/** @} */

//===========================================================================
/** @name SceneGraph traversal functions
  * @{ */
//===========================================================================

public:
  /** \brief set default texture and states for the nodes children
   */
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /** \brief Unbind Texture
   */
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /** \brief Do nothing in picking
   */
  void enterPick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );

  /** \brief Do nothing in picking
   */
  void leavePick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );

/** @} */

private:

  class TextureInfo {
    public:
      TextureInfo():
      id(0),
      mipmapAvailable(false),
      width(0),
      height(0) {}

      GLuint id;
      GLenum target;
      bool mipmapAvailable;
      int width;
      int height;
  };

  void setTextureDataGL ( GLuint _textureId,
                        GLenum _target,
                        GLint _width ,
                        GLint _height,
                        GLenum _format ,
                        GLenum _type,
                        const void * _data) ;

  std::vector<TextureInfo> textures_;
  bool                     alpha_;
  bool                     texture_repeat_;
  GLenum                   tex_mode_;
  GLint                    texture_filter_;
  
  bool                     mipmapping_globally_active_;
  bool                     last_mipmapping_status_;
  bool                     mipmapping_;

  /** \brief Check if a texture is already generated by this Node
   *
   * This function will create a texture if no texture exists and set the activeTexture_
   * to the new one.
   */
  void checkEmpty();

  /** \brief check this node for a texture
   *
   * Returns the id of the texture if this node handles the texture.\n
   * If not -1.
   */
  int available( GLuint _id  );
  
  /** \brief Build mip maps of textures that don't have one yet
   */
  void updateMipmaps(bool _mipmap);

  /** \brief currently active texture
   *
   * This index is an index to the textures_ vector! Not the GLuint!!
   */
  int activeTexture_;

  /** \brief OpenVolumeMesh DrawModes using textures
   *
   * Texturing will be activated for all DrawModes which are combined in this DrawMode
   */
  DrawModes::DrawMode open_volume_mesh_texture_draw_modes_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TEXTURE_NODE_HH defined
//=============================================================================


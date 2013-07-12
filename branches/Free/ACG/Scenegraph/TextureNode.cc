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
//  CLASS TextureNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "TextureNode.hh"
#include "DrawModes.hh"
#include <QGLWidget>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


TextureNode::TextureNode( BaseNode*            _parent,
                          const std::string&   _name,
                          bool                 _texture_repeat,
                          GLint                _texture_filter )
      : BaseNode( _parent, _name ),
        textures_(),
        alpha_( 0 ),
        texture_repeat_( _texture_repeat ),
        tex_mode_( GL_MODULATE ),
        texture_filter_( _texture_filter ),
        mipmapping_globally_active_(true),
        last_mipmapping_status_(true),
        mipmapping_(true),
        activeTexture_(-1)
{
}


//----------------------------------------------------------------------------


TextureNode::~TextureNode()
{
  for (std::vector<TextureInfo>::iterator texturesIt = textures_.begin(); texturesIt != textures_.end(); ++texturesIt) {
    if ( glIsTexture( texturesIt->id ) ) {
      glDeleteTextures( 1, &(texturesIt->id) );
    }
  }
  textures_.clear();
}



//----------------------------------------------------------------------------


bool
TextureNode::read(const char* _filename)
{
   // load to image
   QImage image;
   if ( !image.load( _filename ) )
   {
      std::cerr << "Cannot load texture " << _filename << "\n";
      return false;
   }

   set_texture( image );

   return true;
}


//----------------------------------------------------------------------------


void
TextureNode::applyGLSettings(  )
{
  // GL settings
  glPixelStorei( GL_UNPACK_ROW_LENGTH,  0 );
  glPixelStorei( GL_UNPACK_SKIP_ROWS,   0 );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
  glPixelStorei( GL_UNPACK_ALIGNMENT,   1 );
  glPixelStorei( GL_PACK_ROW_LENGTH,    0 );
  glPixelStorei( GL_PACK_SKIP_ROWS,     0 );
  glPixelStorei( GL_PACK_SKIP_PIXELS,   0 );
  glPixelStorei( GL_PACK_ALIGNMENT,     1 );
}

void
TextureNode::applyTextureParameters( int _id )
{
  if ( texture_repeat_ ) {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  } else {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    // Set BorderColor for Clamping
    const float borderColor[4] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
  }
  
  if((mipmapping_globally_active_ && mipmapping_ && textures_[_id].mipmapAvailable) &&
     (texture_filter_ == GL_LINEAR_MIPMAP_NEAREST ||
      texture_filter_ == GL_LINEAR_MIPMAP_LINEAR  ||
      texture_filter_ == GL_NEAREST_MIPMAP_LINEAR ||
      texture_filter_ == GL_NEAREST_MIPMAP_NEAREST)) {
      
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter_ );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
      
  } else if(texture_filter_ == GL_LINEAR) {
      
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  
  } else {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  }
  
}

//----------------------------------------------------------------------------

void TextureNode::enable_mipmapping() {
    
    if(!mipmapping_) {
        mipmapping_ = true;
        updateMipmaps(mipmapping_globally_active_);
    }
}

//----------------------------------------------------------------------------

void TextureNode::disable_mipmapping() {
    
    if(mipmapping_) {
        mipmapping_ = false;
        updateMipmaps(mipmapping_);
    }
}

//----------------------------------------------------------------------------

void TextureNode::setTextureDataGL (  GLuint _textureId,
                                      GLenum _target,
                                      GLint _width ,
                                      GLint _height,
                                      GLenum _format ,
                                      GLenum _type,
                                      const void * _data) {
  
  applyGLSettings();

  // copy texture to GL
  ACG::GLState::bindTexture( GL_TEXTURE_2D, textures_[_textureId].id );

  if ( mipmapping_ )
    textures_[_textureId].mipmapAvailable = true;
  else
    textures_[_textureId].mipmapAvailable = false;

  applyTextureParameters(_textureId);

  // Load the image
  if ( mipmapping_globally_active_ && mipmapping_ ) {
    gluBuild2DMipmaps( _target ,
                       GL_RGBA ,
                       _width ,
                       _height ,
                       _format ,
                       _type,
                       _data);

  } else {
    glTexImage2D(_target,              // target
                  0,                   // level
                  GL_RGBA,             // internal format
                  _width,              // width  (2^n)
                  _height,             // height (2^m)
                  0,                   // border
                  _format,             // format
                  _type,               // type
                  _data );             // pointer to pixels
  }
  
  // Fill texture info struct width information
  textures_[_textureId].target      = _target;
  textures_[_textureId].width       = _width;
  textures_[_textureId].height      = _height;
  
  // Unbind until we use it
  ACG::GLState::bindTexture(GL_TEXTURE_2D,0);
}


//----------------------------------------------------------------------------

/** \brief Generate mipmaps for each texture that does not have one yet
*/
void TextureNode::updateMipmaps(bool _mipmap) {
    
    // Make sure we have at least on element in the textures list
    checkEmpty();

    for(unsigned int i = 1; i < textures_.size(); ++i) {
        
        // Bind texture
        ACG::GLState::bindTexture( GL_TEXTURE_2D, textures_[i].id );
        
        // Get pixel data out of texture memory
        GLubyte* buffer = (GLubyte *)malloc(textures_[i].width*textures_[i].height*4);
        glGetTexImage(textures_[i].target, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            
        if(_mipmap) {
            
            // Build mipmap
            gluBuild2DMipmaps(textures_[i].target, GL_RGBA, textures_[i].width, textures_[i].height,
                              GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            
            // Set mipmap available flag
            textures_[i].mipmapAvailable = true;
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures_[i].width, textures_[i].height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
            
            // Set mipmap available flag
            textures_[i].mipmapAvailable = false;
        }
        
        // Update parameters such that changes apply during runtime
        applyTextureParameters(i);
    }
}

//----------------------------------------------------------------------------


void
TextureNode::set_texture(const unsigned char * _image, int _width, int _height)
{
  checkEmpty();

  // enough texture mem?
  glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  GLint width;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
  if ( width == 0 ) {
    std::cerr << "Can't load texture";
    return;
  }

  setTextureDataGL(activeTexture_,GL_TEXTURE_2D,_width,_height,GL_RGBA,GL_UNSIGNED_BYTE,_image);

}


//----------------------------------------------------------------------------


void
TextureNode::set_texture(const QImage& _image)
{
  checkEmpty();

  int w=_image.width(), h=_image.height();

  // adjust texture size: 2^k * 2^l
  GLint tex_w, tex_h;
  for ( tex_w=1; tex_w <= w; tex_w <<= 1 ){};
  for ( tex_h=1; tex_h <= h; tex_h <<= 1 ){};
  tex_w >>= 1;
  tex_h >>= 1;

  // enough texture mem?
  glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  GLint width;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
  if ( width == 0 ) {
    std::cerr << "Can't load texture TextureNode::set_texture" << std::endl;
    return;
  }

  // Convert Image
  QImage texture( QGLWidget::convertToGLFormat ( _image.scaled( tex_w, tex_h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );

  // Set the image
  setTextureDataGL(activeTexture_,GL_TEXTURE_2D,tex_w,tex_h,GL_RGBA,GL_UNSIGNED_BYTE,texture.bits());

}

//----------------------------------------------------------------------------


void
TextureNode::set_texture(const float * _image, int _width, int _height )
{
  checkEmpty();

  // enough texture mem?
  glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_FLOAT, 0 );
  GLint width;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
  if ( width == 0 ) {
    std::cerr << "Can't load texture TextureNode::set_texture" << std::endl;
    return;
  }

  // Set the image
  setTextureDataGL(activeTexture_,GL_TEXTURE_2D,_width,_height,GL_RGBA,GL_FLOAT,_image);

}

//----------------------------------------------------------------------------

void TextureNode::checkEmpty() {

  // No texture generated yet!
  if ( textures_.empty() ) {
    textures_.resize(1);
    activeTexture_ = 0;
//     textures_[activeTexture_].id = 0;
    glGenTextures( 1, &(textures_[activeTexture_].id ) );
  }

}

//----------------------------------------------------------------------------

int TextureNode::available( GLuint _id  ) {
  // If the texture is found in the array return its id otherwise -1
  for ( uint i = 0 ; i < textures_.size(); ++i )
    if ( textures_[i].id == _id )
      return i;

  return -1;
}

//----------------------------------------------------------------------------

bool TextureNode::read(const char* _filename, GLuint _id ) {
  if ( available(_id) != -1 ) {
    activeTexture_ = available(_id);
    return read(_filename);
  } else
    std::cerr << "Texture with id " << _id << " not handled by this Node!!!" << std::endl;

  return false;

}


//----------------------------------------------------------------------------



void TextureNode::set_texture(const QImage& _image, GLuint _id) {

  checkEmpty();

  if ( available(_id) != -1 ) {
    activeTexture_ = available(_id);
    set_texture(_image);
  } else
    std::cerr << "Texture with id " << _id << " not handled by this Node!!!" << std::endl;

}


//----------------------------------------------------------------------------



void TextureNode::set_texture(const float * _image, int _width, int _height, GLuint _id) {

  checkEmpty();

  if ( available(_id) != -1 ) {
    activeTexture_ = available(_id);
    set_texture(_image,_width,_height);
  } else
    std::cerr << "Texture with id " << _id << " not handled by this Node!!!" << std::endl;
}



//----------------------------------------------------------------------------


void TextureNode::set_texture(const unsigned char * _image, int _width, int _height, GLuint _id) {

  checkEmpty();

  if ( available(_id) != -1 ) {
    activeTexture_ = available(_id);
    set_texture(_image,_width,_height);

  } else
    std::cerr << "Texture with id " << _id << " not handled by this Node!!!" << std::endl;

}


//----------------------------------------------------------------------------


// add QImage _image as additional texture, using face_texture_index
GLuint
TextureNode::add_texture(const QImage& _image)
{
  checkEmpty();

  // adjust texture size: 2^k * 2^l
  int tex_w, w( _image.width()  );
  int tex_h, h( _image.height() );

  for (tex_w=1; tex_w < w; tex_w <<= 1) {};
  for (tex_h=1; tex_h < h; tex_h <<= 1) {};
  if (5 * tex_w > 7 *w)   // tex_w longer than sqrt(2)*w means too much waste of storage space (7/5 = sqrt(2)-1%)
    tex_w >>= 1;
  if (5 * tex_h > 7 *h)   // tex_h longer than sqrt(2)*h means too much waste of storage space (7/5 = sqrt(2)-1%)
    tex_h >>= 1;

  // enough texture mem?
  glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  GLint testWidth;
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &testWidth );
  if ( testWidth == 0 ) {
    std::cerr << "Can't load texture TextureNode::add_texture" << std::endl;
    return 0;  // does not really help
  }

  // because texture will only be accessed proportionally by texture coordinates, aspect ratio is of no concern
  QImage texture( QGLWidget::convertToGLFormat ( _image.scaled( tex_w, tex_h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );

  textures_.resize(textures_.size()+1);  // can't push_back, because glGenTextures needs a pointer

  // Generate new texture
  glGenTextures(1, &( textures_.back().id ) );

  activeTexture_ = textures_.size() - 1;

  // Set the image
  setTextureDataGL(activeTexture_ ,GL_TEXTURE_2D,tex_w,tex_h,GL_RGBA,GL_UNSIGNED_BYTE,texture.bits());

  // return the id of the new texture
  return textures_.back().id;
}


//----------------------------------------------------------------------------


void TextureNode::enter(GLState& _state , const DrawModes::DrawMode& _drawmode)
{
   if ( _drawmode & ( DrawModes::SOLID_TEXTURED |
                      DrawModes::SOLID_TEXTURED_SHADED |
                      DrawModes::SOLID_ENV_MAPPED |
                      DrawModes::SOLID_2DTEXTURED_FACE |
                      DrawModes::SOLID_2DTEXTURED_FACE_SHADED |
                      DrawModes::SOLID_SHADER |

                      //OpenVolumeMesh DrawModes
                      DrawModes::getDrawMode("Faces (textured)") |
                      DrawModes::getDrawMode("Faces (textured and shaded)") ))
   {
      ACG::GLState::enable( GL_TEXTURE_2D );
      
      mipmapping_globally_active_ = _state.mipmapping_allowed();
      
      // Check if mipmapping status has changed
      if(_state.mipmapping_allowed() != last_mipmapping_status_) {
          
              // Update mipmaps
              updateMipmaps(mipmapping_globally_active_ && mipmapping_);
              
              // Keep track of changes
              last_mipmapping_status_ = _state.mipmapping_allowed();
          
      }
      
      if ( !textures_.empty() ) {
        ACG::GLState::bindTexture( GL_TEXTURE_2D, textures_[activeTexture_].id );
      }
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_ );
   }
}


//----------------------------------------------------------------------------


void TextureNode::leave(GLState& /* _state */ , const DrawModes::DrawMode& _drawmode)
{
   if ( _drawmode & ( DrawModes::SOLID_TEXTURED |
                      DrawModes::SOLID_TEXTURED_SHADED |
                      DrawModes::SOLID_ENV_MAPPED |
                      DrawModes::SOLID_2DTEXTURED_FACE |
                      DrawModes::SOLID_2DTEXTURED_FACE_SHADED |
                      DrawModes::SOLID_SHADER |

                      //OpenVolumeMesh DrawModes
                      DrawModes::getDrawMode("Faces (textured)") |
                      DrawModes::getDrawMode("Faces (textured and shaded)") ))
   {
      ACG::GLState::bindTexture( GL_TEXTURE_2D, 0 );
      ACG::GLState::disable( GL_TEXTURE_2D );
   }
}

void TextureNode::enterPick(GLState& /*_state*/ , PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {

}

void TextureNode::leavePick(GLState& /*_state*/, PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {
}

//----------------------------------------------------------------------------


GLuint TextureNode::activeTexture()
{
  return textures_[activeTexture_].id;
}

//----------------------------------------------------------------------------

bool TextureNode::activateTexture(GLuint _id)
{
  int search = available(_id);

  //==========================================================================
  // If zero is given, unbind all textures
  //==========================================================================
  if ( _id == 0 ) {
    search = 0;
  }

  //==========================================================================
  // Index has not been found ... No corresponding Texture in this node
  //==========================================================================
  if ( search == -1 ) {
    std::cerr << "Texture to activate not found!" << std::endl;
    return false;
  }

  activeTexture_ = search;

  return true;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

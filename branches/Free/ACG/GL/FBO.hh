//=============================================================================
//
//  CLASS FBO
//
// Author:  Dominik Sibbing <sibbing@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Date: 2008$
//
//=============================================================================


#ifndef ACG_FBO_HH
#define ACG_FBO_HH


//== INCLUDES =================================================================

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>

#include <ACG/GL/gl.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class FBO FBO.hh <ACG/.../FBO.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class ACGDLLEXPORT FBO
{
public:
  
  // fbo_ braucht initialen Wert.
  /// Default constructor
  FBO();
  
  /// Destructor
  ~FBO();
  
  /// function to generate the framebuffer object
  void init();

  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment,
    GLsizei _width, GLsizei _height, 
    GLuint _internalFmt, GLenum _format, 
    GLint _wrapMode = GL_CLAMP,
    GLint _minFilter = GL_NEAREST,
    GLint _magFilter = GL_NEAREST);

  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment, GLuint _texture );

  /// function to attach a depth-buffer texture to fbo (using GL_DEPTH_ATTACHMENT)
  void attachTexture2DDepth( GLsizei _width, GLsizei _height, GLuint _internalFmt = GL_DEPTH_COMPONENT32, GLenum _format = GL_DEPTH_COMPONENT );

  /// function to add a depth buffer to the fbo
  void addDepthBuffer( GLuint _width, GLuint _height );
  
  /// function to add a stencil buffer to the fbo
  void addStencilBuffer( GLuint _width, GLuint _height );

  /// return attached texture id
  GLuint getAttachment( GLenum _attachment );

  /// return opengl id
  GLuint getFboID();

  /// resize function (if textures created by this class)
  void resize(GLsizei _width, GLsizei _height);
  
  /// bind the fbo and sets it as rendertarget
  bool bind();
  
  /// unbind fbo, go to normal rendering mode 
  void unbind();
  
  /// function to check the framebuffer status
  bool checkFramebufferStatus();

private:

  /// handle of frame buffer object
  GLuint fbo_;

  /// depthbuffer
  GLuint depthbuffer_;
  
  /// stencilbuffer
  GLuint stencilbuffer_;

  /// attached textures
  std::map<GLenum, GLuint> attachments_; // key: attachment index

  struct RenderTexture
  {
    GLuint id;
    GLenum internalFormat, format;
  };
  /// textures created by this class
  std::vector<RenderTexture> internalTextures_;

  /// width and height of render textures
  GLsizei width_, height_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_FBO_HH defined
//=============================================================================


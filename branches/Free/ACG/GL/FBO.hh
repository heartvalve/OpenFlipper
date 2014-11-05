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

#include <ACG/Config/ACGDefines.hh>
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

  /// enable/disable multisampling
  /// returns the MSAA sample count of the fbo, which might not be equal to the requested _samples count but you'll get something close
  GLsizei setMultisampling(GLsizei _samples, GLboolean _fixedsamplelocations = GL_TRUE);

  /// get number of samples
  GLsizei getMultisamplingCount() const {return samples_;}

  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment,
    GLsizei _width, GLsizei _height, 
    GLuint _internalFmt, GLenum _format, 
    GLint _wrapMode = GL_CLAMP,
    GLint _minFilter = GL_NEAREST,
    GLint _magFilter = GL_NEAREST);


  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment, GLuint _texture, GLenum _target = GL_TEXTURE_2D );

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
  void resize(GLsizei _width, GLsizei _height, bool _forceResize = false);

  /// get width of fbo texture
  GLsizei width() const {return width_;}

  /// get height of fbo texture
  GLsizei height() const {return height_;}
  
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
  typedef std::map<GLenum, std::pair<GLuint, GLenum> > AttachmentList;
  AttachmentList attachments_; // key: attachment index,  value: <tex_id, target>

  struct RenderTexture
  {
    // opengl buf id
    GLuint id;

    // GL_TEXTURE_2D, GL_TEXTURE_2D_MULTISAMPLE..
    GLenum target;

    GLenum internalFormat, format;
  };
  /// textures created by this class
  std::vector<RenderTexture> internalTextures_;

  /// width and height of render textures
  GLsizei width_, height_;

  /// sample count if multisampling
  GLsizei samples_;

  /// enable fixed sample location if multisampling
  GLboolean fixedsamplelocation_;


  /// handle of previously bound fbo
  GLuint prevFbo_;
  GLuint prevDrawBuffer_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_FBO_HH defined
//=============================================================================


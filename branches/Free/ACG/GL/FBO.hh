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
#include <stdlib.h>

#include <ACG/GL/gl.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class FBO FBO.hh <ACG/.../FBO.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class FBO
{
public:
   
  /// Default constructor
  FBO() {}
  
  /// Destructor
  ~FBO();
  
  /// function to generate the framebuffer object
  void init();
  
  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment, GLuint _texture );

  /// function to add a depth buffer to the fbo
  void addDepthBuffer( GLuint _width, GLuint _height );
  
  /// function to add a stencil buffer to the fbo
  void addStencilBuffer( GLuint _width, GLuint _height );
  
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
  

};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_FBO_HH defined
//=============================================================================


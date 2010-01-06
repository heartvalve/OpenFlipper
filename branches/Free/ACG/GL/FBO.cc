//=============================================================================
//
//  CLASS FBO - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "FBO.hh"


//== NAMESPACES ===============================================================

namespace ACG
{

//== IMPLEMENTATION ==========================================================

FBO::
~FBO()
{
   // delete framebuffer object
    glDeleteFramebuffersEXT( 1, &fbo_ );

    // delete renderbuffers
    glDeleteRenderbuffersEXT(1, &depthbuffer_);
    glDeleteRenderbuffersEXT(1, &stencilbuffer_); 
}

//-----------------------------------------------------------------------------

void
FBO::
init()
{
    // Create framebuffer object
    if ( !GLEW_EXT_framebuffer_object )
    {
        std::cout << "Framebuffer object not supported! " << std::endl;
        exit( 1 );
    }

    glGenFramebuffersEXT( 1, &fbo_ );

    // check status
    checkFramebufferStatus();

    // unbind fbo
    unbind();
}

//-----------------------------------------------------------------------------

void
FBO::
attachTexture2D( GLenum _attachment, GLuint _texture )
{
    // bind fbo
    bind();

    // add texture to frame buffer object
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, _attachment, GL_TEXTURE_2D, _texture, 0 );

    // check status
    checkFramebufferStatus();

    // unbind fbo
    unbind();
}

//-----------------------------------------------------------------------------

void
FBO::
addDepthBuffer( GLuint _width, GLuint _height )
{
  // create renderbuffer
  glGenRenderbuffersEXT(1, &depthbuffer_);

  // bind renderbuffer
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer_);

  // malloc
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _width, _height);

  // attach to framebuffer object
  if ( bind() )
     glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer_);

  // check status
  checkFramebufferStatus();

  // normal render mode
  unbind();
}

//-----------------------------------------------------------------------------

void
FBO::
addStencilBuffer( GLuint _width, GLuint _height )
{
  // create renderbuffer
  glGenRenderbuffersEXT(1, &stencilbuffer_);

  // bind renderbuffer
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilbuffer_);

  // malloc
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, _width, _height);

  // attach to framebuffer object
  if ( bind() )
     glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilbuffer_);

  // check status
  checkFramebufferStatus();

  // normal render mode
  unbind();
}



//-----------------------------------------------------------------------------

bool
FBO::
bind()
{
    if ( !fbo_ ) 
       return false;

    // bind framebuffer object
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo_ );

    return true;
}

//-----------------------------------------------------------------------------

void
FBO::
unbind()
{
    //set to normal rendering
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

//-----------------------------------------------------------------------------

bool
FBO::
checkFramebufferStatus()
{
    GLenum status;
    status = ( GLenum ) glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    std::cout << "Framebuffer status: " << status << std::endl;
    switch ( status )
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            std::cout << "Framebuffer ok\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            std::cout << " Framebuffer incomplete attachment\n";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            std::cout << "Unsupported framebuffer format\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            std::cout << "Framebuffer incomplete, missing attachment\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            std::cout <<  "Framebuffer incomplete, attached images must have same dimensions\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            std::cout << "Framebuffer incomplete, attached images must have same format\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            std::cout << "Framebuffer incomplete, missing draw buffer\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            std::cout << "Framebuffer incomplete, missing read buffer\n";
            break;
        default:
            std::cout << "Unhandled case\n";
            break;
    }

    return ( status == GL_FRAMEBUFFER_COMPLETE_EXT );
}


//=============================================================================
} // namespace ACG
//=============================================================================

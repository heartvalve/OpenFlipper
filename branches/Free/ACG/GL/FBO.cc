//=============================================================================
//
//  CLASS FBO - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "FBO.hh"
#include "GLState.hh"
#include "GLError.hh"

//== NAMESPACES ===============================================================

namespace ACG
{

//== IMPLEMENTATION ==========================================================



FBO::FBO()
: fbo_(0), depthbuffer_(0), stencilbuffer_(0), width_(0), height_(0), samples_(0), fixedsamplelocation_(GL_TRUE), prevFbo_(0)
{
}

FBO::
~FBO()
{
  // delete framebuffer object
  if(fbo_)
    glDeleteFramebuffersEXT( 1, &fbo_ );

  // delete render buffer
  if(depthbuffer_)
    glDeleteRenderbuffersEXT(1, &depthbuffer_);

  // delete stencil buffer
  if(stencilbuffer_)
    glDeleteRenderbuffersEXT(1, &stencilbuffer_);

  for (size_t i = 0; i < internalTextures_.size(); ++i)
    glDeleteTextures(1, &internalTextures_[i].id);
}

//-----------------------------------------------------------------------------

void
FBO::
init()
{
  // Create framebuffer object
  if (!checkExtensionSupported("GL_EXT_framebuffer_object")) {
    std::cerr << "Framebuffer object not supported! " << std::endl;
    exit( 1 );
  }

  // test whether fbo hasn't been created before
  if(!fbo_)
    glGenFramebuffersEXT( 1, &fbo_ );

  // check status
  checkFramebufferStatus();

  // unbind fbo
  unbind();
}

//-----------------------------------------------------------------------------

void
FBO::
attachTexture2D( GLenum _attachment, GLuint _texture, GLenum _target )
{
  // bind fbo
  bind();

  // add texture to frame buffer object
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, _attachment, _target, _texture, 0 );

  checkGLError();

  // check status
  checkFramebufferStatus();

  // unbind fbo
  unbind();

  // track texture id
  attachments_[_attachment] = std::pair<GLuint, GLenum>(_texture, _target);
}

//-----------------------------------------------------------------------------

void FBO::attachTexture2D( GLenum _attachment, GLsizei _width, GLsizei _height, GLuint _internalFmt, GLenum _format, GLint _wrapMode /*= GL_CLAMP*/, GLint _minFilter /*= GL_LINEAR*/, GLint _magFilter /*= GL_LINEAR*/ )
{
  // gen texture id
  GLuint texID;
  glGenTextures(1, &texID);

#ifdef GL_ARB_texture_multisample
  GLenum target = samples_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#else
  GLenum target = GL_TEXTURE_2D;
#endif // GL_ARB_texture_multisample


  // store texture id in internal array
  RenderTexture intID;
  intID.id = texID;
  intID.internalFormat = _internalFmt;
  intID.format = _format;
  intID.target = target;
  internalTextures_.push_back(intID);


  // specify texture
  glBindTexture(target, texID);

  glTexParameteri(target, GL_TEXTURE_WRAP_S, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, _wrapMode);


#ifdef GL_ARB_texture_multisample
  if (!samples_)
  {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _magFilter);
    glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, GL_FLOAT, 0);
  }
  else
    glTexImage2DMultisample(target, samples_, _internalFmt, _width, _height, fixedsamplelocation_);
#else
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _minFilter);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _magFilter);
  glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, GL_FLOAT, 0);
#endif // GL_ARB_texture_multisample


  checkGLError();

  width_ = _width;
  height_ = _height;

  glBindTexture(target, 0);

  // attach
  attachTexture2D(_attachment, texID, target);
}

//-----------------------------------------------------------------------------

void FBO::attachTexture2DDepth( GLsizei _width, GLsizei _height, GLuint _internalFmt /*= GL_DEPTH_COMPONENT32*/, GLenum _format /*= GL_DEPTH_COMPONENT */ )
{
  // gen texture id
  GLuint texID;
  glGenTextures(1, &texID);

#ifdef GL_ARB_texture_multisample
  GLenum target = samples_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#else
  GLenum target = GL_TEXTURE_2D;
#endif // GL_ARB_texture_multisample

  // store texture id in internal array
  RenderTexture intID;
  intID.id = texID;
  intID.internalFormat = _internalFmt;
  intID.format = _format;
  intID.target = target;
  internalTextures_.push_back(intID);


  // specify texture
  glBindTexture(target, texID);

  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef GL_ARB_texture_multisample
  if (!samples_)
  {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, _format == GL_DEPTH_STENCIL ? GL_UNSIGNED_INT_24_8 : GL_FLOAT, 0);
  }
  else
    glTexImage2DMultisample(target, samples_, _internalFmt, _width, _height, fixedsamplelocation_);

#else
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, _format == GL_DEPTH_STENCIL ? GL_UNSIGNED_INT_24_8 : GL_FLOAT, 0);
#endif // GL_ARB_texture_multisample


  checkGLError();

  width_ = _width;
  height_ = _height;

  glBindTexture(target, 0);

  // attach
  attachTexture2D(GL_DEPTH_ATTACHMENT, texID, target);
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

  // save previous fbo id
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&prevFbo_);

  // bind framebuffer object
  ACG::GLState::bindFramebuffer( GL_FRAMEBUFFER_EXT, fbo_ );

  return true;
}

//-----------------------------------------------------------------------------

void
FBO::
unbind()
{
  //set to normal rendering
  ACG::GLState::bindFramebuffer( GL_FRAMEBUFFER_EXT, prevFbo_ );
}

//-----------------------------------------------------------------------------

bool
FBO::
checkFramebufferStatus()
{
  GLenum status;
  status = ( GLenum ) glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
  //std::cout << "Framebuffer status: " << status << std::endl;
  switch ( status )
  {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      //std::cout << "Framebuffer ok\n";
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

//-----------------------------------------------------------------------------

GLuint FBO::getAttachment( GLenum _attachment )
{
  return attachments_[_attachment].first;
}

//-----------------------------------------------------------------------------

void FBO::resize( GLsizei _width, GLsizei _height, bool _forceResize )
{
  if (_width != width_ ||_height != height_ || _forceResize)
  {
    bool reattachTextures = false;

    // resize every texture stored in internal array
    for (size_t i = 0; i < internalTextures_.size(); ++i)
    {
      RenderTexture* rt = &internalTextures_[i];

#ifdef GL_ARB_texture_multisample
      // check if we have to convert to multisampling
      if (rt->target != GL_TEXTURE_2D_MULTISAMPLE && samples_ > 0)
      {
        rt->target = GL_TEXTURE_2D_MULTISAMPLE;
        reattachTextures = true;
      }
#endif // GL_ARB_texture_multisample

      glBindTexture(rt->target, rt->id);


#ifdef GL_ARB_texture_multisample
      if (!samples_)
        glTexImage2D(rt->target, 0, rt->internalFormat, _width, _height, 0, rt->format, rt->format == GL_DEPTH_STENCIL ? GL_UNSIGNED_INT_24_8 : GL_FLOAT, 0);
      else
        glTexImage2DMultisample(rt->target, samples_, rt->internalFormat, _width, _height, fixedsamplelocation_);

#else
      glTexImage2D(rt->target, 0, rt->internalFormat, _width, _height, 0, rt->format, rt->format == GL_DEPTH_STENCIL ? GL_UNSIGNED_INT_24_8 : GL_FLOAT, 0);
#endif // GL_ARB_texture_multisample

    }

    // store new size
    width_ = _width;
    height_ = _height;



    if (reattachTextures)
    {
      for (AttachmentList::iterator it = attachments_.begin(); it != attachments_.end(); ++it)
      {
        attachTexture2D( it->first, it->second.first, it->second.second );
      }
    }


    glBindTexture(GL_TEXTURE_2D, 0);

  }

}

GLuint FBO::getFboID()
{
  return fbo_;
}

void FBO::setMultisampling( GLsizei _samples, GLboolean _fixedsamplelocations /*= GL_TRUE*/ )
{
  // clamp sample count to max supported
  GLint maxSamples;
  glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

  if (_samples > maxSamples) _samples = maxSamples;

  // issue texture reloading when params changed
  bool reloadTextures = (samples_ != _samples || fixedsamplelocation_ != _fixedsamplelocations);
    
  samples_ = _samples;
  fixedsamplelocation_ = _fixedsamplelocations;


  // force a texture reloading to apply new multisampling
  if (reloadTextures)
    resize(width_, height_, true);
}


//=============================================================================
} // namespace ACG
//=============================================================================

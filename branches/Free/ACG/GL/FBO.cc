//=============================================================================
//
//  CLASS FBO - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <ACG/GL/acg_glew.hh>
#include "FBO.hh"
#include "GLState.hh"
#include "GLError.hh"
#include "GLFormatInfo.hh"

//== NAMESPACES ===============================================================

namespace ACG
{

//== IMPLEMENTATION ==========================================================


FBO::RenderTexture::RenderTexture()
  : id(0), target(0), internalFormat(0), format(0), gltype(0),
  dim(0,0,0), wrapMode(0), minFilter(0), magFilter(0), owner(false)
{
}


FBO::FBO()
: fbo_(0), depthbuffer_(0), stencilbuffer_(0), width_(0), height_(0), samples_(0), fixedsamplelocation_(GL_TRUE), prevFbo_(0), prevDrawBuffer_(GL_NONE)
{
}

FBO::
~FBO()
{
  del();
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
}

//-----------------------------------------------------------------------------

void FBO::del()
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

  for (AttachmentList::iterator it = attachments_.begin(); it != attachments_.end(); ++it)
    if (it->second.id && it->second.owner)
      glDeleteTextures(1, &it->second.id);
}


//-----------------------------------------------------------------------------

void FBO::attachTexture( GLenum _attachment, GLuint _texture, GLuint _level )
{
#ifdef GL_VERSION_3_2
  // bind fbo
  bind();

  // add texture to frame buffer object
  glFramebufferTexture( GL_FRAMEBUFFER_EXT, _attachment, _texture, _level );

//   GLint layered = 0;
//   glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER_EXT, _attachment, GL_FRAMEBUFFER_ATTACHMENT_LAYERED, &layered);

  checkGLError();

  // check status
  checkFramebufferStatus();

  // unbind fbo
  unbind();


  // store texture id in internal array
  RenderTexture intID;
  intID.id = _texture;

  // track texture id
  attachments_[_attachment] = intID;
#else
  std::cerr << "error: FBO::attachTexture unsupported - update glew headers and rebuild" << std::endl;
#endif
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


  // store texture id in internal array
  RenderTexture intID;
  intID.id = _texture;
  intID.target = _target;

  // track texture id
  attachments_[_attachment] = intID;
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
  intID.gltype = GLFormatInfo(_internalFmt).type();
  intID.target = target;
  intID.dim = ACG::Vec3i(_width, _height, 1);
  intID.wrapMode = _wrapMode;
  intID.minFilter = _minFilter;
  intID.magFilter = _magFilter;
  intID.owner = true;


  // specify texture
  glBindTexture(target, texID);


#ifdef GL_ARB_texture_multisample
  if (!samples_)
  {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, _wrapMode);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, _wrapMode);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _magFilter);
    glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, intID.gltype, 0);
  }
  else
    glTexImage2DMultisample(target, samples_, _internalFmt, _width, _height, fixedsamplelocation_);
#else
  glTexParameteri(target, GL_TEXTURE_WRAP_S, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _minFilter);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _magFilter);
  glTexImage2D(target, 0, _internalFmt, _width, _height, 0, _format, intID.gltype, 0);
#endif // GL_ARB_texture_multisample


  checkGLError();

  width_ = _width;
  height_ = _height;

  glBindTexture(target, 0);

  // attach
  attachTexture2D(_attachment, texID, target);

  // track texture id
  attachments_[_attachment] = intID;
}

//-----------------------------------------------------------------------------

void FBO::attachTexture3D( GLenum _attachment, GLsizei _width, GLsizei _height, GLsizei _depth, GLuint _internalFmt, GLenum _format, GLint _wrapMode , GLint _minFilter , GLint _magFilter  )
{
  // gen texture id
  GLuint texID;
  glGenTextures(1, &texID);

  GLenum target = GL_TEXTURE_3D;

  // store texture id in internal array
  RenderTexture intID;
  intID.id = texID;
  intID.internalFormat = _internalFmt;
  intID.format = _format;
  intID.gltype = GLFormatInfo(_internalFmt).type();
  intID.target = target;
  intID.dim = ACG::Vec3i(_width, _height, _depth);
  intID.wrapMode = _wrapMode;
  intID.minFilter = _minFilter;
  intID.magFilter = _magFilter;
  intID.owner = true;


  // specify texture
  glBindTexture(target, texID);

  glTexParameteri(target, GL_TEXTURE_WRAP_S, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_WRAP_R, _wrapMode);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _minFilter);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _magFilter);
  glTexImage3D(target, 0, _internalFmt, _width, _height, _depth, 0, _format, GL_FLOAT, 0);

  checkGLError();

  width_ = _width;
  height_ = _height;

  glBindTexture(target, 0);

  // attach
  attachTexture(_attachment, texID, 0);

  // track texture id
  attachments_[_attachment] = intID;
}

//-----------------------------------------------------------------------------

void FBO::attachTexture2DDepth( GLsizei _width, GLsizei _height, GLuint _internalFmt /*= GL_DEPTH_COMPONENT32*/, GLenum _format /*= GL_DEPTH_COMPONENT */ )
{
  attachTexture2D(GL_DEPTH_ATTACHMENT, _width, _height, _internalFmt, _format, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//-----------------------------------------------------------------------------

void FBO::attachTexture2DStencil( GLsizei _width, GLsizei _height )
{
  attachTexture2D(GL_STENCIL_ATTACHMENT_EXT, _width, _height, GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//-----------------------------------------------------------------------------

void
FBO::
addDepthBuffer( GLuint _width, GLuint _height )
{
  if (depthbuffer_)
    glDeleteRenderbuffersEXT(1, &depthbuffer_);

  // create renderbuffer
  glGenRenderbuffersEXT(1, &depthbuffer_);

  // bind renderbuffer
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer_);

  // malloc
#ifdef GL_ARB_texture_multisample
  glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples_, GL_DEPTH_COMPONENT, _width, _height);
#else
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _width, _height);
#endif

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
  if (stencilbuffer_)
    glDeleteRenderbuffersEXT(1, &stencilbuffer_);

  // create renderbuffer
  glGenRenderbuffersEXT(1, &stencilbuffer_);

  // bind renderbuffer
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilbuffer_);

  // malloc
#ifdef GL_ARB_texture_multisample
  glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples_, GL_STENCIL_INDEX, _width, _height);
#else
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, _width, _height);
#endif

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
  // save previous fbo id
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&prevFbo_);
  glGetIntegerv(GL_DRAW_BUFFER, (GLint*)&prevDrawBuffer_);

  if ( !fbo_ )
    init();

  if ( !fbo_)
    return false;

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
  ACG::GLState::drawBuffer( prevDrawBuffer_ );
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
    case 0x8D56: // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
      std::cout << "Framebuffer incomplete, attached images must have same multisample count\n";
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
  return attachments_[_attachment].id;
}

//-----------------------------------------------------------------------------

void FBO::resize( GLsizei _width, GLsizei _height, bool _forceResize )
{
  if (_width != width_ ||_height != height_ || _forceResize)
  {
    // resizing already existing textures is highly driver dependent and does not always
    // work for all combinations of texture type (2d, 2dms, 3d) and format
    // safest way to resize is to first delete the FBO and all its internal textures, and then rebuild

    if (fbo_)
      glDeleteFramebuffersEXT(1, &fbo_);
    glGenFramebuffersEXT(1, &fbo_);

    // "detach" all textures
    AttachmentList temp;
    temp.swap(attachments_);

    // reattach all targets
    for (AttachmentList::iterator it = temp.begin(); it != temp.end(); ++it)
    {
      RenderTexture* rt = &it->second;

      // only resize textures that are owned by the FBO
      if (rt->owner)
      {
        glDeleteTextures(1, &rt->id);

        switch (rt->target)
        {
        case GL_TEXTURE_2D: 
#ifdef GL_ARB_texture_multisample
        case GL_TEXTURE_2D_MULTISAMPLE:
#endif
          attachTexture2D(it->first, rt->dim[0], rt->dim[1], rt->internalFormat, rt->format, rt->wrapMode, rt->minFilter, rt->magFilter);
          break;

        case GL_TEXTURE_3D:
          attachTexture3D(it->first, rt->dim[0], rt->dim[1], rt->dim[2], rt->internalFormat, rt->format, rt->wrapMode, rt->minFilter, rt->magFilter);
          break;

        default:
          std::cout << "FBO::resize - unknown resize target " << rt->target << std::endl;
        }
      }
    }

    // reattach render buffers
    if(depthbuffer_)
      addDepthBuffer(_width, _height);

    if(stencilbuffer_)
      addStencilBuffer(_width, _height);
  }
}

GLuint FBO::getFboID()
{
  return fbo_;
}

GLsizei FBO::setMultisampling( GLsizei _samples, GLboolean _fixedsamplelocations /*= GL_TRUE*/ )
{
  // clamp sample count to max supported
  GLint maxSamples;
  glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

  if (_samples >= maxSamples) _samples = maxSamples - 1;

  // gpu driver might cause crash on calling glTexImage2DMultisample if _samples is not a power of 2
  // -> avoid by seeking to next native MSAA sample-count
  
  if (_samples)
  {
    int safeSampleCount = 1;

    while (safeSampleCount < _samples)
      safeSampleCount *= 2;

    while (safeSampleCount >= maxSamples)
      safeSampleCount /= 2;
    
    _samples = safeSampleCount;
  }


  // issue texture reloading when params changed
  bool reloadTextures = (samples_ != _samples || fixedsamplelocation_ != _fixedsamplelocations);
    
  samples_ = _samples;
  fixedsamplelocation_ = _fixedsamplelocations;


  // force a texture reloading to apply new multisampling
  if (reloadTextures)
    resize(width_, height_, true);

  return _samples;
}



//=============================================================================
} // namespace ACG
//=============================================================================

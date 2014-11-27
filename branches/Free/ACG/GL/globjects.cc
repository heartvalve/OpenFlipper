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
*                                                                            *
 *   $Revision$                                                       *
 *   $LastChangedBy$                                                *
 *   $Date$                     *
 *                                                                            *
 \*===========================================================================*/

#include <ACG/GL/acg_glew.hh>
#include <ACG/GL/globjects.hh>
#include <ACG/GL/GLFormatInfo.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <QImage>
#include <QGLWidget>

#ifdef GLI_FOUND
#include <gli/gli.hpp>
#endif

namespace ACG {


//-----------------------------------------------------------------------------



bool Texture::supportsImageLoadStore()
{
  static int status = -1;

  if (status < 0)
  {
#if defined(GL_ARB_shader_image_load_store)
    // core in version 4.2
    status = checkExtensionSupported("ARB_shader_image_load_store") || openGLVersion(4,2);
#else
    // symbol missing, install latest glew version
    status = 0;
#endif
  }

  return status > 0;
}


bool Texture::supportsTextureBuffer()
{
  static int status = -1;

  if (status < 0)
  {
#if defined(GL_ARB_texture_buffer_object)
    // core in version 3.0
    status = checkExtensionSupported("ARB_texture_buffer_object") || openGLVersion(3,0);
#else
    // symbol missing, install latest glew version
    status = 0;
#endif
  }

  return status > 0;
}


//-----------------------------------------------------------------------------

Texture2D::Texture2D(GLenum unit)
  : Texture(GL_TEXTURE_2D, unit),
  width_(0), height_(0),
  internalFormat_(0),
  format_(0), type_(0)
{}


void Texture2D::setData(GLint _level, 
  GLint _internalFormat, 
  GLsizei _width, 
  GLsizei _height, 
  GLenum _format,
  GLenum _type,
  const GLvoid* _data) {

  if (getUnit() == GL_NONE)
    setUnit(GL_TEXTURE0);

  bind();

  glTexImage2D(GL_TEXTURE_2D, _level, _internalFormat, _width, _height, 0, _format, _type, _data);

  width_ = _width;
  height_ = _height;
  internalFormat_ = _internalFormat;
  format_ = _format;
  type_ = _type;
}


void Texture2D::setStorage( GLsizei _levels, GLenum _internalFormat, GLsizei _width, GLsizei _height ) {
#ifdef GL_ARB_texture_storage
  bind();
  glTexStorage2D(GL_TEXTURE_2D, _levels, _internalFormat, _width, _height);

  width_ = _width;
  height_ = _height;
  internalFormat_ = _internalFormat;

  GLFormatInfo finfo(_internalFormat);
  format_ = finfo.format();
  type_ = finfo.type();
#endif // GL_ARB_texture_storage
}


bool Texture2D::getData( GLint _level, void* _dst ) {
  if (is_valid()) {
    GLint curTex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);

    bind();
    glGetTexImage(GL_TEXTURE_2D, _level, format_, type_, _dst);

    glBindTexture(GL_TEXTURE_2D, curTex);

    return true;
  }
  return false;
}

bool Texture2D::getData( GLint _level, std::vector<char>& _dst ) {
  if (is_valid()) {

    GLFormatInfo finfo(internalFormat_);

    if (finfo.isValid()) {
      size_t bufSize = finfo.elemSize() * width_ * height_;

      if (_dst.size() < bufSize)
        _dst.resize(bufSize);

      if (!_dst.empty())
        return getData(_level, &_dst[0]);
    }
  }
  return false;
}



void Texture2D::bindAsImage(GLuint _index, GLenum _access){

#if defined(GL_ARB_shader_image_load_store)
  if (is_valid())
    glBindImageTexture(_index, id(), 0, GL_FALSE, 0, _access, internalFormat_);
  else
    std::cerr << "Texture2D::bindAsImage - error: texture not initialized!" << std::endl;
#else
  std::cerr << "Texture2D::bindAsImage - glBindImageTexture symbol not loaded!" << std::endl;
#endif
}

bool Texture2D::loadFromFile( const std::string& _filename, GLenum _minFilter, GLenum _magFilter )
{
  bool success = false;

  const int numMipmapEnums = 4;
  GLenum mipmapEnums[numMipmapEnums] = {GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR};
  bool mipmaps = false;

  for (int i = 0; i < numMipmapEnums; ++i)
    mipmaps = mipmaps || _minFilter == mipmapEnums[i];

  if (!_filename.empty())
  {
    bind();

#if GLI_VERSION == 51

    if (_filename.find(".dds") != _filename.npos)
    {
      gli::texture2D glitex(gli::load_dds(_filename.c_str()));
      assert(!glitex.empty());

      if(gli::is_compressed(glitex.format()))
      {
        for(gli::texture2D::size_type lod = 0; lod < glitex.levels(); ++lod)
        {
          glCompressedTexSubImage2D(GL_TEXTURE_2D,
            GLint(lod),
            0, 0,
            GLsizei(glitex[lod].dimensions().x),
            GLsizei(glitex[lod].dimensions().y),
            GLenum(gli::internal_format(glitex.format())),
            GLsizei(glitex[lod].size()),
            glitex[lod].data());
        }

        success = true;

        if (mipmaps && glitex.levels() <= 1 && (glitex.dimensions().x > 1 || glitex.dimensions().y > 1))
        {
          std::cout << "error: texture required mipmaps  " << _filename << std::endl;
          success = false;
        }
      }
      else
      {
        if (glitex.levels() > 1 || !mipmaps)
        {
          for(gli::texture2D::size_type lod = 0; lod < glitex.levels(); ++lod)
          {
            setData(GLint(lod),
              GLenum(gli::internal_format(glitex.format())),
              GLsizei(glitex[lod].dimensions().x), GLsizei(glitex[lod].dimensions().y),
              GLenum(gli::external_format(glitex.format())),
              GLenum(gli::type_format(glitex.format())),
              glitex[lod].data());
          }

          success = true;
        }
        else
        {
          success = !gluBuild2DMipmaps(GL_TEXTURE_2D,
            GLenum(gli::internal_format(glitex.format())),
            GLsizei(glitex.dimensions().x), GLsizei(glitex.dimensions().y),
            GLenum(gli::external_format(glitex.format())),
            GLenum(gli::type_format(glitex.format())),
            glitex.data());
        }
      } 

    }
    else
#endif

    {
      QImage qtex;
      
      if (qtex.load(_filename.c_str()))
      {
        QImage gltex = QGLWidget::convertToGLFormat ( qtex );

        success = true;

        if (mipmaps)
          success = !gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, gltex.width(), gltex.height(), GL_RGBA, GL_UNSIGNED_BYTE, gltex.bits());
        else
          setData(0, GL_RGBA, gltex.width(), gltex.height(), GL_RGBA, GL_UNSIGNED_BYTE, gltex.bits());
      }
    }

  }

  if (success)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
  }

  return success;
}



//-----------------------------------------------------------------------------

#if defined(GL_ARB_vertex_buffer_object)

void VertexBufferObject::del() {
    if (valid)
        glDeleteBuffersARB(1, &vbo);
    valid = false;
}

void VertexBufferObject::upload(
        GLsizeiptr size, const GLvoid* data, GLenum usage) {

  if(!valid)
      gen();
  glBufferDataARB(target, size, data, usage);
}

void VertexBufferObject::uploadSubData(
        GLuint _offset, GLuint _size, const GLvoid* _data ) {

  glBufferSubDataARB(target, _offset, _size, _data);
}

void VertexBufferObject::gen() {
    glGenBuffersARB(1, &vbo);
    if(vbo > 0u)
        valid = true;
}

int VertexBufferObject::size() {
  bind();
  int bufsize = 0;
  glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufsize);
  return bufsize;
}

#endif


//-----------------------------------------------------------------------------

TextureBuffer::TextureBuffer(GLenum u)
  : 
#if defined(GL_ARB_texture_buffer_object)
Texture(GL_TEXTURE_BUFFER, u), 
#else
Texture(0, u), 
#endif
  bufferSize_(0), buffer_(0), usage_(0), fmt_(0) {
}


TextureBuffer::~TextureBuffer() {
  if (buffer_)
    glDeleteBuffers(1, &buffer_);
}

void TextureBuffer::setBufferData(
        int _size, const void* _data, GLenum _internalFormat, GLenum _usage) {
#if defined(GL_ARB_texture_buffer_object)
  if (supportsTextureBuffer()) {
    // setup buffer object
    if (!buffer_)
        glGenBuffers(1, &buffer_);

    glBindBuffer(GL_TEXTURE_BUFFER, buffer_);
    glBufferData(GL_TEXTURE_BUFFER, _size, _data, _usage);

    usage_ = _usage;
    fmt_ = _internalFormat;

    // bind buffer to texture
    if (getUnit() == GL_NONE)
        setUnit(GL_TEXTURE0);

    bind();

    glTexBuffer(GL_TEXTURE_BUFFER, _internalFormat, buffer_);

    bufferSize_ = _size;
  }
  else
    std::cerr << "TextureBuffer::setData - gpu does not support buffer textures!" << std::endl;
#else
  std::cerr << "TextureBuffer::setData - glew version too old, rebuild with latest glew!" << std::endl;
#endif
}

bool TextureBuffer::getBufferData(void* _dst) {
#if defined(GL_ARB_texture_buffer_object)
  if (buffer_) {
    glBindBuffer(GL_TEXTURE_BUFFER, buffer_);
    glGetBufferSubData(GL_TEXTURE_BUFFER, 0, bufferSize_, _dst);
    return true;
  }
  else
    std::cerr << "TextureBuffer::getBufferData - gpu does not support buffer textures!" << std::endl;
#else
    std::cerr << "TextureBuffer::getBufferData - glew version too old, rebuild with latest glew!" << std::endl;
#endif
  return false;
}

bool TextureBuffer::getBufferData(std::vector<char>& _dst) {
  if (_dst.size() < size_t(bufferSize_))
    _dst.resize(bufferSize_);

  if (!_dst.empty())
    return getBufferData(&_dst[0]);

  return false;
}

void TextureBuffer::bindAsImage(GLuint _index, GLenum _access){

#if defined(GL_ARB_texture_buffer_object)

#if defined(GL_ARB_shader_image_load_store)
  if (id())
    glBindImageTexture(_index, id(), 0, GL_FALSE, 0, _access, fmt_);
  else
    std::cerr << "TextureBuffer::bindAsImage - error: texture not initialized!" << std::endl;
#else
  std::cerr << "TextureBuffer::bindAsImage - glBindImageTexture symbol not loaded!" << std::endl;
#endif

#endif
}



//-----------------------------------------------------------------------------


#if defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)

void ProgramBaseNV::bind() {
    if (!valid)
        gen();
    glBindProgramNV(target, program);
}

void ProgramBaseNV::unbind() {
    glBindProgramNV(target, 0);
}

bool ProgramBaseNV::load(const char* prog_text) {
    int size = int(strlen(prog_text));
    if (!valid)
        gen();
    glLoadProgramNV(target, program, size, (const GLubyte *) prog_text);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
    if (errpos != -1) {
        fprintf(stderr, "\nprogram error:\n");
        int bgn = std::max(0, errpos - 10), end = std::min(size, bgn + 30);
        for (int i = bgn; i < end; ++i)
            fputc(prog_text[i], stderr);
        fputc('\n', stderr);
        return false;
    }
    return true;
}


void ProgramBaseNV::gen() {
    glGenProgramsNV(1, &program);
    valid = true;
}

void ProgramBaseNV::del() {
    if (valid)
        glDeleteProgramsNV(1, &program);
    valid = false;
}

#endif

#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)

void ProgramBaseARB::bind() {
    if (!valid)
        gen();
    glBindProgramARB(target, program);
}
void ProgramBaseARB::unbind() {
    glBindProgramARB(target, 0);
}

bool ProgramBaseARB::load(const char* prog_text) {
    int size = int(strlen(prog_text));
    if (!valid)
        gen();
    bind();
    glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
    if (errpos != -1) {
        fprintf(stderr, "\nprogram error:\n");
        int bgn = std::max(0, errpos - 10), end = std::min(size, bgn + 30);
        for (int i = bgn; i < end; ++i)
            fputc(prog_text[i], stderr);
        fputc('\n', stderr);
        return false;
    }
    return true;
}

void ProgramBaseARB::gen() {
    glGenProgramsARB(1, &program);
    valid = true;
}
void ProgramBaseARB::del() {
    if (valid)
        glDeleteProgramsARB(1, &program);
    valid = false;
}

#endif // GL_ARB_vertex_program


//-----------------------------------------------------------------------------

// support state unknown : -1
int VertexArrayObject::supportStatus_ = -1;

VertexArrayObject::VertexArrayObject() 
  : id_(0)
{
}

VertexArrayObject::~VertexArrayObject()
{
#ifdef GL_ARB_vertex_array_object
  if (id_)
    glDeleteVertexArrays(1, &id_);
#endif
}


void VertexArrayObject::bind()
{
#ifdef GL_ARB_vertex_array_object
  if (!id_)
    init();

  if (id_)
    glBindVertexArray(id_);
#endif
}

void VertexArrayObject::unbind()
{
#ifdef GL_ARB_vertex_array_object
  glBindVertexArray(0);
#endif
}

void VertexArrayObject::init()
{
#ifdef GL_ARB_vertex_array_object
  if (id_)
    glDeleteVertexArrays(1, &id_);

  glGenVertexArrays(1, &id_);
#endif
}

bool VertexArrayObject::isSupported()
{
#ifndef GL_ARB_vertex_array_object
  // missing definition in gl header!
  supportStatus_ = 0;
#else

  if (supportStatus_ < 0)
    supportStatus_ = checkExtensionSupported("GL_ARB_vertex_array_object") ? 1 : 0;
#endif

  return supportStatus_ > 0;
}



//-----------------------------------------------------------------------------



// support state unknown : -1
int AtomicCounter::supportStatus_ = -1;

AtomicCounter::AtomicCounter(int _numCounters)
  : numCounters_(_numCounters), buffer_(0)
{
}

AtomicCounter::~AtomicCounter()
{
  if (buffer_)
    glDeleteBuffers(1, &buffer_);
}

void AtomicCounter::init()
{
  // check support and initialize
#ifdef GL_ARB_shader_atomic_counters
  if (isSupported() && numCounters_ > 0)
  {
    glGenBuffers(1, &buffer_);
    bind();
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, numCounters_ * sizeof(unsigned int), 0, GL_DYNAMIC_COPY);
    unbind();
  }
#endif

  if (!isValid())
    std::cerr << "atomic counter failed to initialize!" << std::endl;
}

void AtomicCounter::bind()
{
#ifdef GL_ARB_shader_atomic_counters
  // implicit initialization
  if (!isValid())
    init();

  if (isValid())
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer_);
#endif
}

void AtomicCounter::bind(GLuint _index)
{
#ifdef GL_ARB_shader_atomic_counters
  // implicit initialization
  if (!isValid())
    init();

  if (isValid())
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, _index, buffer_);
#endif
}

void AtomicCounter::unbind()
{
#ifdef GL_ARB_shader_atomic_counters
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
#endif
}

void AtomicCounter::set(unsigned int _value)
{
#ifdef GL_ARB_shader_atomic_counters
  // implicit initialization
  bind();

  if (isValid())
  {
    const size_t bufSize = numCounters_ * sizeof(unsigned int);
    //     unsigned int* bufData = new unsigned int[numCounters_];
    //     memset(bufData, int(_value), bufSize);
    // 
    //     glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, bufSize, bufData);
    //     delete [] bufData;

    void* bufPtr = glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, bufSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memset(bufPtr, int(_value), bufSize);
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

    unbind();
  }
#endif
}

void AtomicCounter::get(unsigned int* _out)
{
#ifdef GL_ARB_shader_atomic_counters
  if (isValid())
  {
    bind();

    const size_t bufSize = numCounters_ * sizeof(unsigned int);

    // doesnt work, driver crash on ati:
    //    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, bufSize, _out);

    void* bufPtr = glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, bufSize, GL_MAP_READ_BIT);
    memcpy(_out, bufPtr, bufSize);
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

    unbind();
  }
#endif
}

bool AtomicCounter::isSupported()
{
#ifndef GL_ARB_shader_atomic_counters
  // missing definition in gl header!
  supportStatus_ = 0;
#else

  if (supportStatus_ < 0)
    supportStatus_ = checkExtensionSupported("GL_ARB_shader_atomic_counters") ? 1 : 0;
#endif

  return supportStatus_ > 0;
}

bool AtomicCounter::isValid() const
{
  return buffer_ && numCounters_ > 0;
}


//-----------------------------------------------------------------------------

int QueryCounter::supportStatus_ = -1;

QueryCounter::QueryCounter()
  : state_(-1)
{
  queryObjects_[0] = queryObjects_[1] = 0;
}

QueryCounter::~QueryCounter()
{
  if (queryObjects_[0])
    glDeleteQueries(2, queryObjects_);
}


void QueryCounter::restart()
{
#ifdef GL_ARB_timer_query
  if (isSupported())
  {
    state_ = 0;

    if (!queryObjects_[0])
      glGenQueries(2, queryObjects_);

    glQueryCounter(queryObjects_[0], GL_TIMESTAMP);
  }
#endif
}

void QueryCounter::stop()
{
#ifdef GL_ARB_timer_query
  if (state_ == 0)
  {
    glQueryCounter(queryObjects_[1], GL_TIMESTAMP);
    ++state_;
  }
#endif
}

GLuint64 QueryCounter::elapsedNs()
{
  GLuint64 timing = 0;
#ifdef GL_ARB_timer_query
  stop();

  if (state_ == 1)
  {
    GLint available = 0;
    while (!available)
      glGetQueryObjectiv(queryObjects_[1], GL_QUERY_RESULT_AVAILABLE, &available);

    GLuint64 timeStart;
    glGetQueryObjectui64v(queryObjects_[0], GL_QUERY_RESULT, &timeStart);
    glGetQueryObjectui64v(queryObjects_[1], GL_QUERY_RESULT, &timing);
    timing -= timeStart;
  }
#endif
  return timing;
}

GLuint64 QueryCounter::elapsedMs()
{
  return elapsedNs() / 1000;
}

float QueryCounter::elapsedSecs()
{
  GLuint64 ms = elapsedMs();

  return float(ms) / 1000.0f;
}

bool QueryCounter::isSupported()
{
#ifndef GL_ARB_timer_query
  // missing definition in gl header!
  supportStatus_ = 0;
#else

  if (supportStatus_ < 0)
    supportStatus_ = checkExtensionSupported("GL_ARB_timer_query") || openGLVersion(3,2) ? 1 : 0;
#endif

  return supportStatus_ > 0;
}


//-----------------------------------------------------------------------------


// support state unknown : -1
int UniformBufferObject::supportStatus_ = -1;
int UniformBufferObject::maxBlockSize_ = -1;
int UniformBufferObject::maxBindings_ = -1;
int UniformBufferObject::maxCombinedShaderBlocks_ = -1;
int UniformBufferObject::offsetAlignment_ = -1;

UniformBufferObject::UniformBufferObject()
  : VertexBufferObject(
#ifndef GL_ARB_uniform_buffer_object
  GL_NONE
#else
  GL_UNIFORM_BUFFER
#endif
  ),
  data_(0)
{
}

UniformBufferObject::~UniformBufferObject()
{
}

void UniformBufferObject::bind( GLuint _index )
{
#ifdef GL_ARB_uniform_buffer_object
  glBindBufferBase(GL_UNIFORM_BUFFER, _index, id());
#endif
}


bool UniformBufferObject::isSupported()
{
#ifndef GL_ARB_uniform_buffer_object
  // missing definition in gl header!
  supportStatus_ = 0;
#else

  if (supportStatus_ < 0)
    supportStatus_ = checkExtensionSupported("GL_ARB_uniform_buffer_object") ? 1 : 0;
#endif

  return supportStatus_ > 0;
}

void UniformBufferObject::queryCaps()
{
#ifdef GL_ARB_uniform_buffer_object
  if (isSupported())
  {
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings_);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize_);
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &maxCombinedShaderBlocks_);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offsetAlignment_);
  }
#endif
}

int UniformBufferObject::getMaxBindings()
{
  if (maxBindings_ < 0)
    queryCaps();

  return maxBindings_;
}

int UniformBufferObject::getMaxBlocksize()
{
  if (maxBlockSize_ < 0)
    queryCaps();

  return maxBlockSize_;
}

int UniformBufferObject::getMaxCombinedShaderBlocks()
{
  if (maxCombinedShaderBlocks_ < 0)
    queryCaps();

  return maxCombinedShaderBlocks_;
}

int UniformBufferObject::getOffsetAlignment()
{
  if (offsetAlignment_ < 0)
    queryCaps();

  return offsetAlignment_;
}

void UniformBufferObject::setUniformData( GLSL::Program* _prog, const char* _bufferName, const char* _uniformName, const void* _data, int _datasize, bool _delay )
{
  if (_prog && _bufferName && _uniformName && _data)
  {
    GLuint idx = _prog->getUniformBlockIndex(_bufferName);

    if (idx != GL_INVALID_INDEX)
    {
      size_t bufsize = size_t(_prog->getUniformBlockSize(idx));

      if (data_.size() != bufsize)
        data_.resize(bufsize, 0);

      int offset = -1;
      _prog->getUniformBlockOffsets(1, &_uniformName, &offset);

      if (offset >= 0)
      {
        memcpy(&data_[offset], _data, _datasize);

        if (!_delay)
        {
          VertexBufferObject::bind();

          if (size() != int(bufsize))
            VertexBufferObject::upload(bufsize, &data_[0], GL_DYNAMIC_DRAW);
          else
            uploadSubData(offset, _datasize, _data);
        }
      }
    }
  }
}

void UniformBufferObject::upload()
{
  if (!data_.empty())
  {
    VertexBufferObject::bind();

    VertexBufferObject::upload(data_.size(), &data_[0], GL_DYNAMIC_DRAW);
  }
}


//-----------------------------------------------------------------------------



// support state unknown : -1
int ShaderStorageBufferObject::supportStatus_ = -1;
int ShaderStorageBufferObject::maxBlockSize_ = -1;
int ShaderStorageBufferObject::maxBindings_ = -1;
int ShaderStorageBufferObject::maxCombinedShaderBlocks_ = -1;

ShaderStorageBufferObject::ShaderStorageBufferObject()
  : VertexBufferObject(
#ifndef GL_ARB_shader_storage_buffer_object
   GL_NONE
#else
   GL_SHADER_STORAGE_BUFFER
#endif
   )
{
}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
{
}

void ShaderStorageBufferObject::bind( GLuint _index )
{
#ifdef GL_ARB_shader_storage_buffer_object
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _index, id());
#endif
}

bool ShaderStorageBufferObject::isSupported()
{
#ifndef GL_ARB_shader_storage_buffer_object
  // missing definition in gl header!
  supportStatus_ = 0;
#else

  if (supportStatus_ < 0)
    supportStatus_ = checkExtensionSupported("GL_ARB_shader_storage_buffer_object") ? 1 : 0;
#endif

  return supportStatus_ > 0;
}

void ShaderStorageBufferObject::queryCaps()
{
#ifdef GL_ARB_shader_storage_buffer_object
  if (isSupported())
  {
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindings_);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBlockSize_);
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &maxCombinedShaderBlocks_);
  }
#endif
}

int ShaderStorageBufferObject::getMaxBindings()
{
  if (maxBindings_ < 0)
    queryCaps();

  return maxBindings_;
}

int ShaderStorageBufferObject::getMaxBlocksize()
{
  if (maxBlockSize_ < 0)
    queryCaps();

  return maxBlockSize_;
}

int ShaderStorageBufferObject::getMaxCombinedShaderBlocks()
{
  if (maxCombinedShaderBlocks_ < 0)
    queryCaps();

  return maxCombinedShaderBlocks_;
}


} /* namespace ACG */

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
#include "globjects.hh"


namespace ACG {


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

#endif


//-----------------------------------------------------------------------------


#if defined(GL_ARB_texture_buffer_object)

TextureBuffer::~TextureBuffer() {
    if (buffer_)
        glDeleteBuffers(1, &buffer_);
}

void TextureBuffer::setBufferData(
        int _size, const void* _data, GLenum _internalFormat, GLenum _usage) {
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

void TextureBuffer::bindAsImage(GLuint _index, GLenum _access){

#if defined(GL_ARB_shader_image_load_store)
  if (id())
    glBindImageTexture(_index, id(), 0, GL_FALSE, 0, _access, fmt_);
  else
    std::cerr << "TextureBuffer::bindAsImage - error: texture not initialized!" << std::endl;
#else
  std::cerr << "TextureBuffer::bindAsImage - glBindImageTexture symbol not loaded!" << std::endl;
#endif
}


#endif


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

#endif
} /* namespace ACG */

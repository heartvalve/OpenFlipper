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

#include <GL/glew.h>
#include "globjects.hh"


namespace ACG {

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

    // bind buffer to texture
    if (getUnit() == GL_NONE)
        setUnit(GL_TEXTURE0);

    bind();

    glTexBuffer(GL_TEXTURE_BUFFER, _internalFormat, buffer_);

    bufferSize_ = _size;
}


#endif

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

#endif
} /* namespace ACG */

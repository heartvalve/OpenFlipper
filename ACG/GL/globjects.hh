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
//  OpenGL Objects
//
//=============================================================================


#ifndef GL_OBJECTS_HH
#define GL_OBJECTS_HH


//== INCLUDES =================================================================

// GL
#include <ACG/GL/gl.hh>

// C++
#include <iostream>
#include <fstream>
#include <string>

// C
#include <cstdio>
#include <cstring>

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif


//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


class DisplayList
{
public:

  DisplayList() : valid(false) {}

  virtual ~DisplayList() { del(); }

  void call() { if (valid) glCallList(dlist); }

  void new_list(GLenum mode) { if(!valid) gen(); glNewList(dlist, mode); }

  void end_list() { glEndList(); }

  void del() { if(valid) glDeleteLists(dlist, 1); valid = false; }

  bool is_valid() const { return valid; }

private:

  void gen() { dlist = glGenLists(1); valid=true; }

  bool valid;
  GLuint dlist;
};


//== CLASS DEFINITION =========================================================


#if defined(GL_ARB_vertex_buffer_object)

class VertexBufferObject
{
public:

  VertexBufferObject(GLenum _target) : target(_target), valid(false), vbo(0u) {}

  virtual ~VertexBufferObject() { del(); }

  void del() { if(valid) glDeleteBuffersARB(1, &vbo); valid = false; }
  bool is_valid() const { return valid; }

  void bind()   { if(!valid) gen(); ACG::GLState::bindBuffer(target, vbo); }
  void unbind() { ACG::GLState::bindBuffer(target, 0); }

  void upload(GLsizeiptr size, const GLvoid* data, GLenum usage)
  {
    if(!valid) gen();
    glBufferDataARB(target, size, data, usage);
  }
  
  // Upload a subset of the buffer data
  void uploadSubData(GLuint _offset, GLuint _size, const GLvoid* _data ) {
    glBufferSubDataARB(target, _offset, _size, _data);
  }

  char* offset(unsigned int _offset) const
  {
    return ((char*)NULL + _offset);
  }
  
private:

  void gen() { glGenBuffersARB(1, &vbo); if(vbo > 0u) valid = true; }

  GLenum target;
  bool   valid;
  GLuint vbo;

};


class GeometryBuffer : public VertexBufferObject
{
public:
  GeometryBuffer() : VertexBufferObject(GL_ARRAY_BUFFER_ARB) {}
};


class IndexBuffer : public VertexBufferObject
{
public:
  IndexBuffer() : VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER_ARB) {}
};


#endif


//== CLASS DEFINITION =========================================================


class Texture
{
public:

  Texture(GLenum tgt, GLenum _unit=GL_NONE)
    : target(tgt), unit(_unit), valid(false), texture(0u)
  {}

  virtual ~Texture() { del(); }

  void bind()
  {
    if(!valid) gen();
    activate();
    ACG::GLState::bindTexture(target, texture);
  }
  
  void activate()
  {
    if (unit != GL_NONE) ACG::GLState::activeTexture(unit);
  }

  void parameter(GLenum pname, GLint i)
  {
    activate();
    glTexParameteri(target, pname, i);
  }

  void parameter(GLenum pname, GLfloat f)
  {
    activate();
    glTexParameterf(target, pname, f);
  }

  void parameter(GLenum pname, GLint * ip)
  {
    activate();
    glTexParameteriv(target, pname, ip);
  }

  void parameter(GLenum pname, GLfloat * fp)
  {
    activate();
    glTexParameterfv(target, pname, fp);
  }

  void enable()
  {
    activate();
    ACG::GLState::enable(target);
  }

  void disable()
  {
    activate();
    ACG::GLState::disable(target);
  }

  void del()
  {
    if(valid) glDeleteTextures(1, &texture);
    valid = false;
  }

  void gen() { glGenTextures(1, &texture); valid = (texture > 0u ? true : valid); }

  bool is_valid() const { return valid; }
  
  GLuint id() const { return texture; }
  
  GLenum getUnit() const { return unit; }

private:

  GLenum target, unit;
  bool valid;
  GLuint texture;
};


//-----------------------------------------------------------------------------


class Texture1D : public Texture
{
public:
  Texture1D(GLenum unit=GL_NONE) : Texture(GL_TEXTURE_1D, unit) {}
};


//-----------------------------------------------------------------------------


class Texture2D : public Texture
{
public:
  Texture2D(GLenum unit=GL_NONE) : Texture(GL_TEXTURE_2D, unit) {}
};


//-----------------------------------------------------------------------------


class Texture3D : public Texture
{
public:
  Texture3D(GLenum unit=GL_NONE) : Texture(GL_TEXTURE_3D, unit) {}
};


//-----------------------------------------------------------------------------


#if defined(GL_ARB_texture_cube_map)

class TextureCubeMap : public Texture
{
public:
  TextureCubeMap(GLenum u=GL_NONE) : Texture(GL_TEXTURE_CUBE_MAP_ARB, u) {}
};

#elif defined(GL_EXT_texture_cube_map)

class TextureCubeMap : public Texture
{
public:
  TextureCubeMap(GLenum u=GL_NONE) : Texture(GL_TEXTURE_CUBE_MAP_EXT, u) {}
};

#endif


//-----------------------------------------------------------------------------


#if defined(GL_EXT_texture_rectangle)

class TextureRectangleEXT : public Texture
{
public:
  TextureRectangleEXT(GLenum u=GL_NONE)
    : Texture(GL_TEXTURE_RECTANGLE_EXT, u) {}
};

#endif


#if defined(GL_NV_texture_rectangle)

class TextureRectangleNV : public Texture
{
public:
  TextureRectangleNV(GLenum u=GL_NONE)
    : Texture(GL_TEXTURE_RECTANGLE_NV, u) {}
};

#endif



//== CLASS DEFINITION =========================================================


class ProgramBase
{
public:

  ProgramBase(GLenum tgt) : valid(false), target(tgt), program(0) {}
  virtual ~ProgramBase() {}

  bool   is_valid() const { return valid; }
  GLuint id()       const { return program; }

  virtual void bind()   = 0;
  virtual void unbind() = 0;
  virtual bool load(const char* prog_text) = 0;

  bool load_file(const char* _filename)
  {
    std::ifstream ifs(_filename);
    if (!ifs)
    {
      std::cerr << "Can't open " << _filename << "\n";
      return false;
    }
    std::string prog;
    char line[255];
    while (!ifs.eof())
    {
      if (!ifs.getline(line, 255).bad())
      {
	prog += std::string(line);
	prog += '\n';
      }
    }
    ifs.close();
    return load(prog.c_str());
  }


protected:

  bool valid;
  GLenum target;
  GLuint program;
};


//== CLASS DEFINITION =========================================================


#if defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)

class ProgramBaseNV : public ProgramBase
{
public:

  ProgramBaseNV(GLenum tgt) : ProgramBase(tgt) {}
  ~ProgramBaseNV() { del(); }

  void bind()   { if(!valid) gen(); glBindProgramNV(target, program); }
  void unbind() { glBindProgramNV(target, 0); }

  bool load(const char* prog_text)
  {
    int size= int(strlen(prog_text));
    if(!valid) gen();
    glLoadProgramNV(target, program, size, (const GLubyte *) prog_text);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
    if(errpos != -1)
    {
      fprintf(stderr, "\nprogram error:\n");
      int bgn=std::max(0, errpos - 10), end=std::min(size, bgn+30);
      for(int i=bgn; i<end; ++i) fputc(prog_text[i], stderr);
      fputc('\n', stderr);
      return false;
    }
    return true;
  }


private:
  void gen() { glGenProgramsNV(1, &program); valid=true; }
  void del() { if(valid) glDeleteProgramsNV(1, &program); valid=false; }
};

#endif


//== CLASS DEFINITION =========================================================


#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)

class ProgramBaseARB : public ProgramBase
{
public:

  ProgramBaseARB(GLenum tgt) : ProgramBase(tgt) {}
  ~ProgramBaseARB() { del(); }

  void bind()   { if(!valid) gen(); glBindProgramARB(target, program); }
  void unbind() { glBindProgramARB(target, 0); }

  bool load(const char* prog_text)
  {
    int size=strlen(prog_text);
    if(!valid) gen();
    bind();
    glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
    if(errpos != -1)
    {
      fprintf(stderr, "\nprogram error:\n");
      int bgn=std::max(0, errpos - 10), end=std::min(size, bgn+30);
      for(int i=bgn; i<end; ++i) fputc(prog_text[i], stderr);
      fputc('\n', stderr);
      return false;
    }
    return true;
  }

  void parameter()
  {
  }

private:
  void gen() { glGenProgramsARB(1, &program); valid=true; }
  void del() { if(valid) glDeleteProgramsARB(1, &program); valid=false; }
};

#endif


//-----------------------------------------------------------------------------


#if defined(GL_NV_vertex_program)

class VertexProgramNV : public ProgramBaseNV
{
public:
  VertexProgramNV() : ProgramBaseNV(GL_VERTEX_PROGRAM_NV) {}
};


class VertexStateProgramNV : public ProgramBaseNV
{
public:
  VertexStateProgramNV() : ProgramBaseNV(GL_VERTEX_STATE_PROGRAM_NV) {}
};

#endif


//-----------------------------------------------------------------------------


#if defined(GL_NV_fragment_program)

class FragmentProgramNV : public ProgramBaseNV
{
public:
  FragmentProgramNV() : ProgramBaseNV(GL_FRAGMENT_PROGRAM_NV) {}
};

#endif



//-----------------------------------------------------------------------------


#if defined(GL_ARB_vertex_program)

class VertexProgramARB : public ProgramBaseARB
{
public:
  VertexProgramARB() : ProgramBaseARB(GL_VERTEX_PROGRAM_ARB) {}
};

#endif


//-----------------------------------------------------------------------------


#if defined(GL_ARB_fragment_program)

class FragmentProgramARB : public ProgramBaseARB
{
public:
  FragmentProgramARB() : ProgramBaseARB(GL_FRAGMENT_PROGRAM_ARB) {}
};

#endif


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // GL_OBJECTS_HH defined
//=============================================================================

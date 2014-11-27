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
#include <ACG/GL/GLState.hh>

// C++
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

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

namespace GLSL {
  class Program; // prototype
}

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

class ACGDLLEXPORT VertexBufferObject
{
public:

  VertexBufferObject(GLenum _target) : target(_target), valid(false), vbo(0u) {}

  virtual ~VertexBufferObject() { del(); }

  void del();
  bool is_valid() const { return valid; }

  void bind()   { if(!valid) gen(); ACG::GLState::bindBuffer(target, vbo); }
  void unbind() { ACG::GLState::bindBuffer(target, 0); }

  void upload(GLsizeiptr size, const GLvoid* data, GLenum usage);

  // Upload a subset of the buffer data
  void uploadSubData(GLuint _offset, GLuint _size, const GLvoid* _data );

  char* offset(unsigned int _offset) const
  {
    return ((char*)NULL + _offset);
  }

  GLuint id() const {return vbo;}

  int size();

private:

  void gen();

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


/* Vertex array object
https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object

A VAO is a state collection for vertex, index buffers and vertex attribute locations.
It basically stores all vertex and index buffer related opengl states.
Maybe Vertex- or GeometryInputState is a more fitting name for this class..

extension: https://www.opengl.org/registry/specs/ARB/vertex_array_object.txt
opengl-core: 3.0


usage:

setup VAO:
vao.bind()
usual opengl setup of vertex and indexbuffer and attribute locations

draw with VAO:
vao.bind()
glDraw..()
*/
class ACGDLLEXPORT VertexArrayObject
{
public:
  VertexArrayObject();
  virtual ~VertexArrayObject();

  // check hardware support
  static bool isSupported();

  void bind();
  void unbind();

  // implicitly called on bind(), but can be used to reset the VAO
  void init();

  // opengl object id
  GLuint id() const {return id_;}

  bool is_valid() const {return id_ != 0;}

  operator GLuint() const {return id_;}

private:
  GLuint id_;

  static int supportStatus_;
};



//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT Texture
{
public:

  Texture(GLenum tgt, GLenum _unit=GL_NONE)
    : target(tgt), unit(_unit), valid(false), texture(0u)
  {}

  virtual ~Texture() { del(); }


  void bind(GLenum _unit)
  {
    if(!valid) gen();
    activate(_unit);
    ACG::GLState::bindTexture(target, texture);
  }

  void activate(GLenum _unit)
  {
    if (_unit != GL_NONE) ACG::GLState::activeTexture(_unit);
  }

  void bind() {  bind(unit); }

  void activate() {  activate(unit);  }

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

  void setUnit(GLenum u) {unit = u;}
  GLenum getUnit() const { return unit; }
  GLenum getTarget() const {return target;}

  // test for shader_image_load_store support
  static bool supportsImageLoadStore();


  // test for texture buffer support
  static bool supportsTextureBuffer();

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


class ACGDLLEXPORT Texture2D : public Texture
{
public:
  Texture2D(GLenum unit=GL_NONE);

  // initialize and set texture data via glTexImage2D
  void setData(GLint _level, GLint _internalFormat, GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid* _data);

  // specify storage of texture  (OpenGL 4.2)
  //  use setData with a nullptr instead if 4.2 is not available
  void setStorage(GLsizei _levels, GLenum _internalFormat, GLsizei _width, GLsizei _height);

  // initialize and load from file
  //  supports png, jpg etc. (any format that can be loaded by qt)
  //  additionally supports dds if the gli library is available while building ACG
  bool loadFromFile(const std::string& _filename, GLenum _minFilter = GL_NEAREST_MIPMAP_LINEAR, GLenum _magFilter = GL_LINEAR);

  // use texture as image load/store    (equivalent of unordered access buffers in dx11)
  //  allows data scattering operations in shader ie. random read/write access
  //  ref: https://www.opengl.org/registry/specs/ARB/shader_image_load_store.txt
  // _index zero-based image unit
  // _access access operations in shader: GL_READ_WRITE, GL_READ_ONLY, GL_WRITE_ONLY
  // requires opengl 4.2
  void bindAsImage(GLuint _index, GLenum _access);

  // get params from glTexImage2D
  GLsizei getWidth() const {return width_;}
  GLsizei getHeight() const {return height_;}
  GLint getInternalFormat() const {return internalFormat_;}
  GLenum getFormat() const {return format_;}
  GLenum getType() const {return type_;}

  // read data back to sysmem
  bool getData(GLint _level, void* _dst);
  bool getData(GLint _level, std::vector<char>& _dst);

private:

  GLsizei width_, height_;
  GLint internalFormat_;
  GLenum format_, type_;
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



class ACGDLLEXPORT TextureBuffer : public Texture
{
public:
  TextureBuffer(GLenum u=GL_NONE);

  ~TextureBuffer();

  // _size  size in bytes of buffer data
  // _data  buffer data
  // _internalFormat format of buffer - http://www.opengl.org/sdk/docs/man3/xhtml/glTexBuffer.xml
  // _usage buffer usage hint - https://www.opengl.org/sdk/docs/man3/xhtml/glBufferData.xml
  void setBufferData(int _size, const void* _data, GLenum _internalFormat, GLenum _usage = GL_STATIC_DRAW);

  // use buffer as image load/store    (equivalent of unordered access buffers in dx11)
  //  allows data scattering operations in shader
  //  ref: https://www.opengl.org/registry/specs/ARB/shader_image_load_store.txt
  // _index image unit
  // _access access operations in shader: GL_READ_WRITE, GL_READ_ONLY, GL_WRITE_ONLY
  // requires opengl 4.2
  void bindAsImage(GLuint _index, GLenum _access);

  int getBufferSize() const {return bufferSize_;}

  GLuint getBufferId() const {return buffer_;}

  GLenum getUsage() const {return usage_;}

  GLenum getFormat() const {return fmt_;}


  // read buffer data back to sysmem
  bool getBufferData(void* _dst);
  bool getBufferData(std::vector<char>& _dst);

private:

  int bufferSize_;
  GLuint buffer_;
  GLenum usage_;
  GLenum fmt_;
};




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

  void bind();
  void unbind();

  bool load(const char* prog_text);


private:
  void gen();
  void del();
};

#endif


//== CLASS DEFINITION =========================================================


#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)

class ProgramBaseARB : public ProgramBase
{
public:

  ProgramBaseARB(GLenum tgt) : ProgramBase(tgt) {}
  ~ProgramBaseARB() { del(); }

  void bind();
  void unbind();

  bool load(const char* prog_text);

  void parameter() {}

private:
  void gen();
  void del();
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


//== CLASS DEFINITION =========================================================

/*
Atomic counter for shaders:
 http://www.opengl.org/wiki/Atomic_Counter

This is a global counter that can be incremented/decremented within shaders.
Counting is atomic for all shader invocations (ie. inc/decrement is thread-safe in parallel invocations)

extension: http://www.opengl.org/registry/specs/ARB/shader_atomic_counters.txt
opengl-core: 4.2

usage:
 counter is initialized implicitly or explicitly via init(num)
 -> reset counter via set()
 -> call bind() before rendering
 -> in shader: 
        layout(binding = 0, offset = 0) uniform atomic_uint myCounter;
         ...
        
          uint counterVal = atomicCounter(myCounter);
          counterVal = atomicCounterIncrement(myCounter);
          counterVal = atomicCounterDecrement(myCounter);
*/
class ACGDLLEXPORT AtomicCounter
{
public:
  // _numCounters  number of counters in the buffer, each counter is a uint value
  AtomicCounter(int _numCounters = 1);

  virtual ~AtomicCounter();

  // create counter buffer, implicitly called for a single counter value
  void init();

  // set counter value
  void set(unsigned int _value = 0);

  // read counter values after rendering
  //  _out  ptr to array of size numCounters_, receiving the actual counter values
  void get(unsigned int* _out);

  // bind
  void bind();

  // bind to index corresponding to binding point in shader: layout (binding = _index)
  void bind(GLuint _index);

  // deactivate
  void unbind();

  // check hardware support
  static bool isSupported();

  bool isValid() const;

  GLuint getBufferId() const {return buffer_;}
  int getNumCounters() const {return numCounters_;}

private:

  int numCounters_;
  GLuint buffer_;

  static int supportStatus_;
};


//== CLASS DEFINITION =========================================================

/* 
Timer query:

Performance counter for profiling GPU timings

extension: https://www.opengl.org/registry/specs/ARB/timer_query.txt
opengl-core: 3.2

usage:
counter.restart();
.. gl-calls
GLuint64 ns = counter.elapsedNs();

\note elapsedX() methods synchronize CPU and GPU, so this has a performance hit on CPU side
*/
class ACGDLLEXPORT QueryCounter
{
public:
  QueryCounter();
  virtual ~QueryCounter();

  void restart();
  void stop();

  /// elapsed gpu time since restart() in nanosecs
  GLuint64 elapsedNs();

  /// elapsed gpu time in millisecs
  GLuint64 elapsedMs();

  /// elapsed gpu time in seconds
  float elapsedSecs();

  /// check hw support
  static bool isSupported();

private:

  GLuint queryObjects_[2];
  int state_; // -1 : not started,  0 : started,  1 : stopped

  static int supportStatus_;
};



//== CLASS DEFINITION =========================================================

/*
Uniform buffer object:
 https://www.opengl.org/wiki/Uniform_Buffer_Object

Grouping shader uniforms into a buffer allows to reuse 
the same set of uniforms across multiple shaders.
Also avoids having to call lots of setUniform functions.

extension: https://www.opengl.org/registry/specs/ARB/uniform_buffer_object.txt
opengl-core: 3.1

usage:
ACG::Vec4f vec0 = ..;
ubo.setUniformData(shader, "blockName", "uniformName0", vec0.data());

bind to a binding index:
 ubo.bind(idx);
 shader->setUniformBlockBinding(idx);

in shader:
uniform blockName
{
   vec4 uniformName0;
   vec4 uniformName1;
   ..
};

*/
class ACGDLLEXPORT UniformBufferObject : public VertexBufferObject
{
public:
  UniformBufferObject();

  virtual ~UniformBufferObject();

  // set data for a uniform (makes a byte-wise)
  //  if _delay is true, the buffer is only locally changed and must be updated later via upload().
  //  otherwise, the buffer is immediately updated via glBufferSubData
  void setUniformData(GLSL::Program* _prog, const char* _bufferName, const char* _uniformName, const void* _data, int _datasize, bool _delay = false);

  // upload the buffer after delayed initialization via setUniformData
  void upload();

  // use this to bind to a shader binding point
  void bind(GLuint _index);

  // check hardware support
  static bool isSupported();

  // get hw caps
  static int getMaxBindings();
  static int getMaxBlocksize();
  static int getMaxCombinedShaderBlocks();
  static int getOffsetAlignment();

private:

  // buffer data (optional)
  std::vector<char> data_;

  static void queryCaps();

  // hw caps
  static int supportStatus_;
  static int maxBlockSize_;
  static int maxBindings_;
  static int maxCombinedShaderBlocks_;
  static int offsetAlignment_;
};


//== CLASS DEFINITION =========================================================

/*
Shader storage buffer object:
 http://www.opengl.org/wiki/Shader_Storage_Buffer_Object

Similar to uniform buffer object and also texture buffer.
Can be read and written to from within shaders.
Also, the size of a SSBO is typically only bounded by the physical VRAM limitation,
so it should be used for large data sets or unknown sized arrays.

extension: https://www.opengl.org/registry/specs/ARB/shader_storage_buffer_object.txt
opengl-core: 4.3

usage:
init on application-side with data:
 ssbo.bind();
 ssbo.upload(datasize, dataptr, GL_DYNAMIC_DRAW);

bind to a binding index:
 ssbo.bind(idx);

in shader:
layout (stdxxx, binding = idx) buffer mySSBO
{
   vec4 v4;
   vec4 v_unbounded[];
};

*/
class ACGDLLEXPORT ShaderStorageBufferObject : public VertexBufferObject
{
public:
  ShaderStorageBufferObject();

  virtual ~ShaderStorageBufferObject();

  // use this to bind to a shader binding point
  void bind(GLuint _index);

  // check hardware support
  static bool isSupported();

  // get hw caps
  static int getMaxBindings();
  static int getMaxBlocksize();
  static int getMaxCombinedShaderBlocks();

private:

  static void queryCaps();

  // hw caps
  static int supportStatus_;
  static int maxBlockSize_;
  static int maxBindings_;
  static int maxCombinedShaderBlocks_;
};



//=============================================================================
} // namespace ACG
//=============================================================================
#endif // GL_OBJECTS_HH defined
//=============================================================================

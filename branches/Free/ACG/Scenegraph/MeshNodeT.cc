//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS MeshNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_MESHNODE_C

//== INCLUDES =================================================================


#include "MeshNodeT.hh"
#include "ShaderNode.hh"
#include "DrawModes.hh"
#include "../GL/gl.hh"
#include "../GL/ColorTranslator.hh"

#include <OpenMesh/Core/System/omstream.hh>


//== NAMESPACES ==============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


template<class Mesh>
MeshNodeT<Mesh>::
MeshNodeT(const Mesh&  _mesh,
	  BaseNode*    _parent,
	  std::string  _name)
  : BaseNode(_parent, _name),
    mesh_(_mesh),
    enabled_arrays_(0),
    face_index_buffer_(0),
    vertex_buffer_(0),
    normal_buffer_(0),
    vertexBufferInitialized_(false),
    normalBufferInitialized_(false),
    faceIndexBufferInitialized_(false),
    textureMap_(0)
{
}


//----------------------------------------------------------------------------


template<class Mesh>
MeshNodeT<Mesh>::
~MeshNodeT()
{
  if (vertex_buffer_)
    glDeleteBuffersARB(1, (GLuint*) &vertex_buffer_);

  if (normal_buffer_)
    glDeleteBuffersARB(1, (GLuint*)  &normal_buffer_);

  if (face_index_buffer_)
    glDeleteBuffersARB(1, (GLuint*)  &face_index_buffer_ );

}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
boundingBox(Vec3f& _bbMin, Vec3f& _bbMax)
{
  typename Mesh::ConstVertexIter  v_it(mesh_.vertices_begin()),
                                  v_end(mesh_.vertices_end());

  for (; v_it!=v_end; ++v_it)
  {
    _bbMin.minimize((Vec3f)mesh_.point(v_it));
    _bbMax.maximize((Vec3f)mesh_.point(v_it));
  }
}


//----------------------------------------------------------------------------


template<class Mesh>
unsigned int
MeshNodeT<Mesh>::
availableDrawModes() const
{
  unsigned int drawModes(0);

  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::SOLID_SHADER;

  if (mesh_.has_vertex_normals())
  {
    drawModes |= DrawModes::POINTS_SHADED;
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
    drawModes |= DrawModes::SOLID_PHONG_SHADED;
  }

  if (mesh_.has_vertex_colors())
  {
    drawModes |= DrawModes::POINTS_COLORED;
    drawModes |= DrawModes::SOLID_POINTS_COLORED;
  }

  if (mesh_.has_face_normals())
    drawModes |= DrawModes::SOLID_FLAT_SHADED;

  if (mesh_.has_face_colors())
  {
    drawModes |= DrawModes::SOLID_FACES_COLORED;

    if( mesh_.has_face_normals() )
	drawModes |= DrawModes::SOLID_FACES_COLORED_FLAT_SHADED;
  }

  if (mesh_.has_vertex_texcoords1D())
  {
    drawModes |= DrawModes::SOLID_1DTEXTURED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_1DTEXTURED_SHADED;
  }

  if (mesh_.has_vertex_texcoords2D())
  {
    drawModes |= DrawModes::SOLID_TEXTURED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_TEXTURED_SHADED;
  }

  if (mesh_.has_vertex_texcoords3D())
  {
    drawModes |= DrawModes::SOLID_3DTEXTURED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_3DTEXTURED_SHADED;
  }

  if (mesh_.has_halfedge_texcoords2D())
  {
    drawModes |= DrawModes::SOLID_2DTEXTURED_FACE;
    if (mesh_.has_face_normals())
      drawModes |= DrawModes::SOLID_2DTEXTURED_FACE_SHADED;
  }

  return drawModes;
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
enable_arrays(unsigned int _arrays)
{
  // special case: VBO
  // only use for float data, otherwise it's terribly slow!
  typedef typename Mesh::Point         Point;
  typedef typename Point::value_type   PointScalar;
  typedef typename Mesh::Normal        Normal;
  typedef typename Normal::value_type  NormalScalar;

  bool use_vbo =
    ((_arrays == VERTEX_ARRAY || _arrays == (VERTEX_ARRAY | NORMAL_ARRAY)) &&
     (vertexBufferInitialized_ && normalBufferInitialized_) ) ;
//   omlog() << "Use VBO: " << use_vbo << std::endl;



  // unbind VBO buffers
  if (!use_vbo && vertex_buffer_)
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);



  if (_arrays & VERTEX_ARRAY)
  {
    if (!(enabled_arrays_ & VERTEX_ARRAY))
    {
      enabled_arrays_ |= VERTEX_ARRAY;
      glEnableClientState(GL_VERTEX_ARRAY);

      if (use_vbo )
      {
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertex_buffer_);

         // As we ensure that buffers are converted to float before using them, use Float here
         glVertexPointer(3, GL_FLOAT, 0, 0);
      }
      else
      {
         glVertexPointer(mesh_.points());
      }
    }
  }
  else if (enabled_arrays_ & VERTEX_ARRAY)
  {
    enabled_arrays_ &= ~VERTEX_ARRAY;
    glDisableClientState(GL_VERTEX_ARRAY);
  }


  if (_arrays & NORMAL_ARRAY)
  {
    if (!(enabled_arrays_ & NORMAL_ARRAY))
    {
      enabled_arrays_ |= NORMAL_ARRAY;
      glEnableClientState(GL_NORMAL_ARRAY);

      if (use_vbo)
      {
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, normal_buffer_);

         // As we ensure that buffers are converted to float before using them, use Float here
         glNormalPointer(GL_FLOAT, 0 , 0);
      }
      else
      {
         glNormalPointer(mesh_.vertex_normals());
      }
    }
  }
  else if (enabled_arrays_ & NORMAL_ARRAY)
  {
    enabled_arrays_ &= ~NORMAL_ARRAY;
    glDisableClientState(GL_NORMAL_ARRAY);
  }


  if (_arrays & COLOR_ARRAY)
  {
    if (!(enabled_arrays_ & COLOR_ARRAY))
    {
      enabled_arrays_ |= COLOR_ARRAY;
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(mesh_.vertex_colors());
    }
  }
  else if (enabled_arrays_ & COLOR_ARRAY)
  {
    enabled_arrays_ &= ~COLOR_ARRAY;
    glDisableClientState(GL_COLOR_ARRAY);
  }


  if (_arrays & TEXTURE_COORD_1D_ARRAY)
  {
    if (!(enabled_arrays_ & TEXTURE_COORD_1D_ARRAY))
    {
      enabled_arrays_ |= TEXTURE_COORD_1D_ARRAY;
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(mesh_.texcoords1D());
    }
  }
  else if (enabled_arrays_ & TEXTURE_COORD_1D_ARRAY)
  {
    enabled_arrays_ &= ~TEXTURE_COORD_1D_ARRAY;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }


  if (_arrays & TEXTURE_COORD_2D_ARRAY)
  {
    if (!(enabled_arrays_ & TEXTURE_COORD_2D_ARRAY))
    {
      enabled_arrays_ |= TEXTURE_COORD_2D_ARRAY;
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(mesh_.texcoords2D());
    }
  }
  else if (enabled_arrays_ & TEXTURE_COORD_2D_ARRAY)
  {
    enabled_arrays_ &= ~TEXTURE_COORD_2D_ARRAY;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }


  if (_arrays & TEXTURE_COORD_3D_ARRAY)
  {
    if (!(enabled_arrays_ & TEXTURE_COORD_3D_ARRAY))
    {
      enabled_arrays_ |= TEXTURE_COORD_3D_ARRAY;
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(mesh_.texcoords3D());
    }
  }
  else if (enabled_arrays_ & TEXTURE_COORD_3D_ARRAY)
  {
    enabled_arrays_ &= ~TEXTURE_COORD_3D_ARRAY;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }


  glCheckErrors();
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
update_geometry()
{
  if (GLEW_ARB_vertex_buffer_object) {
    typedef typename Mesh::Point         Point;
    typedef typename Point::value_type   PointScalar;
    typedef typename Mesh::Normal        Normal;
    typedef typename Normal::value_type  NormalScalar;

    //===================================================================
    // Generate a vertex buffer on the GPU
    //===================================================================

    if (!vertex_buffer_)  glGenBuffersARB(1,  (GLuint*) &vertex_buffer_);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertex_buffer_);
    vertexBufferInitialized_ = false;

    //Check if using floats otherwise convert to internal float array
    if ( sizeof(PointScalar) == 4 ) {

      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(PointScalar),
                      mesh_.points(),
                      GL_STATIC_DRAW_ARB);

      vertexBufferInitialized_ = true;

    } else {

      vertices_.clear();
      typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
                                     v_end(mesh_.vertices_end());

      for ( ; v_it != v_end ; ++v_it )
        vertices_.push_back( ACG::Vec3f(mesh_.point(v_it)) );

	   if ( !vertices_.empty() ) {

		  glBufferDataARB(GL_ARRAY_BUFFER_ARB,
					         3 * mesh_.n_vertices() * sizeof(float),
						      &vertices_[0],
						      GL_STATIC_DRAW_ARB);
        vertexBufferInitialized_ = true;

      }
    }

    //===================================================================
    // Generate a normal buffer on the GPU
    //===================================================================

    if (!normal_buffer_)  glGenBuffersARB(1,  (GLuint*)  &normal_buffer_);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, normal_buffer_);
    normalBufferInitialized_ = false;

    // Check if using floats otherwise convert to internal float array
    if ( sizeof(NormalScalar) == 4) {

      glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                      3 * mesh_.n_vertices() * sizeof(NormalScalar),
                      mesh_.vertex_normals(),
                      GL_STATIC_DRAW_ARB);

      normalBufferInitialized_ = true;

    } else {
      normals_.clear();
      typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
                                     v_end(mesh_.vertices_end());

      for ( ; v_it != v_end ; ++v_it )
        normals_.push_back( ACG::Vec3f(mesh_.normal(v_it)) );

	  if ( !normals_.empty() ) {

         glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                         3 * mesh_.n_vertices() * sizeof(float),
						       &normals_[0],
						       GL_STATIC_DRAW_ARB);
                         normalBufferInitialized_ = true;
     }

    }

    // unbind buffers
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  } else omlog() << "MeshNodeT: VBO not supported on this machine\n";
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
update_topology()
{
  if (mesh_.is_trimesh())
  {
    typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
                                        f_end(mesh_.faces_end());
    typename Mesh::ConstFaceVertexIter  fv_it;

    try
    {
      indices_.clear();
      std::vector<unsigned int>().swap(indices_);
      indices_.reserve(mesh_.n_faces()*3);

      for (; f_it!=f_end; ++f_it)
      {
	     indices_.push_back((fv_it=mesh_.cfv_iter(f_it)).handle().idx());
	     indices_.push_back((++fv_it).handle().idx());
	     indices_.push_back((++fv_it).handle().idx());
      }
    }
    catch (...)
    {
      indices_.clear();
      std::vector<unsigned int>().swap(indices_);
      omerr() << "Topology caching failed\n";
    }

    //===================================================================
    // Generate an index buffer on the GPU
    //===================================================================
    faceIndexBufferInitialized_ = false;

    if ( GLEW_ARB_vertex_buffer_object && !indices_.empty() ) {

      // generate buffer
      if (!face_index_buffer_)  glGenBuffersARB(1,  (GLuint*)  &face_index_buffer_);

      // index buffer
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, face_index_buffer_);

      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                      indices_.size() * sizeof(unsigned int),
                      &indices_[0],
                      GL_STATIC_DRAW_ARB);

      faceIndexBufferInitialized_ = true;

      // unbind buffer
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    }

  }

}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
draw(GLState& _state, unsigned int _drawMode)
{
  glDepthFunc(depthFunc());


  if (_drawMode & DrawModes::POINTS)
  {
    enable_arrays(VERTEX_ARRAY);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_vertices();
  }


  if ( ( _drawMode & DrawModes::POINTS_COLORED ) && mesh_.has_vertex_colors())
  {
    enable_arrays(VERTEX_ARRAY | COLOR_ARRAY);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_vertices();
  }


  if ( ( _drawMode & DrawModes::POINTS_SHADED ) && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_vertices();
  }


  if (_drawMode & DrawModes::WIREFRAME)
  {
    glPushAttrib(GL_ENABLE_BIT);

    glDisable( GL_CULL_FACE );

    enable_arrays(VERTEX_ARRAY);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_faces(PER_VERTEX);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopAttrib();
  }


  if (_drawMode & DrawModes::HIDDENLINE)
  {
    enable_arrays(VERTEX_ARRAY);

    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);

    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDepthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);
    draw_faces(PER_VERTEX);
    glDepthFunc(depthFunc());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) && mesh_.has_face_normals())
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_NORMALS);
    glDepthRange(0.0, 1.0);
  }


  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED ) && mesh_.has_vertex_normals() )
  {
//     if ( parent() != 0 ) {
//       if ( parent()->className() == "ShaderNode" ) {
//
//         ShaderNode* node = dynamic_cast< ShaderNode* > ( parent() );
//
//         GLSL::PtrProgram program = node->getShader( DrawModes::SOLID_PHONG_SHADED );
//
//         // Enable own Phong shader
//         program->use();

        enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY);
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glDepthRange(0.01, 1.0);
        draw_faces(PER_VERTEX);
        glDepthRange(0.0, 1.0);

        //disable own Phong shader
//         program->disable();
//       }
//     }
  }

  if ( ( _drawMode & DrawModes::SOLID_ENV_MAPPED ) && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
  }


  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED )&& mesh_.has_face_colors())
  {
    Vec4f base_color_backup = _state.base_color();

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_COLORS);
    glDepthRange(0.0, 1.0);

    _state.set_base_color(base_color_backup);
  }


  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED ) && mesh_.has_face_colors() && mesh_.has_face_normals())
  {
    Vec4f base_color_backup = _state.base_color();
    GLboolean colMaterial;

    glEnable(GL_LIGHTING);

    colMaterial = glIsEnabled(GL_COLOR_MATERIAL);
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );

    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_NORMALS_COLORS);
    glDepthRange(0.0, 1.0);

    if( !colMaterial )
	glDisable(GL_COLOR_MATERIAL);

    _state.set_base_color(base_color_backup);
  }


  if ( ( _drawMode & DrawModes::SOLID_POINTS_COLORED ) && mesh_.has_vertex_colors())
  {
    Vec4f base_color_backup = _state.base_color();

    enable_arrays(VERTEX_ARRAY | COLOR_ARRAY);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);

    _state.set_base_color(base_color_backup);
  }


  if ( ( _drawMode & DrawModes::SOLID_TEXTURED ) && mesh_.has_vertex_texcoords2D())
  {
    enable_arrays(VERTEX_ARRAY | TEXTURE_COORD_2D_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }


  if ( ( _drawMode & DrawModes::SOLID_TEXTURED_SHADED ) && mesh_.has_vertex_texcoords2D() && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY | TEXTURE_COORD_2D_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }


  if ( ( _drawMode & DrawModes::SOLID_1DTEXTURED ) && mesh_.has_vertex_texcoords1D())
  {
    enable_arrays(VERTEX_ARRAY | TEXTURE_COORD_1D_ARRAY);
    glEnable(GL_TEXTURE_1D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_1D);
  }


  if ( ( _drawMode & DrawModes::SOLID_1DTEXTURED_SHADED ) && mesh_.has_vertex_texcoords1D() && mesh_.has_vertex_normals())
  {
    // store and change colors
    const Vec4f ambient  = _state.ambient_color();
    const Vec4f diffuse  = _state.diffuse_color();
    const Vec4f specular = _state.specular_color();
    _state.set_ambient_color  (Vec4f(0.1, 0.1, 0.1, 1.0));
    _state.set_diffuse_color  (Vec4f(0.8, 0.8, 0.8, 1.0));
    _state.set_specular_color (Vec4f(1.0, 1.0, 1.0, 1.0));

    // store and change texture mode
    GLint texmode;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texmode);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY | TEXTURE_COORD_1D_ARRAY);
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_1D);

    // restore colors
    _state.set_ambient_color(ambient);
    _state.set_diffuse_color(diffuse);
    _state.set_specular_color(specular);

    // restore texture mode
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texmode);
  }



  if ( ( _drawMode & DrawModes::SOLID_3DTEXTURED ) && mesh_.has_vertex_texcoords3D())
  {
    enable_arrays(VERTEX_ARRAY | TEXTURE_COORD_3D_ARRAY);
    glEnable(GL_TEXTURE_3D);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_3D);
  }


  if ( ( _drawMode & DrawModes::SOLID_3DTEXTURED_SHADED ) && mesh_.has_vertex_texcoords3D() && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY | TEXTURE_COORD_3D_ARRAY);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_3D);
  }

  // Textured by using coordinates stored in halfedges
  if ( ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE ) && mesh_.has_halfedge_texcoords2D())
  {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_HALFEDGE_TEXTURED);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }

  // Textured by using coordinates stored in halfedges
  if ( ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED ) && mesh_.has_halfedge_texcoords2D() && mesh_.has_face_normals())
  {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_HALFEDGE_TEXTURED);
    glDepthRange(0.0, 1.0);
    glDisable(GL_TEXTURE_2D);
  }

  // If in shader mode, just draw, as the shader has to be set by a shadernode above this node
  if ( (_drawMode & DrawModes::SOLID_SHADER )  ) {
    if ( mesh_.has_face_normals() )
      enable_arrays( VERTEX_ARRAY | NORMAL_ARRAY);
    else
      enable_arrays( VERTEX_ARRAY );
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
  }


  enable_arrays(0);
  glDepthFunc(GL_LESS);
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
draw_vertices()
{
  glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
draw_faces(FaceMode _mode)
{
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
                                      f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  typename Mesh::ConstFaceHalfedgeIter fh_it;

  switch (_mode)
  {
    case FACE_NORMALS:
    {
      if (mesh_.is_trimesh())
      {
         glBegin(GL_TRIANGLES);
         for (; f_it!=f_end; ++f_it)
         {
            glNormal(mesh_.normal(f_it));
            glVertex(mesh_.point(fv_it=mesh_.cfv_iter(f_it)));
            glVertex(mesh_.point(++fv_it));
            glVertex(mesh_.point(++fv_it));
         }
         glEnd();
      }
      else
      {
         for (; f_it!=f_end; ++f_it)
         {
            glBegin(GL_POLYGON);
            glNormal(mesh_.normal(f_it));
            for (fv_it=mesh_.cfv_iter(f_it.handle()); fv_it; ++fv_it)
               glVertex(mesh_.point(fv_it));
            glEnd();
         }
      }
      break;
    }


    case FACE_COLORS:
    {
      if (mesh_.is_trimesh())
      {
         glBegin(GL_TRIANGLES);
         for (; f_it!=f_end; ++f_it)
         {
            glColor(mesh_.color(f_it));
            glVertex(mesh_.point(fv_it=mesh_.cfv_iter(f_it)));
            glVertex(mesh_.point(++fv_it));
            glVertex(mesh_.point(++fv_it));
         }
         glEnd();
      }
      else
      {
         for (; f_it!=f_end; ++f_it)
         {
            glBegin(GL_POLYGON);
            glColor(mesh_.color(f_it));
            for (fv_it=mesh_.cfv_iter(f_it.handle()); fv_it; ++fv_it)
               glVertex(mesh_.point(fv_it));
            glEnd();
         }
      }
      break;
    }


    case FACE_NORMALS_COLORS:
    {
      if (mesh_.is_trimesh())
      {
         glBegin(GL_TRIANGLES);
         for (; f_it!=f_end; ++f_it)
         {
            glColor(mesh_.color(f_it));
            glNormal(mesh_.normal(f_it));
            glVertex(mesh_.point(fv_it=mesh_.cfv_iter(f_it)));
            glVertex(mesh_.point(++fv_it));
            glVertex(mesh_.point(++fv_it));
         }
         glEnd();
      }
      else
      {
         for (; f_it!=f_end; ++f_it)
         {
            glBegin(GL_POLYGON);
            glColor(mesh_.color(f_it));
            glNormal(mesh_.normal(f_it));
            for (fv_it=mesh_.cfv_iter(f_it.handle()); fv_it; ++fv_it)
               glVertex(mesh_.point(fv_it));
            glEnd();
         }
      }
      break;
    }

    // Henrik: Only implemented for 2D halfedge(per face) textures
    case FACE_HALFEDGE_TEXTURED:
    {
      if (mesh_.is_trimesh())
      {
        if ( !textureMap_ || !mesh_.has_face_texture_index() ) {
          typename Mesh::Point point;
          typename Mesh::TexCoord2D tex2d;
          glBegin(GL_TRIANGLES);
          for (; f_it!=f_end; ++f_it)
          {
            glNormal(mesh_.normal(f_it));
            for (fh_it = mesh_.cfh_iter(f_it.handle());fh_it;++fh_it)
            {
              point = mesh_.point(mesh_.to_vertex_handle(fh_it));
              tex2d = mesh_.texcoord2D(fh_it);
              glTexCoord2f(tex2d[0], tex2d[1]);
              glVertex(point);
            }
          }
          glEnd();
        } else {

          typename Mesh::Point point;
          typename Mesh::TexCoord2D tex2d;

          for (; f_it!=f_end; ++f_it)
          {
            int texture_ = mesh_.texture_index(f_it);

            if ( texture_ >= (int)textureMap_->size() || texture_ < 0 ) {
              std::cerr << "Illegal texture index ... trying to access " << texture_ << std::endl;
              continue;
            }

            glBindTexture( GL_TEXTURE_2D, (*textureMap_)[texture_] );

            glBegin(GL_TRIANGLES);

            glNormal(mesh_.normal(f_it));
            glColor(mesh_.color(f_it.handle()));

            for (fh_it = mesh_.cfh_iter(f_it.handle());fh_it;++fh_it)
            {
              point = mesh_.point(mesh_.to_vertex_handle(fh_it));
              tex2d = mesh_.texcoord2D(fh_it);
              glTexCoord2f(tex2d[0], tex2d[1]);
              glVertex(point);
            }

            glEnd();

          }


        }
      }
      else
      {
        // NOT IMPLEMENTED
      }
      break;
    }


    case PER_VERTEX:
    {
      if (mesh_.is_trimesh())
      {
        // try cached triangle indices
        if (!indices_.empty())
        {

          // If we have an index buffer on the GPU, use it
          if ( faceIndexBufferInitialized_  ) {

            // As we have a list of all faces on the GPU bind it
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,face_index_buffer_);

            // Draw it
            glDrawElements(GL_TRIANGLES,
                           indices_.size(),
                           GL_UNSIGNED_INT,
                           0);

            // And unbind it again
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);

          } else {

            glDrawElements(GL_TRIANGLES,
                           indices_.size(),
                           GL_UNSIGNED_INT,
                           &indices_[0]);
          }


        }

        // otherwise use immediate mode
        else
        {
          glBegin(GL_TRIANGLES);
          for (; f_it!=f_end; ++f_it)
          {
            glArrayElement((fv_it=mesh_.cfv_iter(f_it)).handle().idx());
            glArrayElement((++fv_it).handle().idx());
            glArrayElement((++fv_it).handle().idx());
          }
          glEnd();
        }
      }
      else
      {
        for (; f_it!=f_end; ++f_it)
        {
          glBegin(GL_POLYGON);
          for (fv_it=mesh_.cfv_iter(f_it.handle()); fv_it; ++fv_it)
            glArrayElement(fv_it.handle().idx());
          glEnd();
        }
      }
      break;
    }
  }
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
pick(GLState& _state, PickTarget _target)
{
  switch (_target)
  {
    case PICK_VERTEX:
    {
      pick_vertices(_state);
      break;
    }

    case PICK_ANYTHING:
    case PICK_FACE:
    {
      pick_faces(_state);
      break;
    }

    default:
      break;
  }
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
pick_vertices(GLState& _state)
{
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
                                 v_end(mesh_.vertices_end());
  GLuint                         idx(0);

  if (!_state.pick_set_maximum (mesh_.n_vertices()))
  {
    omerr() << "MeshNode::pick_vertices: color range too small, "
            << "picking failed\n";
    return;
  }

  for (; v_it!=v_end; ++v_it, ++idx)
  {
    _state.pick_set_name (idx);
    glBegin(GL_POINTS);
    glVertex(mesh_.point(v_it));
    glEnd();
  }
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
pick_faces(GLState& _state)
{
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
                                      f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  ColorTranslator                     ct;


  if (!_state.pick_set_maximum (mesh_.n_faces()))
  {
    omerr() << "MeshNode::pick_faces: color range too small, "
	    << "picking failed\n";
    return;
  }

  if (mesh_.is_trimesh())
  {
    for (; f_it!=f_end; ++f_it)
    {
      // set index
      _state.pick_set_name (f_it.handle().idx());

      glBegin(GL_TRIANGLES);
      glVertex(mesh_.point(fv_it=mesh_.cfv_iter(f_it)));
      glVertex(mesh_.point(++fv_it));
      glVertex(mesh_.point(++fv_it));
      glEnd();
    }
  }
  else
  {
    for (; f_it!=f_end; ++f_it)
    {
      // set index
      _state.pick_set_name (f_it.handle().idx());

      glBegin(GL_POLYGON);

      for (fv_it=mesh_.cfv_iter(f_it); fv_it; ++fv_it)
	     glVertex(mesh_.point(fv_it));

      glEnd();
    }
  }
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

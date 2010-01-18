/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 7994 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-12-18 12:18:13 +0100 (Fr, 18. Dez 2009) $                   *
 *                                                                           *
\*===========================================================================*/




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
#include <OpenMesh/Core/Utils/Property.hh>

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
    face_index_buffer_(0),
    faceIndexBufferInitialized_(false),
    textureMap_(0),
    propertyMap_(0),
    default_halfedge_textcoord_property_("h:texcoords2D"),
    indexPropertyName_("f:textureindex"),
    updateFaceList_(true),
    updateVertexList_(true),
    updateEdgeList_(true),
    updateAnyList_(true),
    faceBaseIndex_(0),
    vertexBaseIndex_(0),
    edgeBaseIndex_(0),
    anyBaseIndex_(0),
    bbMin_(FLT_MAX,  FLT_MAX,  FLT_MAX),
    bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
  faceList_ = glGenLists (1);
  vertexList_ = glGenLists (1);
  edgeList_ = glGenLists (1);
  anyList_ = glGenLists (3);
}


//----------------------------------------------------------------------------


template<class Mesh>
MeshNodeT<Mesh>::
~MeshNodeT()
{

  if (face_index_buffer_)
    glDeleteBuffersARB(1, (GLuint*)  &face_index_buffer_ );

  if (faceList_)
    glDeleteLists (faceList_, 1);

  if (vertexList_)
    glDeleteLists (vertexList_, 1);

  if (edgeList_)
    glDeleteLists (edgeList_, 1);

  if (anyList_)
    glDeleteLists (anyList_, 3);
}


//----------------------------------------------------------------------------

template<class Mesh>
unsigned int
MeshNodeT<Mesh>::
availableDrawModes() const
{
  unsigned int drawModes(0);

  drawModes |= DrawModes::SOLID_SHADER;

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
  bool use_vbo =
    ((_arrays == VERTEX_ARRAY || _arrays == (VERTEX_ARRAY | NORMAL_ARRAY)) &&
     (vertexBufferInitialized_ && normalBufferInitialized_) ) ;
//   omlog() << "Use VBO: " << use_vbo << std::endl;


  // unbind VBO buffers
  if (!use_vbo && vertex_buffer_)
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

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
update_topology()
{
  updateFaceList_ = true;
  updateVertexList_ = true;

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

  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) && mesh_.has_face_normals())
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_NORMALS);
    glDepthRange(0.0, 1.0);
  }

//   if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) && mesh_.has_vertex_normals())
//   if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED ) && mesh_.has_vertex_normals() )

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
    glEnable(GL_LIGHTING);

    glShadeModel(GL_FLAT);
    glDepthRange(0.01, 1.0);
    draw_faces(FACE_NORMALS_COLORS);
    glDepthRange(0.0, 1.0);

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
  // TODO: Check not only mesh_.has_halfedge_texcoords2D but check if custom property is available
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
  // TODO: Check not only mesh_.has_halfedge_texcoords2D but check if custom property is available
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


    // propertyMap_ maps between an int index stored in the Mesh describing which texture to use
    // and a property name giving 2D Texture coordinates for halfedges ( texcoords for to vertex )

    case FACE_HALFEDGE_TEXTURED:
    {
      if (mesh_.is_trimesh())
      {

        OpenMesh::FPropHandleT< int > texture_index_property;
        if ( !mesh_.get_property_handle(texture_index_property,indexPropertyName_) ) {
          if( indexPropertyName_ != "No Texture Index")
	    std::cerr << "Unable to get per face texture Index property named " << indexPropertyName_ << std::endl;
          if ( !mesh_.get_property_handle(texture_index_property,"f:textureindex") ) {
            std::cerr << "Unable to get standard per face texture Index property" << std::endl;
            texture_index_property.reset();
          }
        }

        // textureMap_ maps between an int index stored in the Mesh describing which texture to use
        // and the GluInt bound by the TextureNode. If such a map is not available, assume TextureNode
        // has already bound a texture and we use only one texture.
        // Additionally if we do not have an texture index property, we do not know which textures
        // should be used .. therefore do not  switch textures if this property is missing.
        if ( !textureMap_ || !texture_index_property.is_valid() ) {

          // Get texture coords property
          OpenMesh::HPropHandleT< typename Mesh::TexCoord2D > texture_coord_property;
          if ( !mesh_.get_property_handle(texture_coord_property,default_halfedge_textcoord_property_) ) {
            std::cerr << "Error: Unable to get per face texture coordinate property named "
                      << default_halfedge_textcoord_property_ << std::endl;
            std::cerr << "Unable to texture without texture coordinates" << std::endl;
            return;
          }

          typename Mesh::Point point;
          typename Mesh::TexCoord2D tex2d;
          glBegin(GL_TRIANGLES);
          for (; f_it!=f_end; ++f_it) {
            glNormal(mesh_.normal(f_it));
            for (fh_it = mesh_.cfh_iter(f_it.handle());fh_it;++fh_it) {
              point = mesh_.point(mesh_.to_vertex_handle(fh_it));
              tex2d = mesh_.property(texture_coord_property,fh_it);
              glTexCoord2f(tex2d[0], tex2d[1]);
              glVertex(point);
            }
          }
          glEnd();
        } else {

          OpenMesh::HPropHandleT< typename Mesh::TexCoord2D > texture_coord_property;
          int last_texture = -1;

          typename Mesh::Point point;
          typename Mesh::TexCoord2D tex2d;

          for (; f_it!=f_end; ++f_it)
          {
            int texture = mesh_.property(texture_index_property,f_it);

            if (texture == -1) ///TODO dont skip the face, draw without texture instead
              continue;

            if ( last_texture != texture ) {

              if ( textureMap_->find(texture) == textureMap_->end() ) {
                std::cerr << "Illegal texture index ... trying to access " << texture << std::endl;
                last_texture = -1;
                continue;
              }

              // Get texture coords property
              if ( !propertyMap_ || !mesh_.get_property_handle(texture_coord_property,(*propertyMap_)[texture]) ) {
                if ( propertyMap_)
                  std::cerr << "Error: Unable to get per face texture coordinate property named "
                            << (*propertyMap_)[texture]  << std::endl;
                if ( !mesh_.get_property_handle(texture_coord_property,"h:texcoords2D") ) {
                  std::cerr << "Fallback: Unable to get standard Property for per halfedge texcoords" << std::endl;
                  std::cerr << "Unable to texture face without texture coordinates" << std::endl;
                  last_texture = -1;
                  continue;
                }
              }

              glBindTexture( GL_TEXTURE_2D, (*textureMap_)[texture] );

              // Remember active texture to skip extra switches
              last_texture = texture;

            }

            glBegin(GL_TRIANGLES);

            glNormal(mesh_.normal(f_it));
            glColor(mesh_.color(f_it.handle()));

            for (fh_it = mesh_.cfh_iter(f_it.handle());fh_it;++fh_it)
            {
              point = mesh_.point(mesh_.to_vertex_handle(fh_it));
              tex2d = mesh_.property(texture_coord_property,fh_it);
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
pick_vertices(GLState& _state, bool _front)
{
 
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


  if ( mesh_.n_faces() > 0 )
  {
    if (!_state.pick_set_maximum (mesh_.n_faces()))
    {
      omerr() << "MeshNode::pick_faces: color range too small, "
        << "picking failed\n";
      return;
    }
  }
  else
  {
    if (!_state.pick_set_maximum (1))
    {
      omerr() << "Strange pickSetMAximum failed for index 1 in MeshNode\n";
      return;
    }
  }

  if (faceList_ && !updateFaceList_ && _state.pick_current_index () == faceBaseIndex_)
  {
    glCallList (faceList_);
    return;
  }

  if (faceList_)
  {
    glNewList (faceList_, GL_COMPILE);
    updateFaceList_ = false;
    faceBaseIndex_ = _state.pick_current_index ();
  }

  if (_state.color_picking ())
  {
    update_pick_buffers ();
    unsigned int idx = 0;

    if (mesh_.is_trimesh())
    {
      for (; f_it!=f_end; ++f_it)
      {
        pickColorBuf_[idx]    = _state.pick_get_name_color (f_it.handle().idx());
        pickColorBuf_[idx+1]  = _state.pick_get_name_color (f_it.handle().idx());
        pickColorBuf_[idx+2]  = _state.pick_get_name_color (f_it.handle().idx());
        pickVertexBuf_[idx]   = mesh_.point(fv_it=mesh_.cfv_iter(f_it));
        pickVertexBuf_[idx+1] = mesh_.point(++fv_it);
        pickVertexBuf_[idx+2] = mesh_.point(++fv_it);
        idx += 3;
      }

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glVertexPointer (&pickVertexBuf_[0]);
      glColorPointer(&pickColorBuf_[0]);

      glDrawArrays(GL_TRIANGLES, 0, mesh_.n_faces() * 3);

      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);

    }
    else
    {
      unsigned int face = 0;
      std::vector <GLint> first;
      std::vector <GLsizei> count;

      first.resize (mesh_.n_faces());
      count.resize (mesh_.n_faces());

      for (; f_it!=f_end; ++f_it, ++face)
      {
        unsigned int cnt = 0;
        first[face] = idx;


        for (fv_it=mesh_.cfv_iter(f_it); fv_it; ++fv_it, ++idx, ++cnt)
        {
          pickVertexBuf_[idx] = mesh_.point(fv_it);
          pickColorBuf_[idx] = _state.pick_get_name_color (f_it.handle().idx());
        }
        count[face] = cnt; 
      }
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glVertexPointer (&pickVertexBuf_[0]);
      glColorPointer(&pickColorBuf_[0]);

      glMultiDrawArrays(GL_POLYGON, &first[0], &count[0], mesh_.n_faces());

      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);
    }
  }
  else
  {

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

  if (faceList_)
  {
    glEndList ();
    glCallList (faceList_);
  }
}

//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
pick_edges(GLState& _state, bool _front)
{
  typename Mesh::ConstEdgeIter        e_it(mesh_.edges_sbegin()),
                                      e_end(mesh_.edges_end());

  if (!_state.pick_set_maximum (mesh_.n_edges()))
  {
    omerr() << "MeshNode::pick_edges: color range too small, "
            << "picking failed\n";
    return;
  }

  if (_front)
  {
    enable_arrays(VERTEX_ARRAY);

    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _state.set_base_color(clear_color);

    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDepthFunc(GL_LEQUAL);
    _state.set_base_color(base_color);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    enable_arrays(0);
  }

  if (edgeList_ && !updateEdgeList_ && _state.pick_current_index () == edgeBaseIndex_)
  {
    glCallList (edgeList_);
    if (_front)
      glDepthFunc(depthFunc());
    return;
  }

  if (edgeList_)
  {
    glNewList (edgeList_, GL_COMPILE);
    updateEdgeList_ = false;
    edgeBaseIndex_ = _state.pick_current_index ();
  }

  if (_state.color_picking ())
  {
    unsigned int idx = 0;
    update_pick_buffers ();

    for (; e_it!=e_end; ++e_it)
    {
      pickColorBuf_[idx]    = _state.pick_get_name_color (e_it.handle().idx());
      pickColorBuf_[idx+1]  = _state.pick_get_name_color (e_it.handle().idx());
      pickVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
      pickVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));
      idx += 2;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer (&pickVertexBuf_[0]);
    glColorPointer(&pickColorBuf_[0]);

    glDrawArrays(GL_LINES, 0, mesh_.n_edges() * 2);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  else
  {
    for (; e_it!=e_end; ++e_it)
    {
      _state.pick_set_name (e_it.handle().idx());
      glBegin(GL_LINES);
      glVertex(mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0))));
      glVertex(mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1))));
      glEnd();
    }
  }

  if (edgeList_)
  {
    glEndList ();
    glCallList (edgeList_);
  }

  if (_front)
    glDepthFunc(depthFunc());
}

//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
pick_any(GLState& _state)
{
  unsigned int numElements = mesh_.n_faces() + mesh_.n_edges() + mesh_.n_vertices();

  // nothing to pick ?
  if (numElements <= 0)
    return;

  if (!_state.pick_set_maximum (numElements))
  {
    omerr() << "MeshNode::pick_any: color range too small, "
            << "picking failed\n";
    return;
  }

  if (anyList_ && !updateAnyList_ && _state.pick_current_index () == anyBaseIndex_)
  {
    glCallList (anyList_);
    glCallList (anyList_+1);
    glCallList (anyList_+2);
    return;
  }

  if (anyList_)
  {
    glNewList (anyList_, GL_COMPILE);
    updateAnyList_ = false;
    anyBaseIndex_ = _state.pick_current_index ();
  }

  // faces
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
                                      f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;

  // edges
  typename Mesh::ConstEdgeIter        e_it(mesh_.edges_sbegin()),
                                      e_end(mesh_.edges_end());

  // vertices
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
                                 v_end(mesh_.vertices_end());
  GLuint                         vidx(mesh_.n_faces() + mesh_.n_edges());



  if (_state.color_picking ())
  {
    update_pick_buffers ();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // faces
    unsigned int idx = 0;
    if (mesh_.is_trimesh())
    {
      for (; f_it!=f_end; ++f_it)
      {
        pickColorBuf_[idx]    = _state.pick_get_name_color (f_it.handle().idx());
        pickColorBuf_[idx+1]  = _state.pick_get_name_color (f_it.handle().idx());
        pickColorBuf_[idx+2]  = _state.pick_get_name_color (f_it.handle().idx());
        pickVertexBuf_[idx]   = mesh_.point(fv_it=mesh_.cfv_iter(f_it));
        pickVertexBuf_[idx+1] = mesh_.point(++fv_it);
        pickVertexBuf_[idx+2] = mesh_.point(++fv_it);
        idx += 3;
      }

      glVertexPointer (&pickVertexBuf_[0]);
      glColorPointer(&pickColorBuf_[0]);

      glDrawArrays(GL_TRIANGLES, 0, mesh_.n_faces() * 3);

    }
    else
    {
      unsigned int face = 0;
      std::vector <GLint> first;
      std::vector <GLsizei> count;

      first.resize (mesh_.n_faces());
      count.resize (mesh_.n_faces());

      for (; f_it!=f_end; ++f_it, ++face)
      {
        unsigned int cnt = 0;
        first[face] = idx;


        for (fv_it=mesh_.cfv_iter(f_it); fv_it; ++fv_it, ++idx, ++cnt)
        {
          pickVertexBuf_[idx] = mesh_.point(fv_it);
          pickColorBuf_[idx] = _state.pick_get_name_color (f_it.handle().idx());
        }
        count[face] = cnt; 
      }

      glVertexPointer (&pickVertexBuf_[0]);
      glColorPointer(&pickColorBuf_[0]);

      glMultiDrawArrays(GL_POLYGON, &first[0], &count[0], mesh_.n_faces());

    }

    if (anyList_)
    {
      glEndList ();
      glNewList (anyList_+1, GL_COMPILE);
    }

    glDepthFunc(GL_LEQUAL);

    // edges
    idx = 0;
    for (; e_it!=e_end; ++e_it)
    {
      pickColorBuf_[idx]    = _state.pick_get_name_color (mesh_.n_faces() + e_it.handle().idx());
      pickColorBuf_[idx+1]  = _state.pick_get_name_color (mesh_.n_faces() + e_it.handle().idx());
      pickVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
      pickVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));
      idx += 2;
    }

    glVertexPointer (&pickVertexBuf_[0]);
    glColorPointer(&pickColorBuf_[0]);

    glDrawArrays(GL_LINES, 0, mesh_.n_edges() * 2);

    if (anyList_)
    {
      glEndList ();
      glNewList (anyList_+2, GL_COMPILE);
    }

    // vertices
    idx = 0;
    for (; v_it!=v_end; ++v_it, ++idx, ++vidx)
    {
      pickColorBuf_[idx] = _state.pick_get_name_color (vidx);
      pickVertexBuf_[idx] = mesh_.point(v_it);
    }

    glVertexPointer (&pickVertexBuf_[0]);
    glColorPointer(&pickColorBuf_[0]);

    glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDepthFunc(depthFunc());
  }
  else
  {
    // faces
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

    if (anyList_)
    {
      glEndList ();
      glNewList (anyList_+1, GL_COMPILE);
    }

    glDepthFunc(GL_LEQUAL);

    // edges
    for (; e_it!=e_end; ++e_it)
    {
      _state.pick_set_name (mesh_.n_faces() + e_it.handle().idx());
      glBegin(GL_LINES);
      glVertex(mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0))));
      glVertex(mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1))));
      glEnd();
    }

    if (anyList_)
    {
      glEndList ();
      glNewList (anyList_+2, GL_COMPILE);
    }

    // vertices
    for (; v_it!=v_end; ++v_it, ++vidx)
    {
      _state.pick_set_name (vidx);
      glBegin(GL_POINTS);
      glVertex(mesh_.point(v_it));
      glEnd();
    }

    glDepthFunc(depthFunc());
  }


  if (anyList_)
  {
    glEndList ();
    glCallList (anyList_);
    glCallList (anyList_+1);
    glCallList (anyList_+2);
  }
}

//----------------------------------------------------------------------------

template<class Mesh>
void
MeshNodeT<Mesh>::
update_pick_buffers ()
{
  unsigned int nfv = 0;
  if (mesh_.is_trimesh())
    nfv = mesh_.n_faces() * 3;
  else
  {
    // count number of vertices we need for faces in our buffer
    typename Mesh::ConstFaceIter    f_it(mesh_.faces_sbegin()),
                                    f_end(mesh_.faces_end());
    typename Mesh::ConstFaceVertexIter  fv_it;
    for (; f_it!=f_end; ++f_it)
    {
      for (fv_it=mesh_.cfv_iter(f_it); fv_it; ++fv_it)
        nfv++;
    }
  }

  pickVertexBuf_.resize (qMax (mesh_.n_vertices(), qMax (mesh_.n_edges() * 2, nfv)));
  pickColorBuf_.resize (qMax (mesh_.n_vertices(), qMax (mesh_.n_edges() * 2, nfv)));
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
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
    textureMap_(0),
    propertyMap_(0),
    default_halfedge_textcoord_property_("h:texcoords2D"),
    bbMin_(FLT_MAX,  FLT_MAX,  FLT_MAX),
    bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{

}


//----------------------------------------------------------------------------


template<class Mesh>
MeshNodeT<Mesh>::
~MeshNodeT()
{

}


//----------------------------------------------------------------------------

template<class Mesh>
unsigned int
MeshNodeT<Mesh>::
availableDrawModes() const
{
  unsigned int drawModes(0);

  drawModes |= DrawModes::SOLID_SHADER;

  if (mesh_.has_vertex_texcoords1D())
  {
    drawModes |= DrawModes::SOLID_1DTEXTURED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_1DTEXTURED_SHADED;
  }

  if (mesh_.has_vertex_texcoords3D())
  {
    drawModes |= DrawModes::SOLID_3DTEXTURED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_3DTEXTURED_SHADED;
  }



  return drawModes;
}


//----------------------------------------------------------------------------


template<class Mesh>
void
MeshNodeT<Mesh>::
enable_arrays(unsigned int _arrays)
{

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
draw(GLState& _state, unsigned int _drawMode)
{
  glDepthFunc(depthFunc());

  if ( ( _drawMode & DrawModes::SOLID_ENV_MAPPED ) && mesh_.has_vertex_normals())
  {
    enable_arrays(VERTEX_ARRAY | NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glDepthRange(0.01, 1.0);
    draw_faces(PER_VERTEX);
    glDepthRange(0.0, 1.0);
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

  }
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

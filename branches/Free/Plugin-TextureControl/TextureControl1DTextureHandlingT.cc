/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#define TEXTURECONTROL_1D_TEXTURE_HANDLING_C

#include "TextureControl.hh"
#include "TextureMath.hh"

template< typename MeshT >
void TextureControlPlugin::copyTexture ( Texture& _texture , MeshT& _mesh, OpenMesh::VPropHandleT< double > _texProp )
{
  // Compute the minimal and maximal values for this texture
  double max = -FLT_MIN;
  double min =  FLT_MAX;


  // Compute minimal and maximal value of the coordinates
  // Keep track of absolute values!
  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin() ; v_it != _mesh.vertices_end(); ++v_it) {

      max = std::max( _mesh.property(_texProp,*v_it) , max);
      min = std::min( _mesh.property(_texProp,*v_it) , min);

  }


  if ( !_mesh.has_vertex_texcoords2D() )
    _mesh.request_vertex_texcoords2D();
  
  TextureMath convert(_texture.parameters,min,max);

  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {
    // Get the value of the property
    double value = _mesh.property(_texProp, *v_it);

    // Mangle it with the predefined user options (including absolute, clamping, ...)
    value = convert.transform(value);
    
    // Write result to the OpenMesh texture coordinates ( 2d accessing the diagonal of a 2d texture)
    _mesh.set_texcoord2D( *v_it, ACG::Vec2f(float(value), float(value) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( Texture& _texture , MeshT& _mesh, OpenMesh::HPropHandleT< double > _texProp )
{
  // Compute the minimal and maximal values for this texture
  double max = -FLT_MIN;
  double min =  FLT_MAX;

  // Compute minimal and maximal value of the coordinates
  // Keep track of absolute values!
  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin() ; h_it != _mesh.halfedges_end(); ++h_it) {

      max = std::max( _mesh.property(_texProp,*h_it) , max);
      min = std::min( _mesh.property(_texProp,*h_it) , min);

  }

  if ( !_mesh.has_halfedge_texcoords2D() )
    _mesh.request_halfedge_texcoords2D();
  
  TextureMath convert(_texture.parameters,min,max);

  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin(); h_it != _mesh.halfedges_end(); ++h_it) {

    // Get the value of the property
    double value = _mesh.property(_texProp, *h_it);
    
    // Mangle it with the predefined user options (including absolute, clamping, ...)
    value = convert.transform(value);
    
    // Write result to the OpenMesh texture coordinates ( 2d accessing the diagonal of a 2d texture)
    _mesh.set_texcoord2D( *h_it, ACG::Vec2f(float(value), float(value) ) );
  }
}


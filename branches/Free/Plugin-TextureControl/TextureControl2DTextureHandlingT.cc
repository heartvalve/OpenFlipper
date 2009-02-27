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

#define TEXTURECONTROL_2D_TEXTURE_HANDLING_C

#include "TextureControl.hh"

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int /*_textureid*/, MeshT& _mesh, OpenMesh::VPropHandleT< OpenMesh::Vec2d > _texProp )
{
  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {

    // Get the value of the property
    OpenMesh::Vec2d value = _mesh.property(_texProp, v_it);

    // Write result to the openmesh texture coordinates
    _mesh.set_texcoord2D( v_it, ACG::Vec2f(float(value[0]), float(value[1]) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int /*_textureid*/, MeshT& _mesh, OpenMesh::HPropHandleT< OpenMesh::Vec2d > _texProp )
{
  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin(); h_it != _mesh.halfedges_end(); ++h_it) {

    // Get the value of the property
    OpenMesh::Vec2d value = _mesh.property(_texProp, h_it);

    // Write result to the openmesh texture coordinates
    _mesh.set_texcoord2D( h_it, ACG::Vec2f(float(value[0]), float(value[1]) ) );
  }
}



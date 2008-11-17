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
//   $Revision: 3583 $
//   $Author: moebius $
//   $Date: 2008-10-27 17:23:38 +0100 (Mo, 27. Okt 2008) $
//
//=============================================================================

#define TEXTURECONTROL_1D_TEXTURE_HANDLING_C

#include "TextureControl.hh"

template< typename MeshT >
void TextureControlPlugin::computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::VPropHandleT< double > _texture,
                                                               double& _min , double& _max) {
   const bool   abs = textures_[_textureid].abs;
   const bool   clamp = textures_[_textureid].clamp ;
   const double clamp_max = textures_[_textureid].clamp_max;
   const double clamp_min = textures_[_textureid].clamp_min;

   _max = FLT_MIN;
   _min = FLT_MAX;

   for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin() ; v_it != _mesh.vertices_end(); ++v_it) {
      if ( abs ) {
         _max = std::max( fabs(_mesh.property(_texture,v_it)) , _max);
         _min = std::min( fabs(_mesh.property(_texture,v_it)) , _min);
      } else {
         _max = std::max( _mesh.property(_texture,v_it) , _max);
         _min = std::min( _mesh.property(_texture,v_it) , _min);
      }
   }

   if ( clamp ) {
      if ( _max > clamp_max )
         _max = clamp_max;
      if (_min < clamp_min)
         _min = clamp_min;
   }
}

template< typename MeshT >
void TextureControlPlugin::computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::HPropHandleT< double > _texture,
                                                               double& _min , double& _max) {
   const bool   abs = textures_[_textureid].abs;
   const bool   clamp = textures_[_textureid].clamp ;
   const double clamp_max = textures_[_textureid].clamp_max;
   const double clamp_min = textures_[_textureid].clamp_min;

   _max = FLT_MIN;
   _min = FLT_MAX;

   for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin() ; h_it != _mesh.halfedges_end(); ++h_it) {
      if ( abs ) {
         _max = std::max( fabs(_mesh.property(_texture,h_it)) , _max);
         _min = std::min( fabs(_mesh.property(_texture,h_it)) , _min);
      } else {
         _max = std::max( _mesh.property(_texture,h_it) , _max);
         _min = std::min( _mesh.property(_texture,h_it) , _min);
      }
   }

   if ( clamp ) {
      if ( _max > clamp_max )
         _max = clamp_max;
      if (_min < clamp_min)
         _min = clamp_min;
   }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::VPropHandleT< double > _texProp )
{
  // Compute some basic values for this texture
  double max,min;
  computeMinMaxScalar(_textureid, _mesh, _texProp, min, max);

  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {
    // Get the value of the property
    double value = _mesh.property(_texProp, v_it);

    // Mangle it with the predefined user options
    computeValue(_textureid, min, max, value);

    // Write result to the openmesh texture coordinates ( 2d accessing the diagonal of a 2d texture)
    _mesh.set_texcoord2D( v_it, ACG::Vec2f(float(value), float(value) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::HPropHandleT< double > _texProp )
{
  // Compute some basic values for this texture
  double max,min;
  computeMinMaxScalar(_textureid, _mesh, _texProp, min, max);

  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin(); h_it != _mesh.halfedges_end(); ++h_it) {

    // Get the value of the property
    double value = _mesh.property(_texProp, h_it);

    // Mangle it with the predefined user options
    computeValue(_textureid, min, max, value);

    // Write result to the openmesh texture coordinates ( 2d accessing the diagonal of a 2d texture)
    _mesh.set_texcoord2D( h_it, ACG::Vec2f(float(value), float(value) ) );
  }
}


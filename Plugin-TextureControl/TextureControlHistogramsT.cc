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
//   $Revision: 3735 $
//   $Author: moebius $
//   $Date: 2008-11-19 11:18:02 +0100 (Mi, 19. Nov 2008) $
//
//=============================================================================


#include "TextureControl.hh"

template< typename MeshT >
void TextureControlPlugin::getOriginalHistogram(std::vector< double>& _x, std::vector< double>& _y,
                                                int _textureid, MeshT& _mesh,
                                                OpenMesh::VPropHandleT< double > _texProp) {
  _x.clear();
  _y.clear();

  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {
    double value = _mesh.property(_texProp, v_it);
  }

}



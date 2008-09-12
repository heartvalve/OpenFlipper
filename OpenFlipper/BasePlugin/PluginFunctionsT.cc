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
//  Plugin Functions
//
//=============================================================================

#define PLUGINFUNCTIONS_C

#include <OpenFlipper/common/Types.hh>

#include "PluginFunctions.hh"

namespace PluginFunctions {

 template <class MeshT , typename propT >
bool get_property_handle(MeshT* _mesh , QString _name , OpenMesh::VPropHandleT< propT > & _property ) {

  std::string name(_name.toAscii( ) );

  typename MeshT::prop_iterator p_it = _mesh->vprops_begin();
  int idx = 0;
  for ( ; p_it != _mesh->vprops_end() ; ++p_it , ++idx) {
    if ( (*p_it) && (*p_it)->name() == name ) {
      _property = ( OpenMesh::VPropHandleT< propT > )idx;
      return true;
    }
  }
  _property = OpenMesh::VPropHandleT< propT >  (-1);
  return false;

}

template <class MeshT , typename propT >
bool get_property_handle(MeshT* _mesh , QString _name , OpenMesh::FPropHandleT< propT > & _property ) {

  std::string name (_name.toAscii( ));

  typename MeshT::prop_iterator p_it = _mesh->fprops_begin();
  int idx = 0;
  for ( ; p_it != _mesh->fprops_end() ; ++p_it , ++idx) {
    if ( (*p_it) && (*p_it)->name() == name ) {
      _property = ( OpenMesh::FPropHandleT< propT > )idx;
      return true;
    }
  }
  _property = OpenMesh::FPropHandleT< propT >  (-1);
  return false;
}

template <class MeshT , typename propT >
bool get_property_handle(MeshT* _mesh , QString _name , OpenMesh::HPropHandleT< propT > & _property ) {

  std::string name (_name.toAscii( ));

  typename MeshT::prop_iterator p_it = _mesh->hprops_begin();
  int idx = 0;
  for ( ; p_it != _mesh->hprops_end() ; ++p_it , ++idx) {
    if ( (*p_it) && (*p_it)->name() == name ) {
      _property = ( OpenMesh::HPropHandleT< propT > )idx;
      return true;
    }
  }
  _property = OpenMesh::HPropHandleT< propT >  (-1);
  return false;
}

}

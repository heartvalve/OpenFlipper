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
//  CLASS ObjectIDPointerManagerT - IMPLEMENTATION
//
//=============================================================================

#define OBJECTIDPOINTERMANAGERT_C

//== INCLUDES =================================================================

#include "ObjectIDPointerManagerT.hh"

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ==========================================================

template< class Pointer >
bool ObjectIDPointerManagerT<Pointer>::get_pointer(int _identifier, Pointer & _pointer) {
  typename PointerObjectIDMap::iterator po_itr;
  typename PointerObjectIDMap::iterator po_itr_end = pointer_objectID_map_.end();

  po_itr =  pointer_objectID_map_.find(_identifier);

  // Last position reached = no element with ID _identifier in map
  if (po_itr == po_itr_end)
    return false;

  // Give pointer back
  _pointer = (Pointer)po_itr->second;
  return true;
}

//-----------------------------------------------------------------------------

template< class Pointer >
Pointer ObjectIDPointerManagerT<Pointer>::get_pointer(int _identifier) {
  Pointer tmp;
  if ( get_pointer(_identifier,tmp) )
    return tmp;
  else
    return NULL;
}

//-----------------------------------------------------------------------------

template< class Pointer >
bool ObjectIDPointerManagerT<Pointer>::add_pointer(int _identifier, const Pointer & _pointer) {
  typename PointerObjectIDMap::iterator po_itr;
  typename PointerObjectIDMap::iterator po_itr_end = pointer_objectID_map_.end();

  po_itr =  pointer_objectID_map_.find(_identifier);

  // Was already there
  if (po_itr != po_itr_end)
      return false;

  pointer_objectID_map_.insert(PointerObjectIDPair(_identifier, _pointer));
  return true;
}

//-----------------------------------------------------------------------------

template< class Pointer >
bool ObjectIDPointerManagerT<Pointer>::delete_object(int _identifier) {
  typename PointerObjectIDMap::iterator po_itr;
  typename PointerObjectIDMap::iterator po_itr_end = pointer_objectID_map_.end();

  po_itr =  pointer_objectID_map_.find(_identifier);

  // Last position reached = no element with ID _identifier in map
  if (po_itr == po_itr_end)
      return false;

  pointer_objectID_map_.erase(_identifier);
  return true;
}

//-----------------------------------------------------------------------------

template< class Pointer >
void ObjectIDPointerManagerT<Pointer>::get_object_ids(std::vector<int> & _ids)
{
  _ids.clear();

  typename PointerObjectIDMap::iterator po_itr;
  typename PointerObjectIDMap::iterator po_itr_end = pointer_objectID_map_.end();

  for (po_itr = pointer_objectID_map_.begin(); po_itr != po_itr_end; ++po_itr)
    _ids.push_back((*po_itr).first);
}

//-----------------------------------------------------------------------------

//=============================================================================
//=============================================================================

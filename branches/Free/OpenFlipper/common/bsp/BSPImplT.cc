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
//  CLASS BSPImplT
//
//=============================================================================

#define BSPIMPLT_C

//== INCLUDES =================================================================


#include "BSPImplT.hh"
#include <float.h>


//== CLASS DEFINITION =========================================================


template <class BSPCore>
typename BSPImplT<BSPCore>::NearestNeighbor
BSPImplT<BSPCore>::
nearest(const Point& _p) const
{
  NearestNeighborData  data;
  data.ref  = _p;
  data.dist = FLT_MAX;
  _nearest(this->root_, data);
  return NearestNeighbor(data.nearest, sqrt(data.dist));
}


//-----------------------------------------------------------------------------


template <class BSPCore>
void
BSPImplT<BSPCore>::
_nearest(Node* _node, NearestNeighborData& _data) const
{
  // terminal node
  if (!_node->left_child_)
  {
    Scalar dist;
    
    for (HandleIter it=_node->begin(); it!=_node->end(); ++it)
    {
      dist = this->traits_.sqrdist(*it, _data.ref);
      if (dist < _data.dist)
      {
	_data.dist = dist;
	_data.nearest = *it;
      }
    }
  }


  
  // non-terminal node
  else
  {
    Scalar dist = _node->plane_.distance(_data.ref);
    
    if (dist > 0.0)
    {
      _nearest(_node->left_child_, _data);
      if (dist*dist < _data.dist)
	_nearest(_node->right_child_, _data);
    }
    else
    {
      _nearest(_node->right_child_, _data);
      if (dist*dist < _data.dist)
	_nearest(_node->left_child_, _data);
    }    
  }
}


//=============================================================================

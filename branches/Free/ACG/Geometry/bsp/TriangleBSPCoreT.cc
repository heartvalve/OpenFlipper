/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 12862 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-11-16 14:25:59 +0100 (Wed, 16 Nov 2011) $                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS TriangleBSPCoreT
//
//=============================================================================

#define TRIANGLEBSPCORET_C

//== INCLUDES =================================================================


#include "TriangleBSPCoreT.hh"


//== CLASS DEFINITION =========================================================

template <class BSPTraits>
void
TriangleBSPCoreT<BSPTraits>::
build(unsigned int _max_handles, unsigned int _max_depth)
{
  // init
  delete root_;
  root_ = new Node(handles_, 0);

  // delete own handles (don't store them twice)
  handles_ = Handles();

  nodes=1;
  traits_.calculateBoundingBoxRoot (root_);
  // call recursive helper
  _build(root_, _max_handles, _max_depth);
  
}


//-----------------------------------------------------------------------------


template <class BSPTraits>
void
TriangleBSPCoreT<BSPTraits>::
_build(Node*         _node,
       unsigned int  _max_handles, 
       unsigned int  _depth)
{
  // should we stop at this level ?
  if ((_depth == 0) || ((_node->end()-_node->begin()) <= (int)_max_handles))
    return;
  
  Point median;
  int axis;
  // compute bounding boxes for children
  traits_.calculateBoundingBox (_node, median, axis);
  
  // construct splitting plane
  const Point XYZ[3] = { Point(1,0,0), Point(0,1,0), Point(0,0,1) };
  _node->plane_ = Plane(median, XYZ[axis]);

  // partition for left and right child
  Handles lhandles, rhandles;
  lhandles.reserve(_node->handles_.size()/2);
  rhandles.reserve(_node->handles_.size()/2);

  HandleIter it;
  Point p0, p1, p2;
  bool left, right;
  for (it=_node->begin(); it!=_node->end(); ++it)
  {
    traits_.points(*it, p0, p1, p2);
    left = right = false;

    if (_node->plane_(p0))  left  = true;
    else                    right = true;
    if (_node->plane_(p1))  left  = true;
    else                    right = true;
    if (_node->plane_(p2))  left  = true;
    else                    right = true;

    if (left)  lhandles.push_back(*it);
    if (right) rhandles.push_back(*it);
  }

  // check it
  if (lhandles.size() == _node->handles_.size() ||
      rhandles.size() == _node->handles_.size())
    return;
  else
    _node->handles_ = Handles();


  // create children
  _node->left_child_  = new Node(lhandles, _node);  lhandles = Handles();
  _node->right_child_ = new Node(rhandles, _node);  rhandles = Handles();
  nodes+=2;
  
  //save bounding boxes for children
  /*
  _node->left_child_->bb_min  = _node->bb_min;
  _node->left_child_->bb_max  = _node->bb_max;
  _node->left_child_->bb_max[axis] = median [axis];
  
  _node->right_child_->bb_min = _node->bb_min;
  _node->right_child_->bb_min[axis] = median [axis];
  _node->right_child_->bb_max = _node->bb_max;
  */
  _node->right_child_->bb_min  = _node->bb_min;
  _node->right_child_->bb_max  = _node->bb_max;
  _node->right_child_->bb_max[axis] = median [axis];
  
  _node->left_child_->bb_min = _node->bb_min;
  _node->left_child_->bb_min[axis] = median [axis];
  _node->left_child_->bb_max = _node->bb_max;

  // recurse to childen
  _build(_node->left_child_,  _max_handles, _depth-1);
  _build(_node->right_child_, _max_handles, _depth-1);
}

//=============================================================================



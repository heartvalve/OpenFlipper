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
//  CLASS TriangleBSPCoreT
//
//=============================================================================

#define TRIANGLEBSPCORET_C

//== INCLUDES =================================================================


#include "TriangleBSPCoreT.hh"


//== CLASS DEFINITION =========================================================


template <class BSPTraits>
TriangleBSPCoreT<BSPTraits>::Node::
Node(const Handles& _handles, Node* _parent)
  : handles_(_handles), 
    parent_(_parent), left_child_(0), right_child_(0)
{}


//-----------------------------------------------------------------------------


template <class BSPTraits>
TriangleBSPCoreT<BSPTraits>::Node::
~Node() 
{ 
  delete left_child_; 
  delete right_child_;

  if (parent_) 
  {
    if (this == parent_->left_child_) 
      parent_->left_child_ = 0;
    else 
      parent_->right_child_ = 0;
  }
}


//-----------------------------------------------------------------------------


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



  // compute bounding box
  HandleIter it;
  Point p0, p1, p2;
  Point bb_min; bb_min.vectorize(FLT_MAX);
  Point bb_max; bb_max.vectorize(-FLT_MAX);
  for (it=_node->begin(); it!=_node->end(); ++it)
  {
    traits_.points(*it, p0, p1, p2);
    bb_min.minimize(p0);
    bb_min.minimize(p1);
    bb_min.minimize(p2);
    bb_max.maximize(p0);
    bb_max.maximize(p1);
    bb_max.maximize(p2);
  }


  // split longest side of bounding box
  Point   bb     = bb_max - bb_min;
  Scalar  length = bb[0];
  int     axis   = 0;
  if (bb[1] > length) length = bb[ (axis=1) ];
  if (bb[2] > length) length = bb[ (axis=2) ];


  // construct splitting plane
  const Point XYZ[3] = { Point(1,0,0), Point(0,1,0), Point(0,0,1) };
  _node->plane_ = Plane((bb_min+bb_max)*0.5, XYZ[axis]);


  // partition for left and right child
  Handles lhandles, rhandles;
  lhandles.reserve(_node->handles_.size()/2);
  rhandles.reserve(_node->handles_.size()/2);

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



  // recurse to childen
  _build(_node->left_child_,  _max_handles, _depth-1);
  _build(_node->right_child_, _max_handles, _depth-1);
}


//=============================================================================

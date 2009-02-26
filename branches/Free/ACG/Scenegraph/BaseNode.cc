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
//  CLASS BaseNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


unsigned int BaseNode::last_id_used__ = 0; 


//----------------------------------------------------------------------------


BaseNode::
BaseNode(BaseNode* _parent, std::string _name)
  : parent_(_parent), 
    name_(_name), 
    status_(Active),
    drawMode_(DrawModes::DEFAULT),
    depth_func_(GL_LESS),
    pickingEnabled_(true),
    dirty_ (false),
    traverseMode_ (BaseNode::NodeFirst)
{  
  id_ = ++last_id_used__;
  if (_parent!=0) _parent->push_back(this);

  DrawModes::initializeDefaultDrawModes();
}
 

//----------------------------------------------------------------------------


BaseNode::
BaseNode(BaseNode* _parent, BaseNode* _child, std::string _name)
  : parent_(_parent), 
    name_(_name), 
    status_(Active),
    drawMode_(DrawModes::DEFAULT)
{  
  assert(_parent != 0 && _child != 0);

  id_ = ++last_id_used__;

  _parent->push_back(this);
  _child->set_parent(this);

  DrawModes::initializeDefaultDrawModes();
}
 

//----------------------------------------------------------------------------


BaseNode::~BaseNode() 
{
  // remove myself from parent's children
  if (parent_!=0) 
  {
    ChildIter me(parent_->find(this));
    assert(me != parent_->childrenEnd());
    parent_->remove(me);
  }


  // remove me (as parent) from my children
  for (BaseNode::ChildIter cIt=childrenBegin(); cIt!=childrenEnd(); ++cIt)
    (*cIt)->parent_ = 0;
}


//----------------------------------------------------------------------------


void
BaseNode::
set_parent(BaseNode* _parent)
{ 
  if (parent_)
  {
    ChildIter me(parent_->find(this));
    if (me != parent_->childrenEnd())
      parent_->remove(me);
  }
  
  parent_ = _parent;
  
  if (parent_)
  {
    ChildIter me(parent_->find(this));
    if (me == parent_->childrenEnd())
      parent_->push_back(this);
  }
}


//----------------------------------------------------------------------------


void 
BaseNode::delete_subtree()
{
  while (!children_.empty())
    children_.front()->delete_subtree();
  delete this;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

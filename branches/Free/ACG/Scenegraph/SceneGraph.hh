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
//  CLASS SceneGraph
//
//=============================================================================

#ifndef ACG_SCENEGRAPH_HH
#define ACG_SCENEGRAPH_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"
#include "../GL/gl.hh"
#include "../Math/VectorT.hh"
#include <float.h>

#include <QMouseEvent>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** Traverse the scenegraph starting at the node \c _node and apply
    the action \c action to each node. When arriving at a node, its
    BaseNode::enter() function is called, then \c _action is applied
    and the node's children are traversed. After that the
    BaseNode::leave() method is called.

    \see ACG::SceneGraph::BaseNode
**/

template <class Action>
void
traverse( BaseNode*     _node,
	  Action&       _action,
	  GLState&      _state,
	  unsigned int  _drawmode=DrawModes::DEFAULT)
{
  if (_node)
  {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    if (status != BaseNode::HideSubtree)
    {
      unsigned int drawmode = ((_node->drawMode() == DrawModes::DEFAULT) ?
			       _drawmode : _node->drawMode());

      if (_node->status() != BaseNode::HideNode)
      {
        _node->enter(_state, drawmode);
        if (_node->traverseMode() == BaseNode::NodeFirst)
          process_children &= _action(_node, _state);
      }

      if (process_children)
      {
	     BaseNode::ChildIter cIt(_node->childrenBegin()),
	                         cEnd(_node->childrenEnd());
	     for (; cIt != cEnd; ++cIt)
	       traverse(*cIt, _action, _state, _drawmode);
      }

      if (_node->status() != BaseNode::HideNode)
      {
        if (_node->traverseMode() == BaseNode::ChildrenFirst)
          _action(_node, _state);
        _node->leave(_state, drawmode);
      }
    }
  }
}


//----------------------------------------------------------------------------


/** Traverse the scenegraph starting at the node \c _node and apply
    the action \c _action to each node. This traversal function does
    \b not call then BaseNode::enter() and BaseNode::leave() methods
    and therefore doesn't need the \c GLState& reference.
**/
template <class Action>
void
traverse( BaseNode* _node, Action& _action )
{
  if (_node)
  {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    if (status != BaseNode::HideSubtree)
    {
      if (_node->status() != BaseNode::HideNode &&
	  _node->traverseMode() == BaseNode::NodeFirst)
	process_children &= _action(_node);

      if (process_children)
      {
	BaseNode::ChildIter cIt(_node->childrenBegin()),
	                    cEnd(_node->childrenEnd());
	for (; cIt != cEnd; ++cIt)
	  traverse(*cIt, _action);
      }

      if (_node->status() != BaseNode::HideNode &&
	  _node->traverseMode() == BaseNode::ChildrenFirst)
	_action(_node);
    }
  }
}


//----------------------------------------------------------------------------


/** Collect bounding box information from all nodes, using the
    BaseNode::boundingBox() method. The result can be accessed by
    bbMin() and bbMax(), yielding the 2 extreme corners of the bounding box.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/
class BoundingBoxAction
{
public:

  BoundingBoxAction() :
    bbMin_( FLT_MAX,  FLT_MAX,  FLT_MAX),
    bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
  {}

  bool operator()(BaseNode* _node)
  {
    _node->boundingBox(bbMin_, bbMax_);
    return true;
  }

  /// Returns minimum point of the bounding box
  const Vec3f& bbMin() const { return bbMin_; }
  /// Returns maximum point of the bounding box
  const Vec3f& bbMax() const { return bbMax_; }

private:

  Vec3f  bbMin_, bbMax_;
};


//----------------------------------------------------------------------------


/** This action is used to find the node specified by \c _node_id in a
    scenegraph. The result can be accessed by node_ptr() and is 0 if
    no node with BaseNode::id() equal to \c _node_id has been found.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class FindNodeAction
{
public:

  /// constructor: _node_id is the node to be searched for
  FindNodeAction(unsigned int _node_id) :
    node_id_(_node_id), node_ptr_(0)  {}

  bool operator()(BaseNode* _node)
  {
    if (_node->id() == node_id_)
    {
      node_ptr_ = _node;
      return false;
    }
    return true;
  }

  /// Get the pointer of the node (is 0 if node was not found)
  BaseNode*  node_ptr() { return node_ptr_; }

private:

  unsigned int  node_id_;
  BaseNode*     node_ptr_;
};


/** Traverse scenegraph starting at _root, looking for a node whose id
    is \c _node_idx. Returns 0 if node wasn't found.
**/
BaseNode* find_node( BaseNode* _root, unsigned int _node_idx );


//----------------------------------------------------------------------------


/** This action collects the draw modes available in a scenegraph.
    The result is the logical \c OR of all nodes'
    BaseNode::availableDrawModes() and can be accessed by drawModes().

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class CollectDrawModesAction
{
public:

  CollectDrawModesAction() : drawModes_(0) {}

  bool operator()(BaseNode* _node)
  {
    drawModes_ |= _node->availableDrawModes();
    return true;
  }

  /// Get the collected draw modes
  unsigned int drawModes() const { return drawModes_; }

private:

  unsigned int drawModes_;
};

//----------------------------------------------------------------------------


/** This action collects the activated draw modes available in a scenegraph.
    The result is the logical \c OR of all activated draw modes in the nodes'
    BaseNode::drawMode() and can be accessed by drawMode().

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class CollectActiveDrawModesAction
{
public:

  CollectActiveDrawModesAction() : drawMode_(0) {}

  bool operator()(BaseNode* _node)
  {
    drawMode_ |= _node->drawMode();
    return true;
  }

  /// Get the collected draw modes
  unsigned int drawMode() const { return drawMode_; }

private:

  unsigned int drawMode_;
};

//----------------------------------------------------------------------------


/** This action sets the draw mode on all nodes in a scenegraph.
    If the drawMode is not supported by the node, it will set it to default
    draw mode. If the given mode is the default mode it will be set on all
    nodes.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class SetDrawModesAction
{
public:

  SetDrawModesAction(unsigned int _mode) : newModes_(_mode) {}

  bool operator()(BaseNode* _node)
  {
    if ( newModes_ == DrawModes::DEFAULT )
      _node->drawMode( DrawModes::DEFAULT );

    unsigned int availableModes = _node->availableDrawModes();

    if ( availableModes & newModes_ )
      _node->drawMode( availableModes & newModes_ );
    else
      _node->drawMode( DrawModes::DEFAULT );

    return true;
  }

private:
  unsigned int newModes_;
};


//----------------------------------------------------------------------------


/** This action is used to traverse and draw a scenegraph using the
    draw mode specified in the constructor.
    Specify if you want to draw solid or transparent objects.
    Needs GLState.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class DrawAction
{
public:

  /// Constructor: draws the scenegraph using _drawMode
  DrawAction(unsigned int _drawMode, bool _blending)
    : drawMode_(_drawMode), blending_(_blending) {}

  bool operator()(BaseNode* _node, GLState& _state)
  {
    // draw only if Material status == DrawAction status
    if(_state.blending() == blending_)
    {
      _node->setDirty (false);
      if (_node->drawMode() == DrawModes::DEFAULT)
	_node->draw(_state, drawMode_);
      else
	_node->draw(_state, _node->drawMode());
    }
    return true;
  }

private:

  unsigned int drawMode_;
  bool         blending_;
};


//----------------------------------------------------------------------------


/** This action is used to pick an object in a scene. All GL states
    for picking must be set before using this action. It merely draws the
    scene AND uses the picking name stack. Needs GLState.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class ACGDLLEXPORT PickAction
{
public:

  /// constructor: what picking target to use
  PickAction(PickTarget _target) : pickTarget_(_target) {}

  bool operator()(BaseNode* _node, GLState& _state);

private:

  PickTarget  pickTarget_;
};


//----------------------------------------------------------------------------


/** This action is used to give mouse events to scenegraph nodes like e.g.
    the manipulator nodes.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class MouseEventAction
{
public:


  MouseEventAction(QMouseEvent* _event) : event_(_event) {}

  bool operator()(BaseNode* _node, GLState& _state)
  {
    _node->mouseEvent(_state, event_);
    return true;
  }

private:

  QMouseEvent* event_;
};

//----------------------------------------------------------------------------


/** This action is used to check if one or more does of the scenegraph need
    to be redrawn

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/

class CheckDirtyAction
{
public:


  CheckDirtyAction() : dirty_(false) {}

  bool operator()(BaseNode* _node)
  {
    dirty_ |= _node->isDirty();
    // don't traverse children if current node is _dirty
    return !dirty_;
  }

  bool isDirty() const { return dirty_; };

private:

  bool dirty_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SCENEGRAPH_HH defined
//=============================================================================


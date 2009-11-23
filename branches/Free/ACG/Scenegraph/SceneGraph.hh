/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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

/** Template functions to check if an action has enter or leave member functions
**/

template<bool C, typename T = void>
struct enable_if {
  typedef T type;
};

template<typename T>
struct enable_if<false, T> { };

#define HAS_MEM_FUNC(func) \
    template<typename T, typename Sign>                                               \
    struct has_##func {                                                               \
        template <typename U, U> struct type_check;                                   \
        template <typename _1> static char (& chk(type_check<Sign, &_1::func> *))[1]; \
        template <typename   > static char (& chk(...))[2];                           \
        static bool const value = sizeof(chk<T>(0)) == 1;                             \
    };

HAS_MEM_FUNC(enter)

// if the enter function is implemented
template<typename Action>
typename enable_if<has_enter <Action, void (Action::*) (BaseNode *) >::value, void>::type
if_has_enter(Action &_action, BaseNode *_node) {
  _action.enter (_node);
}

// if the enter function isn't implemented
template<typename Action>
typename enable_if<!has_enter <Action, void (Action::*) (BaseNode *) >::value, void>::type
if_has_enter(Action &, BaseNode *) {
}

HAS_MEM_FUNC(leave)

// if the enter function is implemented
template<typename Action>
typename enable_if<has_leave <Action, void (Action::*) (BaseNode *) >::value, void>::type
if_has_leave(Action &_action, BaseNode *_node) {
  _action.leave (_node);
}

// if the enter function isn't implemented
template<typename Action>
typename enable_if<!has_enter <Action, void (Action::*) (BaseNode *) >::value, void>::type
if_has_leave(Action &, BaseNode *) {
}

//----------------------------------------------------------------------------


/** Traverse the scenegraph starting at the node \c _node and apply
    the action \c _action to each node. This traversal function will call the
    enter/leave functions of the action if they have been implemented.
**/
template <class Action>
void
traverse( BaseNode* _node, Action& _action )
{
  if (_node)
  {
    BaseNode::StatusMode status(_node->status());
    bool process_children(status != BaseNode::HideChildren);

    // If the subtree is hidden, ignore this node and its children while rendering
    if (status != BaseNode::HideSubtree)
    {

      // If the node itself is hidden, ignore it but continue with its children
      if (_node->status() != BaseNode::HideNode)
      {
        // Executes this nodes enter function (if available)
        if_has_enter (_action, _node);

        // Test rendering order. If NodeFirst, execute this node and the children later.
        if (_node->traverseMode() & BaseNode::NodeFirst)
          process_children &= _action(_node);
      }

      if (process_children)
      {

        BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

        // Process all children which are not second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if (~(*cIt)->traverseMode() & BaseNode::SecondPass)
            traverse(*cIt, _action);

        // Process all children which are second pass
        for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
          if ((*cIt)->traverseMode() & BaseNode::SecondPass)
            traverse(*cIt, _action);

      }

      // If the node is not hidden
      if (_node->status() != BaseNode::HideNode)
      {

        // If the children had to be rendered first, we now render the node afterwards
        if (_node->traverseMode() & BaseNode::ChildrenFirst)
          _action(_node);

        // Call the leave function of the node.
        if_has_leave (_action, _node);
      }

    }
  }
}

//----------------------------------------------------------------------------

/** This is a meta action class that is used to wrap an action from the
    traverse( BaseNode*, Action&, GLState&, unsigned int) into the
    traverse( BaseNode*, Action&) function
**/

template <class Action>
class MetaAction
{
  public:
    MetaAction (Action & _action, GLState& _state, unsigned int _drawmode) :
      action_(_action),
      state_(_state),
      drawmode_(_drawmode)
    {
    }

    bool operator()(BaseNode* _node)
    {
      return action_(_node, state_);
    }

    void enter (BaseNode *_node)
    {
      unsigned int drawmode = ((_node->drawMode() == DrawModes::DEFAULT) ?
                               drawmode_ : _node->drawMode());
      _node->enter(state_, drawmode);
    }

    void leave (BaseNode *_node)
    {
      unsigned int drawmode = ((_node->drawMode() == DrawModes::DEFAULT) ?
                               drawmode_ : _node->drawMode());
      _node->leave(state_, drawmode);
    }

  private:
    Action  &action_;
    GLState &state_;
    unsigned int drawmode_;

};

//----------------------------------------------------------------------------

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
  MetaAction<Action> action (_action, _state, _drawmode);
  traverse (_node, action);
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
    bbMax_(-FLT_MAX, -FLT_MAX, -FLT_MAX),
    state_(false)
  { }

  bool operator()(BaseNode* _node)
  {
    Vec3f bbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
    Vec3f bbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    _node->boundingBox(bbMin, bbMax);

    if ((bbMin[0] > bbMax[0]) ||
        (bbMin[1] > bbMax[1]) ||
        (bbMin[2] > bbMax[2]))
      return true;

    bbMin_.minimize(state_.modelview().transform_point (bbMin));
    bbMin_.minimize(state_.modelview().transform_point (bbMax));
    bbMax_.maximize(state_.modelview().transform_point (bbMin));
    bbMax_.maximize(state_.modelview().transform_point (bbMax));
    return true;
  }

  void enter (BaseNode *_node)
  {
    _node->enter(state_, DrawModes::DEFAULT);
  }

  void leave (BaseNode *_node)
  {
    _node->leave(state_, DrawModes::DEFAULT);
  }

  /// Returns minimum point of the bounding box
  const Vec3f& bbMin() const { return bbMin_; }
  /// Returns maximum point of the bounding box
  const Vec3f& bbMax() const { return bbMax_; }

private:

  Vec3f        bbMin_, bbMax_;
  GLState      state_;
};


//-----------------------------------------------------------------------------


/** Get the maximum number of render passes that will be used
    to render the scene graph. renderPass() returns a bit mask
    of length 32 that holds 1 at position i if the node will be i-th
    drawn in the i-th render pass.

    So if renderPass() == 0x00...001011, the node will be drawn
    during render pass 1, 2 and 4.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/
class RenderPassInfoAction
{
public:

  RenderPassInfoAction() :
  passes_(0u) {}

  bool operator()(BaseNode* _node) {

      // Get render pass
      BaseNode::RenderPassBitMask p = _node->renderPass();

      // Convert render pass bit mask to
      // decimal value (0x001011 -> 4)
      // Note: Same as (int)log2(bitmask)
      unsigned int c = 0;
      while(p != 0u) {
          p = p >> 1;
          ++c;
      }
      passes_ = c > passes_ ? c : passes_;

      return true;
  }

  unsigned int getNumRenderPasses() const { return passes_; }

private:

  unsigned int passes_;
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
ACGDLLEXPORT
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
  PickAction(GLState &_state, PickTarget _target, unsigned int _drawmode) :
    state_(_state),
    pickTarget_(_target),
    drawmode_(_drawmode) {}

  bool operator()(BaseNode* _node);

  void enter(BaseNode* _node)
  {
    if (_node->drawMode() == DrawModes::DEFAULT)
        _node->enterPick(state_, pickTarget_, drawmode_);
      else
        _node->enterPick(state_, pickTarget_, _node->drawMode());
  }

  void leave(BaseNode* _node)
  {
    if (_node->drawMode() == DrawModes::DEFAULT)
        _node->leavePick(state_, pickTarget_, drawmode_);
      else
        _node->leavePick(state_, pickTarget_, _node->drawMode());
  }

private:

  GLState      &state_;
  PickTarget   pickTarget_;
  unsigned int drawmode_;
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


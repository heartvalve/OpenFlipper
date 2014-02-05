/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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

/** \file SceneGraph.hh
 *
 *   This file contains the traversal functions used to apply actions to the SceneGraph.
 *
 */


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
#include <cfloat>

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

//---------------------------------------------------------------------------------

/** Traverse the scenegraph starting at the node \c _node and apply
    the action \c _action to each node. This traversal function will call the
    enter/leave functions of the action if they have been implemented.
    This function traverses the scene graph multiple times if multipass
    rendering is turned on. GLState holds attributes to control
    render passes. Attention: Render passes are 1-indexed.\n
    
    !!! You should ot use this function directly. Use the traverse_multipass function
    which controls the glstate too. This function will also manage the passes for you!!!
    
**/
template <class Action>
void
traverse_multipass ( BaseNode* _node, Action& _action, const unsigned int& _pass )
{
  
    // Process node if it exists
    if (_node) {
        BaseNode::StatusMode status(_node->status());
        bool process_children(status != BaseNode::HideChildren);

        // If the subtree is hidden, ignore this node and its children while rendering
        if (status != BaseNode::HideSubtree) {

            // Executes this nodes enter function (if available and active in multipass)
            if ( _node->multipassStatusActive(_pass) ) {
              if_has_enter(_action, _node);
            }

            // If the node itself is hidden, don't call the action on it.
            // Additionally check if rendering order is node first. otherwise, we will call it after the children.
            // And check if it should be called in this rendering pass.
            if ( (_node->status() != BaseNode::HideNode )  && ( _node->traverseMode() & BaseNode::NodeFirst ) && _node->multipassNodeActive(_pass))
                process_children &= _action(_node);

            if (process_children) {

                BaseNode::ChildIter cIt, cEnd(_node->childrenEnd());

                // Process all children
                for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
                    if (~(*cIt)->traverseMode() & BaseNode::SecondPass)
                        traverse_multipass(*cIt, _action, _pass);

                // Process all children which are second pass
                for (cIt = _node->childrenBegin(); cIt != cEnd; ++cIt)
                    if ((*cIt)->traverseMode() & BaseNode::SecondPass)
                        traverse_multipass(*cIt, _action, _pass);

            }

            
            // If we are in childrenfirst node, the children have been painted andwe now check, if we can draw this node.
            // If its hidden, ignore it.
            // If it should not be rendered in this pass, ignore it too.
            if ( (_node->traverseMode() & BaseNode::ChildrenFirst ) && (_node->status() != BaseNode::HideNode) && _node->multipassNodeActive(_pass) )
                _action(_node);

            // Call the leave function of the node (if available and active in multipass).
            if ( _node->multipassStatusActive(_pass) )
              if_has_leave(_action, _node);

        } // if (status != BaseNode::HideSubtree)
    } // if(node_)
}


//----------------------------------------------------------------------------

/** Traverse the scenegraph starting at the node \c _node and apply
    the action \c action to each node. When arriving at a node, its
    BaseNode::enter() function is called, then \c _action is applied
    and the node's children are traversed. After that the
    BaseNode::leave() method is called. Do this in multiple passes.
    
    <b>You have to initialize glstate before doing this:</b>\n
    In the state you give here use GLState::set_max_render_passes to
    set the maximum number of renderpasses that should be performed.
    This is not computed here, as the number only changes if the scenegraph
    is changed and not for all render calls.

    \see ACG::SceneGraph::BaseNode
**/

template <class Action>
void
traverse_multipass( BaseNode*           _node,
                    Action&             _action,
                    GLState&            _state,
                    DrawModes::DrawMode /*_drawmode*/=DrawModes::DEFAULT)
{
    // Reset render pass counter
    _state.reset_render_pass();

    // Get max render passes
    unsigned int max_passes = _state.max_render_passes();

    // Render all passes
    for(unsigned int pass = BaseNode::PASS_1; pass <= (BaseNode::PASS_1 + max_passes); ++pass) {

        // Traverse scenegraph
        traverse_multipass (_node, _action, pass);
        // Increment render pass counter by 1
        _state.next_render_pass();
    }

    // Reset render pass counter
    _state.reset_render_pass();
}

//--------------------------------------------------------------------------------

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
    Vec3d bbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
    Vec3d bbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
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
  const Vec3d& bbMin() const { return bbMin_; }
  /// Returns maximum point of the bounding box
  const Vec3d& bbMax() const { return bbMax_; }

private:

  Vec3d        bbMin_, bbMax_;
  GLState      state_;
};


//-----------------------------------------------------------------------------


/** Get the maximum number of render passes that will be used
    to render the scene graph. multipassStatus() and multipassNode return
    a bit mask of length 32 that holds 1 at position i if the node will be 
    i-th drawn in the i-th render pass.

    So if renderPass() == 0x00...001011, the node will be drawn
    during render pass 1, 2 and 4.

    \note This class implements an action that should be used as a
    parameter for the traverse() functions.
**/
class MultiPassInfoAction
{
public:

  MultiPassInfoAction() :
    statusPasses_(BaseNode::ALLPASSES),
    nodePasses_(BaseNode::ALLPASSES)
  {}

  bool operator()(BaseNode* _node) {

      // Get status pass 
      BaseNode::MultipassBitMask statusPass = _node->multipassStatus();

      // Ignore if set to ALLPASSES as we want to get the real maximum pass number
      if ( statusPass != BaseNode::ALLPASSES) {
        // Convert render pass bit mask to
        // decimal value (0x001011 -> 4)
        // Note: Same as (int)log2(bitmask)
        unsigned int c = 0;
        
        // Skip the first one as this is the ALLPASSES flag
        statusPass = statusPass >> 1;
        
        while( statusPass != 0u ) {
          statusPass = statusPass >> 1;
          ++c;
        }
        statusPasses_ = c > statusPasses_ ? c : statusPasses_;
      }
      
      
      // Get Node pass 
      BaseNode::MultipassBitMask nodePass = _node->multipassNode();
      
      // Ignore if set to ALLPASSES as we want to get the real maximum pass number
      if ( nodePass != BaseNode::ALLPASSES) {
        // Convert render pass bit mask to
        // decimal value (0x001011 -> 4)
        // Note: Same as (int)log2(bitmask)
        unsigned int c = 0;
        
        // Skip the first one as this is the ALLPASSES flag
        nodePass = nodePass >> 1;
        
        while(nodePass != 0u) {
          nodePass = nodePass >> 1;
          ++c;
        }
        nodePasses_ = c > nodePasses_ ? c : nodePasses_;
      }

      return true;
  }

  /** \brief Get the number of required traverse passes from Scenegraph
  *
  * This number is the maximum of status and node passes required.
  */
  unsigned int getMaxPasses() const { 
    unsigned int maxpasses = std::max(statusPasses_,nodePasses_);
    
    // if maxpasses is 0 we have all nodes in ALLPASSES mode so we render only once
    return maxpasses == 0 ? 1 : maxpasses; 
  }

  /** \brief Get the number of required status traversals from Scenegraph
  *
  * This number is the number of requried status passes required.
  */
  unsigned int getStatusPasses() { return statusPasses_ == 0 ? 1 : statusPasses_; };
  
  /** \brief Get the number of required node traversals from Scenegraph
  *
  * This number is the of required node passes.
  */
  unsigned int getNodePasses() { return nodePasses_ == 0 ? 1 : nodePasses_; };

private:

  unsigned int statusPasses_;
  unsigned int nodePasses_;
  
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

  CollectDrawModesAction() : drawModes_(DrawModes::NONE) {}

  bool operator()(BaseNode* _node)
  {
    drawModes_ |= _node->availableDrawModes();
    return true;
  }

  /// Get the collected draw modes
  DrawModes::DrawMode drawModes() const { return drawModes_; }

private:

  DrawModes::DrawMode drawModes_;
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

  CollectActiveDrawModesAction() : drawMode_(DrawModes::NONE) {}

  bool operator()(BaseNode* _node)
  {
    drawMode_ |= _node->drawMode();
    return true;
  }

  /// Get the collected draw modes
  DrawModes::DrawMode drawMode() const { return drawMode_; }

private:

  DrawModes::DrawMode drawMode_;
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

  /** \brief Set draw modes for all nodes traversed with this action
  *
  * This action can be used to set the drawmodes for all nodes which
  * are traversed using this action. 
  *
  * @param _mode  The draw mode set for the traversed nodes
  * @param _force If true, the mode is set ignoring if its supported by the nodes
  */
  SetDrawModesAction(DrawModes::DrawMode _mode, bool _force = false ) : newModes_(_mode),force_(_force) {}

  bool operator()(BaseNode* _node)
  {
    if ( newModes_ == DrawModes::DEFAULT )
      _node->drawMode( DrawModes::DEFAULT );

    DrawModes::DrawMode availableModes = _node->availableDrawModes();

    if ( force_ ) {
      // if force, we ignore if the mode is supported by the node and set it
      _node->drawMode( newModes_ );
    } else if ( availableModes & newModes_ ) {
      // If its supported, we set it
      _node->drawMode( availableModes & newModes_ );
    } else {
      // otherwise we switch the node to default draw mode (which will use the global mode)
      _node->drawMode( DrawModes::DEFAULT );
    }
    
    
    return true;
  }

private:
  DrawModes::DrawMode newModes_;
  bool                force_; 
  
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
  DrawAction(DrawModes::DrawMode _drawMode, GLState& _state, bool _blending) : 
     state_(_state),
     drawMode_(_drawMode), 
     blending_(_blending) {}

  bool operator()( BaseNode* _node )
  {
    // draw only if Material status == DrawAction status
    if(state_.blending() == blending_)
    {
      _node->setDirty (false);
      if (_node->drawMode() == DrawModes::DEFAULT)
        _node->draw(state_, drawMode_);
      else
        _node->draw(state_, _node->drawMode());
    }
    return true;
  }
  
  void enter(BaseNode* _node)
  {
    if (_node->drawMode() == DrawModes::DEFAULT)
      _node->enter(state_, drawMode_);
    else
      _node->enter(state_, _node->drawMode());
  }
  
  void leave(BaseNode* _node)
  {
    if (_node->drawMode() == DrawModes::DEFAULT)
      _node->leave(state_, drawMode_);
    else
      _node->leave(state_, _node->drawMode());
  }

private:

  GLState&            state_;
  DrawModes::DrawMode drawMode_;
  bool                blending_;
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
  PickAction(GLState &_state, PickTarget _target, DrawModes::DrawMode _drawmode) :
    state_(_state),
    pickTarget_(_target),
    drawmode_(_drawmode) {}

  /** Action applied to the node
  */
  bool operator()(BaseNode* _node);
  
  /** Action applied to the node
   *  Convenience operator which is used to handle calls with internal/external stack with the same operator
   */
  bool operator()(BaseNode* _node, GLState& _state);

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

  GLState             &state_;
  PickTarget          pickTarget_;
  DrawModes::DrawMode drawmode_;
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


  MouseEventAction(QMouseEvent* _event, GLState& _state) : 
    state_(_state),
    event_(_event) {}

  bool operator()(BaseNode* _node )
  {
    _node->mouseEvent(state_, event_);
    return true;
  }

private:
  GLState&     state_;
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


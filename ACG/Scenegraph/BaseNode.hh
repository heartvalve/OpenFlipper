/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS BaseNode
//
//=============================================================================


#ifndef ACG_BASE_NODE_HH
#define ACG_BASE_NODE_HH


//== INCLUDES =================================================================

// ACG
#include "../Math/VectorT.hh"
#include "../GL/GLState.hh"
#include "../Config/ACGDefines.hh"

// Qt
//#include <qgl.h>
#include <QMouseEvent>

// stdc++
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include <ACG/Scenegraph/DrawModes.hh>


//== NAMESPACES ===============================================================


namespace ACG {

class IRenderer;

namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/// What target to use for picking
enum PickTarget 
{
  /// picks faces (may not be implemented for all nodes)
  PICK_CELL,
  /// picks faces (should be implemented for all nodes)
  PICK_FACE,
  /// picks edges (may not be implemented for all nodes)
  PICK_EDGE,
  /// picks verices (may not be implemented for all nodes)
  PICK_VERTEX,
  /// pick any of the prior targets (should be implemented for all nodes)
  PICK_ANYTHING,

  /// picks only visible front edges (may not be implemented for all nodes)
  PICK_FRONT_EDGE,
  /// picks only visible front verices (may not be implemented for all nodes)
  PICK_FRONT_VERTEX,

  /// Pick spline curve or surface (picks u or u,v coords respectively)
  PICK_SPLINE
};


/// Convinience macro that sets up the BaseNode::className() function
#define ACG_CLASSNAME(_className) \
 virtual const std::string& className() const { \
  static std::string cname( #_className ); return cname; \
}


/** \class BaseNode BaseNode.hh <ACG/Scenegraph/BaseNode.hh>

    This is the base for all scenegraph nodes. All virtual functions
    should be reimplemented when inheriting from this class.  
**/

class ACGDLLEXPORT BaseNode 
{
public:


  /// Default constructor
  BaseNode(BaseNode* _parent=0, std::string _name="<unknown>");

  /// Put this node between _parent and _child
  BaseNode(BaseNode* _parent, BaseNode* _child, std::string _name="<unknown>");
 
  /// Destructor.
  virtual ~BaseNode();

  /** \brief Delete the whole subtree of this node
  *
  * This function will remove the whole subtree below this node.
  * All children in this nodes bubtree will be automatically removed from the tree
  * and their destructor is called.
  * The node itself will be removed from the list of its parents children.
  * Afterwards it will also call its own destructor.
  **/
  void delete_subtree();


  // --- basic interface ---

  /// Return class name (implemented by the ACG_CLASSNAME macro)
  virtual const std::string& className() const = 0;

  /** Return a list of available draw modes for this node: should be OR'ed
      from the items of the enum DrawModeIDs. */
  virtual DrawModes::DrawMode availableDrawModes() const { return DrawModes::NONE; }

  /** Compute the bounding box of this node and update the values
      _bbMin and _bbMax accordingly. Do not initialize _bbMin and
      _bbMax since they may already store values of previous nodes' 
      bounding box computation.
  */
  virtual void boundingBox(Vec3d& /* _bbMin */, Vec3d& /*_bbMax*/ ) {}

  /** This function is called when traversing the scene graph and
      arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leave()
      function.  
      \see MaterialNode 
  */
  virtual void enter(GLState& /*_state */, const DrawModes::DrawMode& /*_drawMode*/ ) {}

  /** \brief Draw this node using the draw modes _drawMode
   *
   * This function is called when the scenegraph is traversed by the classical draw routines.
   *
   * In this call the node should draw it's content via OpenGL.
   *
   * \note Keep in mind, that you should use
   *       the currently active state and don't change it (at least reset it the original values you got).
   *       Otherwise nodes that are drawn after this node might get an inconsistent state.
   */
  virtual void draw(GLState& /* _state */, const DrawModes::DrawMode& /* _drawMode */)  {}

  /** \brief Deferred draw call with shader based renderer.
   *
   * The renderer calls this function to collect the geometry that should be rendered from the nodes.
   *
   * Add any renderable geometry to the renderer via _renderer->addRenderObject()
   *
   * \note You should not draw anything yourself in this function.
   */
  virtual void draw2(IRenderer* /*_renderer*/, GLState& /* _state */, const DrawModes::DrawMode& /* _drawMode */)  {}

  /** The leave function is used to restore GL states the have been changed.
      This function must restore the status before enter() ! 
  */
  virtual void leave(GLState& /* _state */, const DrawModes::DrawMode& /* _drawMode */) {}

  /** This function is called when traversing the scene graph during picking
      and arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leavePick()
      function. Its default implementation will call the enter() function.
  */
  virtual void enterPick(GLState& _state , PickTarget _target, const DrawModes::DrawMode& _drawMode );
  
  /** Draw the node using the GL picking name stack. The node's ID
      will already be on the name stack, so only names identifing e.g. faces
      should be used ( by glLoadName() ).
  */
  virtual void pick(GLState& /* _state */, PickTarget /* _target */ ) {}

  /** The leavePick function is used to restore GL states the have been changed.
      This function must restore the status before enterPick() !
      Its default implementation will call the leave() function.
  */
  virtual void leavePick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );
  
  /** Enable or Disable picking for this node
   *  ( default: enabled )
   */
  void enablePicking(bool _enable) { pickingEnabled_ = _enable; };
  
  /** Check if picking is enabled for this node 
   */
  bool pickingEnabled() { return pickingEnabled_; };
  
  /// Handle mouse event (some interaction, e.g. modeling)
  virtual void mouseEvent(GLState& /* _state */, QMouseEvent* /* _event */ ) {}

  /// mark node for redrawn
  void setDirty (bool _dirty = true) { dirty_ = _dirty; }

  /// Check if node should be redrawn
  bool isDirty () const { return dirty_; }


  // --- iterators ---

  /// allows to iterate over children
  typedef std::list<BaseNode*>::const_iterator ConstChildIter;
  /// allows to iterate over children
  typedef std::list<BaseNode*>::iterator ChildIter;

  /// allows to reverse iterate over children
  typedef std::list<BaseNode*>::const_reverse_iterator ConstChildRIter;
  /// allows to reverse iterate over children
  typedef std::list<BaseNode*>::reverse_iterator ChildRIter;

  /// Returns: begin-iterator of children
  ChildIter childrenBegin() { return children_.begin(); }
  /// Same but \c cont
  ConstChildIter childrenBegin() const { return children_.begin(); }
  /// Returns: end-iterator of children
  ChildIter childrenEnd() { return children_.end(); }
  /// Same but \c const
  ConstChildIter childrenEnd() const { return children_.end(); }
  
  /// Returns: reverse begin-iterator of children
  ChildRIter childrenRBegin() { return children_.rbegin(); }
  /// Same but const
  ConstChildRIter childrenRBegin() const { return children_.rbegin(); }
  /// Returns: reverse end-iterator of children
  ChildRIter childrenREnd() { return children_.rend(); }
  /// Same but \c const
  ConstChildRIter childrenREnd() const { return children_.rend(); }




  
  // --- insert / remove ---

  /// Insert _node at the end of the list of children.
  void push_back(BaseNode* _node) 
  {
    if (_node)
    {
      children_.push_back(_node);
      _node->parent_=this;
    }
  }

  /** Remove child node at position _pos.
      This _pos \a must \a be \a reachable from childrenBegin().<br>
      This method has no effect if called with childrenEnd() as parameter.  */
  void remove(ChildIter _pos) 
  {
    if (_pos == childrenEnd()) return;
    //(*_pos)->parent_=0;
    children_.erase(_pos); 
  }

  /// number of children
  unsigned int nChildren() const { return children_.size(); }

  /** Find a specific node in the list of children.<br>
      This method is designed to convert a node pointer to an iterator
      that may be used e.g. for insert()'ing a new node at a distinct
      position.<br>
      Returns childrenEnd() if no appropriate node is found.
   */
  ChildIter find(BaseNode* _node) 
  {
    ChildIter i=std::find(children_.begin(),children_.end(),_node);
    return i;
  } 


  /** Find a node of a given name */
  
  BaseNode * find( const std::string & _name )
  {
    if ( name() == _name )
      return this;

    for ( BaseNode::ChildIter cIt = childrenBegin();
	  cIt != childrenEnd(); ++cIt )
    {
      BaseNode * n = (*cIt)->find( _name );
      if ( n ) return n;
    }

    return 0;
  } 
  

  /// Get the nodes parent node
  BaseNode* parent() { return parent_; }
  
  /** \brief Set the parent of this node.
  *
  * This function will remove this node from its original parents children, if the parent exists.
  * And will add it to the new parents children. 
  */
  void set_parent(BaseNode* _parent);


  // --- status info ---

  
  /// Status modi
  enum StatusMode
  {
    /// Draw node & children
    Active = 0x1,
    /// Hide this node, but draw children
    HideNode  = 0x2,
    /// Draw this node, but hide children
    HideChildren = 0x4,
    /// Hide this node and its children
    HideSubtree  = 0x8
  };
  /// Get node's status
  StatusMode status() const { return status_; }
  /// Set the status of this node.
  void set_status(StatusMode _s) { status_ = _s; }
  /// Hide Node: set status to HideNode
  void hide() { set_status(HideNode); }
  /// Show node: set status to Active
  void show() { set_status(Active); }
  /// Is node visible (status == Active)?
  bool visible() { return status_ == Active; }
  /// Is node not visible (status != Active)?
  bool hidden() { return status_ != Active; }


  /// Returns: name of node (needs not be unique)
  std::string name() const { return name_; }
  /// rename a node 
  void name(const std::string& _name) { name_ = _name; }

  
  /** Get unique ID of node. IDs are always positive and may be used
      e.g. for picking.
    */
  unsigned int id() const { return id_; }
  


  //--- draw mode ---

  /// Return the own draw modes of this node
  DrawModes::DrawMode drawMode() const { return drawMode_; }
  /** Set this node's own draw mode. It will be used for drawing instead of 
      the the global draw mode. */
  void drawMode(DrawModes::DrawMode _drawMode) { drawMode_ = _drawMode; }

  //--- traverse type ---

  /// Node traverse types
  enum TraverseMode
  {
    /// Execute action on node first and then on its children
    NodeFirst = 0x1,
    /// Execute action the children first and then on this node
    ChildrenFirst = 0x2,
    /// Draw node in second pass
    SecondPass = 0x4
  };

  /// Return how the node should be traversed
  unsigned int traverseMode () const { return traverseMode_; }

  /// Set traverse mode for node
  void setTraverseMode(unsigned int _mode) { traverseMode_ = _mode; }

  //===========================================================================
  /** @name Render pass controls
  *  The render pass controls are only used during multipass traversal. There
  *  are two types of multipass controls. One type controls if the enter and
  *  leave functions of the nodes are used (RenderStatusPass) or if the actual 
  *  draw function is called (RenderDrawPass). The bitmasks define if the functions
  *  are called by the traverse_multipass operation. The bitmask are initialized
  *  to run in the first path.\n
  *
  * @{ */
  //===========================================================================

public:
  
  /// Multipass pass bit mask type
  typedef unsigned int MultipassBitMask;
  

  /// This enum should be used to enable rendering of a node in different
  enum PASSES {
    NOPASS = 0,
    ALLPASSES = 1 << 0,
    PASS_1    = 1 << 1,
    PASS_2    = 1 << 2,
    PASS_3    = 1 << 3,
    PASS_4    = 1 << 4,
    PASS_5    = 1 << 5,
    PASS_6    = 1 << 6,
    PASS_7    = 1 << 7,
    PASS_8    = 1 << 8
  };
  
  /** \brief Get the current multipass settings for the nodes status functions
  *
  * Get a bitmask defining in which traverse pass the enter and leave nodes are used. Use
  * the PASSES enum above to control multipass rendering!
  *
  * @return Bitmask defining in which traverse pass the enter and leave nodes are used 
  */
  MultipassBitMask multipassStatus() const {return multipassStatus_;};
  
  
  /** \brief Set multipass settings for the nodes status functions
  *
  * Set a bitmask defining in which traverse pass the enter and leave nodes are used. Use
  * the PASSES enum above to control multipass rendering!
  *
  * Set to ALLPASSES if you want to render in all passes  
  *
  * @param _passStatus Bitmask defining in which traverse pass the enter and leave nodes are used 
  */
  void setMultipassStatus(const MultipassBitMask _passStatus) { multipassStatus_ = _passStatus; };
  
  /** \brief Set multipass status to traverse in a specific pass
  *
  * Change multipass setting for the nodes status functions. The node will
  * call its enter and leave functions in the given pass if its set active.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Pass in which the node should be rendered
  * @param _active Activate or deactivate in this pass?
  */
  void multipassStatusSetActive(const unsigned int _i, bool _active);
 
  /** \brief Get multipass status to traverse in a specific pass
  *
  * Check multipass setting for the nodes status functions if they should
  * be called in the given render pass. 
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Check this pass if the nodes enter/leave functions are active
  */  
  bool multipassStatusActive(const unsigned int _i) const;  
  
  
  
  /** \brief Get the current multipass settings for the node
  *
  * Get a bitmask defining in which traverse path an action is applied to the node. (1-indexed)
  * Use the PASSES enum above to control multipass rendering!
  *
  * @return Bitmask defining in which traverse passes an action is applied to the node.
  */
  MultipassBitMask multipassNode() const {return multipassNode_;};  
  
  
  
  /** \brief Set multipass settings for the node
  *
  * Set a bitmask defining in which traverse path an action is applied to the node. (1-indexed)
  * Set to ALLPASSES if you want to render in all passes.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _passNode Bitmask defining in which traverse passes an action is applied to the node.
  */
  void setMultipassNode(const MultipassBitMask _passNode) { multipassNode_ = _passNode; };  
  
  /** \brief Set Node status to traverse in a specific pass
  *
  * Change multipass setting for the node. An action will be
  * applied to this node in the given pass.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i      Pass in which the node should be rendered
  * @param _active Enable or disable node in this pass?
  */
  void multipassNodeSetActive(const unsigned int _i , bool _active);
  
  /** \brief Get Node status to traverse in a specific pass
  *
  * Check multipass setting for the node if an action will be
  * applied in the given pass.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Check this pass if an action will be applied to the node.
  */  
  bool multipassNodeActive(const unsigned int _i) const;  

private:

  /** multi pass bit mask (1-indexed)
  * Defines in which multipass runs the enter and leave functions should be called.
  * (Only applies during multipass traversal!)
  */
  MultipassBitMask multipassStatus_;
  
  /** multi pass bit mask (1-indexed)
  * Defines in which multipass runs an action should be applied to the node.
  * (Only applies during multipass traversal!)
  */  
  MultipassBitMask multipassNode_;

  /** @} */

private:

  /// Copy constructor. Disabled.
  BaseNode(const BaseNode&);
  /// Assigment operator. Disabled.
  void operator=(const BaseNode&);

  
  /// pointer to parent node
  BaseNode* parent_;

  /// name of node
  std::string name_;

  /// node status()
  StatusMode status_;
  
  /// list of children
  std::list<BaseNode*> children_; 

  /// used to provide unique IDs to nodes
  static unsigned int last_id_used__;

  /// ID of node
  unsigned int id_;

  /// private draw mode
  DrawModes::DrawMode drawMode_;
  
  /** Flag indicating if picking should be done for this object
   * This flag has to be checked by your node if you implement picking
   */
  bool pickingEnabled_;

  /// Flag indicating that the node has to be redrawn
  bool dirty_;

  /// traverse mode
  unsigned int traverseMode_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_BASE_NODE_HH defined
//=============================================================================

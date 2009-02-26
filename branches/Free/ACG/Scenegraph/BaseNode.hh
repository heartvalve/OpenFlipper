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


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/// What target to use for picking
enum PickTarget 
{
  /// picks faces (should be implemented for all nodes)
  PICK_FACE,
  /// picks edges (may not be implemented for all nodes)
  PICK_EDGE,
  /// picks verices (may not be implemented for all nodes)
  PICK_VERTEX,
  /// pick any of the prior targets (should be implemented for all nodes)
  PICK_ANYTHING,

  PICK_COLOR_PASS1,
  PICK_COLOR_PASS2
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

  /** Remove the while subtree below this node (including this node).
      All node will be destucted, **/
  void delete_subtree();



  // --- basic interface ---

  /// Return class name (implemented by the ACG_CLASSNAME macro)
  virtual const std::string& className() const = 0;

  /** Return a list of available draw modes for this node: should be OR'ed
      from the items of the enum DrawModeIDs. */
  virtual unsigned int availableDrawModes() const { return 0; }

  /** Compute the bounding box of this node and update the values
      _bbMin and _bbMax accordingly. Do not initialize _bbMin and
      _bbMax since they may already store values of previous nodes' 
      bounding box computation.
  */
  virtual void boundingBox(Vec3f& /* _bbMin */, Vec3f& /*_bbMax*/ ) {}

  /** This function is called when traversing the scene graph and
      arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leave()
      function.  
      \see MaterialNode 
  */
  virtual void enter(GLState& /*_state */, unsigned int /*_drawMode*/ ) {}

  /** Draw this node using the draw modes _drawMode */
  virtual void draw(GLState& /* _state */, unsigned int /* _drawMode */)  {}

  /** The leave function is used to restore GL states the have been changed.
      This function must restore the status before enter() ! 
  */
  virtual void leave(GLState& /* _state */, unsigned int /* _drawMode */) {}

  /** Draw the node using the GL picking name stack. The node's ID
      will already be on the name stack, so only names identifing e.g. faces
      should be used ( by glLoadName() ).
  */
  virtual void pick(GLState& /* _state */, PickTarget /* _target */ ) {}
  
  /** Enable or Disable picking for this node
   *  ( default: enabled )
   */
  void enablePicking(bool _enable) { pickingEnabled_ = _enable; };
  
  /** Check if picking is enabled for this node 
   */
  bool pickingEnabled() { return pickingEnabled_; };
  
  /// Handle mouse event (some interaction, e.g. modeling)
  virtual void mouseEvent(GLState& /* _state */, QMouseEvent* /* _event */ ) {}


  /// get glDepthFunc() parameter
  GLenum depthFunc() const { return depth_func_; }

  /// set glDepthFunc parameter (it's GL_LESS by default)
  void depthFunc(GLenum _func) { depth_func_ = _func; }

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
  

  BaseNode* parent() { return parent_; }
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
  unsigned int drawMode() const { return drawMode_; }
  /** Set this node's own draw mode. It will be used for drawing instead of 
      the the global draw mode. */
  void drawMode(unsigned int _drawMode) { drawMode_ = _drawMode; }

  //--- traverse type ---

  /// Node traverse types
  enum TraverseMode
  {
    /// Execute action on node first and then on its children
    NodeFirst = 0x1,
    /// Execute action the children first and then on this node
    ChildrenFirst = 0x2
  };

  /// Return how the node should be traversed
  TraverseMode traverseMode () const { return traverseMode_; }

  /// Set traverse mode for node
  void setTraverseMode(TraverseMode _mode) { traverseMode_ = _mode; }
  
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
  unsigned int drawMode_;

  /// depth func
  GLenum depth_func_;
  
  /** Flag indicating if picking should be done for this object
   * This flag has to be checked by your node if you implement picking
   */
  bool pickingEnabled_;

  /// Flag indicating that the node has to be redrawn
  bool dirty_;

  /// traverse mode
  TraverseMode traverseMode_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_BASE_NODE_HH defined
//=============================================================================

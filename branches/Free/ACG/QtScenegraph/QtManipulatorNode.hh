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
//   $Revision: 3468 $
//   $Author: moebius $
//   $Date: 2008-10-17 14:58:52 +0200 (Fr, 17. Okt 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtManipulatorNode
//
//=============================================================================


#ifndef ACG_QTMANIPULATOR_NODE_HH
#define ACG_QTMANIPULATOR_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "ACG/Scenegraph/ManipulatorNode.hh"

// Qt
#include <QEvent>
#include <QMouseEvent>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class QtManipulatorNode QtManipulatorNode.hh <ACG/QtScenegraph/QtManipulatorNode.hh>
Additional overlay class for emitting signals if manipulator is moved
*/

class ACGDLLEXPORT QtManipulatorNode : public QObject , public ManipulatorNode
{
  Q_OBJECT

  signals:
    /** This signal is emitted everytime the manipulator is moved
     * Do not traverse Scenegraph when called by this function!!!!!!!
     */
    void manipulatorMoved( QtManipulatorNode* _node, QMouseEvent* _event);

    /** The visibility of this node has changed
     * @param _visible Is it now visible?
     */
    void visibilityChanged( QtManipulatorNode* _node, bool _visible );

    /** this signal is emited every time the position of the manipulator is changed
     */
    void positionChanged( QtManipulatorNode * _node );

  public:


  /// Default constructor.
  QtManipulatorNode( BaseNode* _parent=0,
		               const std::string& _name="<QtManipulatorNode>" );

  /// Destructor.
  ~QtManipulatorNode();


  /// class name
  ACG_CLASSNAME(QtManipulatorNode);

  //
  // METHODS
  //

  /// get mouse events
  void mouseEvent(GLState& _state, QMouseEvent* _event);

  /// Set an identifier for that manipulator
  void setIdentifier( int _id) { identifier_ = _id; };

  /// Get an identifier for that manipulator
  int getIdentifier( ) { return identifier_; };

  void show() { ManipulatorNode::show(); emit visibilityChanged( this, visible() );  };
  void hide() { ManipulatorNode::hide(); emit visibilityChanged( this, visible() );  };

  void set_center( const Vec3d& _c ) { ManipulatorNode::set_center(_c); emit positionChanged( this ); };

  private:
    int identifier_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_QTMANIPULATOR_NODE_HH defined
//=============================================================================


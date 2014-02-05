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
     * @param _node    The node whose visibility has been changed
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

  void rotate(double _angle, const Vec3d& _axis) { TransformNode::rotate(_angle, _axis); emit positionChanged( this ); };

  void translate( double _s ) { ManipulatorNode::translate(_s); emit positionChanged( this ); };

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


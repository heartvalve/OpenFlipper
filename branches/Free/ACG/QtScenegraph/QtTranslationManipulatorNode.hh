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
//  CLASS QtTranslationManipulatorNode
//
//=============================================================================


#ifndef ACG_QTTRANSLATIONMANIPULATOR_NODE_HH
#define ACG_QTTRANSLATIONMANIPULATOR_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "ACG/Scenegraph/TranslationManipulatorNode.hh"

// Qt
#include <QEvent>
#include <QMouseEvent>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class QtTranslationManipulatorNode QtTranslationManipulatorNode.hh <ACG/QtScenegraph/QtTranslationManipulatorNode.hh>
Additional overlay class for emitting signals if manipulator is moved
*/

class ACGDLLEXPORT QtTranslationManipulatorNode : public QObject , public TranslationManipulatorNode
{
  Q_OBJECT

  signals:
    /** This signal is emitted everytime the manipulator is moved
     * Do not traverse Scenegraph when called by this function!!!!!!!
     */
    void manipulatorMoved( QtTranslationManipulatorNode * _node, QMouseEvent* _event);

    /** The visibility of this node has changed
     * @param _visible Is it now visible?
     */
    void visibilityChanged( QtTranslationManipulatorNode * _node, bool _visible );

    /** this signal is emited every time the position of the manipulator is changed
     */
    void positionChanged( QtTranslationManipulatorNode * _node );

  public:


  /// Default constructor.
  QtTranslationManipulatorNode( BaseNode* _parent=0,
		               const std::string& _name="<QtTranslationManipulatorNode>" );

  /// Destructor.
  ~QtTranslationManipulatorNode();


  /// class name
  ACG_CLASSNAME(QtTranslationManipulatorNode);

  //
  // METHODS
  //

  /// get mouse events
  void mouseEvent(GLState& _state, QMouseEvent* _event);

  /// Set an identifier for that manipulator
  void setIdentifier( int _id) { identifier_ = _id; };

  /// Get an identifier for that manipulator
  int getIdentifier( ) { return identifier_; };

  void show() { TranslationManipulatorNode::show(); emit visibilityChanged( this, visible() );  };
  void hide() { TranslationManipulatorNode::hide(); emit visibilityChanged( this, visible() );  };

  void set_center( const Vec3d& _c ) { TranslationManipulatorNode::set_center(_c); emit positionChanged( this ); };


  private:
    int identifier_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_QTTRANSLATIONMANIPULATOR_NODE_HH defined
//=============================================================================


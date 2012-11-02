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
//  CLASS QtTranslationManipulatorNode
//
//=============================================================================


#ifndef ACG_QTTRANSLATIONMANIPULATOR_NODE_HH
#define ACG_QTTRANSLATIONMANIPULATOR_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "ACG/Scenegraph/TranslationManipulatorNode.hh"

// Qt
#include <QVariant>
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
    /** \brief This signal is emitted every time the manipulator is moved by the user via mouse
     *
     * \note Do not traverse Scenegraph when called by this function or you end up in an endless loop!!
     */
    void manipulatorMoved( QtTranslationManipulatorNode * _node, QMouseEvent* _event);

    /** The visibility of this node has changed
     * @param _node    The node whose visibility has been changed
     * @param _visible Is it now visible?
     */
    void visibilityChanged( QtTranslationManipulatorNode * _node, bool _visible );

    /** \brief This signal is emitted every time the position of the manipulator is changed
     *
     * This slot is only triggered, when the center of the manipulator has been changed.
     * Changes in orientation are not triggering this signal
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

  /// Set additional data for the node
  void setData(QVariant _data) {data_ = _data;};
  
  /// Get additional data for the node
  QVariant getData() {return data_;};
  
  void show() { TranslationManipulatorNode::show(); emit visibilityChanged( this, visible() );  };
  void hide() { TranslationManipulatorNode::hide(); emit visibilityChanged( this, visible() );  };

  void set_center( const Vec3d& _c ) { TranslationManipulatorNode::set_center(_c); emit positionChanged( this ); };

  private:
    int identifier_;
    QVariant  data_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_QTTRANSLATIONMANIPULATOR_NODE_HH defined
//=============================================================================


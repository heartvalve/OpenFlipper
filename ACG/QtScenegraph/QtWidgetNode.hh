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
 *   $Revision: 8876 $                                                       *
 *   $Author: dekkers $                                                      *
 *   $Date: 2010-04-01 14:31:02 +0200 (Do, 01. Apr 2010) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS QtWidgetNode
//
//=============================================================================


#ifndef ACG_QTWIDGETNODE_HH
#define ACG_QTWIDGETNODE_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <QWidget>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================

	      

/** \class QtWidgetNode QtWidgetNode.hh <ACG/QtScenegraph/QtWidgetNode.hh>

    Integrates QtWidgets directly into the OpenGL scenegraph
**/

class ACGDLLEXPORT QtWidgetNode : public BaseNode
{
public:
   
  /// default constructor
  QtWidgetNode( QWidget*     _widget,
                BaseNode*    _parent = 0,
	        std::string  _name="<QtWidgetNode>" )
    : BaseNode(_parent, _name),
      widget_(_widget)
  {}
 
  /// destructor
  ~QtWidgetNode() {}

  /// static name of this class
  ACG_CLASSNAME(QtWidgetNode);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw points and normals
  void draw(GLState& _state, DrawModes::DrawMode _drawMode);
  
  /// get mouse events
  virtual void mouseEvent(GLState& _state, QMouseEvent* _event);

private:
  
  QWidget* widget_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_QTWIDGETNODE_HH defined
//=============================================================================

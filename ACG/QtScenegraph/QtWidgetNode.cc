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
//  CLASS QtWidgetNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtWidgetNode.hh"
#include <ACG/GL/gl.hh>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
QtWidgetNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  std::cerr << "QtWidgetNode Got boundingbox call " << std::endl;
//   ConstPointIter p_it=points_.begin(), p_end=points_.end();
//   for (; p_it!=p_end; ++p_it) {
//     _bbMin.minimize(*p_it); 
//     _bbMax.maximize(*p_it);
//   }
}


//----------------------------------------------------------------------------

  
DrawModes::DrawMode 
QtWidgetNode::
availableDrawModes() const
{
  return ( DrawModes::SOLID_FLAT_SHADED );
}


//----------------------------------------------------------------------------


void
QtWidgetNode::
draw(GLState&  _state  , DrawModes::DrawMode _drawMode)
{
  QPixmap pixmap = QPixmap::grabWidget(widget_);
  
  std::cerr << "QtWidgetNode Got draw call " << std::endl;
}

void
QtWidgetNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{

  Vec2i         newPoint2D(_event->pos().x(), _event->pos().y());
  
//   if ( _event == QEvent::MouseButtonPress ) {
//     
//   } else if ( _event == QEvent::MouseButtonDblClick ) {
//     
//   } else if ( _event == QEvent::MouseButtonRelease ) {
//     
//   } else if ( _event == QEvent::MouseMove ) {
//     widget_->mouse
//   }
  
  std::cerr << "QtWidgetNode Got Mouse Event " << newPoint2D << std::endl;
  
//   Vec3d         newPoint3D;
  
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

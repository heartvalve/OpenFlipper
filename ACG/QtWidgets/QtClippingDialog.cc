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
//  CLASS QtClippingDialog - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "QtClippingDialog.hh"

#include "../Scenegraph/ClippingNode.hh"

#include <QPushButton>
#include <QSlider>
#include <QLineEdit>


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ========================================================== 


QtClippingDialog::
QtClippingDialog( QWidget                  * _parent,
		  SceneGraph::ClippingNode * _node )
  : QDialog( _parent ),
    node_( _node )
{
  ui_.setupUi( this );

  connect( ui_.setButton, SIGNAL( clicked() ),
	   this, SLOT( set_plane() ) );
  connect( ui_.sweepRangeSlider, SIGNAL( sliderMoved(int) ),
	   this, SLOT( sweep_plane(int) ) );

  update_values();
  connect( ui_.okButton, SIGNAL( clicked() ),
	   this, SLOT( accept() ) );

  layout()->setSizeConstraint( QLayout::SetFixedSize );

}
 

//-----------------------------------------------------------------------------


void 
QtClippingDialog::show()
{
  update_values();
  QDialog::show();
}


//-----------------------------------------------------------------------------


void 
QtClippingDialog::update_values()
{
  QString s;

  const Vec3f& position = node_->position();
  ui_.positionXLineEdit->setText(s.setNum(position[0], 'f', 3));
  ui_.positionYLineEdit->setText(s.setNum(position[1], 'f', 3));
  ui_.positionZLineEdit->setText(s.setNum(position[2], 'f', 3));

  const Vec3f& normal = node_->normal();
  ui_.normalXLineEdit->setText(s.setNum(normal[0], 'f', 3));
  ui_.normalYLineEdit->setText(s.setNum(normal[1], 'f', 3));
  ui_.normalZLineEdit->setText(s.setNum(normal[2], 'f', 3));

  float slice_width = node_->slice_width();
  ui_.sliceWidthLineEdit->setText(s.setNum(slice_width, 'f', 3));
}


//-----------------------------------------------------------------------------


void 
QtClippingDialog::set_plane()
{
  // set plane
  Vec3f position( ui_.positionXLineEdit->text().toFloat(),
		  ui_.positionYLineEdit->text().toFloat(),
		  ui_.positionZLineEdit->text().toFloat() );

  Vec3f normal( ui_.normalXLineEdit->text().toFloat(),
		ui_.normalYLineEdit->text().toFloat(),
		ui_.normalZLineEdit->text().toFloat() );

  float slice_width = ui_.sliceWidthLineEdit->text().toFloat();

  node_->set_plane(position, normal, slice_width);


  // reset slider
  ui_.sweepRangeSlider->setValue(0);


  emit signalNodeChanged(node_);
}


//-----------------------------------------------------------------------------


void 
QtClippingDialog::set_sweep_range(float _radius)
{
  QString s;
  ui_.sweepRangeLineEdit->setText(s.setNum(_radius, 'f', 3));
}


//-----------------------------------------------------------------------------


void 
QtClippingDialog::sweep_plane(int _i)
{
  float range  = ui_.sweepRangeLineEdit->text().toFloat();
  float offset = ((float)_i) / 100.0 * range;

  node_->set_offset(offset);

  emit signalNodeChanged(node_);
}


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================

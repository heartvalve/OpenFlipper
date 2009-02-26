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

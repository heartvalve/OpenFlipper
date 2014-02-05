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
 *   $Revision: 9595 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-06-17 12:48:23 +0200 (Thu, 17 Jun 2010) $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtTextureDialog - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtTextureDialog.hh"
#include "../Scenegraph/TextureNode.hh"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <QColorDialog>
#include <QSlider>


//== NAMESPACES ==============================================================


namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================


QtTextureDialog::QtTextureDialog( QWidget                  * _parent,
				                        SceneGraph::TextureNode * _node )
  : QDialog( _parent ),
    node_(_node) {
  
    ui_.setupUi( this );
  
    // get initial values from node
    mipmapping_   = bak_mipmapping_   = node_->mipmapping();
    
    ui_.mipmapping->setChecked(mipmapping_);

    connect( ui_.mipmapping, SIGNAL( toggled(bool) ),
            this, SLOT( changeMipmapping(bool) ) );

    connect( ui_.okButton, SIGNAL( clicked() ),
            this, SLOT( accept() ) );
    connect( ui_.cancelButton, SIGNAL( clicked() ),
            this, SLOT( reject() ) );
}

//-----------------------------------------------------------------------------

void QtTextureDialog::reject() {
    
    undoChanges();
    QDialog::reject();
}


//-----------------------------------------------------------------------------


void QtTextureDialog::applyChanges() {

    if ( mipmapping_ )
        node_->enable_mipmapping();
    else
        node_->disable_mipmapping();

    emit signalNodeChanged(node_);
}


//-----------------------------------------------------------------------------


void QtTextureDialog::undoChanges() {

    if ( bak_mipmapping_ )
        node_->enable_mipmapping();
    else
        node_->disable_mipmapping();

    emit signalNodeChanged(node_);
}


//-----------------------------------------------------------------------------


void
QtTextureDialog::changeMipmapping(bool _b) {
  
    mipmapping_ = _b;
    applyChanges();
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================

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
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fri, 09 May 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

//== IMPLEMENTATION ==========================================================



void CoreWidget::showHelpBrowserUser( ) {

  if ( OpenFlipper::Options::nogui() )
    return;

  if ( helpBrowserUser_ == 0 ) {
    helpBrowserUser_ = new HelpWidget( this ,true);
  }

  //show the widget centered
  QPoint center;
  center.setX( x() + width() / 2 );
  center.setY( y() + height() / 2 );

  helpBrowserUser_->setGeometry(center.x() - helpBrowserUser_->width() / 2,
                              center.y() - helpBrowserUser_->height()/ 2, helpBrowserUser_->width(), helpBrowserUser_->height());

  helpBrowserUser_->show();

}

void CoreWidget::showHelpBrowserDeveloper( ) {

  if ( OpenFlipper::Options::nogui() )
    return;

  if ( helpBrowserDeveloper_ == 0 ) {
    helpBrowserDeveloper_ = new HelpWidget( this ,false);
  }

  //show the widget centered
  QPoint center;
  center.setX( x() + width() / 2 );
  center.setY( y() + height() / 2 );

  helpBrowserDeveloper_->setGeometry(center.x() - helpBrowserDeveloper_->width() / 2,
                              center.y() - helpBrowserDeveloper_->height()/ 2, helpBrowserDeveloper_->width(), helpBrowserDeveloper_->height());

  helpBrowserDeveloper_->show();

}

//=============================================================================

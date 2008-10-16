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
//   $Revision: 1727 $
//   $Author: moebius $
//   $Date: 2008-05-10 00:35:25 +0200 (Sat, 10 May 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS MViewWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"
// -------------------- ACG
#include "OpenFlipper/common/GlobalOptions.hh"

// -------------------- Qt

//== IMPLEMENTATION ==========================================================

void CoreWidget::slotAddToolbar(QToolBar* _toolbar) {
  addToolBar( _toolbar );
  int newNumber = toolbarCount_;
  toolbarCount_++;
  QString number = QString::number(newNumber);
  _toolbar->setObjectName("Toolbar" + number);
}

void CoreWidget::slotRemoveToolbar(QToolBar* _toolbar) {
  removeToolBar( _toolbar );
}


//=============================================================================

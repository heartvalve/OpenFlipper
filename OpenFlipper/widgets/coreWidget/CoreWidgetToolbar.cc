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

  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {
    if ( toolbars_[i]->windowTitle() == _toolbar->windowTitle() ) {
      emit log(LOGERR,"slotAddToolbar: Toolbar already added to system: " + _toolbar->windowTitle() );
      return;
    }
  }

  _toolbar->setObjectName( _toolbar->windowTitle() );
  toolbars_.push_back( _toolbar );
  addToolBar( _toolbar );
}

void CoreWidget::getToolBar( QString _name, QToolBar*& _toolbar) {

  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {

    if ( toolbars_[i]->windowTitle() == _name ) {
      _toolbar = toolbars_[i];
      return;
    }

  }

  _toolbar = 0;

  emit log(LOGERR,"getToolBar: Toolbar not found." );
}

void CoreWidget::slotRemoveToolbar(QToolBar* _toolbar) {
  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {

    if ( toolbars_[i]->windowTitle() == _toolbar->windowTitle() ) {
      std::cerr << "Todo : erase Toolbar from list" << std::endl;
      removeToolBar( _toolbar );
      return;
    }

  }

  emit log(LOGERR,"Remove Toolbar: Toolbar not found." );
}


//=============================================================================

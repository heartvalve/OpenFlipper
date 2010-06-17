/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



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
  
  int id = -1;
  
  // Find the plugin which added this Toolbox
  for ( uint i = 0 ; i < plugins_.size(); ++i ) {
    if ( plugins_[i].plugin == sender() ) {
      id = i;
      break;
    }
  }
  
  // Find the scripting plugin because we assign this toolBox to it as we did not find the original sender
  if ( id == -1 ) {
    for ( uint i = 0 ; i < plugins_.size(); ++i ) {
      if ( plugins_[i].name == "Scripting" ) {
        id = i;
        break;
      }
    }
    
  }
  
  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {
    if ( toolbars_[i]->windowTitle() == _toolbar->windowTitle() ) {
      emit log(LOGERR,tr("slotAddToolbar: Toolbar already added to system: ") + _toolbar->windowTitle() );
      return;
    }
  }

  _toolbar->setObjectName( _toolbar->windowTitle() );
  toolbars_.push_back( _toolbar );
  addToolBar( _toolbar );
  
  if ( id != -1 )
    plugins_[id].toolbars.push_back( std::pair< QString,QToolBar* >( _toolbar->windowTitle() , _toolbar) );
  
  // add widget name to viewMode 'all'
  if ( !viewModes_[0]->visibleToolbars.contains( _toolbar->windowTitle() ) ){
    viewModes_[0]->visibleToolbars << _toolbar->windowTitle();
    viewModes_[0]->visibleToolbars.sort();
  }      
  
}

void CoreWidget::getToolBar( QString _name, QToolBar*& _toolbar) {

  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {

    if ( toolbars_[i]->windowTitle() == _name ) {
      _toolbar = toolbars_[i];
      return;
    }

  }

  _toolbar = 0;

  emit log(LOGERR,tr("getToolBar: Toolbar not found.") );
}

void CoreWidget::slotRemoveToolbar(QToolBar* _toolbar) {
  for ( uint i = 0 ; i < toolbars_.size(); ++i ) {

    if ( toolbars_[i]->windowTitle() == _toolbar->windowTitle() ) {
      std::cerr << "Todo : erase Toolbar from list" << std::endl;
      removeToolBar( _toolbar );
      return;
    }

  }

  emit log(LOGERR,tr("Remove Toolbar: Toolbar not found.") );
}

QSize CoreWidget::defaultIconSize(){
  return defaultIconSize_;
}

//=============================================================================

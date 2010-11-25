/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION of Comunication with plugins
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

#include "OpenFlipper/BasePlugin/PluginFunctionsCore.hh"

//== IMPLEMENTATION ==========================================================

//========================================================================================
// ===             Backup Communication                       ============================
//========================================================================================

void Core::slotBackupGroup(QString _name, int& _groupId ) {
  if ( sender() != 0 ) {
    if ( sender()->metaObject() != 0 ) {
      _name = QString(sender()->metaObject()->className()) + ": " + _name;
      
      if ( OpenFlipper::Options::doSlotDebugging() ) 
        emit log(LOGINFO,"slotBackupGroup( " + _name + " ) called by " + QString( sender()->metaObject()->className() ) );
    }
  }
  
  _groupId = nextBackupGroupId_;
  emit createBackupGroup( _name , nextBackupGroupId_);
  ++nextBackupGroupId_;
}

/// Called if a backup is requested by the plugins
void Core::slotBackup( int _objectId, QString _name, int& _internalId, int _groupId ) {
  if ( sender() != 0 ) {
    if ( sender()->metaObject() != 0 ) {
      _name = QString(sender()->metaObject()->className()) + ": " + _name;
      
      if ( OpenFlipper::Options::doSlotDebugging() ) 
        emit log(LOGINFO,"slotBackup( " + QString::number(_objectId) + ", " + _name + "," + QString::number(_internalId) + (" ) called by ") + QString( sender()->metaObject()->className() ) );
      
    }
  }
  
  _internalId = nextBackupId_;
  emit createBackup(  _objectId , _name , nextBackupId_, _groupId);
  ++nextBackupId_;
}

/// Called if a backup is requested by the plugins (convenience function without getting id of the object)
void Core::slotBackup( int _objectId, QString _name ) {
  if ( sender() != 0 ) {
    if ( sender()->metaObject() != 0 ) {
      _name = QString(sender()->metaObject()->className()) + ": " + _name;
      
      if ( OpenFlipper::Options::doSlotDebugging() ) 
        emit log(LOGINFO,"slotBackup( " + QString::number(_objectId) + ", " + _name + tr(" ) called by ") + QString( sender()->metaObject()->className() ) );
      
    }
  }
  
  emit createBackup(  _objectId , _name , nextBackupId_);
  ++nextBackupId_;
}

void Core::slotRestoreGroup( int _groupId) {
  emit restoreGroup(_groupId);
}

void Core::slotRestore( int _objectId, int _internalId ) {
  emit restoreObject(_objectId,_internalId);
}

//=============================================================================

/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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




#include "DataControlPlugin.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

void DataControlPlugin::slotContextMenuTarget( ) {
  QVariant contextObject = targetAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;
  
  object->target( targetAction_->isChecked() );
}

void DataControlPlugin::slotContextMenuHide( ) {
  QVariant contextObject = targetAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;
  
  object->hide();
}


void DataControlPlugin::slotContextMenuSource( ) {
  QVariant contextObject = sourceAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;
  
  object->source( sourceAction_->isChecked() );
}

void DataControlPlugin::slotContextMenuRemove( ) {
  QVariant contextObject = sourceAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  emit deleteObject( objectId );
}

void DataControlPlugin::slotUpdateContextMenu( int _objectId) {
  if ( _objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) )
    return;
  
  sourceAction_->setText( tr("Source" ));
  sourceAction_->setChecked( object->source() );
  
  targetAction_->setText( tr("Target" ));
  targetAction_->setChecked( object->target() );
}



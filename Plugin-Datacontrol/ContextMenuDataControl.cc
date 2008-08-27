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




#include "DataControlPlugin.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

void DataControlPlugin::slotContextMenuTarget( ) {
  QVariant contextObject = targetAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) )
    return;
  
  object->target( targetAction_->isChecked() );
  
  emit activeObjectChanged();
  emit updated_objects(objectId);
}

void DataControlPlugin::slotContextMenuHide( ) {
  QVariant contextObject = targetAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) )
    return;
  
  object->hide();
  
  emit update_view();
  emit updated_objects(objectId);
}


void DataControlPlugin::slotContextMenuSource( ) {
  QVariant contextObject = sourceAction_->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) )
    return;
  
  object->source( sourceAction_->isChecked() );
  
  emit updated_objects(objectId);
}

void DataControlPlugin::slotUpdateContextMenu( int _objectId) {
  if ( _objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(_objectId,object) )
    return;
  
  sourceAction_->setText( "Source" );
  sourceAction_->setChecked( object->source() );
  
  targetAction_->setText( "Target" );
  targetAction_->setChecked( object->target() );
}



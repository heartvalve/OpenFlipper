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


int DataControlPlugin::getObject( QString _name ) {
  
  BaseObject* object = PluginFunctions::objectRoot()->childExists(_name); 
  
  if ( object == 0)
    return -1;
  
  return object->id();
}

QString DataControlPlugin::getObjectName( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) {
    emit log(LOGERR,"getObjectName : unable to get object" ); 
    return QString("Unknown Object");
  } else 
    return  object->name() ;
  
}

void DataControlPlugin::hideObject( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) 
    return;
  
  if ( object == 0)
    return;
  
  object->hide();
}

void DataControlPlugin::showObject( int objectId ) {
  
  BaseObjectData* object;
  if ( ! PluginFunctions::get_object(objectId,object) ) 
    return;
  
  if ( object == 0)
    return;
  
  object->show();
}



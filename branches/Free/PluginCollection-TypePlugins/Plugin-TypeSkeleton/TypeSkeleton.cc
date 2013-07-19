/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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


#include "TypeSkeleton.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/BackupData.hh>
#include "SkeletonBackup.hh"

TypeSkeletonPlugin::TypeSkeletonPlugin()
  : showIndicesAction_(0),
    showCoordFramesAction_(0),
    showMotionAction_(0)
{
}

/** \brief Second initialization phase
 *
 */
void TypeSkeletonPlugin::pluginsInitialized()
{
  
  if ( OpenFlipper::Options::gui() ){
  
    QMenu* contextMenu = new QMenu("Options");
    
    QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

    //Show indices
    showIndicesAction_ = new QAction(tr("Show &Indices"), this);
    showIndicesAction_->setStatusTip(tr("Show Joint Indices"));
    showIndicesAction_->setIcon( QIcon(iconPath + "showIndices.png") );
    showIndicesAction_->setCheckable(true);
    showIndicesAction_->setChecked(false);
    //Show coord-frames
    showCoordFramesAction_ = new QAction(tr("Show &Coordinate Frames"), this);
    showCoordFramesAction_->setStatusTip(tr("Show Coordinate Frames for joints"));
    showCoordFramesAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    showCoordFramesAction_->setCheckable(true);
    showCoordFramesAction_->setChecked(false);
    //Show motion space
    showMotionAction_ = new QAction(tr("Show &Motion Path"), this);
    showMotionAction_->setStatusTip(tr("Show path of motions for joints"));
    showMotionAction_->setIcon( QIcon(iconPath + "motionPath.png") );
    showMotionAction_->setCheckable(true);
    showMotionAction_->setChecked(false);
    
    connect(showIndicesAction_,     SIGNAL(triggered()), this, SLOT(slotShowIndices()) );
    connect(showCoordFramesAction_, SIGNAL(triggered()), this, SLOT(slotShowCoordFrames()) );
    connect(showMotionAction_,      SIGNAL(triggered()), this, SLOT(slotShowMotionPath()) );
    
    contextMenu->addAction(showIndicesAction_);
    contextMenu->addAction(showCoordFramesAction_);
    contextMenu->addAction(showMotionAction_);

    emit addContextMenuItem(contextMenu->menuAction(), DATA_SKELETON, CONTEXTOBJECTMENU);
  }
}

void TypeSkeletonPlugin::slotUpdateContextMenu( int _objectId )
{
  if ( _objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(_objectId,object) )
    return;

  SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(object);
  
  if(skeletonObject != 0){
    showIndicesAction_->setChecked( skeletonObject->indicesVisible() );
    showCoordFramesAction_->setChecked( skeletonObject->skeletonNode()->coordFramesVisible() );
    showMotionAction_->setChecked( skeletonObject->motionPathVisible() );
  }
}

void TypeSkeletonPlugin::slotShowIndices(){
  
  QVariant contextObject = showIndicesAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(object);
  
  if(skeletonObject != 0){
    skeletonObject->showIndices( showIndicesAction_->isChecked() );
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

void TypeSkeletonPlugin::slotShowCoordFrames(){
  
  QVariant contextObject = showCoordFramesAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(object);
  
  if(skeletonObject != 0){
    skeletonObject->skeletonNode()->showCoordFrames( showCoordFramesAction_->isChecked() );
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

void TypeSkeletonPlugin::slotShowMotionPath(){
  
  QVariant contextObject = showMotionAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(object);
  
  if(skeletonObject != 0){
    skeletonObject->showMotionPath( showMotionAction_->isChecked() );
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

bool TypeSkeletonPlugin::registerType() {
  addDataType("Skeleton",tr("Skeleton"));
  setTypeIcon("Skeleton", "SkeletonType.png");
  return true;
}

int TypeSkeletonPlugin::addEmpty(){
    
  // new object data struct
  SkeletonObject * object = new SkeletonObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = QString(tr("New Skeleton %1.skl").arg( object->id() ));

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit emptyObjectAdded (object->id() );

  return object->id();
}

void TypeSkeletonPlugin::generateBackup( int _id, QString _name, UpdateType _type ){
  
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_id, object);
  
  SkeletonObject* skelObj = PluginFunctions::skeletonObject(object);
  
  if ( skelObj != 0 ){

    //get backup object data
    BackupData* backupData = 0;

    if ( object->hasObjectData( OBJECT_BACKUPS ) )
      backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
    else{
      //add backup data
      backupData = new BackupData(object);
      object->setObjectData(OBJECT_BACKUPS, backupData);
    }
    
    //store a new backup
    SkeletonBackup* backup = new SkeletonBackup(skelObj, _name, _type);
    backupData->storeBackup( backup );
  }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typeskeletonplugin , TypeSkeletonPlugin );
#endif


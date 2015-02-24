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

#include "BackupPlugin.hh"
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include "GroupBackup.hh"

//-----------------------------------------------------------------------------
BackupPlugin::BackupPlugin():
globalBackup_(),
backupMenu_(0),
backupsEnabledAction_(0),
undoMenuAction_(0),
redoMenuAction_(0),
undoToolAction_(0),
redoToolAction_(0),
undoContextAction_(0),
redoContextAction_(0),
maxBackupSpinBox_(0)
{

}

//-----------------------------------------------------------------------------

void BackupPlugin::initializePlugin()
{
  int maxBackups = OpenFlipperSettings().value("BackupPlugin/MaxBackups",static_cast<unsigned>(globalBackup_.maxBackups())).toInt();
  globalBackup_.setMaxBackups(maxBackups);
}

//-----------------------------------------------------------------------------

void BackupPlugin::pluginsInitialized() {

  // Create Backup menu
  emit getMenubarMenu(tr("&Backup"), backupMenu_, true );

  if ( OpenFlipper::Options::gui() ) {
    //construct the menu
    backupsEnabledAction_ = new QAction("Backups Enabled",0);
    backupsEnabledAction_->setCheckable(true);
    connect (backupsEnabledAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnableDisableBackups()) );

    undoMenuAction_ = new QAction(tr("&Undo"), this);
    undoMenuAction_->setEnabled(false);
    undoMenuAction_->setStatusTip(tr("Undo the last action."));
    undoMenuAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-undo.png") );
    connect(undoMenuAction_, SIGNAL(triggered()), this, SIGNAL( undo() ) );

    redoMenuAction_ = new QAction(tr("&Redo"), this);
    redoMenuAction_->setEnabled(false);
    redoMenuAction_->setStatusTip(tr("Redo the last action"));
    redoMenuAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-redo.png") );
    connect(redoMenuAction_, SIGNAL(triggered()), this, SIGNAL( redo() ) );

    backupMenu_->addAction(undoMenuAction_);
    backupMenu_->addAction(redoMenuAction_);
    backupMenu_->addSeparator();
    backupMenu_->addAction(backupsEnabledAction_);

    // Add a backup Toolbar
    QToolBar* toolbar = new QToolBar("Backup Toolbar");

    //Undo
    undoToolAction_ = new QAction(tr("&Undo"), this);
    undoToolAction_->setEnabled(false);
    undoToolAction_->setStatusTip(tr("Undo the last action."));
    undoToolAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-undo.png") );
    connect(undoToolAction_, SIGNAL(triggered()), this, SIGNAL( undo() ) );
    toolbar->addAction(undoToolAction_);

    //Redo
    redoToolAction_ = new QAction(tr("&Redo"), this);
    redoToolAction_->setEnabled(false);
    redoToolAction_->setStatusTip(tr("Redo the last action"));
    redoToolAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-redo.png") );
    connect(redoToolAction_, SIGNAL(triggered()), this, SIGNAL( redo() ) );
    toolbar->addAction(redoToolAction_);

    emit addToolbar( toolbar );

    //the release event does not contain the modifier
    emit registerKey(Qt::Key_Z,  Qt::ControlModifier, tr("Undo Action"));
    emit registerKey(Qt::Key_Z, (Qt::ControlModifier | Qt::ShiftModifier),   tr("Redo Action"));

    //add actions for the context menu
    undoContextAction_ = new QAction(tr("&Undo"), this);
    undoContextAction_->setEnabled(false);
    undoContextAction_->setStatusTip(tr("Undo the last action."));
    undoContextAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-undo.png") );
    connect(undoContextAction_, SIGNAL(triggered()), this, SLOT(slotObjectUndo()) );
    emit addContextMenuItem(undoContextAction_, DATA_ALL, CONTEXTOBJECTMENU);

    redoContextAction_ = new QAction(tr("&Redo"), this);
    redoContextAction_->setEnabled(false);
    redoContextAction_->setStatusTip(tr("Redo the last action"));
    redoContextAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-redo.png") );
    connect(redoContextAction_, SIGNAL(triggered()), this, SLOT(slotObjectRedo()) );
    emit addContextMenuItem(redoContextAction_, DATA_ALL, CONTEXTOBJECTMENU);

    //update option widget since it is created earlier
    if (maxBackupSpinBox_)
      maxBackupSpinBox_->setValue(globalBackup_.maxBackups());

  }

//createBackup(int _objectId, QString _name, UpdateType _type= UPDATE_ALL)
  emit setSlotDescription("createBackup(int,QString)", tr("Creates a backup which can be restored via undo."),
                               QString("objectId,name").split(","), QString("Id of the object,name of the backup").split(","));
  emit setSlotDescription("createBackup(int,QString,UpdateType)", tr("Creates a backup which can be restored via undo."),
                             QString("objectId,name,type").split(","), QString("Id of the object,name of the backup,updatetypes which are changed").split(","));
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotAllCleared(){
  globalBackup_.clear();
  updateButtons();
}

//-----------------------------------------------------------------------------

void BackupPlugin::updateButtons() {

  if ( globalBackup_.undoAvailable() ){
    undoMenuAction_->setText( tr("Undo '%1'").arg( globalBackup_.undoName() ) );
    undoMenuAction_->setEnabled(true);
    undoToolAction_->setText( tr("Undo '%1'").arg( globalBackup_.undoName() ) );
    undoToolAction_->setEnabled(true);
  } else {
    undoMenuAction_->setText( tr("Undo") );
    undoMenuAction_->setEnabled(false);
    undoToolAction_->setText( tr("Undo") );
    undoToolAction_->setEnabled(false);
  }

  if ( globalBackup_.redoAvailable() ){
    redoMenuAction_->setText( tr("Redo '%1'").arg( globalBackup_.redoName() ) );
    redoMenuAction_->setEnabled(true);
    redoToolAction_->setText( tr("Redo '%1'").arg( globalBackup_.redoName() ) );
    redoToolAction_->setEnabled(true);
  } else {
    redoMenuAction_->setText( tr("Redo") );
    redoMenuAction_->setEnabled(false);
    redoToolAction_->setText( tr("Redo") );
    redoToolAction_->setEnabled(false);
  }

  backupsEnabledAction_->setChecked( OpenFlipper::Options::backupEnabled() );
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotEnableDisableBackups() {
  OpenFlipper::Options::enableBackup(backupsEnabledAction_->isChecked());
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotKeyEvent( QKeyEvent* _event ){
   switch (_event->key())
   {
      case Qt::Key_Z :
         if ( _event->modifiers() == Qt::ControlModifier )
           emit undo();
         else if ( _event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) )
           emit redo();
         break;
      default:
         break;
  }
}

//-----------------------------------------------------------------------------

void BackupPlugin::objectDeleted(int _objectid) {
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotUpdateContextMenu( int _objectId ){

  //disable everything
  undoContextAction_->setText( tr("Undo") );
  undoContextAction_->setEnabled(false);
  redoContextAction_->setText( tr("Redo") );
  redoContextAction_->setEnabled(false);

  //get backup data
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId, object);

  if ( object != 0 ){

    if ( object->hasObjectData( OBJECT_BACKUPS ) ){

      //get backup object data
      BackupData* backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));

      if ( backupData->undoAvailable() ){
        undoContextAction_->setData(_objectId);
        undoContextAction_->setText( tr("Undo '%1'").arg( backupData->undoName() ) );
        undoContextAction_->setEnabled( !backupData->undoBlocked() );
      }

      if ( backupData->redoAvailable() ){
        redoContextAction_->setData(_objectId);
        redoContextAction_->setText( tr("Redo '%1'").arg( backupData->redoName() ) );
        redoContextAction_->setEnabled( !backupData->redoBlocked() );
      }
    }
  }

}

//-----------------------------------------------------------------------------

void BackupPlugin::slotObjectUndo(){

  int id = undoContextAction_->data().toInt();
  emit undo(id);
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotObjectRedo(){

  int id = undoContextAction_->data().toInt();
  emit redo(id);
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotCreateBackup( int _objectid, QString _name, UpdateType _type){

  if ( !OpenFlipper::Options::backupEnabled() )
    return;

  BaseObjectData* object;

  if ( !PluginFunctions::getObject(_objectid,object) ){
    emit log(LOGWARN,"Unable to find backup object with id " + QString::number(_objectid));
    return;
  }

  //tell TypePlugin to Backup
  emit generateBackup( _objectid, _name, _type );

  //add global backup
  IdList groupIds;
  groupIds.push_back( _objectid );

  bool skipBackup = false;

  if ( globalBackup_.undoAvailable() ){
    //skip 'Original Object' Backups in global view
    BackupData* backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));

    if ( backupData != 0 )
      skipBackup = !backupData->undoAvailable();
  }

  if ( !skipBackup ){
    GroupBackup* backup = new GroupBackup(groupIds, _name);
    globalBackup_.storeBackup( backup );
  }

  updateButtons();
  emit log(LOGOUT,"Created backup for " + QString::number(_objectid)+ " , Name : '" + _name + "'" );
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotCreateBackup( IdList _objectids , QString _name, std::vector<UpdateType> _types){

  if ( !OpenFlipper::Options::backupEnabled() )
    return;

  IdList groupIds;

  if ( _objectids.size() != _types.size() ){
    emit log(LOGWARN,"Unable to create backup sizes of ids and updateTypes do not match!");
    return;
  }

  //generate backups on all objects
  for (unsigned int i=0; i < _objectids.size(); ++i){

    BaseObjectData* object;

    if ( !PluginFunctions::getObject(_objectids[i],object) ){
      emit log(LOGWARN,"Unable to find backup object with id " + QString::number(_objectids[i]));
      continue;
    }

    //tell TypePlugin to generate a backup
    emit generateBackup( _objectids[i], _name, _types[i] );
    groupIds.push_back(  _objectids[i] );
  }

  //add global backup
  if ( ! groupIds.empty() ){

    GroupBackup* backup = new GroupBackup(groupIds, _name);
    globalBackup_.storeBackup( backup );

    updateButtons();
    emit log(LOGOUT,"Created grouped backup, Name : '" + _name + "'" );
  }
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotUndo(int _objectid){

  emit aboutToRestore(_objectid);
  globalBackup_.undo(_objectid);
  emit restored(_objectid);
  emit updatedObject(_objectid, UPDATE_ALL);

  updateButtons();
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotUndo(){
  GroupBackup* group = dynamic_cast< GroupBackup* >( globalBackup_.currentState() );

  if (group) {
    IdList ids = group->objectIDs();
    IdList::const_iterator it, end;
    for (it = ids.begin(), end = ids.end(); it != end; ++it)
      emit aboutToRestore(*it);

    globalBackup_.undo();

    for (unsigned int i=0; i < group->objectIDs().size(); i++)
    {
      emit restored(group->objectIDs()[i]);
      emit updatedObject(group->objectIDs()[i], UPDATE_ALL);
    }

    updateButtons();
  } else
    emit log(LOGWARN,"Unable to find the current GroupBackup");
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotRedo(int _objectid){

  emit aboutToRestore(_objectid);
  globalBackup_.redo(_objectid);
  emit restored(_objectid);
  emit updatedObject(_objectid, UPDATE_ALL);

  updateButtons();
}

//-----------------------------------------------------------------------------

void BackupPlugin::slotRedo(){
  globalBackup_.redo();

  GroupBackup* group = dynamic_cast< GroupBackup* >( globalBackup_.currentState() );

  if ( group != 0)
    for (unsigned int i=0; i < group->objectIDs().size(); i++)
    {
      emit restored(group->objectIDs()[i]);
      emit updatedObject(group->objectIDs()[i], UPDATE_ALL);
    }

  updateButtons();
}

//-----------------------------------------------------------------------------

void BackupPlugin::createBackup(int _objectId, QString _name, UpdateType _type)
{
  slotCreateBackup(_objectId,_name,_type);
}

//-----------------------------------------------------------------------------

bool BackupPlugin::initializeOptionsWidget(QWidget*& _widget)
{
  QLabel* maxBackupLabel = new QLabel();
  maxBackupLabel->setText(tr("Max. saved backups: "));
  maxBackupSpinBox_ = new QSpinBox();
  maxBackupSpinBox_->setValue(globalBackup_.maxBackups());
  maxBackupSpinBox_->setRange(0,100);

  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(maxBackupLabel);
  layout->addWidget(maxBackupSpinBox_);

  QWidget* baseWidget = new QWidget();
  baseWidget->setLayout(layout);
  _widget = baseWidget;

  return true;
}

//-----------------------------------------------------------------------------

void BackupPlugin::applyOptions()
{
  int maxBackups = maxBackupSpinBox_->value();
  globalBackup_.setMaxBackups(maxBackups);
  OpenFlipperSettings().setValue("BackupPlugin/MaxBackups", maxBackups);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( backupplugin , BackupPlugin );
#endif


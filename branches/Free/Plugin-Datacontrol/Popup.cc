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
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/QtWidgets/QtMaterialDialog.hh>

#include <QInputDialog>
#include <QMessageBox>
#include "Menu.hh"

#include <ObjectTypes/Light/LightWidget.hh>


//******************************************************************************

/** \brief Slot for Remove action in ContextMenu
 * 
 */
void DataControlPlugin::slotPopupRemove (  ) {

  QItemSelectionModel* selection = view_->selectionModel();

  if (selection == 0) return;

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows();


  QMessageBox msgBox;
  msgBox.setText(tr("Do you really want to remove the selected objects?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);

  QString text;

  for ( int i = 0 ; i < indexList.size() ; ++i)
    text += model_->itemName( indexList[i] ) + "\n";

  msgBox.setDetailedText(text);

  if ( msgBox.exec() == QMessageBox::No)
    return;

  for ( int i = 0 ; i < indexList.size() ; ++i) {

    int id = model_->itemId( indexList[i] );


    if ( id != -1 )
      emit deleteObject( id );
  }

  emit updateView();
}


//******************************************************************************

/** \brief Slot for Ungroup action in ContextMenu
 * 
 */
void DataControlPlugin::slotUngroup (  ) {
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );

  //get the object
  int id = model_->itemId( indexList[0] );
  unGroupObject(id);
}


//******************************************************************************

/** \brief Slot for Copy action in ContextMenu
 * 
 */
void DataControlPlugin::slotCopy() {
  QItemSelectionModel* selection = view_->selectionModel();

  if (selection == 0) return;

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows();

  for ( int i = 0 ; i < indexList.size() ; ++i) {

    int id = model_->itemId( indexList[i] );

    if ( id > 0){

      int newObject;

      emit copyObject( id, newObject );
    }

  }

  emit updateView();
}


//******************************************************************************

/** \brief Slot for Group action in ContextMenu
 * 
 */
void DataControlPlugin::slotGroup() {

  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );


  //get object ids
  IdList ids;
  for (int i = 0; i < indexList.size(); ++i)
    ids.push_back( model_->itemId(indexList[i]) );

  //group all objects
  groupObjects(ids);
}


//******************************************************************************

/** \brief ContextMenu requested - creates the contextMenu
 * 
 * @param _pos Position where it was requested
 */
void DataControlPlugin::slotCustomContextMenuRequested ( const QPoint & _pos ) {
  popupIndex_ = view_->indexAt(_pos);

  if (!popupIndex_.isValid())
    return;

  TreeItem* item = model_->getItem(popupIndex_);


  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();

  Menu    menu(0);
  QAction* action;
  QIcon icon;
  if ( selectedRows > 1 ) {
    action = menu.addAction("Copy",this,SLOT ( slotCopy() ));
    icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-copy.png");
    action->setIcon(icon);
    menu.addAction(tr("Group"),this,SLOT ( slotGroup() ));
    menu.addSeparator();
    menu.addAction(tr("Remove"),this,SLOT ( slotPopupRemove() ));
   }else
    // check if the item is a group item
    if ( item->isGroup() ) {
      action = menu.addAction(tr("Zoom to objects"),this,SLOT ( slotZoomTo() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-in.png");
      action->setIcon(icon);
      menu.addAction(tr("Ungroup"),this,SLOT ( slotUngroup() ));
      menu.addSeparator();
      action = menu.addAction(tr("Rename"),this,SLOT ( slotRename() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
      action->setIcon(icon);
      menu.addSeparator();
      action = menu.addAction(tr("Remove"),this,SLOT ( slotPopupRemove() ));
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
      action->setIcon(icon);
    } else {
      
        //show object name and id in first entry of the menu
        int id = model_->itemId( indexList[0] );
        BaseObject* item = 0;
        if ( id != -1 && PluginFunctions::getObject(id,item) ) {
          QAction* header = menu.addAction(tr("Object ")+QString::number(id)+": "+item->name());
          header->setIcon( typeIcon(item->dataType()) );
        }
        
        menu.addSeparator();
        
        if(item->dataType() == DATA_LIGHT) {
            action = menu.addAction(tr("Copy"),this,SLOT ( slotCopy() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-copy.png");
            action->setIcon(icon);
            action = menu.addAction(tr("Rename"),this,SLOT ( slotRename() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
            action->setIcon(icon);
            action = menu.addAction(tr("Remove"),this,SLOT ( slotPopupRemove() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
            action->setIcon(icon);
            menu.addSeparator();
	    
	    LightObject* light = 0;
	    if ( item->id() != -1 && PluginFunctions::getObject( item->id(), light ) ) {
		if (light != 0 && !light->lightSource()->enabled()) {
		
		    action = menu.addAction(tr("Switch On"),this,SLOT ( slotSwitchLight() ));
		    icon = typeIcon(DATA_LIGHT);
		}
	    }
	    if (light == 0 || light->lightSource()->enabled())
	    {
		action = menu.addAction(tr("Switch Off"),this,SLOT ( slotSwitchLight() ));
		icon = QIcon (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"LightOff.png");
	    }
            action->setIcon(icon);
	    
            action = menu.addAction(tr("Edit Light"),this,SLOT ( slotEditLight() ));
            icon = typeIcon(DATA_LIGHT);
            action->setIcon(icon);
            
        } else {
            action = menu.addAction(tr("Zoom to object"),this,SLOT ( slotZoomTo() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-in.png");
            action->setIcon(icon);

            action = menu.addAction(tr("Copy"),this,SLOT ( slotCopy() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-copy.png");
            action->setIcon(icon);

            action = menu.addAction(tr("Rename"),this,SLOT ( slotRename() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"edit-rename.png");
            action->setIcon(icon);

            action = menu.addAction(tr("Material Properties"),this,SLOT ( slotMaterialProperties() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png");
            action->setIcon(icon);

            menu.addAction(tr("Copy Material Properties to Targeted Objects"), this, SLOT ( slotCopyMaterialToTargeted() ));
            menu.addAction(tr("Copy Material Properties to Clipboard"), this, SLOT ( slotCopyMaterialToClipboard() ));
            menu.addAction(tr("Paste Material Properties from Clipboard"), this, SLOT ( slotPasteMaterialFromClipboard() ));

            menu.addAction(tr("Group"),this,SLOT ( slotGroup() ));

            menu.addSeparator();
            action = menu.addAction(tr("Remove"),this,SLOT ( slotPopupRemove() ));
            icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-delete-item.png");
            action->setIcon(icon);
        }
    }



  menu.exec(view_->mapToGlobal( _pos) );

}


//******************************************************************************

/** \brief Generate a ContextMenu for the header of the TreeView
 * 
 * @param _pos Position where the Menu has to be created
 */
void DataControlPlugin::slotHeaderCustomContextMenuRequested ( const QPoint & _pos ) {

  headerPopupType_ = viewHeader_->logicalIndexAt( _pos );

  QMenu menu(0);

  QIcon icon;

  //get all used types
  QVector<DataType> types;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                    o_it != PluginFunctions::objectsEnd(); ++o_it)
    if ( !types.contains( o_it->dataType() ) )
      types.push_back( o_it->dataType() );

  QAction* action;

  switch (headerPopupType_) {
//     case 0 :
//       std::cerr << "0";
//       break;
    //Show / Hide
    case 1 :
      icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
      action = menu.addAction(icon,tr("Show all"),this,SLOT ( showAll() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, tr("Show all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( showAll() ));
        action->setData( QVariant( types[i].value() ) );
      }

      menu.addSeparator();
      action = menu.addAction(tr("Hide all"),this,SLOT ( hideAll() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(tr("Hide all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( hideAll() ));
        action->setData( QVariant( types[i].value() ) );
      }

      break;
    // Source
    case 2 :
      action = menu.addAction(tr("Select all"),this,SLOT ( setAllSource() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, tr("Select all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( setAllSource() ));
        action->setData( QVariant( types[i].value() ) );
      }

      menu.addSeparator();
      action = menu.addAction(tr("Deselect all"),this,SLOT ( clearAllSource() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, tr("Deselect all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( clearAllSource() ));
        action->setData( QVariant( types[i].value() ) );
      }

      break;
    // Target
    case 3 :
      action = menu.addAction(tr("Select all"),this,SLOT ( setAllTarget() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, tr("Select all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( setAllTarget() ));
        action->setData( QVariant( types[i].value() ) );
      }

      menu.addSeparator();
      action = menu.addAction(tr("Deselect all"),this,SLOT ( clearAllTarget() ));
      action->setData( QVariant() );

      if (types.count() > 1)
      for (int i=0; i < types.count(); i++){
        action = menu.addAction(icon, tr("Deselect all of type '") + dataTypeName(types[i]) + "'",this,SLOT ( clearAllTarget() ));
        action->setData( QVariant( types[i].value() ) );
      }

      break;
    default :
//       std::cerr << "def";
      break;
  }

  menu.exec(viewHeader_->mapToGlobal( _pos ) );
}


//******************************************************************************

/** \brief show a rename dialog for object names
 * 
 */
void DataControlPlugin::slotRename(){
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1){
    int id = model_->itemId( indexList[0]);
    BaseObject* item = 0;

    if ( id != -1 && PluginFunctions::getObject(id,item) ){

      bool ok;
      QString newName = QInputDialog::getText(0, tr("Rename"),
                                              tr("Enter a new name:"), QLineEdit::Normal,
                                              item->name(), &ok);
      if (ok && !newName.isEmpty()){
        item->setName(newName);
        view_->resizeColumnToContents(0);
      }
    }
  }
}


//******************************************************************************

/** \brief show the material properties dialog
 * 
 */
void DataControlPlugin::slotCopyMaterialToTargeted() {
    BaseObject* item = 0;

    //check if it was called from object contextMenu or from the toolBox
    QAction* action = dynamic_cast< QAction* > ( sender() );

    if ( action ){
      bool ok = false;

      int id = action->data().toInt(&ok);

      if ( ok && id > 0 )
        PluginFunctions::getObject(id,item);
    }

    if ( item == 0 ){
      // the slot was called from toolbox
      QItemSelectionModel* selection = view_->selectionModel();

      // Get all selected rows
      QModelIndexList indexList = selection->selectedRows ( 0 );
      int selectedRows = indexList.size();
      if (selectedRows == 1){
        int id = model_->itemId( indexList[0] );

        if ( id > 0 )
          PluginFunctions::getObject(id,item);
      }
    }

    if ( item != 0 ){

      BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);
      const ACG::SceneGraph::Material &sourceMaterial = itemData->materialNode()->material();

      for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS);
              o_it != PluginFunctions::objectsEnd(); ++o_it) {

          MaterialNode * const materialNode = o_it->materialNode();
          if (materialNode) {
              materialNode->material() = sourceMaterial;
          }
      }

      emit updateView();
    }
}

void DataControlPlugin::slotCopyMaterialToClipboard() {
    BaseObject* item = 0;

    //check if it was called from object contextMenu or from the toolBox
    QAction* action = dynamic_cast< QAction* > ( sender() );

    if ( action ){
      bool ok = false;

      int id = action->data().toInt(&ok);

      if ( ok && id > 0 )
        PluginFunctions::getObject(id,item);
    }

    if ( item == 0 ){
      // the slot was called from toolbox
      QItemSelectionModel* selection = view_->selectionModel();

      // Get all selected rows
      QModelIndexList indexList = selection->selectedRows ( 0 );
      int selectedRows = indexList.size();
      if (selectedRows == 1){
        int id = model_->itemId( indexList[0] );

        if ( id > 0 )
          PluginFunctions::getObject(id,item);
      }
    }

    if ( item != 0 ){

      BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);
      const ACG::SceneGraph::Material &sourceMaterial = itemData->materialNode()->material();

      const QString materialJson = sourceMaterial.serializeToJson();
      QApplication::clipboard()->setText(materialJson);
    }
}

void DataControlPlugin::slotPasteMaterialFromClipboard() {
    BaseObject* item = 0;

    //check if it was called from object contextMenu or from the toolBox
    QAction* action = dynamic_cast< QAction* > ( sender() );

    if ( action ){
      bool ok = false;

      int id = action->data().toInt(&ok);

      if ( ok && id > 0 )
        PluginFunctions::getObject(id,item);
    }

    if ( item == 0 ){
      // the slot was called from toolbox
      QItemSelectionModel* selection = view_->selectionModel();

      // Get all selected rows
      QModelIndexList indexList = selection->selectedRows ( 0 );
      int selectedRows = indexList.size();
      if (selectedRows == 1){
        int id = model_->itemId( indexList[0] );

        if ( id > 0 )
          PluginFunctions::getObject(id,item);
      }
    }

    if ( item != 0 ){

      BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);
      ACG::SceneGraph::Material &destMaterial = itemData->materialNode()->material();

      destMaterial.deserializeFromJson(QApplication::clipboard()->text());

      emit updateView();
    }
}

void DataControlPlugin::slotMaterialProperties(){

  BaseObject* item = 0;

  //check if it was called from object contextMenu or from the toolBox
  QAction* action = dynamic_cast< QAction* > ( sender() );

  if ( action ){
    bool ok = false;

    int id = action->data().toInt(&ok);

    if ( ok && id > 0 )
      PluginFunctions::getObject(id,item);
  }

  if ( item == 0 ){
    // the slot was called from toolbox
    QItemSelectionModel* selection = view_->selectionModel();

    // Get all selected rows
    QModelIndexList indexList = selection->selectedRows ( 0 );
    int selectedRows = indexList.size();
    if (selectedRows == 1){
      int id = model_->itemId( indexList[0] );

      if ( id > 0 )
        PluginFunctions::getObject(id,item);
    }
  }

  if ( item != 0 ){

    BaseObjectData* itemData = dynamic_cast< BaseObjectData* > (item);

    ACG::QtWidgets::QtMaterialDialog* dialog = new ACG::QtWidgets::QtMaterialDialog( 0, itemData->materialNode() );

    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);

    connect(dialog, SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
            this,   SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)) );

    dialog->setWindowIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png"));

    dialog->show();

  }
}


//******************************************************************************

void DataControlPlugin::slotNodeChanged( ACG::SceneGraph::BaseNode* /*_node*/ ){
  emit updateView();
}

//******************************************************************************

/**
 * \brief Switch light source on or off
 */
void DataControlPlugin::slotSwitchLight() {
    
    QItemSelectionModel* selection = view_->selectionModel();
    
    // Get all selected rows
    QModelIndexList indexList = selection->selectedRows ( 0 );
    int selectedRows = indexList.size();
    if (selectedRows == 1) {
        
        LightObject* light = 0;
        int id = model_->itemId( indexList[0]);
        
        if (id == -1 || !PluginFunctions::getObject( id, light ) )
            return;
        
        if(light == 0) return;
        
        light->lightSource()->enabled() ?
            light->lightSource()->disable() :
            light->lightSource()->enable();
            
        emit updatedObject(id, UPDATE_ALL);
    }
}

//******************************************************************************

/**
* \brief Edit the light source's parameters
*/
void DataControlPlugin::slotEditLight() {
 
  QItemSelectionModel* selection = view_->selectionModel();
  
  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1) {
    
    LightObject* light = 0;
    int id = model_->itemId( indexList[0]);
    
    if (id == -1 || !PluginFunctions::getObject( id, light ) )
      return;
    
    if(light == 0) return;
    
    LightWidget* widget = new LightWidget( light->lightNode() );
    widget->setWindowFlags(Qt::WindowStaysOnTopHint);
    widget->show();
  }
}

//******************************************************************************

/** \brief Zoom to an object
 * 
 */
void DataControlPlugin::slotZoomTo(){
  QItemSelectionModel* selection = view_->selectionModel();

  // Get all selected rows
  QModelIndexList indexList = selection->selectedRows ( 0 );
  int selectedRows = indexList.size();
  if (selectedRows == 1){
    BaseObject* item = 0;
    int id = model_->itemId( indexList[0]);

    if (id == -1 || !PluginFunctions::getObject( id, item) )
      return;

    if ( item->isGroup() ) {
      //zoom to all objects in this group
      QList< BaseObject* > children = item->getLeafs();

      //compute boundingBox
      bool firstRound = true;
      ACG::Vec3d bbmin(FLT_MAX,FLT_MAX,FLT_MAX);
      ACG::Vec3d bbmax(FLT_MIN,FLT_MIN,FLT_MIN);

      for (int i=0; i < children.size(); i++){
        BaseObjectData* child = dynamic_cast< BaseObjectData* > (children[i]);
        if (child){

          if ( !child->visible() ){
            child->show();
          }

          ACG::Vec3d cur_min;
          ACG::Vec3d cur_max;

          child->getBoundingBox(cur_min, cur_max);

          if (firstRound){
            bbmin = cur_min;
            bbmax = cur_max;
            firstRound = false;
          }else{
            bbmin[0] = std::min( bbmin[0], cur_min[0]);
            bbmin[1] = std::min( bbmin[1], cur_min[1]);
            bbmin[2] = std::min( bbmin[2], cur_min[2]);
            bbmax[0] = std::max( bbmax[0], cur_max[0]);
            bbmax[1] = std::max( bbmax[0], cur_max[1]);
            bbmax[2] = std::max( bbmax[0], cur_max[2]);
          }

        }
      }
      //zoom to objects
      ACG::Vec3d bbcenter = (bbmax + bbmin) * 0.5;

      double bbradius = (bbmax - bbmin).norm();

      ACG::Vec3d eye = bbcenter + (PluginFunctions::eyePos() - bbcenter).normalize() * bbradius ;

      PluginFunctions::flyTo(eye, bbcenter );

    }else{
      //zoom to object
      BaseObjectData* obj = dynamic_cast< BaseObjectData* >(item);

      if (obj){

        if ( !obj->visible() )
          obj->show();

        ACG::Vec3d bbmin;
        ACG::Vec3d bbmax;

        obj->getBoundingBox(bbmin, bbmax);

        if ((bbmin[0] > bbmax[0]) || (bbmin[1] > bbmax[1]) || (bbmin[2] > bbmax[2])){
          std::cerr << "Error while computing bounding box!";
          return;
        }

        ACG::Vec3d bbcenter = (bbmax + bbmin) * 0.5;

        double bbradius = (bbmax - bbmin).norm();

        ACG::Vec3d eye = bbcenter + (PluginFunctions::eyePos() - bbcenter).normalize() * bbradius ;

        PluginFunctions::flyTo(eye, bbcenter );
      }
    }
  }
}

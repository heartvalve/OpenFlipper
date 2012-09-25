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

#include <QtGui>
#include "ViewControlPlugin.hh"
#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/Geometry/Algorithms.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <ACG/Scenegraph/CoordsysNode.hh>

#define SHOW_SELECTION    "Selections"
#define SHOW_AREAS        "Modeling Areas"
#define SHOW_AREA         "Modeling Area"
#define SHOW_HANDLE       "Handle   Area"
#define SHOW_FEATURES     "Feature Selection"
#define USEGLOBALDRAWMODE "Use Global DrawMode"
#define SETSHADERS        "Set Shader"

ViewControlPlugin::ViewControlPlugin():
viewControlMenu_(0),
lastObjectId_(0),
shaderWidget_(0),
toolbar_(0),
toolbarViewingDirections_(0),
viewTop_(0),
viewBottom_(0),
viewLeft_(0),
viewRight_(0),
viewFront_(0),
viewBack_(0)
{

}

void ViewControlPlugin::pluginsInitialized() {

  // Disable Context Menu for draw Modes
  OpenFlipper::Options::drawModesInContextMenu(false);

  // Create a new visualization context menu
  viewControlMenu_ = new QMenu("Visualization");

  // Set an icon for the menu
  QIcon icon = QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
  viewControlMenu_->setIcon(icon);

  // Add it as context menu for all registered data types
  for(std::vector<TypeInfo>::const_iterator it = typesBegin(); it != typesEnd(); ++it) {
      if(it->name == "Unknown" || it->name == "Group" || it->name == "All") continue;
      emit addContextMenuItem(viewControlMenu_->menuAction(), it->type, CONTEXTOBJECTMENU );
  }
  
  connect( viewControlMenu_,  SIGNAL( triggered(QAction*) ), this, SLOT( contextMenuTriggered(QAction*) ));

  setDescriptions();
  
  // Create toolbar
  toolbar_ = new QToolBar(tr("Viewing Directions"));

  // Action group for toolbar
  toolbarViewingDirections_ = new QActionGroup(toolbar_);
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  // different viewing direction buttons for toolbar
  viewTop_ = new QAction( QIcon(iconPath + "viewcontrol_top.png"), tr("View from top") , toolbarViewingDirections_);
  toolbar_->addAction( viewTop_ );
  viewBottom_ = new QAction( QIcon(iconPath + "viewcontrol_bottom.png"), tr("View from bottom") , toolbarViewingDirections_);
  toolbar_->addAction( viewBottom_ );
  viewLeft_ = new QAction( QIcon(iconPath + "viewcontrol_left.png"), tr("View from left") , toolbarViewingDirections_);
  toolbar_->addAction( viewLeft_ );
  viewRight_ = new QAction( QIcon(iconPath + "viewcontrol_right.png"), tr("View from right") , toolbarViewingDirections_);
  toolbar_->addAction( viewRight_ );
  viewFront_ = new QAction( QIcon(iconPath + "viewcontrol_front.png"), tr("View from front") , toolbarViewingDirections_);
  toolbar_->addAction( viewFront_ );
  viewBack_ = new QAction( QIcon(iconPath + "viewcontrol_back.png"), tr("View from back") , toolbarViewingDirections_);
  toolbar_->addAction( viewBack_ );

  connect( toolbarViewingDirections_, SIGNAL( triggered(QAction*) ), this, SLOT(setView(QAction*)) );
  
  // Add the generated toolbar to the core
  emit addToolbar(toolbar_);


}

void ViewControlPlugin::updateShaderList() {

  //=======================================================================================
  // Get Shaders
  //=======================================================================================
  shaderList_.clear();
  QStringList shadersDirs = OpenFlipper::Options::shaderDir().entryList( QDir::Dirs| QDir::NoDotAndDotDot ,QDir::Name);

  for ( int i = 0 ; i < shadersDirs.size(); ++i ) {
    ShaderInfo info;

    QFileInfo descriptionFile(OpenFlipper::Options::shaderDirStr() +
                              OpenFlipper::Options::dirSeparator() +
                              shadersDirs[i] + OpenFlipper::Options::dirSeparator() + "shader.ini" );

    INIFile ini;
    if ( !descriptionFile.exists() || !ini.connect( descriptionFile.filePath() ,false ) ) {
      emit log( LOGWARN, "Unable to open description file for shader " + shadersDirs[i] + " at " + descriptionFile.filePath() );

      info.description = "Unknown";
      info.version     = "Unknown";
      info.name        = shadersDirs[i];
      info.details     = "----------";
      info.example     = "";

    } else {

      QString tmp;
      if ( ini.get_entry(tmp,"Info","name") )
        info.name = tmp;
      else {
        info.name = shadersDirs[i];
        emit log( LOGWARN, "Unable to get name from description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"Info","description") )
        info.description = tmp;
      else {
        info.description = "Unknown";
        emit log( LOGWARN, "Unable to get shader description from description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"Info","details") )
        info.details = tmp;
      else {
        info.details     = "----------";
        emit log( LOGWARN, "Unable to get shader details from description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"Info","version") )
        info.version = tmp;
      else {
        info.version     = "Unknown";
        emit log( LOGWARN, "Unable to get shader version from description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"Info","example") )
        info.example = OpenFlipper::Options::shaderDirStr() + OpenFlipper::Options::dirSeparator() +
                       shadersDirs[i] + OpenFlipper::Options::dirSeparator() + tmp;
      else {
        info.example = "";
        emit log( LOGWARN, "Unable to get example image from Description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"FragmentShader","file") )
        info.fragmentShader = tmp;
      else {
        info.fragmentShader = "Fragment.glsl";
        emit log( LOGWARN, "Unable to get fragment shader filename from Description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"VertexShader","file") )
        info.vertexShader = tmp;
      else {
        info.vertexShader = "Vertex.glsl";
        emit log( LOGWARN, "Unable to get vertex shader filename from Description file for shader " + shadersDirs[i] );
      }

      if ( ini.get_entry(tmp,"PickVertexShader","file") )
        info.pickVertexShader = tmp;
      else
        info.pickVertexShader = "";

      if ( ini.get_entry(tmp,"PickFragmentShader","file") )
        info.pickFragmentShader = tmp;
      else
        info.pickFragmentShader = "";

      //uniform names
      std::vector<QString> uniforms;
      if ( ini.get_entry(uniforms,"Info","uniforms") ) {
        info.hasUniforms = true;
        for ( uint i = 0 ; i < uniforms.size() ; ++i)
            info.uniforms << uniforms[i];
      } else {
        info.hasUniforms = false;
        info.uniforms.clear();
      }

      //unform types
      uniforms.clear();
      if ( info.hasUniforms && ini.get_entry(uniforms,"Info","uniformTypes") ) {
        for ( uint i = 0 ; i < uniforms.size() ; ++i)
            info.uniformTypes << uniforms[i];
      } else if ( info.hasUniforms ) {
        info.hasUniforms = false;
        info.uniforms.clear();
        info.uniformTypes.clear();
        emit log( LOGERR, "Uniforms for shader " + shadersDirs[i] + " defined but no type available, disabled uniforms." );
      }

      //uniform default values
      uniforms.clear();
      if ( info.hasUniforms && ini.get_entry(uniforms,"Info","uniformsDefault" ) ) {
        for ( uint i = 0 ; i < uniforms.size() ; ++i)
            info.uniformsDefault << uniforms[i];
      } else if ( info.hasUniforms ) {
        info.hasUniforms = false;
        info.uniforms.clear();
        info.uniformTypes.clear();
        emit log( LOGERR, "Uniforms for shader " + shadersDirs[i] + " defined but no defaults available, disabled uniforms." );
      }
      //uniform min values
      uniforms.clear();
      if ( info.hasUniforms && ini.get_entry(uniforms,"Info","uniformsMin" ) ) {
        for ( uint i = 0 ; i < uniforms.size() ; ++i)
            info.uniformsMin << uniforms[i];
      } else if ( info.hasUniforms )
        info.uniformsMin = info.uniformsDefault;

      //uniform max values
      uniforms.clear();
      if ( info.hasUniforms && ini.get_entry(uniforms,"Info","uniformsMax" ) ) {
        for ( uint i = 0 ; i < uniforms.size() ; ++i)
            info.uniformsMax << uniforms[i];
      } else if ( info.hasUniforms )
        info.uniformsMax = info.uniformsDefault;

      ini.disconnect();
    }

    info.directory = descriptionFile.path();

    QFileInfo vertexShaderFile( info.directory + OpenFlipper::Options::dirSeparator() + info.vertexShader );
    if ( !vertexShaderFile.exists() ) {
      emit log( LOGERR, "Unable to find vertex shader file " + vertexShaderFile.filePath() );
      continue;
    }

    QFileInfo fragmentShaderFile( info.directory + OpenFlipper::Options::dirSeparator() + info.fragmentShader );
    if ( !fragmentShaderFile.exists() ) {
      emit log( LOGERR, "Unable to find fragment shader file " + fragmentShaderFile.filePath() );
      continue;
    }

    shaderList_.push_back(info);
  }

}


void ViewControlPlugin::contextMenuTriggered(QAction* _action){
  if ( _action->text() == SHOW_SELECTION) {

    QVariant contextObject = _action->data();
    int objectId = contextObject.toInt();

    if ( objectId == -1)
      return;

    showSelection( objectId , !selectionVisible( objectId) );

    emit updateView();
  }

  if ( _action->text() == SHOW_AREAS) {

    QVariant contextObject = _action->data();
    int objectId = contextObject.toInt();

    if ( objectId == -1)
      return;

    showModelingAreas( objectId, !modelingAreasVisible(objectId) );

    emit updateView();
  }

  if ( _action->text() == SHOW_AREA) {

    QVariant contextObject = _action->data();
    int objectId = contextObject.toInt();

    if ( objectId == -1)
      return;

    showAreas( AREA, objectId, !areasVisible(AREA,objectId) );

    emit updateView();
  }

  if ( _action->text() == SHOW_HANDLE) {

    QVariant contextObject = _action->data();
    int objectId = contextObject.toInt();

    if ( objectId == -1)
      return;

    showAreas( HANDLEAREA, objectId, !areasVisible(HANDLEAREA,objectId) );

    emit updateView();
  }

   if ( _action->text() == SHOW_FEATURES) {

    QVariant contextObject = _action->data();
    int objectId = contextObject.toInt();

    if ( objectId == -1)
      return;

    BaseObjectData* object = 0;
    if ( ! PluginFunctions::getObject(objectId,object) )
      return;

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        triMeshObject->hideFeatures( triMeshObject->featuresVisible() );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        polyMeshObject->hideFeatures( polyMeshObject->featuresVisible() );
    }

    emit updateView();

  }

  if ( _action->text() == SETSHADERS ) {
    if ( shaderWidget_ == 0 ) {
      shaderWidget_ = new ShaderWidget(0);
      connect(shaderWidget_->availableShaders,SIGNAL(itemClicked(QListWidgetItem*)),
              this                           ,SLOT(slotShaderClicked(QListWidgetItem*)));
      connect(shaderWidget_->availableShaders,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
              this                           ,SLOT(slotShaderDoubleClicked(QListWidgetItem*)));

      connect(shaderWidget_->setShaderButton,SIGNAL(clicked()),this, SLOT(slotSetShader()));
      connect(shaderWidget_->closeButton,SIGNAL(clicked()),shaderWidget_, SLOT(close()));

      connect(shaderWidget_->uniforms, SIGNAL(itemChanged(QTableWidgetItem*)),
              this,                    SLOT(itemChanged(QTableWidgetItem*)) );
    }

    initShaderWidget();

    shaderWidget_->show();

    emit updateView();
  }

}

bool ViewControlPlugin::selectionVisible( int _id ) {
  if ( _id == -1)
      return false;

  BaseObjectData* object = 0;
  if ( ! PluginFunctions::getObject(_id,object) )
    return false;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
    if ( triMeshObject )
      return triMeshObject->selectionVisible();
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
    if ( polyMeshObject )
      return polyMeshObject->selectionVisible();
  }

  return false;
}

bool ViewControlPlugin::areasVisible( StatusBits _bits, int _id ) {

  if ( _id == -1)
    return false;

  BaseObjectData* object = 0;
  if ( ! PluginFunctions::getObject(_id,object) )
    return false;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
    if ( triMeshObject ) {
      return triMeshObject->areaVisible( _bits );
    }
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
    if ( polyMeshObject )
      return( polyMeshObject->areaVisible( _bits ) );
  }

  return false;

}

bool ViewControlPlugin::modelingAreasVisible( int _id ) {
  return areasVisible(StatusBits(HANDLEAREA | AREA), _id);
}

void ViewControlPlugin::showAreas(  StatusBits _bits, int _id , bool _state  ) {

  if ( _id == -1)
      return;

  BaseObjectData* object = 0;
  if ( ! PluginFunctions::getObject(_id,object) )
    return;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
    if ( triMeshObject )
      triMeshObject->hideArea( _bits, !_state);
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
    if ( polyMeshObject )
      polyMeshObject->hideArea( _bits, !_state);
  }

}

void ViewControlPlugin::showModelingAreas( int _id , bool _state  ) {
  showAreas(StatusBits(HANDLEAREA | AREA) , _id, _state );
}


void ViewControlPlugin::showSelection( int _id , bool _state ) {

    if ( _id == -1)
      return;

    BaseObjectData* object = 0;
    if ( ! PluginFunctions::getObject(_id,object) )
      return;

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        triMeshObject->hideSelection( !_state );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        polyMeshObject->hideSelection( !_state );
    }

}

void ViewControlPlugin::slotUpdateContextMenu( int _objectId ){

  viewControlMenu_->clear();

  lastObjectId_ = _objectId;

  BaseObjectData* object = 0;
  if ( !PluginFunctions::getObject( _objectId, object ) ) {
    emit log(LOGERR,"Unable to create Context Menu ... Unable to get Object");
    return;
  }

  if (object->dataType( DATA_TRIANGLE_MESH ) || object->dataType( DATA_POLY_MESH ) )  {

    QAction* act;
    act = viewControlMenu_->addAction( SHOW_SELECTION );
    act->setCheckable(true);

    act->setStatusTip( "Switch visualization for selection on/off" );
    act->setToolTip( "Switch visualization for selection on/off" );
    act->setWhatsThis( "Switch the visualization of your current selections on and off." );

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"selections.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->selectionVisible() );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->selectionVisible() );
    }

    // ============================================
    // Action for all modeling areas
    // ============================================
    act = viewControlMenu_->addAction( SHOW_AREAS );
    act->setCheckable(true);

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"areaSelections.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->areaVisible(  StatusBits(HANDLEAREA | AREA)  ) );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->areaVisible(  StatusBits(HANDLEAREA | AREA)  ) );
    }


    // ============================================
    // Action for handle areas
    // ============================================
    act = viewControlMenu_->addAction( SHOW_HANDLE );
    act->setCheckable(true);

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"handleSelection.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->areaVisible( HANDLEAREA   ) );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->areaVisible( HANDLEAREA  ) );
    }

    // ============================================
    // Action for modeling areas
    // ============================================
    act = viewControlMenu_->addAction( SHOW_AREA );
    act->setCheckable(true);

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"modelingSelection.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->areaVisible( AREA ) );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->areaVisible( AREA ) );
    }

    // ============================================
    // Action for features
    // ============================================


    act = viewControlMenu_->addAction( SHOW_FEATURES );
    act->setCheckable(true);

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"featureSelections.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->featuresVisible() );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->featuresVisible() );
    }


  }

  // If this Object has a shader Node, allow settings for the shader
  if ( object->shaderNode() ) {
    viewControlMenu_->addSeparator();

    QAction* act = viewControlMenu_->addAction( SETSHADERS );
    act->setCheckable( false );
  }

  viewControlMenu_->addSeparator();

  QActionGroup * globalDrawActionsGroup = new QActionGroup( this );

  QAction * action = new QAction( USEGLOBALDRAWMODE , globalDrawActionsGroup );
  action->setCheckable( false );

  viewControlMenu_->addActions(globalDrawActionsGroup->actions());

  connect( globalDrawActionsGroup, SIGNAL( triggered( QAction * ) ),
            this                 , SLOT(   slotDrawModeSelected( QAction * ) ) );

  QActionGroup * drawGroup = new QActionGroup( this );
  drawGroup->setExclusive( false );

  // Collect available draw Modes for this object
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  ACG::SceneGraph::traverse( object->baseNode() , actionAvailable);
  availDrawModes_ = actionAvailable.drawModes();

  // Collect available draw Modes for this object
  ACG::SceneGraph::CollectActiveDrawModesAction actionActive;
  ACG::SceneGraph::traverse( object->baseNode() , actionActive);
  activeDrawModes_ = actionActive.drawMode();

  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds =  availDrawModes_.getAtomicDrawModes();

  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    ACG::SceneGraph::DrawModes::DrawMode id    = availDrawModeIds[i];
    std::string  descr = id.description();

    QCheckBox *checkBox = new QCheckBox(QString(descr.c_str()), viewControlMenu_);
    checkBox->setChecked(activeDrawModes_.containsAtomicDrawMode(id));
    QWidgetAction *checkableAction = new QWidgetAction(drawGroup);
    checkableAction->setText(descr.c_str());
    checkableAction->setDefaultWidget(checkBox);
    connect(checkBox, SIGNAL(toggled(bool) ), checkableAction, SLOT(trigger() ) );
  }

  viewControlMenu_->addActions( drawGroup->actions() );

  connect( drawGroup, SIGNAL( triggered( QAction * ) ),
      this, SLOT( slotDrawModeSelected( QAction * ) ) );

}

void ViewControlPlugin::slotDrawModeSelected( QAction * _action) {

  //======================================================================================
  // Get the mode toggled
  //======================================================================================
  ACG::SceneGraph::DrawModes::DrawMode mode(0);
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
  availDrawModeIds = availDrawModes_.getAtomicDrawModes();
  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    QString descr = QString( availDrawModeIds[i].description().c_str() );

    if ( descr == _action->text() ) {
      mode = availDrawModeIds[i];
      break;
    }
  }

  //======================================================================================
  // possibly combine draw modes
  //======================================================================================
  if ( _action->text() != USEGLOBALDRAWMODE ) {
    // As this is not the global draw mode, filter out default as draw mode or it will interfere with the other modes!
    activeDrawModes_ = ( ( activeDrawModes_ | ACG::SceneGraph::DrawModes::DEFAULT ) ^ ACG::SceneGraph::DrawModes::DEFAULT );
    
    // If shift is pressed, we combine the modes (and toggle therefore xor)
    // Otherwise we directly take the new mode
    if ( qApp->keyboardModifiers() & Qt::ShiftModifier )
      activeDrawModes_ = ( activeDrawModes_ ^ mode);
    else
    {
      emit hideContextMenu();
      activeDrawModes_ = mode ;
    }
    
  } else {
    // Switch back to global drawmode-> default
    activeDrawModes_ = ACG::SceneGraph::DrawModes::DEFAULT;
  }

  //======================================================================================
  // Now do the update in the sceneGraph
  //======================================================================================

  // Get the associated object
  BaseObjectData* object = 0;
  PluginFunctions::getObject( lastObjectId_, object );

  // Set draw Modes for this object ( force it when we do not set the global draw mode, to override global draw mode and force the modes on the nodes )
  ACG::SceneGraph::SetDrawModesAction actionActive( activeDrawModes_ , _action->text() != USEGLOBALDRAWMODE );
  
  
  if ( object )
    ACG::SceneGraph::traverse( object->primaryNode() , actionActive);
  else
    PluginFunctions::setDrawMode( activeDrawModes_ , PluginFunctions::activeExaminer() );

  emit updateView();
}

void ViewControlPlugin::initShaderWidget(){

  //update the shader list

  updateShaderList();

  shaderWidget_->availableShaders->clear();

  for ( uint i = 0 ; i < shaderList_.size(); ++i ) {
    shaderWidget_->availableShaders->addItem( shaderList_[i].name );
  }

  shaderWidget_->availableShaders->setCurrentRow(0);
  slotShaderClicked(shaderWidget_->availableShaders->currentItem());

}

void ViewControlPlugin::slotShaderClicked( QListWidgetItem * _item ){
   int index = -1;
   for ( int i = 0 ; i < (int)shaderList_.size(); ++i) {
      if ( shaderList_[i].name == _item->text() ) {
         index = i;
         break;
      }
   }

   if ( index == -1 ) {
      std::cerr << "Strange index Error! " << std::endl;
      return;
   }

   shaderWidget_->shaderName->setText( shaderList_[index].name );
   shaderWidget_->description->setText( shaderList_[index].description );
   shaderWidget_->details->setText( shaderList_[index].details );
   shaderWidget_->version->setText( shaderList_[index].version );
   shaderWidget_->vertexShader->setText( shaderList_[index].vertexShader );
   shaderWidget_->fragmentShader->setText( shaderList_[index].fragmentShader );
   shaderWidget_->pickVertexShader->setText( shaderList_[index].pickVertexShader );
   shaderWidget_->pickFragmentShader->setText( shaderList_[index].pickFragmentShader );
   shaderWidget_->example->setPixmap( QPixmap(shaderList_[index].example) );

   //get shader paths
   QStringList shadersDirs = OpenFlipper::Options::shaderDir().entryList( QDir::Dirs| QDir::NoDotAndDotDot ,QDir::Name);
   QString shaderDir = OpenFlipper::Options::shaderDirStr() + OpenFlipper::Options::dirSeparator();

   QString vertexFile = shaderDir + shadersDirs[ shaderWidget_->availableShaders->currentRow() ] +
                        OpenFlipper::Options::dirSeparator() + shaderWidget_->vertexShader->text();
   QString fragmentFile = shaderDir + shadersDirs[ shaderWidget_->availableShaders->currentRow() ] +
                           OpenFlipper::Options::dirSeparator() + shaderWidget_->fragmentShader->text();

   //update drawMode checkStates
   shaderWidget_->drawModes->clear();

   std::vector< ACG::SceneGraph::DrawModes::DrawMode > availDrawModeIds;
   availDrawModeIds = availDrawModes_.getAtomicDrawModes( );

   for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
   {
     ACG::SceneGraph::DrawModes::DrawMode id    = availDrawModeIds[i];


      std::vector< QString > dm = drawModeToDescriptions( id );

      if ( !dm.empty() && dm[0].trimmed() != ""){
         QListWidgetItem* item = new QListWidgetItem(dm[0]);

         item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

         BaseObjectData* object = 0;

         PluginFunctions::getObject( lastObjectId_, object );
         if ( object && object->shaderNode() ){
	    if ( object->shaderNode()->vertexShaderName( id ) == vertexFile.toStdString() &&
		object->shaderNode()->fragmentShaderName( id ) == fragmentFile.toStdString() )
		item->setCheckState( Qt::Checked );
	    else
	    {
		//std::cerr << "vertex:" << object->shaderNode()->vertexShaderName( id ) << ", this: " <<  vertexFile.toStdString() << std::endl;
		//std::cerr << "fragment:" << object->shaderNode()->fragmentShaderName( id ) << ", this: " <<  fragmentFile.toStdString() << std::endl;
		item->setCheckState( Qt::Unchecked );
	    }
         }
         shaderWidget_->drawModes->addItem(item);
      }
   }

   if ( shaderList_[index].hasUniforms ) {
    //set up uniform table
    shaderWidget_->uniforms->clear();
    shaderWidget_->uniforms->setColumnCount ( 1 );
    shaderWidget_->uniforms->setRowCount ( shaderList_[index].uniforms.count() );

    shaderWidget_->uniforms->setHorizontalHeaderLabels( QStringList("Value") );
    shaderWidget_->uniforms->setVerticalHeaderLabels( shaderList_[index].uniforms );

    for (int i=0; i < shaderList_[index].uniforms.count(); i++){
      QTableWidgetItem* item = new QTableWidgetItem( shaderList_[index].uniformsDefault[i] );
      item->setFlags( Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      shaderWidget_->uniforms->setItem(i,0,item );
    }

    shaderWidget_->uniformBox->setVisible( true );

   }else
    shaderWidget_->uniformBox->setVisible( false );

   emit updateView();
}

void ViewControlPlugin::slotShaderDoubleClicked( QListWidgetItem * /*_item*/ ){
//   slotSetShader();
}

void ViewControlPlugin::itemChanged(QTableWidgetItem* item){

  //get current shader index
   int index = -1;
   for ( int i = 0 ; i < (int)shaderList_.size(); ++i) {
      if ( shaderList_[i].name == shaderWidget_->availableShaders->currentItem()->text() ) {
         index = i;
         break;
      }
   }

   if ( index == -1 ) {
      std::cerr << "Error: Shader Index not found! " << std::endl;
      return;
   }


  int row = item->row();

  if ( row > shaderList_[index].uniforms.count() ){
    std::cerr << "Error: uniform index out of bounds" << std::endl;
    return;
  }

  //handle different datatypes
  if (shaderList_[index].uniformTypes[row] == "float"){
    bool ok;

    float value = item->text().toFloat(&ok);

    if (!ok)
      item->setText( shaderList_[index].uniformsDefault[row] );
    else if (shaderList_[index].uniformsDefault[row] != shaderList_[index].uniformsMin[row]
            && value < shaderList_[index].uniformsMin[row].toFloat())
      item->setText( shaderList_[index].uniformsMin[row] );
    else if (shaderList_[index].uniformsDefault[row] != shaderList_[index].uniformsMin[row]
            && value > shaderList_[index].uniformsMax[row].toFloat())
      item->setText( shaderList_[index].uniformsMax[row] );
  }

}

void ViewControlPlugin::slotSetShader()
{

  //get current shader index
  int index = -1;
  for (int i = 0; i < (int) shaderList_.size(); ++i) {
    if (shaderList_[i].name == shaderWidget_->availableShaders->currentItem()->text()) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    std::cerr << "Error: Shader Index not found! " << std::endl;
    return;
  }

  std::vector<QString> mode;
  for (int i = 0; i < shaderWidget_->drawModes->count(); i++) {
    if (shaderWidget_->drawModes->item(i)->checkState() == Qt::Checked) {
      mode.push_back(shaderWidget_->drawModes->item(i)->text());
      setShader(lastObjectId_, descriptionsToDrawMode(mode), shaderList_[index]);
      mode.clear();
    } else {
      mode.push_back(shaderWidget_->drawModes->item(i)->text());
      disableShader(lastObjectId_, descriptionsToDrawMode(mode), &shaderList_[index]);
      mode.clear();
    }
  }
}

void ViewControlPlugin::disableShader(
    int _objectId,
    ACG::SceneGraph::DrawModes::DrawMode _drawMode,
    ShaderInfo* _shader)
{

  BaseObjectData* object = 0;
  PluginFunctions::getObject(_objectId, object);

  if (object) {
    if (!_shader)
      object->shaderNode()->disableShader(_drawMode);
    else {
      std::string shadeDir = _shader->directory.toStdString() + OpenFlipper::Options::dirSeparator().toStdString();

      if (object->shaderNode()->vertexShaderName(_drawMode) == shadeDir + _shader->vertexShader.toStdString()
          && object->shaderNode()->fragmentShaderName(_drawMode) == shadeDir + _shader->fragmentShader.toStdString())
        object->shaderNode()->disableShader(_drawMode);
    }
  }

  emit updateView();
}

void ViewControlPlugin::setShader(int _id, ACG::SceneGraph::DrawModes::DrawMode _drawMode, ShaderInfo _shader) {

    if ( OpenFlipper::Options::nogui() )
        return;

    BaseObjectData* object = 0;
    PluginFunctions::getObject( _id, object );
    
   //get shader paths
   QFileInfo vertexFile (_shader.name + OpenFlipper::Options::dirSeparator() + _shader.vertexShader);
   QFileInfo fragmentFile (_shader.name + OpenFlipper::Options::dirSeparator() + _shader.fragmentShader);
   QFileInfo pickVertexFile (_shader.name + OpenFlipper::Options::dirSeparator() + _shader.pickVertexShader);
   QFileInfo pickFragmentFile (_shader.name + OpenFlipper::Options::dirSeparator() + _shader.pickFragmentShader);      

    if ( object && object->shaderNode()
	&& _shader.vertexShader.length() && _shader.fragmentShader.length()) {

        if (vertexFile.absolutePath() != fragmentFile.absolutePath()) {
            emit log(LOGERR, "Cannot set shader. Currently shader files have to be in the same folder.");
            return;
        }

        if (_shader.pickVertexShader.length() && _shader.pickFragmentShader.length())
        {
            if (vertexFile.absolutePath() != pickVertexFile.absolutePath() ||
                vertexFile.absolutePath() != pickFragmentFile.absolutePath()) {
                emit log(LOGERR, "Cannot set picking shader. Currently shader files have to be in the same folder.");
                return;
            }
            
	    object->shaderNode()->setShaderDir( (_shader.directory + OpenFlipper::Options::dirSeparator()).toStdString() );

            object->shaderNode()->setShader(_drawMode,
					    vertexFile.fileName().toStdString(),
                                            fragmentFile.fileName().toStdString(),
                                            pickVertexFile.fileName().toStdString(),
                                            pickFragmentFile.fileName().toStdString());
        }
        else
        {
	    object->shaderNode()->setShaderDir( (_shader.directory + OpenFlipper::Options::dirSeparator()).toStdString() );
            object->shaderNode()->setShader(_drawMode, vertexFile.fileName().toStdString(), fragmentFile.fileName().toStdString());
        }
    }


    // set uniforms if available
    if (_shader.hasUniforms) {
        GLSL::PtrProgram shader = object->shaderNode()->getShader( _drawMode );
        if ( shader == 0 ) {
            std::cerr << "Error: Unable to get shader for shader mode" << std::endl;
        } else {
            shader->use();

            for (int u=0; u < _shader.uniforms.count(); u++) {
                //float uniforms
                if (_shader.uniformTypes[u] == "float" ) {
                    shaderWidget_->uniforms->setCurrentCell(u,0);
                    float value = shaderWidget_->uniforms->currentItem()->text().toFloat();
                    shader->setUniform(_shader.uniforms[u].toUtf8(), value);
                } else
                    //vec3 uniforms
                    if (_shader.uniformTypes[u] == "vec3" ) {
                        shaderWidget_->uniforms->setCurrentCell(u,0);
                        QStringList vecStr = shaderWidget_->uniforms->currentItem()->text().split(",");
                        if (vecStr.count() == 3) {
                            ACG::Vec3f value( vecStr[0].toFloat(), vecStr[1].toFloat(), vecStr[2].toFloat() );
                            shader->setUniform(_shader.uniforms[u].toUtf8(), value);
                        }
                    } else
                        std::cerr << "Error: handling of uniformType '" << _shader.uniformTypes[u].toStdString() << "' not yet implemented" << std::endl;

            }

            shader->disable();
        }
    }

    emit updateView();
}

void ViewControlPlugin::setShader(int _id, QString _drawMode, QString _name ){

  if ( OpenFlipper::Options::nogui() )
    return;

  updateShaderList();

  //get current shader index
   int index = -1;
   for ( int i = 0 ; i < (int)shaderList_.size(); ++i) {
      if ( shaderList_[i].name == _name ) {
         index = i;
         break;
      }
   }

   if ( index == -1 ) {
      std::cerr << "Error: Shader not found! " << std::endl;
      return;
   }
   
   std::vector <QString> mode;
   mode.push_back(_drawMode);
   
   setShader(_id, listToDrawMode(mode), shaderList_[index]);
}

//-----------------------------------------------------------------------------

QStringList ViewControlPlugin::getUniforms(QString _shader){
  for (uint i=0; i < shaderList_.size(); i++)
    if ( shaderList_[i].name == _shader)
      return shaderList_[i].uniforms;

  return QStringList();
}

QString ViewControlPlugin::getUniformType(QString _shader, QString _uniform ){
  
  for (uint i=0; i < shaderList_.size(); i++)
    if ( shaderList_[i].name == _shader){

      for (int u=0; u < shaderList_[i].uniforms.count(); u++)
        if ( shaderList_[i].uniforms[u] == _uniform )
          return shaderList_[i].uniformTypes[u];
    }

  return QString();
}

QString ViewControlPlugin::getUniformDefault(QString _shader, QString _uniform ){
  
  for (uint i=0; i < shaderList_.size(); i++)
    if ( shaderList_[i].name == _shader){

      for (int u=0; u < shaderList_[i].uniforms.count(); u++)
        if ( shaderList_[i].uniforms[u] == _uniform )
          return shaderList_[i].uniformsDefault[u];
    }

  return QString();
}

QString ViewControlPlugin::getUniformMin(QString _shader, QString _uniform ){
  
  for (uint i=0; i < shaderList_.size(); i++)
    if ( shaderList_[i].name == _shader){

      for (int u=0; u < shaderList_[i].uniforms.count(); u++)
        if ( shaderList_[i].uniforms[u] == _uniform )
          return shaderList_[i].uniformsMin[u];
    }

  return QString();
}

QString ViewControlPlugin::getUniformMax(QString _shader, QString _uniform ){
  
  for (uint i=0; i < shaderList_.size(); i++)
    if ( shaderList_[i].name == _shader){

      for (int u=0; u < shaderList_[i].uniforms.count(); u++)
        if ( shaderList_[i].uniforms[u] == _uniform )
          return shaderList_[i].uniformsMax[u];
    }

  return QString();
}

//-----------------------------------------------------------------------------

/// set the value of a uniform in a shader for a specific drawMode
void ViewControlPlugin::setUniform(int _objID, ACG::SceneGraph::DrawModes::DrawMode _drawMode, QString _shader, QString _uniform, QString _value ){

      BaseObjectData* object = 0;

      PluginFunctions::getObject( _objID, object );
      if ( object && object->shaderNode() ){

        for (uint i=0; i < shaderList_.size(); i++)
          if ( shaderList_[i].name == _shader){

            // set uniforms if available
            if (shaderList_[i].hasUniforms){
              GLSL::PtrProgram shader = object->shaderNode()->getShader( _drawMode );
              if ( shader == 0 ) {
                std::cerr << "Error: Unable to get shader for shader mode" << std::endl;
              } else {
                shader->use();

                for (int u=0; u < shaderList_[i].uniforms.count(); u++){

                  //only apply to uniform which was given as param
                  if ( shaderList_[i].uniforms[u] != _uniform )
                    continue;

                  //float uniforms
                  if (shaderList_[i].uniformTypes[u] == "float" )
                    shader->setUniform(shaderList_[i].uniforms[u].toUtf8(), _value.toFloat() );

                  //vec3 uniforms
                  else if (shaderList_[i].uniformTypes[u] == "vec3" ){
                    QStringList vecStr = _value.split(",");
                    if (vecStr.count() == 3){
                      ACG::Vec3f value( vecStr[0].toFloat(), vecStr[1].toFloat(), vecStr[2].toFloat() );
                      shader->setUniform(shaderList_[i].uniforms[u].toUtf8(), value);
                    }
                  }else
                    std::cerr << "Error: handling of uniformType '" << shaderList_[i].uniformTypes[u].toStdString() << "' not yet implemented" << std::endl;
                }

                shader->disable();
              }
            }
          }
      }
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::setViewingDirection( Vector _direction, Vector _upvector , int _viewer){
  PluginFunctions::viewingDirection(_direction, _upvector, _viewer);
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::rotate( Vector _axis, double _angle, Vector _center , int _viewer) {
  PluginFunctions::rotate( _axis, _angle, _center ,_viewer);
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::translate( Vector _vec , int _viewer) {
  PluginFunctions::translate( _vec ,_viewer);
}


void ViewControlPlugin::enableBackfaceCulling( bool _state , int _viewer  ) {
  if ( _viewer == PluginFunctions::ALL_VIEWERS )
    for ( int i = 0 ; i < PluginFunctions::viewers( ); ++i )
      PluginFunctions::viewerProperties(i).backFaceCulling(_state);
  else if ( _viewer == PluginFunctions::ACTIVE_VIEWER )
    PluginFunctions::viewerProperties(PluginFunctions::activeExaminer()).backFaceCulling(_state);
  else if ( _viewer >= 0 && _viewer < PluginFunctions::viewers( ) )
    PluginFunctions::viewerProperties(_viewer).backFaceCulling(_state);
  else
    std::cerr << "Illegal viewer requested! " << std::endl;

}


//-----------------------------------------------------------------------------

void
ViewControlPlugin::setDrawMode(QString _mode, int _viewer)
{

  QStringList list = _mode.split(';');

  std::vector< QString > drawModeList;

  for ( int i = 0 ; i < list.size() ; ++i )
    drawModeList.push_back(list[i]);

  ACG::SceneGraph::DrawModes::DrawMode mode = listToDrawMode(drawModeList);

  PluginFunctions::setDrawMode( mode , _viewer );
  emit updateView();
}

//-----------------------------------------------------------------------------

void 
ViewControlPlugin::setObjectDrawMode(QString _mode, int _objectID, bool _force)
{
  BaseObjectData* object = 0;
  PluginFunctions::getObject( _objectID, object );
 
  QStringList list = _mode.split(';');

  std::vector< QString > drawModeList;

  for ( int i = 0 ; i < list.size() ; ++i )
    drawModeList.push_back(list[i]);

  ACG::SceneGraph::DrawModes::DrawMode mode = listToDrawMode(drawModeList);
 
  // Set draw Modes for this object ( force it when we do not set the global draw mode, to override global draw mode and force the modes on the nodes )
  ACG::SceneGraph::SetDrawModesAction actionActive( mode , _force );

  if ( object )
    ACG::SceneGraph::traverse( object->primaryNode() , actionActive);
}

//-----------------------------------------------------------------------------

Vector ViewControlPlugin::viewingDirection( int _viewer ) {
  return PluginFunctions::viewingDirection(_viewer);
}

Vector ViewControlPlugin::upVector( int _viewer ) {
  return PluginFunctions::upVector(_viewer);
}

Vector ViewControlPlugin::eyePosition( int _viewer ) {
  return PluginFunctions::eyePos(_viewer);
}

void ViewControlPlugin::setSceneCenter( Vector _center, int _viewer ) {
  PluginFunctions::setScenePos(_center, _viewer);
}

Vector ViewControlPlugin::sceneCenter( int _viewer ) {
  return PluginFunctions::sceneCenter(_viewer);
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::setView(int _mode, int _viewer ) {

  switch ( _mode ){
      case PluginFunctions::VIEW_TOP : //TOP
      setViewingDirection( ACG::Vec3d(0.0, -1.0, 0.0), ACG::Vec3d(0.0, 0.0, -1.0), _viewer );
      break;
    case PluginFunctions::VIEW_BOTTOM : //BOTTOM
      setViewingDirection( ACG::Vec3d(0.0, 1.0, 0.0), ACG::Vec3d(0.0, 0.0, -1.0), _viewer );
      break;
    case PluginFunctions::VIEW_LEFT : //LEFT
      setViewingDirection( ACG::Vec3d(1.0, 0.0, 0.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      break;
    case PluginFunctions::VIEW_RIGHT : //RIGHT
      setViewingDirection( ACG::Vec3d(-1.0, 0.0, 0.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      break;
    case PluginFunctions::VIEW_FRONT : //FRONT
      setViewingDirection( ACG::Vec3d(0.0, 0.0, -1.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      break;
    case PluginFunctions::VIEW_BACK : //BACK
      setViewingDirection( ACG::Vec3d(0.0, 0.0, 1.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      break;
    default : 
      emit log(LOGERR, "ViewControl: Unknown view mode: " + QString::number(_mode));
      break;
  }
  
  updateView();
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::setView(QAction* _action) {

  if ( _action == viewTop_) 		setView (PluginFunctions::VIEW_TOP, 	PluginFunctions::ACTIVE_VIEWER);
  else if ( _action == viewBottom_) 	setView (PluginFunctions::VIEW_BOTTOM, 	PluginFunctions::ACTIVE_VIEWER);
  else if ( _action == viewLeft_) 	setView (PluginFunctions::VIEW_LEFT, 	PluginFunctions::ACTIVE_VIEWER);
  else if ( _action == viewRight_) 	setView (PluginFunctions::VIEW_RIGHT, 	PluginFunctions::ACTIVE_VIEWER);
  else if ( _action == viewFront_) 	setView (PluginFunctions::VIEW_FRONT, 	PluginFunctions::ACTIVE_VIEWER);
  else if ( _action == viewBack_) 	setView (PluginFunctions::VIEW_BACK, 	PluginFunctions::ACTIVE_VIEWER);
  else emit log(LOGERR, "ViewControl: Unknown view mode action!");
}

//-----------------------------------------------------------------------------

void ViewControlPlugin::setEyePosition(Vector /*_eye*/){
  emit log(LOGERR, "not yet implemented");
}


void ViewControlPlugin::viewAll() {
  PluginFunctions::viewAll();
}

void ViewControlPlugin::viewAll(int _viewer) {
  PluginFunctions::viewAll(_viewer);
}

void ViewControlPlugin::viewHome() {
  PluginFunctions::viewHome();
}

void ViewControlPlugin::viewHome(int _viewer) {
  PluginFunctions::viewHome(_viewer);
}

void ViewControlPlugin::orthographicProjection() {
  PluginFunctions::orthographicProjection();
}

void ViewControlPlugin::orthographicProjection(int _viewer) {
  PluginFunctions::orthographicProjection( _viewer );
}


void ViewControlPlugin::perspectiveProjection() {
  PluginFunctions::perspectiveProjection();
}

void ViewControlPlugin::perspectiveProjection(int _viewer) {
  PluginFunctions::perspectiveProjection(_viewer);
}

void ViewControlPlugin::setFOVY( double _fovy ) {
  PluginFunctions::setFOVY(_fovy);
  
  emit updateView();
}

void ViewControlPlugin::setCoordsysProjection(bool _orthogonal) {
  
  // Find coordsys node
  ACG::SceneGraph::BaseNode* node = 0;
  node = PluginFunctions::getSceneGraphRootNode()->find("Core Coordsys Node");
  if (node != 0) {
    ACG::SceneGraph::CoordsysNode* cnode = dynamic_cast<ACG::SceneGraph::CoordsysNode*> (node);
    if (cnode->getProjectionMode() == ACG::SceneGraph::CoordsysNode::PERSPECTIVE_PROJECTION) {
      if ( _orthogonal) {
        cnode->setProjectionMode(ACG::SceneGraph::CoordsysNode::ORTHOGRAPHIC_PROJECTION);
        emit updateView();
      }
    } else {
      if ( !_orthogonal) {
        cnode->setProjectionMode(ACG::SceneGraph::CoordsysNode::PERSPECTIVE_PROJECTION);
        emit updateView();
      }
    }
    
  } else {
    emit log(LOGERR,tr("setCoordsysProjection(): Could not find coordsys node, thus its projection mode will not be toggled."));
  }
  
}

void ViewControlPlugin::setDescriptions() {
  emit setSlotDescription("translate(Vector,int)", "Translate Scene",
                          QString("TranslationVector,Viewer").split(","),
                          QString("vector for the translation.,Viewer id (default is all)").split(","));
  emit setSlotDescription("translate(Vector)", "Translate Scene in all Viewers",
                          QString("TranslationVector").split(","),
                          QString("vector for the translation.").split(","));
  emit setSlotDescription("rotate(Vector,double,Vector,int)", "Rotate Scene",
                          QString("Axis,Angle,Center,Viewer").split(","),
                          QString("Rotation axis., Rotation Angle., Rotation Center.").split(","));
  emit setSlotDescription("rotate(Vector,double,Vector)", "Rotate Scene in all viewers",
                          QString("Axis,Angle,Center").split(","),
                          QString("Rotation axis., Rotation Angle., Rotation Center.").split(","));
  emit setSlotDescription("setViewingDirection(Vector,Vector,int)", "Set the viewing direction",
                          QString("direction,upVector,Viewer").split(","),
                          QString("Viewing direction., Up-Vector.,Viewer id (default is all)").split(","));
  emit setSlotDescription("setViewingDirection(Vector,Vector)", "Set the viewing direction in all viewers",
                          QString("direction,upVector").split(","),
                          QString("Viewing direction., Up-Vector.").split(","));
  emit setSlotDescription("setDrawMode(QString,int)", "Set the drawMode",
                          QString("DrawMode,Viewer").split(","),
                          QString("the drawMode ( ; separated list ),Viewer id (default is all)").split(","));
  emit setSlotDescription("setDrawMode(QString)", "Set the drawMode for all viewers",
                          
                          QStringList("DrawMode"), QStringList("the drawMode ( ; separated list )"));
                          
  emit setSlotDescription("setObjectDrawMode(QString,int,bool)", "Set the drawMode for an object",
                          QString("DrawMode,ObjectID,Force").split(","),
                          QString("the drawMode ( ; separated list ),Object id,Apply without checking support(default is true)").split(","));                        

  emit setSlotDescription("viewAll()", "Change View on all viewers to view whole scene",
                          QStringList(), QStringList());
  emit setSlotDescription("viewAll(int)", "Change View on given viewer to view whole scene",
                          QStringList("Viewer"), QStringList("Id of the viewer to change"));

  emit setSlotDescription("viewHome()", "Change View on all viewers to view home position",
                          QStringList(), QStringList());
  emit setSlotDescription("viewHome(int)", "Change View on given viewer to view home position",
                          QStringList("Viewer"), QStringList("Id of the viewer to change"));


  emit setSlotDescription("orthographicProjection()", "Change Viewer to orthographic projection",
                          QStringList(), QStringList());
  emit setSlotDescription("orthographicProjection(int)", "Change all Viewers to orthographic projection",
                          QStringList("Viewer"), QStringList("Id of the viewer to change"));


  emit setSlotDescription("perspectiveProjection()", "Change Viewer to perspective projection",
                          QStringList(), QStringList());
  emit setSlotDescription("perspectiveProjection(int)", "Change all Viewers to perspective projection",
                          QStringList("Viewer"), QStringList("Id of the viewer to change"));
                          
  emit setSlotDescription("setFOVY(double)", "Set fovy angle of projection for all viewers.",
                          QStringList("fovy"), QStringList("FOVY angle"));
                          
  emit setSlotDescription("setCoordsysProjection(bool)", "Set the projection mode of the coordinate system.",
                          QStringList("orthogonal"), QStringList("If true, orthogonal projection otherwise perspective projection"));

  emit setSlotDescription("upVector()", "Get the current upVector.",
                          QStringList(), QStringList());

  emit setSlotDescription("upVector(int)", "Get the current upVector of a specific viewer.",
                          QStringList("ViewerId"), QStringList("Id of the viewer"));

  emit setSlotDescription("eyePosition()", "Get the current eyePosition.",
                          QStringList(), QStringList());

  emit setSlotDescription("eyePosition(int)", "Get the current eyePosition of a specific viewer.",
                            QStringList("ViewerId"), QStringList("Id of the viewer"));

  emit setSlotDescription("sceneCenter()", "Get the current sceneCenter.",
                            QStringList(), QStringList());

  emit setSlotDescription("sceneCenter(int)", "Get the current sceneCenter of a specific viewer.",
                            QStringList("ViewerId"), QStringList("Id of the viewer"));

  emit setSlotDescription("viewingDirection()", "Get the current viewingDirection.",
                            QStringList(), QStringList());

  emit setSlotDescription("viewingDirection(int)", "Get the current viewingDirection of a specific viewer.",
                            QStringList("ViewerId"), QStringList("Id of the viewer"));
                          
}


Q_EXPORT_PLUGIN2( viewcontrolplugin , ViewControlPlugin );


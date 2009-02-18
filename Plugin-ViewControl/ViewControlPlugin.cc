
#include <QtGui>
#include "ViewControlPlugin.hh"
#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/Geometry/Algorithms.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#define SHOW_SELECTION "Selections"
#define SHOW_AREAS "Modeling Areas"
#define SHOW_FEATURES "Feature Selection"
#define USEGLOBALDRAWMODE "Use Global DrawMode"
#define SETSHADERS "Set Shader"

void ViewControlPlugin::pluginsInitialized() {

  shaderWidget_ = 0;

  // Disable Context Menu for draw Modes
  OpenFlipper::Options::drawModesInContextMenu(false);

  viewControlMenu_ = new QMenu("Visualization");

  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"drawModes.png");
  viewControlMenu_->setIcon(icon);

  emit addContextMenu(viewControlMenu_ , DATA_TRIANGLE_MESH , CONTEXTTOPLEVELMENU );
  emit addContextMenu(viewControlMenu_ , DATA_POLY_MESH     , CONTEXTTOPLEVELMENU );

  // Show the menu if the user did not click on an object
  emit addContextMenu(viewControlMenu_ , DATA_NONE          , CONTEXTTOPLEVELMENU );

  connect( viewControlMenu_,  SIGNAL( triggered(QAction*) ), this, SLOT( contextMenuTriggered(QAction*) ));

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

bool ViewControlPlugin::modelingAreasVisible( int _id ) {

  if ( _id == -1)
      return false;

  BaseObjectData* object = 0;
  if ( ! PluginFunctions::getObject(_id,object) )
    return false;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
    if ( triMeshObject )
      return triMeshObject->areasVisible();
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
    if ( polyMeshObject )
      return( polyMeshObject->areasVisible() );
  }

  return false;

}

void ViewControlPlugin::showModelingAreas( int _id , bool _state  ) {

  if ( _id == -1)
      return;

  BaseObjectData* object = 0;
  if ( ! PluginFunctions::getObject(_id,object) )
    return;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
    if ( triMeshObject )
      triMeshObject->hideAreas( !_state);
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
    if ( polyMeshObject )
      polyMeshObject->hideAreas( !_state);
  }

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
  PluginFunctions::getObject( _objectId, object );

  if ( object && (object->dataType( DATA_TRIANGLE_MESH ) || object->dataType( DATA_POLY_MESH ) ) ) {

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

    act = viewControlMenu_->addAction( SHOW_AREAS );
    act->setCheckable(true);

    act->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"areaSelections.png"));

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMeshObject* triMeshObject = PluginFunctions::triMeshObject( object );
      if ( triMeshObject )
        act->setChecked( triMeshObject->areasVisible() );
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMeshObject* polyMeshObject = PluginFunctions::polyMeshObject( object );
      if ( polyMeshObject )
        act->setChecked( polyMeshObject->areasVisible() );
    }

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
  if ( object && object->shaderNode() ) {
    viewControlMenu_->addSeparator();

    QAction* act = viewControlMenu_->addAction( SETSHADERS );
    act->setCheckable( false );
  }

  viewControlMenu_->addSeparator();

  if ( object ) {
    QActionGroup * globalDrawActionsGroup = new QActionGroup( this );

    QAction * action = new QAction( USEGLOBALDRAWMODE , globalDrawActionsGroup );
    action->setCheckable( false );

    viewControlMenu_->addActions(globalDrawActionsGroup->actions());

    connect( globalDrawActionsGroup, SIGNAL( triggered( QAction * ) ),
              this                 , SLOT(   slotDrawModeSelected( QAction * ) ) );
  }

  QActionGroup * drawGroup = new QActionGroup( this );
  drawGroup->setExclusive( false );

  // Collect available draw Modes for this object
  ACG::SceneGraph::CollectDrawModesAction actionAvailable;
  if ( object )
    ACG::SceneGraph::traverse( object->baseNode() , actionAvailable);
  else
    ACG::SceneGraph::traverse( PluginFunctions::getRootNode() , actionAvailable);
  availDrawModes_ = actionAvailable.drawModes();

  // Collect available draw Modes for this object
  if ( object ) {
    ACG::SceneGraph::CollectActiveDrawModesAction actionActive;
    ACG::SceneGraph::traverse( object->baseNode() , actionActive);
    activeDrawModes_ = actionActive.drawMode();
  } else {
    activeDrawModes_ = PluginFunctions::drawMode();
  }

  std::vector< unsigned int > availDrawModeIds;
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availDrawModes_ );

  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    unsigned int id    = availDrawModeIds[i];
    std::string  descr = ACG::SceneGraph::DrawModes::description( id );

    QAction * action = new QAction( descr.c_str(), drawGroup );
    action->setCheckable( true );
    action->setChecked( ACG::SceneGraph::DrawModes::containsId( activeDrawModes_, id ) );
    drawMenuActions_.push_back( action );
  }

  viewControlMenu_->addActions( drawGroup->actions() );

  connect( drawGroup, SIGNAL( triggered( QAction * ) ),
      this, SLOT( slotDrawModeSelected( QAction * ) ) );

}

void ViewControlPlugin::slotDrawModeSelected( QAction * _action) {

  //======================================================================================
  // Get the mode toggled
  //======================================================================================
  unsigned int mode = 0;
  std::vector< unsigned int > availDrawModeIds;
  availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availDrawModes_ );
  for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
  {
    QString descr = QString( ACG::SceneGraph::DrawModes::description( availDrawModeIds[i] ).c_str() );

    if ( descr == _action->text() ) {
      mode = availDrawModeIds[i];
      break;
    }
  }

  //======================================================================================
  // possibly combine draw modes
  //======================================================================================
  if ( _action->text() != USEGLOBALDRAWMODE ) {
    if ( qApp->keyboardModifiers() & Qt::ShiftModifier )
      activeDrawModes_ = ( activeDrawModes_ ^ mode);
    else
      activeDrawModes_ = mode ;
  } else
    activeDrawModes_ = ACG::SceneGraph::DrawModes::DEFAULT;

  //======================================================================================
  // Now do the update in the sceneGraph
  //======================================================================================

  BaseObjectData* object = 0;
  PluginFunctions::getObject( lastObjectId_, object );

  // Set draw Modes for this object
  ACG::SceneGraph::SetDrawModesAction actionActive( activeDrawModes_ );
  if ( object )
    ACG::SceneGraph::traverse( object->baseNode() , actionActive);
  else
    PluginFunctions::setDrawMode( activeDrawModes_ );

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

   shaderWidget_->name->setText( shaderList_[index].name );
   shaderWidget_->description->setText( shaderList_[index].description );
   shaderWidget_->details->setText( shaderList_[index].details );
   shaderWidget_->version->setText( shaderList_[index].version );
   shaderWidget_->vertexShader->setText( shaderList_[index].vertexShader );
   shaderWidget_->fragmentShader->setText( shaderList_[index].fragmentShader );
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

   std::vector< unsigned int > availDrawModeIds;
   availDrawModeIds = ACG::SceneGraph::DrawModes::getDrawModeIDs( availDrawModes_ );

   for ( unsigned int i = 0; i < availDrawModeIds.size(); ++i )
   {
      unsigned int id    = availDrawModeIds[i];


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
            item->setCheckState( Qt::Unchecked );
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

void ViewControlPlugin::slotSetShader(){

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

   //get shader paths
   QStringList shadersDirs = OpenFlipper::Options::shaderDir().entryList( QDir::Dirs| QDir::NoDotAndDotDot ,QDir::Name);
   QString shaderDir = OpenFlipper::Options::shaderDirStr() + OpenFlipper::Options::dirSeparator();

   QString vertexFile = shadersDirs[ shaderWidget_->availableShaders->currentRow() ] +
                        OpenFlipper::Options::dirSeparator() + shaderWidget_->vertexShader->text();
   QString fragmentFile = shadersDirs[ shaderWidget_->availableShaders->currentRow() ] +
                           OpenFlipper::Options::dirSeparator() + shaderWidget_->fragmentShader->text();
   //get drawmodes
   std::vector< QString > mode;

   for (int i=0; i < shaderWidget_->drawModes->count(); i++)
      if (shaderWidget_->drawModes->item(i)->checkState() == Qt::Checked){
         mode.push_back( shaderWidget_->drawModes->item(i)->text() );
      }

   //set the shader
   if ( QFile( shaderDir + vertexFile ).exists() && QFile( shaderDir + fragmentFile ).exists() ) {
      BaseObjectData* object = 0;

      PluginFunctions::getObject( lastObjectId_, object );
      if ( object && object->shaderNode() ){
         object->shaderNode()->setShader(descriptionsToDrawMode(mode), vertexFile.toStdString() , fragmentFile.toStdString());

        // set uniforms if available
        if (shaderList_[index].hasUniforms){
          GLSL::PtrProgram shader = object->shaderNode()->getShader( descriptionsToDrawMode(mode) );
          if ( shader == 0 ) {
            std::cerr << "Error: Unable to get shader for shader mode" << std::endl;
          } else {
            shader->use();

            for (int u=0; u < shaderList_[index].uniforms.count(); u++){
              //float uniforms
              if (shaderList_[index].uniformTypes[u] == "float" ){
                shaderWidget_->uniforms->setCurrentCell(u,0);
                float value = shaderWidget_->uniforms->currentItem()->text().toFloat();
                shader->setUniform(shaderList_[index].uniforms[u].toStdString().c_str(), value);
              }
            }

            shader->disable();
          }
        }


      }
   }


   emit updateView();
}

void ViewControlPlugin::setShader(int _id, QString _drawMode, QString _vertexShader, QString _fragmentShader ){
  std::cerr << "Todo : Scripting to set Shader" << std::endl;
//   if ( OpenFlipper::Options::nogui() )
//     return;
//
//   BaseObjectData* object = 0;
//   PluginFunctions::getObject( _id, object );
//
//   if ( object && object->shaderNode() ){
//     object->shaderNode()->setShader( descriptionsToDrawMode(_drawMode), _vertexShader.toStdString() , _fragmentShader.toStdString());
//   }

}

void ViewControlPlugin::setShader(int _id, QString _drawMode, QString _name ){

  if ( OpenFlipper::Options::nogui() )
    return;

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

  BaseObjectData* object = 0;
  PluginFunctions::getObject( _id, object );

  if ( object && object->shaderNode() ){
    std::cerr << "TODO : Get right shader from list, set uniforms as stored in the list" << std::endl;
  }

}


Q_EXPORT_PLUGIN2( viewcontrolplugin , ViewControlPlugin );


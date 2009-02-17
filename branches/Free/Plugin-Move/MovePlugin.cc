
#include <QtGui>

#include "MovePlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/QtScenegraph/QtTranslationManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <MeshTools/MeshFunctions.hh>
#include <MeshTools/MeshInfoT.hh>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef ENABLE_POLYLINE_SUPPORT
#include <ObjectTypes/PolyLine/PolyLine.hh>
#endif



/** \brief Default Constructor
 * 
 */
MovePlugin::MovePlugin()
{
    manip_size_          = 1.0;
    manip_size_modifier_ = 1.0;

    selectionType_ = VERTEX;
    selectionConnected_ = false;

    axisA_ = 0;
    axisB_ = 1;
}


/*******************************************************************************
        BaseInterface implementation
 *******************************************************************************/

/** \brief Initialization of the plugin when it is loaded by the core
 * 
 */
void MovePlugin::pluginsInitialized() {

  //PICKMODES
  emit addPickMode("Separator");
  emit addHiddenPickMode("Move");
  emit setPickModeMouseTracking ("Move", true);
  emit addHiddenPickMode("MoveSelection");
  emit setPickModeMouseTracking ("MoveSelection", true);

  //SCRIPTING SLOT DESCRIPTIONS
  setDescriptions();


  //TOOLBAR
  toolbar_ = new QToolBar("Transform and Move");

  toolBarActions_ = new QActionGroup(toolbar_);

  moveAction_ = new QAction(tr("&Move objects"), toolBarActions_);
  std::cerr << "Todo : Show all manipulators placed" << std::endl;
  moveAction_->setStatusTip(tr("Move object in 3D."));
  moveAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"move-objects.png") );
  moveAction_->setCheckable(true);
  toolbar_->addAction(moveAction_);

  moveSelectionAction_ = new QAction(tr("Move selections on objects"), toolBarActions_);
  moveSelectionAction_->setStatusTip(tr("Move selections in 3D."));
  moveSelectionAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"move-selections.png") );
  moveSelectionAction_->setCheckable(true);
  toolbar_->addAction(moveSelectionAction_);

  connect(toolBarActions_, SIGNAL(triggered(QAction*)), this, SLOT(slotSetMoveMode(QAction*)) );

  emit addToolbar(toolbar_);
}


/*******************************************************************************
        ToolBoxInterface implementation
 *******************************************************************************/

/** \brief Create the move toolbox-widget and return a reference to it
 *
 * @param _widget A reference to the move toolbox that we will return
 * @return return wether the widget was successfully generated
 */
bool MovePlugin::initializeToolbox(QWidget*& _widget)
{
   toolboxActive_ = false;
   tool_ = new moveToolbarWidget();
   tool_ -> setMaximumWidth(300);
   _widget = tool_;

   connect(tool_->moveToOrigin,SIGNAL(clicked() ),this,SLOT(slotMoveToOrigin()));

   tool_->moveToOrigin->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "moveToCOG.png") );
   tool_->moveToOrigin->setIconSize(QSize(48,48));

   connect(tool_->posButton,SIGNAL(clicked() ),this,SLOT(slotSetPosition()));
   connect(tool_->axisAButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisA()));
   connect(tool_->axisBButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisB()));
   connect(tool_->dirButton,SIGNAL(clicked() ),this,SLOT(slotSetDirection()));
   connect(tool_->transButton,SIGNAL(clicked() ),this,SLOT(slotTranslation()));

   connect(tool_->rotButton,SIGNAL(clicked() ),this,SLOT(slotRotate()));
   connect(tool_->scaleButton,SIGNAL(clicked() ),this,SLOT(slotScale()));

   connect(tool_->projectTangentButton,SIGNAL(clicked() ),this,SLOT(slotProjectToTangentPlane()));
   connect(tool_->moveManipToCOG,SIGNAL(clicked() ),this,SLOT(slotMoveManipToCOG()));

   connect(tool_->unifyBoundingBoxDiagonal,SIGNAL(clicked() ),this,SLOT(slotUnifyBoundingBoxDiagonal()));
   tool_->unifyBoundingBoxDiagonal->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "unifyBB.png") );
   tool_->unifyBoundingBoxDiagonal->setIconSize(QSize(48,48));

   lastActiveManipulator_ = -1;

   return true;
}


//------------------------------------------------------------------------------

/** \brief Toolbox state changes
 * 
 * @param _activated new state
 */
void MovePlugin::toolboxActivated( bool _activated  )
{
   toolboxActive_ = _activated;

   showManipulators();

   emit updateView();
}


/*******************************************************************************
        MouseInterface implementation
 *******************************************************************************/

/** \brief MouseWheel event occured
 * 
 * @param _event the event that occured
 */
void MovePlugin::slotMouseWheelEvent(QWheelEvent * _event, const std::string & /*_mode*/)
{
   manip_size_modifier_ = manip_size_modifier_ - (float)_event->delta() / 120.0 * 0.1;
   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
         o_it->manipulatorNode()->set_size(manip_size_ * manip_size_modifier_);
}


//------------------------------------------------------------------------------

/** \brief MousePress event occured
 * 
 * @param _event the event that occured
 */
void MovePlugin::slotMouseEvent( QMouseEvent* _event )
{
    if ( ( (PluginFunctions::pickMode() == ("Move")) || (PluginFunctions::pickMode() == ("MoveSelection")) ) &&
         PluginFunctions::actionMode() == Viewer::PickingMode ) {

         if (_event->type() == QEvent::MouseButtonDblClick) {

               placeManip(_event);
               updateManipulatorDialog();
               return;
         }

         // interaction
         ACG::SceneGraph::MouseEventAction action(_event);
         PluginFunctions::traverse(action);

    }
}


/*******************************************************************************
        PickingInterface implementation
 *******************************************************************************/

/** \brief slot is called when the pickMode changed
 * 
 * @param _mode new pickMode
 */
void MovePlugin::slotPickModeChanged( const std::string& _mode)
{
  showManipulators();

  moveAction_->setChecked(_mode == "Move");
  moveSelectionAction_->setChecked(_mode == "MoveSelection");
}


/*******************************************************************************
        MovePlugin implementation
 *******************************************************************************/

/** \brief Move object with given transformation matrix
 * 
 * @param mat transformation matrix
 * @param _id id of the object
 */
void MovePlugin::moveObject(ACG::Matrix4x4d mat, int _id) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) ) {
    emit log(LOGERR,"moveObject called for not existing Object Id");
    return;
  }

  switch ( object->dataType() ) {
    case DATA_TRIANGLE_MESH:
      transformMesh(mat , *PluginFunctions::triMesh(object) );
      break;
    case DATA_POLY_MESH:
      transformMesh(mat , *PluginFunctions::polyMesh(object) );
      break;
    case DATA_POLY_LINE:
      #ifdef ENABLE_POLYLINE_SUPPORT
      transformPolyLine(mat , *PluginFunctions::polyLine(object) );
      #endif
      break;
    default:
      emit log(LOGERR,"moveObject called for unsupported Object Type");
      return;
  }

  emit updatedObject(_id);
  emit createBackup(_id,"Move");
}


//------------------------------------------------------------------------------

/** \brief Move selection on an object with given transformation matrix
 * 
 * Which Selection (Vertex, Edge, Face) is used is determined by the
 * current SelectionMode in SelectionPlugin.
 * If the Plugin is unable to communicate with SelectionPlugin, Vertex Selection is used.
 *
 * @param mat 
 * @param _id 
 */
void MovePlugin::moveSelection(ACG::Matrix4x4d mat, int _id) {

  if (selectionType_ == VERTEX)
    transformVertexSelection( _id , mat );
  else if (selectionType_ == FACE)
    transformFaceSelection( _id , mat );
  else if (selectionType_ == EDGE)
    transformEdgeSelection( _id , mat );

  emit updatedObject(_id);
//   emit createBackup(_id,"MoveSelection");
}


//------------------------------------------------------------------------------

/** \brief move the object when its manipulator moves
 * 
 * @param _node the manipulator node
 * @param _event the mouse event
 */
void MovePlugin::manipulatorMoved( QtTranslationManipulatorNode* _node , QMouseEvent* _event) {

  // React on event only in move mode
  if ( PluginFunctions::pickMode() != "Move" && PluginFunctions::pickMode() != "MoveSelection" )
    return;

  OpenFlipper::Options::redrawDisabled( true );

  // Apply changes only on Release for moveMode and after every movement in MoveSelection Mode
  if ( (_event->type() == QEvent::MouseButtonRelease) || (PluginFunctions::pickMode() == "MoveSelection") ) {

    int objectId = _node->getIdentifier();

    ACG::Matrix4x4d mat;
    mat.identity();
    mat = _node->matrix();

    // Reset Node
    _node->loadIdentity();
    _node->set_center(mat.transform_point(_node->center()));

    // Always move the object which corresponds to the manipulator
    if (PluginFunctions::pickMode() != "MoveSelection")
      moveObject( mat, objectId );
    else
      moveSelection( mat, objectId );

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS ) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
      if ( ( o_it->id() != objectId ) && !o_it->manipulatorNode()->visible() ) { // If it has its own manipulator active, dont move it
        if (PluginFunctions::pickMode() != "MoveSelection")
          moveObject( mat,o_it->id() );
        else
          moveSelection( mat,o_it->id() );
      }
    }

    lastActiveManipulator_ = objectId;
    updateManipulatorDialog();
  }

  OpenFlipper::Options::redrawDisabled( false );

}


//------------------------------------------------------------------------------

/** \brief update object when its manipulator changes position
 * 
 * @param _node the manipulator node
 */
void MovePlugin::ManipulatorPositionChanged(QtTranslationManipulatorNode* _node ) {

  // Position has been changed of the manipulator by a direct function
  int objectId = _node->getIdentifier();

  BaseObjectData* object;
  PluginFunctions::getObject(objectId,object);

  // Assume that it has a good position now
  object->manipPlaced( true );

  // Show manipulator only if in Move mode
  if ( PluginFunctions::pickMode() == "Move" )
    _node->show();

  lastActiveManipulator_ = objectId;
  updateManipulatorDialog();

}


//------------------------------------------------------------------------------

/** \brief Place and show the Manipulator
 * 
 * @param _event  mouseEvent that occured
 */
void MovePlugin::placeManip(QMouseEvent * _event)
{
   manip_size_ = PluginFunctions::sceneRadius() * 0.1;

   unsigned int  node_idx, target_idx;
   OpenMesh::Vec3d         hitPoint;

   if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx,&hitPoint) ) {

      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         object->manipPlaced( true );

         if ( !object->picked(node_idx) ) {
            emit log(LOGWARN,"Picking failed");
         }

         object->manipulatorNode()->loadIdentity();
         object->manipulatorNode()->set_center(hitPoint);
         object->manipulatorNode()->set_draw_cylinder(true);
         object->manipulatorNode()->set_size(manip_size_ * manip_size_modifier_);
         object->manipulatorNode()->show();

         object->manipulatorNode()->apply_transformation( PluginFunctions::pickMode() == "Move" );

         connect(object->manipulatorNode() , SIGNAL(manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)),
                 this                      , SLOT(  manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)));

         connect(object->manipulatorNode() , SIGNAL(positionChanged(QtTranslationManipulatorNode*)),
                 this                      , SLOT(  ManipulatorPositionChanged(QtTranslationManipulatorNode*)));

         lastActiveManipulator_ = object->id();

         emit updateView();
      } else {
         emit log(LOGERR,"Picking ok, but Object was not found in the Object list");
      }

   } else {
       emit log(LOGWARN,"Picking failed");
   }

}


//------------------------------------------------------------------------------

/** \brief Checks if the manipulators should be visible or not
 * 
 */
void MovePlugin::showManipulators( )
{
   if ( toolboxActive_
      || (PluginFunctions::pickMode() == "Move" ) || (PluginFunctions::pickMode() == "MoveSelection" ) ) {
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
            o_it != PluginFunctions::objectsEnd(); ++o_it)
         if ( o_it->manipPlaced() ) {
              o_it->manipulatorNode()->show();
              o_it->manipulatorNode()->apply_transformation( PluginFunctions::pickMode() == "Move" );
         }
   } else {
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
            o_it != PluginFunctions::objectsEnd(); ++o_it)
         o_it->manipulatorNode()->hide();
   }

   emit updateView();

}


//------------------------------------------------------------------------------

/** \brief Position of manipulator in tab changed
 * 
 */
void MovePlugin::slotSetPosition() {
   TriMesh::Point newpos;

   bool ok = false;
   newpos[0] =  (tool_->nposx->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for X Coordinate"); return; }
   newpos[1] =  (tool_->nposy->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Y Coordinate"); return; }
   newpos[2] =  (tool_->nposz->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Z Coordinate"); return; }

   BaseObjectData* object;
   if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
      if (  object->manipulatorNode()->visible() )
         object->manipulatorNode()->set_center( newpos );
      updateManipulatorDialog();
      emit updateView();
   }

}


//------------------------------------------------------------------------------

/** \brief Toggle the first axis for changing direction in tab
 * 
 */
void MovePlugin::slotToggleAxisA() {

  axisA_ = (axisA_ + 1) % 3;

  if (axisA_ == axisB_)
    axisA_ = (axisA_ + 1) % 3;

  switch(axisA_){
    case 0: tool_->axisAButton->setText("X Direction"); break;
    case 1: tool_->axisAButton->setText("Y Direction"); break;
    case 2: tool_->axisAButton->setText("Z Direction"); break;
    default: break;
  }
}


//------------------------------------------------------------------------------

/** \brief Toggle the second axis for changing direction in tab
 * 
 */
void MovePlugin::slotToggleAxisB() {

  axisB_ = (axisB_ + 1) % 3;

  if (axisA_ == axisB_)
    axisB_ = (axisB_ + 1) % 3;

  switch(axisB_){
    case 0: tool_->axisBButton->setText("X Direction"); break;
    case 1: tool_->axisBButton->setText("Y Direction"); break;
    case 2: tool_->axisBButton->setText("Z Direction"); break;
    default: break;
  }
}


//------------------------------------------------------------------------------

/** \brief Set Direction of manipulator in tab changed
 * 
 */
void MovePlugin::slotSetDirection() {

   ACG::Vec3d newdirA,newdirB;
   ACG::Vec3d dirX,dirY;
   ACG::Vec3d dirZ(0.0,0.0,0.0);

   bool ok = false;
   newdirA[0] =  (tool_->ndirAx->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for X Coordinate");  return; }
   newdirA[1] =  (tool_->ndirAy->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Y Coordinate"); return; }
   newdirA[2] =  (tool_->ndirAz->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Z Coordinate"); return; }

   newdirB[0] =  (tool_->ndirBx->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for X Coordinate");  return; }
   newdirB[1] =  (tool_->ndirBy->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Y Coordinate"); return; }
   newdirB[2] =  (tool_->ndirBz->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Z Coordinate"); return; }

   bool xAxis = false;
   bool yAxis = false;

   switch(axisA_){
    case  0: dirX = newdirA; xAxis = true; break;
    case  1: dirY = newdirA; yAxis = true; break;
    default: dirZ = newdirA; break;
   }

   switch(axisB_){
    case  0: dirX = newdirB; xAxis = true; break;
    case  1: dirY = newdirB; yAxis = true; break;
    default: dirZ = newdirB; break;
   }

   if (!xAxis)
     dirX = dirY % dirZ;

   if (!yAxis)
     dirY = dirX % dirZ;


   if ( (dirX | dirY) != 0.0){
      emit log(LOGERR,"The axes of the new direction have to be orthogonal");
      return;
   }

   // Apply to All Target Objects
   if ( tool_->targetObjects->isChecked() ) {
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it){

         o_it->manipulatorNode()->set_direction( dirX, dirY );
      }
   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
         if (  object->manipulatorNode()->visible() ){

            object->manipulatorNode()->set_direction( dirX, dirY );
         }
      } else return;
   }

   updateManipulatorDialog();
   emit updateView();
}


//------------------------------------------------------------------------------

/** \brief perform a translation for Manipulator in tab
 * 
 */
void MovePlugin::slotTranslation() {

   ACG::Vec3d translation;

   bool ok = false;
   translation[0] =  (tool_->translationX->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for X Coordinate");  return; }
   translation[1] =  (tool_->translationY->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Y Coordinate"); return; }
   translation[2] =  (tool_->translationZ->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Z Coordinate"); return; }

    // Apply to All Target Objects
   if ( tool_->targetObjects->isChecked() ) {

      int manipcount = 0; // Check how many of the target meshes have an visible manipulator
      int targets = 0;        // Count the number of target meshes
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
         ++targets;
         if ( ! o_it->manipulatorNode()->hidden() ) {
            ++ manipcount;
         }
      }

      if (manipcount == 0 ) // No manipulator -> no translation
         return;

      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
            o_it != PluginFunctions::objectsEnd(); ++o_it)  {
         if ( manipcount > 1 ) { // Use manipulator direction for each target mesh
               if ( o_it->manipulatorNode()->hidden() )
                  continue;
         }

         translate( o_it->id() , translation );

         o_it->manipulatorNode()->set_center( o_it->manipulatorNode()->center() + translation  );
         emit createBackup(o_it->id(),"Translation");
      }

   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
         if (  object->manipulatorNode()->visible() ) {

            translate( object->id() , translation );

            object->manipulatorNode()->set_center( object->manipulatorNode()->center() + translation  );
            emit createBackup(object->id(),"Translation");
         }
      } else return;
   }

   updateManipulatorDialog();
   emit updateView();
}


//------------------------------------------------------------------------------

/** \brief Project the current manipulator onto the tangent plane of the object
 * 
 */
void MovePlugin::slotProjectToTangentPlane() {

   if ( tool_->targetObjects->isChecked() ) {
     emit log(LOGWARN,"TODO Project for multiple targets");
     return;
   } else {
     emit log(LOGWARN,"TODO Project for one target");
     return;
   }

}


//------------------------------------------------------------------------------

/** \brief Move the current manipulator to the cog of the object
 * 
 */
void MovePlugin::slotMoveManipToCOG() {
   if ( tool_->targetObjects->isChecked() ) {
       for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
            if (  o_it->manipulatorNode()->hidden() )
               continue;

            if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
               o_it->manipulatorNode()->set_center( MeshInfo::cog(*PluginFunctions::triMesh(*o_it) ) );
            else if ( o_it->dataType( DATA_POLY_MESH ) )
               o_it->manipulatorNode()->set_center( MeshInfo::cog(*PluginFunctions::polyMesh(*o_it)) );

            updateManipulatorDialog();
            o_it->manipulatorNode()->loadIdentity();
       }
   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
         if (  object->manipulatorNode()->visible() ) {

            if ( object->dataType( DATA_TRIANGLE_MESH ) )
               object->manipulatorNode()->set_center( MeshInfo::cog(*PluginFunctions::triMesh(object)) );
            else if ( object->dataType( DATA_POLY_MESH ) )
               object->manipulatorNode()->set_center( MeshInfo::cog(*PluginFunctions::polyMesh(object)) );

            updateManipulatorDialog();
            object->manipulatorNode()->loadIdentity();
         }
      }
   }
   emit updateView();
}


//------------------------------------------------------------------------------

/** \brief Rotate Manipulator (with values from Tab)
 * 
 */
void MovePlugin::slotRotate() {
   TriMesh::Point axis;
   double angle;

   bool ok = false;
   axis[0] =  (tool_->rotx->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for X Coordinate");  return; }
   axis[1] =  (tool_->roty->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Y Coordinate"); return; }
   axis[2] =  (tool_->rotz->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for Z Coordinate"); return; }

   angle =  (tool_->rotAngle->text()).toDouble(&ok);
   if ( !ok ) {  emit log(LOGERR,"Wrong Format for Angle"); return; }

    if ( tool_->targetObjects->isChecked() ) {
       for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
            if (  o_it->manipulatorNode()->hidden() )
               continue;
            o_it->manipulatorNode()->rotate(  angle,axis);

            if (o_it->dataType( DATA_TRIANGLE_MESH ) )
               transformMesh(  o_it->manipulatorNode()->matrix() , (*PluginFunctions::triMesh(*o_it)) );

            if (o_it->dataType( DATA_POLY_MESH ) )
               transformMesh(  o_it->manipulatorNode()->matrix() , (*PluginFunctions::polyMesh(*o_it)) );

            o_it->manipulatorNode()->loadIdentity();
            updateManipulatorDialog();

            emit createBackup(o_it->id(),"Rotation");
       }
   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {

         object->manipulatorNode()->rotate(angle,axis);

         if (object->dataType( DATA_TRIANGLE_MESH ) )
               transformMesh(  getLastManipulatorMatrix(true) , (*PluginFunctions::triMesh(object)) );

         if (object->dataType( DATA_POLY_MESH ) )
               transformMesh(  getLastManipulatorMatrix(true) , (*PluginFunctions::polyMesh(object)) );

         updateManipulatorDialog();

         emit createBackup(object->id(),"Rotation");
      }
   }
   emit updateView();

}


//------------------------------------------------------------------------------

/** \brief Scale Manipulator (with values from Tab)
 * 
 */
void MovePlugin::slotScale() {
   TriMesh::Point scale;

   bool ok = false;
   scale[0] =  (tool_->scalex->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for factor 1");  return; }
   scale[1] =  (tool_->scaley->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for factor 2"); return; }
   scale[2] =  (tool_->scalez->text()).toDouble(&ok);
   if ( !ok ) { emit log(LOGERR,"Wrong Format for factor 3"); return; }

   if ( tool_->targetObjects->isChecked() ) {
       for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
            if (  o_it->manipulatorNode()->hidden() )
               continue;
            o_it->manipulatorNode()->scale(  scale);

            if (o_it->dataType( DATA_TRIANGLE_MESH ) )
               transformMesh(  o_it->manipulatorNode()->matrix() , (*PluginFunctions::triMesh(*o_it)) );

            if (o_it->dataType( DATA_POLY_MESH ) )
               transformMesh(  o_it->manipulatorNode()->matrix() , (*PluginFunctions::polyMesh(*o_it)) );

            o_it->manipulatorNode()->loadIdentity();
            updateManipulatorDialog();

            emit createBackup(o_it->id(),"Scaling");
       }
   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {

         object->manipulatorNode()->scale(  scale);

         if (object->dataType( DATA_TRIANGLE_MESH ) )
            transformMesh(  getLastManipulatorMatrix(true) , (*PluginFunctions::triMesh(object)) );

         if (object->dataType( DATA_POLY_MESH ) )
            transformMesh(  getLastManipulatorMatrix(true) , (*PluginFunctions::polyMesh(object)) );

         updateManipulatorDialog();

         emit createBackup(object->id(),"Scaling");
      }
   }
   emit updateView();
}


//------------------------------------------------------------------------------


/** \brief Move currently active or first Mesh with its COG to the origin
 * 
 */
void MovePlugin::slotMoveToOrigin() {

   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
      if ( o_it->dataType( DATA_TRIANGLE_MESH )) {
         TriMesh& mesh = *PluginFunctions::triMesh(*o_it);
         const TriMesh::Point cog = MeshInfo::cog(mesh);

         for ( TriMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end() ; ++v_it)
            mesh.set_point(v_it , ( mesh.point(v_it) ) - cog );

         o_it->manipulatorNode()->set_center( o_it->manipulatorNode()->center() - cog );
      }

      if ( o_it->dataType( DATA_POLY_MESH )) {
         PolyMesh& mesh = *PluginFunctions::polyMesh(*o_it);
         const PolyMesh::Point cog = MeshInfo::cog(mesh);

         for ( PolyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end() ; ++v_it)
            mesh.set_point(v_it , ( mesh.point(v_it) ) - cog );

         o_it->manipulatorNode()->set_center( o_it->manipulatorNode()->center() - cog );

      }

      emit updatedObject( o_it->id() );

      updateManipulatorDialog();
      o_it->manipulatorNode()->loadIdentity();

      emit createBackup(o_it->id(),"Move to origin");
   }

   emit updateView();
}


//------------------------------------------------------------------------------

/** \brief Scale Boundingbox Diagonal to unit size
 * 
 */
void MovePlugin::slotUnifyBoundingBoxDiagonal()
{
   if ( tool_->targetObjects->isChecked() ) {
       for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
            if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
              unifyBBDiag(*PluginFunctions::triMesh(*o_it));
            else if ( o_it->dataType( DATA_POLY_MESH ) )
             unifyBBDiag(*PluginFunctions::polyMesh(*o_it));

            emit updatedObject( o_it->id() );

       }
   } else {
      BaseObjectData* object;
      if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
         if (  object->manipulatorNode()->visible() ) {

            if ( object->dataType( DATA_TRIANGLE_MESH ) )
              unifyBBDiag(*PluginFunctions::triMesh(object));
            else if ( object->dataType( DATA_POLY_MESH ) )
             unifyBBDiag(*PluginFunctions::polyMesh(object));
         }

         emit updatedObject( object->id() );
      }
   }
   emit updateView();
}


//------------------------------------------------------------------------------

/** \brief Update the Dialog with the last clicked manipulator
 * 
 */
void MovePlugin::updateManipulatorDialog() {
   BaseObjectData* object;
   if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
      if (  object->manipulatorNode()->visible() ) {
        const TriMesh::Point pos = object->manipulatorNode()->center();

        QString num;
        num = QString::number(pos[0]); tool_->posx->setText(num);
        num = QString::number(pos[1]); tool_->posy->setText(num);
        num = QString::number(pos[2]); tool_->posz->setText(num);

        TriMesh::Point direction = object->manipulatorNode()->directionX();
        num = QString::number(direction[0]); tool_->dirxx->setText(num);
        num = QString::number(direction[1]); tool_->dirxy->setText(num);
        num = QString::number(direction[2]); tool_->dirxz->setText(num);
  
        direction = object->manipulatorNode()->directionY();
        num = QString::number(direction[0]); tool_->diryx->setText(num);
        num = QString::number(direction[1]); tool_->diryy->setText(num);
        num = QString::number(direction[2]); tool_->diryz->setText(num);
  
        direction = object->manipulatorNode()->directionZ();
        num = QString::number(direction[0]); tool_->dirzx->setText(num);
        num = QString::number(direction[1]); tool_->dirzy->setText(num);
        num = QString::number(direction[2]); tool_->dirzz->setText(num);

      }
   }
}


//------------------------------------------------------------------------------

/** \brief Called by Toolbar to enable move mode
 * 
 * @param _action the action that was triggered
 */
void MovePlugin::slotSetMoveMode(QAction* _action) {

  if (_action == moveAction_){
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("Move");

    moveAction_->setChecked( true );
  }

  if (_action == moveSelectionAction_){

    connectSelectionActions();

    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("MoveSelection");

    moveSelectionAction_->setChecked( true );
  }
}


//------------------------------------------------------------------------------

/** \brief Get the Matrix of the last active Manipulator ( Identity if not found or hidden Manipulator )
  *
  * @param _reset reset the transformation matrix of the manipulator to identity)
  * @return current transformation matrix of the manipulator
  */
ACG::Matrix4x4d MovePlugin::getLastManipulatorMatrix(bool _reset) {
   ACG::Matrix4x4d matrix;
   matrix.identity();

   BaseObjectData* object;
   if ( PluginFunctions::getObject(lastActiveManipulator_ , object) )
         if (  object->manipulatorNode()->visible() ) {
            matrix = object->manipulatorNode()->matrix();
            if (_reset)
               object->manipulatorNode()->loadIdentity();
         }
   return matrix;
}


//------------------------------------------------------------------------------

/** \brief Transform a mesh with the given transformation matrix
 * 
 * @param _mat transformation matrix
 * @param _mesh the mesh
 */
template< typename MeshT >
void MovePlugin::transformMesh(ACG::Matrix4x4d _mat , MeshT& _mesh ) {
   typename MeshT::VertexIter v_it     = _mesh.vertices_begin();
   typename MeshT::VertexIter v_end = _mesh.vertices_end();
   for (; v_it!=v_end; ++v_it) {
            _mesh.set_point(v_it,(typename MeshT::Point)_mat.transform_point((OpenMesh::Vec3d)(_mesh.point(v_it))));
            _mesh.set_normal(v_it,(typename MeshT::Point)_mat.transform_vector((OpenMesh::Vec3d)(_mesh.normal(v_it))));
   }

   typename MeshT::FaceIter f_it     = _mesh.faces_begin();
   typename MeshT::FaceIter f_end = _mesh.faces_end();
   for (; f_it != f_end; ++f_it)
            _mesh.set_normal(f_it,(typename MeshT::Point)_mat.transform_vector((OpenMesh::Vec3d)(_mesh.normal(f_it))));
}


//------------------------------------------------------------------------------

#ifdef ENABLE_POLYLINE_SUPPORT

/** \brief Transform a polyline with the given transformation matrix
 * 
 * @param _mat transformation matrix
 * @param _polyLine the polyline
 */
template< class PolyLineT >
void MovePlugin::transformPolyLine( ACG::Matrix4x4d _mat , PolyLineT& _polyLine  ) {
  #ifdef USE_OPENMP
  #pragma omp parallel for
  #endif
  for ( int i = 0 ; i < (int)_polyLine.n_vertices(); ++i )
    _polyLine.point(i) = _mat.transform_point( _polyLine.point(i) );
}

#endif


//------------------------------------------------------------------------------

/** \brief scale mesh to have a boundingboxdiagonal of one
 * 
 * @param _mesh the mesh
 */
template< typename MeshT >
void MovePlugin::unifyBBDiag(MeshT& _mesh )
{
  typename MeshT::VertexIter v_it  = _mesh.vertices_begin();
  typename MeshT::VertexIter v_end = _mesh.vertices_end();

  // no vertices?
  if( v_it == v_end) return;

  typename MeshT::Point bb_min = _mesh.point(v_it);
  typename MeshT::Point bb_max = _mesh.point(v_it);

  for(; v_it!=v_end; ++v_it)
  {
    bb_min.minimize( _mesh.point(v_it));
    bb_max.maximize( _mesh.point(v_it));
  }

  typename MeshT::Point bb_center =  0.5 * (bb_min + bb_max) ;

  typename MeshT::Scalar scale = 1.0/(bb_max-bb_min).norm();

  for( v_it = _mesh.vertices_begin(); v_it!=v_end; ++v_it)
  {

     _mesh.point(v_it) = (_mesh.point(v_it) - bb_center) * scale + bb_center;
  }

}


//------------------------------------------------------------------------------

/** \brief Connect to SelectionPlugin
 * 
 */
void MovePlugin::connectSelectionActions(){

  if (!selectionConnected_){

    selectionConnected_ = true;

    bool connected = false;

    QToolBar* selToolBar = 0;
    emit getToolBar( "Selection", selToolBar );
  
    if (selToolBar != 0){

      QActionGroup* actionGroup = 0;
  
      for (int i=0; i < selToolBar->actions().count(); i++){
        if( selToolBar->actions().at(i)->text() == "Enable Vertex Selection"){

          actionGroup = selToolBar->actions().at(i)->actionGroup();
          
          if ( selToolBar->actions().at(i)->isChecked() )
            selectionType_ = VERTEX;

        } else if ( (selToolBar->actions().at(i)->text() == "Enable Edge Selection")
                && (selToolBar->actions().at(i)->isChecked()) )
            selectionType_ = EDGE;
        else if ( (selToolBar->actions().at(i)->text() == "Enable Face Selection")
                && (selToolBar->actions().at(i)->isChecked()) )
            selectionType_ = FACE;
      }
  
      if (actionGroup != 0){
        connect( actionGroup, SIGNAL( triggered(QAction*) ), this, SLOT(slotSelectionModeChanged(QAction*)) );
        connected = true;
      }
    }

    if (!connected)
      emit log(LOGWARN, "Unable to connect to Selection-Plugin. MoveSelection will work on vertices only.");
  }
}


//------------------------------------------------------------------------------

/** \brief The SelectionMode in SelectionPlugin Changed
 * 
 * @param _action the action on the Selection-Toolbar that was hit
 */
void MovePlugin::slotSelectionModeChanged(QAction* _action){

  if (_action->text() == "Enable Vertex Selection")
    selectionType_ = VERTEX;
  if (_action->text() == "Enable Edge Selection")
    selectionType_ = EDGE;
  if (_action->text() == "Enable Face Selection")
    selectionType_ = FACE;
}

Q_EXPORT_PLUGIN2( moveplugin , MovePlugin );


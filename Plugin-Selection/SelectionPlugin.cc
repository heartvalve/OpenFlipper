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





#include <QtGui>

#include "SelectionPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/MeshNodeT.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include "OpenFlipper/INIFile/INIFile.hh"

#include <QFileDialog>

/** \brief Default Constructor
 */
SelectionPlugin::SelectionPlugin() :
  lasso_(0),
  waitingForPolyLineSelection_(false),
  polyLineID_(-1)
{
  selectionType_ = VERTEX;

  sourceSelection_ = false;
  deselection_ = false;
}

/*******************************************************************************
        BaseInterface implementation
 *******************************************************************************/


/** \brief Initialization of the plugin when it is loaded by the core
 */
void SelectionPlugin::initializePlugin() {

  if ( OpenFlipper::Options::nogui() )
    return;

  // create sphere for paint sphere selection
  sphere_mat_node_  = new ACG::SceneGraph::MaterialNode(PluginFunctions::getRootNode(), "Sphere Color");
  sphere_mat_node_->applyProperties( MaterialNode::Blending  |
                                     MaterialNode::Material  |
                                     MaterialNode::AlphaTest |
                                     MaterialNode::BackFaceCulling );
  sphere_mat_node_->set_color(ACG::Vec4f(1.0, 0.0, 0.0, 0.3));
  sphere_mat_node_->enable_blending();
  sphere_mat_node_->disable_alpha_test();
  sphere_mat_node_->enable_backface_culling();

  sphere_node_ = new ACG::SceneGraph::GlutPrimitiveNode(ACG::SceneGraph::GlutPrimitiveNode::SPHERE, sphere_mat_node_, "Sphere");
  sphere_node_->drawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);
  sphere_node_->hide();

  sphere_radius_ = 0.1 * PluginFunctions::sceneRadius();

  sphere_selection_ = false;


  //register keys for the plugin
  emit registerKey(Qt::Key_C,      Qt::NoModifier, "Clear Selection");
  emit registerKey(Qt::Key_I,      Qt::NoModifier, "Invert Selection");
  emit registerKey(Qt::Key_Delete, Qt::NoModifier, "Delete Selection");

  //the release event does not contain the modifier
  emit registerKey(Qt::Key_Control, Qt::NoModifier, "Deselect instead of Select");
  emit registerKey(Qt::Key_Shift,   Qt::NoModifier,   "Switch between source/target Selection");
  //the press event contains the modifier
  emit registerKey(Qt::Key_Control, Qt::ControlModifier, "Deselect instead of Select");
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier,   "Switch between source/target Selection");

  //different combinations of keyPresses
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier | Qt::ControlModifier, "Source Deselection");
  emit registerKey(Qt::Key_Control, Qt::ShiftModifier | Qt::ControlModifier, "Source Deselection");
}

//***********************************************************************************


/** \brief Second initialization step after all plugins were loaded by the core
 */
void SelectionPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::nogui() )
    return;

  // PICKMODES
  emit addHiddenPickMode(TOGGLE_SELECTION);
  emit addHiddenPickMode(PAINT_SPHERE_SELECTION);
  emit addHiddenPickMode(CLOSEST_BOUNDARY_SELECTION);
  emit addHiddenPickMode(LASSO_SELECTION);
  emit addHiddenPickMode(SURFACE_LASSO_SELECTION);
  emit addHiddenPickMode(CONNECTED_COMPONENT_SELECTION);

  emit setPickModeMouseTracking(PAINT_SPHERE_SELECTION, true);
  emit setPickModeCursor(SURFACE_LASSO_SELECTION, Qt::CrossCursor);

  // CONTEXT MENU
  contextMenu_ = new QMenu("Select");

  QAction* lastAction;

  lastAction = contextMenu_->addAction( "All" );
  lastAction->setToolTip("Select all");
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Clear" );
  lastAction->setToolTip("Clear selection");
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Invert" );
  lastAction->setToolTip("Invert selection");
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Shrink" );
  lastAction->setToolTip("Deselect the boundary of the current selection");
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Grow" );
  lastAction->setToolTip("Extend the selection at their boundary");
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Boundary" );
  lastAction->setToolTip("Select the boundary of the object");
  lastAction->setStatusTip( lastAction->toolTip() );


  emit addContextMenu(contextMenu_ , DATA_TRIANGLE_MESH , CONTEXTSELECTIONMENU );
  emit addContextMenu(contextMenu_ , DATA_POLY_MESH     , CONTEXTSELECTIONMENU );
  emit addContextMenu(contextMenu_ , DATA_POLY_LINE     , CONTEXTSELECTIONMENU );
  emit addContextMenu(contextMenu_ , DATA_BSPLINE_CURVE , CONTEXTSELECTIONMENU );

  connect( contextMenu_ , SIGNAL( triggered(QAction*) ),
           this,          SLOT(selectionContextMenu(QAction*)) );


  // TOOLBAR
  toolBar_ = new QToolBar("Selection");
  emit addToolbar(toolBar_);

  // TOOLBAR - SELECTION TYPES
  toolBarActions_ = new QActionGroup(toolBar_);
  toolBarTypes_ = new QActionGroup(toolBar_);
  toolBarTypes_->setExclusive( true );

  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  objectAction_ = new QAction( QIcon(iconPath + "selection_object.png"), "Enable Object Selection", toolBarTypes_ );
  objectAction_->setCheckable( true );
  objectAction_->setChecked( false );
  toolBar_->addAction( objectAction_ );
  vertexAction_ = new QAction( QIcon(iconPath + "selection_vertex.png"), "Enable Vertex Selection", toolBarTypes_ );
  vertexAction_->setCheckable( true );
  vertexAction_->setChecked( true );
  toolBar_->addAction( vertexAction_ );
  edgeAction_ = new QAction( QIcon(iconPath + "selection_edge.png"), "Enable Edge Selection", toolBarTypes_ );
  edgeAction_->setCheckable( true );
  toolBar_->addAction( edgeAction_ );
  faceAction_ = new QAction( QIcon(iconPath + "selection_face.png"), "Enable Face Selection", toolBarTypes_ );
  faceAction_->setCheckable( true );
  toolBar_->addAction( faceAction_ );
  toolBar_->addSeparator();

  // TOOLBAR - SELECTION ACTIONS
  toggleAction_ = new QAction( QIcon(iconPath + "selection_toggleV.png"), "Toggle", toolBarActions_ );
  toggleAction_->setCheckable( true );
  toolBar_->addAction( toggleAction_ );
  lassoAction_ = new QAction( QIcon(iconPath + "lasso.png"), "Lasso", toolBarActions_ );
  lassoAction_->setCheckable( true );
  toolBar_->addAction( lassoAction_ );
  paintSphereAction_ = new QAction( QIcon(iconPath + "paintSphereVertex.png"), "Paint Sphere", toolBarActions_ );
  paintSphereAction_->setCheckable( true );
  toolBar_->addAction( paintSphereAction_ );
  boundaryAction_ = new QAction( QIcon(iconPath + "selection_boundary.png"), "Closest Boundary", toolBarActions_ );
  boundaryAction_->setCheckable( true );
  toolBar_->addAction( boundaryAction_ );
  #ifdef ENABLE_POLYLINE_SUPPORT
  surfaceLassoAction_ = new QAction( QIcon(iconPath + "surface-lasso.png"), "Surface Lasso", toolBarActions_ );
  surfaceLassoAction_->setCheckable( true );
  toolBar_->addAction( surfaceLassoAction_ );
  #endif
  connectedAction_ = new QAction( QIcon(iconPath + "selection_connectedV.png"), "Connected Component", toolBarActions_ );
  connectedAction_->setCheckable( true );
  toolBar_->addAction( connectedAction_ );

  connect( toolBarActions_, SIGNAL( triggered(QAction*) ), this, SLOT(toolBarActionClicked(QAction*)) );
  connect( toolBarTypes_, SIGNAL( triggered(QAction*) ), this, SLOT(toolBarActionClicked(QAction*)) );
}

//***********************************************************************************

/** \brief Slot for receiving updates about changed objects
 *
 * this slot is needed for the surface selection, because the polyline for the selection is drawn
 * by the PolyLine Plugin and this function keeps track if drawing is completed.
 *
 * @param _id object-id of changed object
 */
void SelectionPlugin::slotObjectUpdated(int _id){
#ifdef ENABLE_POLYLINE_SUPPORT

  if (waitingForPolyLineSelection_){

    //is object a polyLine?
    BaseObjectData *obj;
    PluginFunctions::getObject(_id, obj);
    // get polyline object
    PolyLineObject* pline  = PluginFunctions::polyLineObject(obj);
    if (pline){
      if (polyLineID_ == -1){
        polyLineID_ = _id;
      }else if (polyLineID_ != _id){
        //the user added a second polyline so delete the first
        PluginFunctions::deleteObject(polyLineID_);
        polyLineID_ = _id;
      }
      if (pline->line()->is_closed()){
        PluginFunctions::pickMode(SURFACE_LASSO_SELECTION);
        waitingForPolyLineSelection_ = false;
      }
    }
  }
#endif
}


/*******************************************************************************
        ToolboxInterface implementation
 *******************************************************************************/
/** \brief Create the selection toolbox-widget and return a reference to it
 *
 * @param _widget A reference to the selection toolbox that we will return
 * @return return wether the widget was successfully generated
 */
bool SelectionPlugin::initializeToolbox(QWidget*& _widget)
{

  tool_ = new selectionToolbarWidget();
  _widget = tool_;
  QSize size(300, 300);
  tool_->resize(size);

  connect( tool_->selectBoundaryVerticesButton,SIGNAL(clicked()), this,SLOT(slotSelectBoundaryVertices()) );
  connect( tool_->selectBoundaryFacesButton,SIGNAL(clicked()), this,SLOT(slotSelectBoundaryFaces()) );

  connect( tool_->clearVertexSelectionButton ,SIGNAL(clicked()), this,SLOT(slotClearVertexSelection()) );
  connect( tool_->clearFaceSelectionButton ,SIGNAL(clicked()), this,SLOT(slotClearFaceSelection()) );

  connect( tool_->selectAllVerticesButton ,SIGNAL(clicked()), this,SLOT(slotSelectAllVertices()) );
  connect( tool_->selectAllFacesButton ,SIGNAL(clicked()), this,SLOT(slotSelectAllFaces()) );

  connect( tool_->invertVertexSelectionButton ,SIGNAL(clicked()), this,SLOT(slotInvertVertexSelection()) );
  connect( tool_->invertFaceSelectionButton ,SIGNAL(clicked()), this,SLOT(slotInvertFaceSelection()) );

  connect( tool_->growVertexSelectionButton ,SIGNAL(clicked()), this,SLOT(slotGrowVertexSelection()) );
  connect( tool_->growFaceSelectionButton ,SIGNAL(clicked()), this,SLOT(slotGrowFaceSelection()) );

  connect( tool_->shrinkVertexSelectionButton ,SIGNAL(clicked()), this,SLOT(slotShrinkVertexSelection()) );
  connect( tool_->shrinkFaceSelectionButton ,SIGNAL(clicked()), this,SLOT(slotShrinkFaceSelection()) );

  connect( tool_->setAreaButton ,SIGNAL(clicked()), this,SLOT(slotSetArea()) );
  connect( tool_->setHandleButton ,SIGNAL(clicked()), this,SLOT(slotSetHandle()) );
  connect( tool_->clearAreaButton ,SIGNAL(clicked()), this,SLOT(slotClearArea()) );
  connect( tool_->clearHandleButton ,SIGNAL(clicked()), this,SLOT(slotClearHandle()));
//    connect( tool_->saveModelingArea ,SIGNAL(clicked()), this,SLOT(slotSaveModelingArea()));
//    connect( tool_->loadModelingArea ,SIGNAL(clicked()), this,SLOT(slotLoadModelingArea()));
//    connect( tool_->saveHandleRegion ,SIGNAL(clicked()), this,SLOT(slotSaveHandleRegion()));
//    connect( tool_->loadHandleRegion ,SIGNAL(clicked()), this,SLOT(slotLoadHandleRegion()));
//    connect( tool_->saveSelection ,SIGNAL(clicked()), this,SLOT(slotSaveSelection()));
//    connect( tool_->loadSelection ,SIGNAL(clicked()), this,SLOT(slotLoadSelection()));
//    connect( tool_->saveSelectionFaces ,SIGNAL(clicked()), this,SLOT(slotSaveFaceSelection()));
//    connect( tool_->loadSelectionFaces ,SIGNAL(clicked()), this,SLOT(slotLoadFaceSelection()));

  return true;
}


/*******************************************************************************
        KeyInterface implementation
 *******************************************************************************/

/** \brief Handle key press events for registered keys
 *
 * @param _event the key event that occurred
 */
void SelectionPlugin::slotKeyEvent( QKeyEvent* _event ) {
  //check if we want to select or deselect
  if ( (_event->modifiers() & Qt::ControlModifier) || (_event->key() == Qt::Key_Control) )
    deselection_ = true;
  else
    deselection_ = false;

  //check if we want to select source or target
  if ( (_event->modifiers() & Qt::ShiftModifier) || (_event->key() == Qt::Key_Shift) )
    sourceSelection_ = true;
  else
    sourceSelection_ = false;

  //handle remaining keys
  switch (_event->key())
  {
    case Qt::Key_C :
        slotClearSelection();
        return;
        break;
    case Qt::Key_I :
        slotInvertSelection();
        return;
        break;
    case Qt::Key_Delete :
        slotDeleteSelection();
    default:
    break;
  }

}

//******************************************************************************

/** \brief Handle key release events for registered keys
 *
 * @param _event the key event that occurred
 */
void SelectionPlugin::slotKeyReleaseEvent( QKeyEvent* _event ) {

  //check if we want to select or deselect
  if ( (_event->modifiers() & Qt::ControlModifier) || (_event->key() == Qt::Key_Control) )
    deselection_ = false;

  //check if we want to select source or target
  if ( (_event->modifiers() & Qt::ShiftModifier) || (_event->key() == Qt::Key_Shift) )
    sourceSelection_ = false;
}


/*******************************************************************************
        MouseInterface implementation
 *******************************************************************************/

/** \brief Handle mouse events
 *
 * maps to a selection-function dependent on current pickMode
 *
 * @param _event the mouse event that occurred
 */
void SelectionPlugin::slotMouseEvent( QMouseEvent* _event ) {

  if ( PluginFunctions::pickMode() == TOGGLE_SELECTION)              toggleSelection(_event);       else
  if ( PluginFunctions::pickMode() == PAINT_SPHERE_SELECTION)        paintSphereSelection(_event);  else
  if ( PluginFunctions::pickMode() == CLOSEST_BOUNDARY_SELECTION)    closestBoundarySelection(_event); else
  if ( PluginFunctions::pickMode() == LASSO_SELECTION) {
    if ( lasso_ == 0 ) {
      // create lasso For screen lasso selection
      lasso_ = new ACG::QtLasso(PluginFunctions::viewerProperties().glState());
      connect(lasso_ , SIGNAL(signalLassoSelection(ACG::QtLasso::SelectionMode)),
              this   , SLOT(slotLassoSelection(ACG::QtLasso::SelectionMode)));
    }

    lasso_->slotMouseEvent(_event);

  } else
  if ( PluginFunctions::pickMode() == CONNECTED_COMPONENT_SELECTION) componentSelection(_event);
#ifdef ENABLE_POLYLINE_SUPPORT
  else
  if ( PluginFunctions::pickMode() == SURFACE_LASSO_SELECTION)       surfaceLassoSelection(_event);
#endif

}

//******************************************************************************

/** \brief adjust the Sphere radius for PaintSphereSelection
 *
 * @param _event the mouse wheel event that occurred
 * @param  unused
 */
void SelectionPlugin::slotMouseWheelEvent(QWheelEvent * _event, const std::string & /*_mode*/) {
   if ( PluginFunctions::pickMode() == PAINT_SPHERE_SELECTION  ) {
      float d = -(float)_event->delta() / 120.0 * 0.1;
      sphere_radius_ *= 1.0 + d;

      sphere_node_->set_size(sphere_radius_);
      sphere_node_->show();
      emit updateView();
   }
}


/*******************************************************************************
        PickingInterface implementation
 *******************************************************************************/

/** \brief Update selection states when the PickMode changes
 *
 * @param _mode the new PickMode
 */
void SelectionPlugin::slotPickModeChanged( const std::string& _mode) {

  toggleAction_->setChecked(      _mode == TOGGLE_SELECTION );
  paintSphereAction_->setChecked( _mode == PAINT_SPHERE_SELECTION );
  boundaryAction_->setChecked(    _mode == CLOSEST_BOUNDARY_SELECTION );
  lassoAction_->setChecked(       _mode == LASSO_SELECTION );
  connectedAction_->setChecked(   _mode == CONNECTED_COMPONENT_SELECTION );

   if ( _mode != PAINT_SPHERE_SELECTION && sphere_node_->visible() ){
      sphere_node_->hide();
      emit updateView();
   }

   if ( (_mode != LASSO_SELECTION) && (lasso_ != 0) ) {
     lasso_->reset();
     delete lasso_;
     lasso_ = 0;
   }

#ifdef ENABLE_POLYLINE_SUPPORT
  bool check = (_mode == SURFACE_LASSO_SELECTION) || (_mode == "PolyLine" && waitingForPolyLineSelection_);
  surfaceLassoAction_->setChecked(check);

   if (waitingForPolyLineSelection_)
     if (_mode != "" && _mode != "PolyLine" && _mode != SURFACE_LASSO_SELECTION ){
      waitingForPolyLineSelection_ = false;
      PluginFunctions::deleteObject(polyLineID_);
      polyLineID_ = -1;
    }
#endif
}


/*******************************************************************************
        SelectionPlugin implementation
 *******************************************************************************/

/** \brief called when an action in the selection toolbar was clicked
 *
 * @param _action the action that was clicked
 */
void SelectionPlugin::toolBarActionClicked(QAction * _action)
{
  //Selction Types
  if ( _action->text() == "Enable Object Selection" ||
       _action->text() == "Enable Vertex Selection" ||
       _action->text() == "Enable Edge Selection" ||
       _action->text() == "Enable Face Selection"){

    unsigned char type = 0;
    if ( objectAction_->isChecked() ){
      type = type | OBJECT;

      toolBarActionClicked( toggleAction_ ); //automatically switch to toggle

    }if ( vertexAction_->isChecked() )
      type = type | VERTEX;
    if ( edgeAction_->isChecked() )
      type = type | EDGE;
    if ( faceAction_->isChecked() )
      type = type | FACE;

    selectionType_ = type;

    //for object selection only toggle and lasso are available
    paintSphereAction_->setEnabled( !objectAction_->isChecked() );
    boundaryAction_->setEnabled( !objectAction_->isChecked() );
    surfaceLassoAction_->setEnabled( !objectAction_->isChecked() );
    connectedAction_->setEnabled( !objectAction_->isChecked() );

  }else{
    //first check if a selection type was set
    if (selectionType_ == 0){
      emit log(LOGERR,"Choose at least one selection type first.");
      _action->setChecked(false);
      return;
    }
    //Selction Modes
    PluginFunctions::actionMode( Viewer::PickingMode );

    if (_action->text() == "Toggle")
      PluginFunctions::pickMode( TOGGLE_SELECTION );
    else if (_action->text() == "Paint Sphere")
      PluginFunctions::pickMode( PAINT_SPHERE_SELECTION );
    else if (_action->text() == "Closest Boundary")
      PluginFunctions::pickMode( CLOSEST_BOUNDARY_SELECTION );
    else if (_action->text() == "Lasso")
      PluginFunctions::pickMode( LASSO_SELECTION );
    else if (_action->text() == "Connected Component")
      PluginFunctions::pickMode( CONNECTED_COMPONENT_SELECTION );
    else if (_action->text() == "Surface Lasso"){
      waitingForPolyLineSelection_ = true;
      PluginFunctions::pickMode("PolyLine");
    }
  }
}

//******************************************************************************

/** \brief Use current selection to set AREA bits
 */
void SelectionPlugin::slotSetArea() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        set_area(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        set_area(PluginFunctions::polyMesh(*o_it));
    o_it->update();
  }

  emit updateView();
}


//******************************************************************************


/** \brief Use current selection to set HANDLE bits
 */
void SelectionPlugin::slotSetHandle() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        set_handle(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        set_handle(PluginFunctions::polyMesh(*o_it));
    o_it->update();
  }

  emit updateView();
}


//******************************************************************************


/** \brief Clear Handle Bits
 */
void SelectionPlugin::slotClearHandle() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_handle(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_handle(PluginFunctions::polyMesh(*o_it));
    o_it->update();
  }

  emit updateView();
}


//******************************************************************************

/** \brief Clear Area Bits
 */
void SelectionPlugin::slotClearArea() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_area(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_area(PluginFunctions::polyMesh(*o_it));
    o_it->update();
  }

  emit updateView();
}


//******************************************************************************

/** \brief Delete all selected elements of the target meshes
 */
void SelectionPlugin::slotDeleteSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( tool_->selectOnAll->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
    if ( o_it->visible() ){
      bool changed = false;
      if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
          changed = changed || deleteSelection(PluginFunctions::triMesh(*o_it));
      if ( o_it->dataType( DATA_POLY_MESH ) )
          changed = changed || deleteSelection(PluginFunctions::polyMesh(*o_it));
      o_it->update();
      emit updatedObject(o_it->id());
    }
  }
  emit updateView();
}


//******************************************************************************

/** \brief store selections of a given object to a given ini file
 *
 * @param _ini reference to the ini file
 * @param _id  id of the object
 */
void SelectionPlugin::saveIniFile( INIFile& _ini , int _id) {

   BaseObjectData* object;
   if ( !PluginFunctions::getObject(_id,object) ) {
     emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
     return;
   }

   std::vector< int > ids;

   // The objects section should already exist
   QString sectionName = object->name();
   if ( !_ini.section_exists( sectionName ) ) {
     emit log(LOGERR,"Cannot find object section id " + QString::number(_id) + " in saveFile" );
     return;
   }

  _ini.add_entry(sectionName , "VertexSelection" , getVertexSelection( object->id()) );
  _ini.add_entry(sectionName , "EdgeSelection"   , getEdgeSelection  ( object->id()) );

  if ( object->dataType( DATA_POLY_MESH ) || object->dataType( DATA_TRIANGLE_MESH )) {
    _ini.add_entry(sectionName , "FaceSelection"  , getFaceSelection   ( object->id() ) );
    _ini.add_entry(sectionName , "HandleRegion"   , getHandleVertices  ( object->id() ) );
    _ini.add_entry(sectionName , "ModelingRegion" , getModelingVertices( object->id() ) );
  }

}


//******************************************************************************

/** \brief load selections for a given object from a given ini file
 *
 * @param _ini reference to the ini file
 * @param _id  id of the object
 */
void SelectionPlugin::loadIniFile( INIFile& _ini, int _id )
{
  std::cerr << "a" << std::endl;
  BaseObjectData* object;
  if ( !PluginFunctions::getObject(_id,object) ) {
    emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
    return;
  }

  std::vector< int > ids;
  bool invert = false;

  bool updated_selection = false;
  bool updated_modeling_regions = false;

  QString sectionName = object->name();

  if ( _ini.get_entry( ids , sectionName , "ModelingRegion" )) {
    invert = false;
    _ini.get_entry( invert , sectionName , "InvertModeling" );

    if ( invert ) {
      setAllModelingVertices( object->id() );
      unselectModelingVertices( object->id() , ids );
    } else {
      clearModelingVertices( object->id() );
      selectModelingVertices( object->id() , ids);
    }


    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      update_regions(PluginFunctions::triMesh(object));
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      update_regions(PluginFunctions::polyMesh(object));
    }

    updated_modeling_regions = true;
  }

  if ( _ini.get_entry( ids , sectionName , "HandleRegion" )) {
    invert = false;
    _ini.get_entry( invert , sectionName , "InvertHandle" );

    if ( invert ) {
      setAllHandleVertices( object->id() );
      unselectHandleVertices( object->id() , ids );
    } else {
      clearHandleVertices( object->id() );
      selectHandleVertices( object->id() , ids);
    }

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      update_regions(PluginFunctions::triMesh(object));
    }

    if ( object->dataType( DATA_POLY_MESH ) ) {
      update_regions(PluginFunctions::polyMesh(object));
    }

    updated_modeling_regions = true;
  }

  if ( _ini.get_entry( ids , sectionName , "VertexSelection" )) {
    invert = false;
    _ini.get_entry( invert , sectionName , "InvertVertexSelection" );

    if ( invert ) {
      selectAllVertices( object->id() );
      unselectVertices( object->id(),ids );
    } else {
      clearVertexSelection(object->id());
      selectVertices( object->id(),ids );
    }

    updated_selection = true;
  }

  if ( _ini.get_entry( ids , sectionName , "EdgeSelection" )) {
    invert = false;
    _ini.get_entry( invert , sectionName , "InvertEdgeSelection" );

    if ( invert ) {
      selectAllEdges( object->id() );
      unselectEdges( object->id(),ids );
    } else {
      clearEdgeSelection(object->id());
      selectEdges( object->id(),ids );
    }

    updated_selection = true;
  }

  if ( _ini.get_entry( ids , sectionName , "FaceSelection" )) {
    invert = false;
    _ini.get_entry( invert , sectionName , "InvertFaceSelection" );

    if ( invert ) {
      selectAllFaces( object->id() );
      unselectFaces( object->id(),ids );
    } else {
      clearFaceSelection(object->id());
      selectFaces( object->id(),ids );
    }

    updated_selection = true;
  }

  if ( updated_selection || updated_modeling_regions ) {
    object->update();
    emit updateView();
  }

};


Q_EXPORT_PLUGIN2( selectionplugin , SelectionPlugin );


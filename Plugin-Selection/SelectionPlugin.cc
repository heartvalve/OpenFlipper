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
#include <fstream>
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
  waitingForPolyLineSelection_(false),
  polyLineID_(-1)
{
  selectionType_ = VERTEX;

  sourceSelection_ = false;
  deselection_ = false;
  lasso_selection_ = false;
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

  line_node_ = new ACG::SceneGraph::LineNode (ACG::SceneGraph::LineNode::PolygonMode,
                                              PluginFunctions::getRootNode(), "Lasso line");
  line_node_->set_line_width (2.0);
  line_node_->depthFunc (GL_ALWAYS);
  line_node_->setTraverseMode (BaseNode::NodeFirst | BaseNode::SecondPass);
  line_node_->hide();

  //register keys for the plugin
  emit registerKey(Qt::Key_C,      Qt::NoModifier, "Clear Selection");
  emit registerKey(Qt::Key_I,      Qt::NoModifier, "Invert Selection");
  emit registerKey(Qt::Key_Delete, Qt::NoModifier, "Delete Selection");

  //the release event does not contain the modifier
  emit registerKey(Qt::Key_Control, Qt::NoModifier, "Deselect instead of Select", true);
  emit registerKey(Qt::Key_Shift,   Qt::NoModifier,   "Switch between source/target Selection", true);
  //the press event contains the modifier
  emit registerKey(Qt::Key_Control, Qt::ControlModifier, "Deselect instead of Select", true);
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier,   "Switch between source/target Selection", true);

  //different combinations of keyPresses
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier | Qt::ControlModifier, "Source Deselection", true);
  emit registerKey(Qt::Key_Control, Qt::ShiftModifier | Qt::ControlModifier, "Source Deselection", true);

  //register keys for the plugin
  emit registerKey(Qt::Key_F8,      Qt::NoModifier, "Save Selections");
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
  emit addHiddenPickMode(VOLUME_LASSO_SELECTION);
  emit addHiddenPickMode(SURFACE_LASSO_SELECTION);
  emit addHiddenPickMode(CONNECTED_COMPONENT_SELECTION);

  emit setPickModeMouseTracking(PAINT_SPHERE_SELECTION, true);
  emit setPickModeMouseTracking(LASSO_SELECTION, true);
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


  emit addContextMenuItem(contextMenu_->menuAction() , DATA_TRIANGLE_MESH , CONTEXTOBJECTMENU );
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_POLY_MESH     , CONTEXTOBJECTMENU );
#ifdef ENABLE_POLYLINE_SUPPORT
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_POLY_LINE     , CONTEXTOBJECTMENU );
#endif
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_BSPLINE_CURVE , CONTEXTOBJECTMENU );

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
  toggleAction_ = new QAction( QIcon(iconPath + "selection_toggle.png"),
                              "<B>Toggle Selection</B><br>Select or deselect clicked elements.", toolBarActions_ );
  toggleAction_->setCheckable( true );
  toolBar_->addAction( toggleAction_ );
  lassoAction_ = new QAction( QIcon(iconPath + "selection_lasso.png"),
                              "<B>Lasso Selection</B><br>Draw a Lasso to select elements on the surface.", toolBarActions_ );
  lassoAction_->setCheckable( true );
  toolBar_->addAction( lassoAction_ );
  volumeLassoAction_ = new QAction( QIcon(iconPath + "selection_lasso2.png"),
                              "<B>Volume Lasso Selection</B><br>Draw a Lasso to select elements in the drawn volume.", toolBarActions_ );
  volumeLassoAction_->setCheckable( true );
  toolBar_->addAction( volumeLassoAction_ );
  paintSphereAction_ = new QAction( QIcon(iconPath + "selection_paintSphere.png"),
                              "<B>Sphere Selection</B><br>Select elements by painting with a sphere.", toolBarActions_ );
  paintSphereAction_->setCheckable( true );
  toolBar_->addAction( paintSphereAction_ );
  boundaryAction_ = new QAction( QIcon(iconPath + "selection_boundary.png"),
                              "<B>Closest Boundary</B><br>Select the closest boundary to a clicked point.", toolBarActions_ );
  boundaryAction_->setCheckable( true );
  toolBar_->addAction( boundaryAction_ );
  #ifdef ENABLE_POLYLINE_SUPPORT
  surfaceLassoAction_ = new QAction( QIcon(iconPath + "surface-lasso.png"),
                              "<B>Surface Lasso</B><br>Draw a Lasso on the surface.", toolBarActions_ );
  surfaceLassoAction_->setCheckable( true );
  toolBar_->addAction( surfaceLassoAction_ );
  #endif
  connectedAction_ = new QAction( QIcon(iconPath + "selection_connected.png"),
                               "<B>Connected Component</B><br>Select the connect component to a clicked element.", toolBarActions_ );
  connectedAction_->setCheckable( true );
  toolBar_->addAction( connectedAction_ );

  connect( toolBarActions_, SIGNAL( triggered(QAction*) ), this, SLOT(toolBarActionClicked(QAction*)) );
  connect( toolBarTypes_,   SIGNAL( triggered(QAction*) ), this, SLOT(toolBarActionClicked(QAction*)) );


  //connect the toolbar Actions to the Toolbox Buttons
  connect( tool_->objectSelection, SIGNAL(clicked()), objectAction_, SLOT(trigger()) );
  connect( tool_->vertexSelection, SIGNAL(clicked()), vertexAction_, SLOT(trigger()) );
  connect( tool_->edgeSelection,   SIGNAL(clicked()), edgeAction_,   SLOT(trigger()) );
  connect( tool_->faceSelection,   SIGNAL(clicked()), faceAction_,   SLOT(trigger()) );

  connect( objectAction_, SIGNAL(toggled(bool)),tool_->objectSelection , SLOT(setChecked(bool)) );
  connect( vertexAction_, SIGNAL(toggled(bool)),tool_->vertexSelection , SLOT(setChecked(bool)) );
  connect( edgeAction_,   SIGNAL(toggled(bool)),tool_->edgeSelection ,   SLOT(setChecked(bool)) );
  connect( faceAction_,   SIGNAL(toggled(bool)),tool_->faceSelection ,   SLOT(setChecked(bool)) );

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
        emit deleteObject(polyLineID_);
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

  //Selection Mode
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  tool_->objectSelection->setIcon( QIcon(iconPath + "selection_object.png") );
  tool_->vertexSelection->setIcon( QIcon(iconPath + "selection_vertex.png") );
  tool_->edgeSelection->setIcon( QIcon(iconPath + "selection_edge.png") );
  tool_->faceSelection->setIcon( QIcon(iconPath + "selection_face.png") );

  // Set combo box entries for the different modes
  tool_->convertFrom->addItem("Modeling Area");
  tool_->convertFrom->addItem("Handle Area");
  tool_->convertFrom->addItem("Feature Area");

  tool_->convertTo->addItem("Modeling Area");
  tool_->convertTo->addItem("Handle Area");
  tool_->convertTo->addItem("Feature Area");

  // Set combo box entries for the different selection types
  tool_->convertFrom->addItem("Vertex Selection");
  tool_->convertFrom->addItem("Edge Selection");
  tool_->convertFrom->addItem("Face Selection");

  tool_->convertTo->addItem("Vertex Selection");
  tool_->convertTo->addItem("Edge Selection");
  tool_->convertTo->addItem("Face Selection");

  // Check checkboxes by default
  tool_->checkAddArea->setChecked(true);
  tool_->checkConvert->setChecked(true);
  tool_->checkSelectionConvert->setChecked(true);

  // Convert button
  connect(tool_->convertButton, SIGNAL(clicked()), this, SLOT(slotConvertSelectionType()));

  connect (tool_->restrictOnTargets, SIGNAL(clicked()), this, SLOT(slotToggleSelectionRestriction()) );

  //Selection Buttons
  connect( tool_->selectAll,       SIGNAL(clicked()), this,SLOT(slotSelectAll()) );
  connect( tool_->clearSelection,  SIGNAL(clicked()), this,SLOT(slotClearSelection()) );

  connect( tool_->growSelection,   SIGNAL(clicked()), this,SLOT(slotGrowSelection()) );
  connect( tool_->shrinkSelection, SIGNAL(clicked()), this,SLOT(slotShrinkSelection()) );

  connect( tool_->invertSelection, SIGNAL(clicked()), this,SLOT(slotInvertSelection()) );
  connect( tool_->selectBoundary,  SIGNAL(clicked()), this,SLOT(slotSelectBoundary()) );

  connect( tool_->deleteSelection,  SIGNAL(clicked()), this,SLOT(slotDeleteSelection()) );

  connect( tool_->loadSelection, SIGNAL(clicked()), this,SLOT(slotLoadSelection()) );
  connect( tool_->saveSelection, SIGNAL(clicked()), this,SLOT(slotSaveSelection()) );

  // Clear Properties and Selections
  connect( tool_->clearModelingArea, SIGNAL(clicked()), this,SLOT(slotClearArea()) );
  connect( tool_->clearHandleRegion, SIGNAL(clicked()), this,SLOT(slotClearHandle()));
  connect( tool_->clearFeatures,     SIGNAL(clicked()), this,SLOT(slotClearFeatures()));

  connect( tool_->clearVertexSelection, SIGNAL(clicked()), this,SLOT(slotClearAllVertexSelections()) );
  connect( tool_->clearEdgeSelection, SIGNAL(clicked()), this,SLOT(slotClearAllEdgeSelections()));
  connect( tool_->clearFaceSelection, SIGNAL(clicked()), this,SLOT(slotClearAllFaceSelections()));

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
    case Qt::Key_F8 :
        saveSelections();
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
  if ( PluginFunctions::pickMode() == LASSO_SELECTION)               handleLassoSelection(_event, false); else
  if ( PluginFunctions::pickMode() == VOLUME_LASSO_SELECTION)        handleLassoSelection(_event, true); else
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
  volumeLassoAction_->setChecked( _mode == VOLUME_LASSO_SELECTION );
  connectedAction_->setChecked(   _mode == CONNECTED_COMPONENT_SELECTION );

   if ( _mode != PAINT_SPHERE_SELECTION && sphere_node_->visible() ){
      sphere_node_->hide();
      emit updateView();
   }

   if ( _mode == PAINT_SPHERE_SELECTION && sphere_radius_ == 0.1f){
      //init the sphere radius once
      sphere_radius_ = 0.05 * PluginFunctions::sceneRadius();
   }


#ifdef ENABLE_POLYLINE_SUPPORT
  bool check = (_mode == SURFACE_LASSO_SELECTION) || (_mode == "PolyLine" && waitingForPolyLineSelection_);
  surfaceLassoAction_->setChecked(check);

   if (waitingForPolyLineSelection_)
     if (_mode != "" && _mode != "PolyLine" && _mode != SURFACE_LASSO_SELECTION ){
      waitingForPolyLineSelection_ = false;
      emit deleteObject(polyLineID_);
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
#ifdef ENABLE_POLYLINE_SUPPORT
    surfaceLassoAction_->setEnabled( !objectAction_->isChecked() );
#endif
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

    if (_action == toggleAction_)
      PluginFunctions::pickMode( TOGGLE_SELECTION );
    else if (_action == paintSphereAction_)
      PluginFunctions::pickMode( PAINT_SPHERE_SELECTION );
    else if (_action == boundaryAction_)
      PluginFunctions::pickMode( CLOSEST_BOUNDARY_SELECTION );
    else if (_action == lassoAction_)
      PluginFunctions::pickMode( LASSO_SELECTION );
    else if (_action == volumeLassoAction_)
      PluginFunctions::pickMode( VOLUME_LASSO_SELECTION );
    else if (_action == connectedAction_)
      PluginFunctions::pickMode( CONNECTED_COMPONENT_SELECTION );
    else if (_action == surfaceLassoAction_){
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
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		// Add selection to existing modeling area
		// if corresponding checkbox is checked
		if (tool_->checkAddArea->checkState() == Qt::Checked) {
			idList list = getModelingVertices(o_it->id());
			selectVertices(o_it->id(), list);
		}

		if (o_it->dataType(DATA_TRIANGLE_MESH)) {
			set_area(PluginFunctions::triMesh(*o_it));
		}
		if (o_it->dataType(DATA_POLY_MESH)) {
			set_area(PluginFunctions::polyMesh(*o_it));
		}

		o_it->update();
	}

  emit updateView();
}


//******************************************************************************


/** \brief Use current selection to set HANDLE bits
 */
void SelectionPlugin::slotSetHandle() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		// Add selection to existing handle area
		// if corresponding checkbox is checked
		if (tool_->checkAddArea->checkState() == Qt::Checked) {
			idList list = getHandleVertices(o_it->id());
			selectVertices(o_it->id(), list);
		}

		if (o_it->dataType(DATA_TRIANGLE_MESH))
			set_handle(PluginFunctions::triMesh(*o_it));
		if (o_it->dataType(DATA_POLY_MESH))
			set_handle(PluginFunctions::polyMesh(*o_it));
		o_it->update();
	}

	emit updateView();
}


//******************************************************************************


/** \brief convert current selection to feature
 */
void SelectionPlugin::slotSetFeatures() {
	PluginFunctions::IteratorRestriction restriction;
	if (!tool_->restrictOnTargets->isChecked())
		restriction = PluginFunctions::ALL_OBJECTS;
	else
		restriction = PluginFunctions::TARGET_OBJECTS;

	for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); o_it
			!= PluginFunctions::objectsEnd(); ++o_it) {

		if (o_it->dataType(DATA_TRIANGLE_MESH))
			set_features(PluginFunctions::triMesh(*o_it));
		if (o_it->dataType(DATA_POLY_MESH))
			set_features(PluginFunctions::polyMesh(*o_it));
		o_it->update();
	}

	emit updateView();
}

//******************************************************************************


/** \brief Clear Handle Bits
 */
void SelectionPlugin::slotClearHandle() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
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
  if ( !tool_->restrictOnTargets->isChecked() )
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

/** \brief Clear Features
 */
void SelectionPlugin::slotClearFeatures() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_features(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_features(PluginFunctions::polyMesh(*o_it));
    o_it->update();
  }

  emit updateView();
}

//******************************************************************************

/** \brief Delete all selected elements of the target meshes
 */
void SelectionPlugin::slotDeleteSelection() {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
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


/** \brief Toggle the selection Restriction
 *
 */
void SelectionPlugin::slotToggleSelectionRestriction(){
  if ( tool_->restrictOnTargets->isChecked() )
      tool_->restrictOnTargets->setText("Select on target objects only");
  else
      tool_->restrictOnTargets->setText("Select on all objects");
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

//******************************************************************************

void SelectionPlugin::slotConvertSelectionType() {

	QString sfrom, sto;

	sfrom = tool_->convertFrom->currentText();
	sto = tool_->convertTo->currentText();

	// Clear selection after selection conversion
	bool unselectAfter = tool_->checkSelectionConvert->checkState() == Qt::Checked;
	// Clear area after area conversion
	bool clearAfter = tool_->checkConvert->checkState() == Qt::Checked;

	if(sfrom == tr("Vertex Selection") && sto == tr("Edge Selection")) {
		convertVtoESelection(unselectAfter);
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Face Selection")) {
		convertVtoFSelection(unselectAfter);
	}
	else if(sfrom == tr("Edge Selection") && sto == tr("Vertex Selection")) {
		convertEtoVSelection(unselectAfter);
	}
	else if(sfrom == tr("Edge Selection") && sto == tr("Face Selection")) {
		convertEtoFSelection(unselectAfter);
	}
	else if(sfrom == tr("Face Selection") && sto == tr("Vertex Selection")) {
		convertFtoVSelection(unselectAfter);
	}
	else if(sfrom == tr("Face Selection") && sto == tr("Edge Selection")) {
		convertFtoESelection(unselectAfter);
	}
	else if(sfrom == tr("Modeling Area") && sto == tr("Handle Area")) {
		StatusBits from = AREA;
		StatusBits to = HANDLEAREA;
		convertSelectionType(from, to, clearAfter);
	}
	else if(sfrom == tr("Handle Area") && sto == tr("Modeling Area")) {
		StatusBits from = HANDLEAREA;
		StatusBits to = AREA;
		convertSelectionType(from, to, clearAfter);
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Modeling Area")) {
		slotSetArea();
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Handle Area")) {
		slotSetHandle();
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Feature Area")) {
		slotSetFeatures();
	}
	// Feature still to be implemented
}

//******************************************************************************

void SelectionPlugin::convertSelectionType(StatusBits _from, StatusBits _to, bool _clearAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	if(_from == AREA && _to == HANDLEAREA) {

		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {

			// Delete current handle region if
			// checkbox "Add to area" is NOT checked
			if (tool_->checkAddArea->checkState() != Qt::Checked &&
					selectionType_ == VERTEX) {
				clearHandleVertices(o_it->id());
			}

			idList list = getModelingVertices(o_it->id());
			if(_clearAfter) { clearModelingVertices(o_it->id()); }
			selectHandleVertices(o_it->id(), list);

			o_it->update();
		}
	}
	else if(_from == HANDLEAREA && _to == AREA) {

		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {

			// Delete current modeling area if
			// checkbox "Add to area" is NOT checked
			if (tool_->checkAddArea->checkState() != Qt::Checked &&
					selectionType_ == VERTEX) {
				clearModelingVertices(o_it->id());
			}

			idList list = getHandleVertices(o_it->id());
			if(_clearAfter) { clearHandleVertices(o_it->id()); }
			selectModelingVertices(o_it->id(), list);

			o_it->update();
		}
	}

	// NOTE: Feature conversion is still to be implemented

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::convertVtoESelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getVertexSelection(o_it->id());
		if(_unselectAfter) { clearVertexSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertVertexToEdgeSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertVertexToEdgeSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

void SelectionPlugin::convertVtoFSelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getVertexSelection(o_it->id());
		if(_unselectAfter) { clearVertexSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertVertexToFaceSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertVertexToFaceSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

void SelectionPlugin::convertEtoVSelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getEdgeSelection(o_it->id());
		if(_unselectAfter) { clearEdgeSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertEdgeToVertexSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertEdgeToVertexSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

void SelectionPlugin::convertEtoFSelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getEdgeSelection(o_it->id());
		if(_unselectAfter) { clearEdgeSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertEdgeToFaceSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertEdgeToFaceSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

void SelectionPlugin::convertFtoVSelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getFaceSelection(o_it->id());
		if(_unselectAfter) { clearFaceSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertFaceToVertexSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertFaceToVertexSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

void SelectionPlugin::convertFtoESelection(bool _unselectAfter) {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		idList list = getFaceSelection(o_it->id());
		if(_unselectAfter) { clearFaceSelection(o_it->id()); }

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertFaceToEdgeSelection(PluginFunctions::triMesh(o_it), list);
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertFaceToEdgeSelection(PluginFunctions::polyMesh(o_it), list);
		}

		o_it->update();
	}

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::slotClearAllVertexSelections() {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		clearVertexSelection(o_it->id());

		o_it->update();
	}

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::slotClearAllEdgeSelections() {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		clearEdgeSelection(o_it->id());

		o_it->update();
	}

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::slotClearAllFaceSelections() {

	PluginFunctions::IteratorRestriction restriction;
	if ( !tool_->restrictOnTargets->isChecked() ) {
		restriction = PluginFunctions::ALL_OBJECTS;
	}
	else {
		restriction = PluginFunctions::TARGET_OBJECTS;
	}

	for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
		o_it != PluginFunctions::objectsEnd(); ++o_it)   {

		clearFaceSelection(o_it->id());

		o_it->update();
	}

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::saveSelections()
{
  // process all target meshes
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS ,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));

  for ( ; o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    QString suggest = o_it->name() + ".sel";

    QString filename = QFileDialog::getSaveFileName(0, tr("Save Selection"), suggest, tr("Selections (*.sel )"));

    if (!filename.isEmpty())
      saveSelection( o_it->id(), filename);
  }

}

//******************************************************************************

void SelectionPlugin::saveSelection( int _objectId , QString _filename)
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,"saveSelection: unable to get object" );
    return;
  }

  if( object->dataType( DATA_TRIANGLE_MESH))
    saveSelection( *PluginFunctions::triMesh(object), _filename);
  else
    if( object->dataType( DATA_POLY_MESH))
      saveSelection( *PluginFunctions::polyMesh(object), _filename);
#ifdef ENABLE_POLYLINE_SUPPORT
    else
      if( object->dataType( DATA_POLY_LINE))
	;
#endif
#ifdef ENABLE_BSPLINECURVE_SUPPORT
      else
	if( object->dataType( DATA_BSPLINE_CURVE))
	  ;
#endif
	else
	  emit log(LOGERR,"saveSelection : Unsupported object Type" );
}

//******************************************************************************

void SelectionPlugin::loadSelection( int _objectId  , QString _filename)
{

  if ( !_filename.isEmpty() ){

    INIFile ini;

    if ( ! ini.connect(_filename,false) ) {
      emit log(LOGERR,"Failed to load Selection from file " + _filename);
      return;
    }

    loadIniFile(ini, _objectId );

    ini.disconnect();
  }
}

//******************************************************************************


void SelectionPlugin::saveSelection( TriMesh& _mesh, QString _filename)
{
  std::vector<int> vsel;
  std::vector<int> esel;
  std::vector<int> fsel;

  // get vertex selections
  TriMesh::VertexIter v_it  = _mesh.vertices_begin();
  TriMesh::VertexIter v_end = _mesh.vertices_end();

  for(; v_it != v_end; ++v_it)
  {
    if( _mesh.status(v_it.handle()).selected())
      vsel.push_back( v_it.handle().idx());
  }

  // get edge selections
  TriMesh::EdgeIter e_it  = _mesh.edges_begin();
  TriMesh::EdgeIter e_end = _mesh.edges_end();

  for(; e_it != e_end; ++e_it)
  {
    if( _mesh.status(e_it.handle()).selected())
      esel.push_back( e_it.handle().idx());
  }

  // get face selections
  TriMesh::FaceIter f_it  = _mesh.faces_begin();
  TriMesh::FaceIter f_end = _mesh.faces_end();

  for(; f_it != f_end; ++f_it)
  {
    if( _mesh.status(f_it.handle()).selected())
      fsel.push_back( f_it.handle().idx());
  }

  std::ofstream fout( _filename.toAscii().data(), std::ios::out);

  // write number of selected items
  fout << vsel.size() << " " << esel.size() << " " << fsel.size() << std::endl;

  // write vertex selections
  for(unsigned int i=0; i<vsel.size(); ++i)
    fout << vsel[i] << std::endl;

  // write edge selections
  for(unsigned int i=0; i<esel.size(); ++i)
    fout << esel[i] << std::endl;

  // write face selections
  for(unsigned int i=0; i<fsel.size(); ++i)
    fout << fsel[i] << std::endl;

  fout.close();
}


//******************************************************************************


void SelectionPlugin::saveSelection( PolyMesh& _mesh, QString _filename)
{
  std::vector<int> vsel;
  std::vector<int> esel;
  std::vector<int> fsel;

  // get vertex selections
  PolyMesh::VertexIter v_it  = _mesh.vertices_begin();
  PolyMesh::VertexIter v_end = _mesh.vertices_end();

  for(; v_it != v_end; ++v_it)
  {
    if( _mesh.status(v_it.handle()).selected())
      vsel.push_back( v_it.handle().idx());
  }

  // get edge selections
  PolyMesh::EdgeIter e_it  = _mesh.edges_begin();
  PolyMesh::EdgeIter e_end = _mesh.edges_end();

  for(; e_it != e_end; ++e_it)
  {
    if( _mesh.status(e_it.handle()).selected())
      esel.push_back( e_it.handle().idx());
  }

  // get face selections
  PolyMesh::FaceIter f_it  = _mesh.faces_begin();
  PolyMesh::FaceIter f_end = _mesh.faces_end();

  for(; f_it != f_end; ++f_it)
  {
    if( _mesh.status(f_it.handle()).selected())
      fsel.push_back( f_it.handle().idx());
  }

  std::ofstream fout( _filename.toAscii().data(), std::ios::out);

  // write number of selected items
  fout << vsel.size() << " " << esel.size() << " " << fsel.size() << std::endl;

  // write vertex selections
  for(unsigned int i=0; i<vsel.size(); ++i)
    fout << vsel[i] << std::endl;

  // write edge selections
  for(unsigned int i=0; i<esel.size(); ++i)
    fout << esel[i] << std::endl;

  // write face selections
  for(unsigned int i=0; i<fsel.size(); ++i)
    fout << fsel[i] << std::endl;

  fout.close();
}


Q_EXPORT_PLUGIN2( selectionplugin , SelectionPlugin );


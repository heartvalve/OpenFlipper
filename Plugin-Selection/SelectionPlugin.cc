/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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
#include <OpenFlipper/common/Types.hh>

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
  
  tool_ = new selectionToolbarWidget();
  QSize size(300, 300);
  tool_->resize(size);
  
  //Selection Mode
  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();
  
  tool_->objectSelection->setIcon( QIcon(iconPath + "selection_object.png") );
  tool_->vertexSelection->setIcon( QIcon(iconPath + "selection_vertex.png") );
  tool_->edgeSelection->setIcon( QIcon(iconPath + "selection_edge.png") );
  tool_->faceSelection->setIcon( QIcon(iconPath + "selection_face.png") );
  
  // Set combo box entries for the different modes
  tool_->convertFrom->addItem(tr("Modeling Area"));
  tool_->convertFrom->addItem(tr("Handle Area"));
  tool_->convertFrom->addItem(tr("Feature Area"));
  
  tool_->convertTo->addItem(tr("Modeling Area"));
  tool_->convertTo->addItem(tr("Handle Area"));
  tool_->convertTo->addItem(tr("Feature Area"));
  
  // Set combo box entries for the different selection types
  tool_->convertFrom->addItem(tr("Vertex Selection"));
  tool_->convertFrom->addItem(tr("Edge Selection"));
  tool_->convertFrom->addItem(tr("Face Selection"));
  
  tool_->convertTo->addItem(tr("Vertex Selection"));
  tool_->convertTo->addItem(tr("Edge Selection"));
  tool_->convertTo->addItem(tr("Face Selection"));
  
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
  
  connect( tool_->colorizeSelection,SIGNAL(clicked()), this,SLOT(slotColorizeSelection()) );
  
  connect( tool_->loadSelection, SIGNAL(clicked()), this,SLOT(slotLoadSelection()) );
  connect( tool_->saveSelection, SIGNAL(clicked()), this,SLOT(slotSaveSelection()) );
  
  // Clear Properties and Selections
  connect( tool_->clearModelingArea, SIGNAL(clicked()), this,SLOT(slotClearArea()) );
  connect( tool_->clearHandleRegion, SIGNAL(clicked()), this,SLOT(slotClearHandle()));
  connect( tool_->clearFeatures,     SIGNAL(clicked()), this,SLOT(slotClearFeatures()));
  
  connect( tool_->clearVertexSelection, SIGNAL(clicked()), this,SLOT(slotClearAllVertexSelections()) );
  connect( tool_->clearEdgeSelection, SIGNAL(clicked()), this,SLOT(slotClearAllEdgeSelections()));
  connect( tool_->clearFaceSelection, SIGNAL(clicked()), this,SLOT(slotClearAllFaceSelections()));
  
  emit addToolbox("Selections",tool_);

  // create sphere for paint sphere selection
  std::string nodeName = std::string( tr("Selection Plugin: Selection Sphere Material").toUtf8() );
  sphere_mat_node_  = new ACG::SceneGraph::MaterialNode(0, nodeName );
  PluginFunctions::addGlobalNode(sphere_mat_node_);
  sphere_mat_node_->applyProperties( MaterialNode::Blending  |
                                     MaterialNode::Material  |
                                     MaterialNode::AlphaTest |
                                     MaterialNode::BackFaceCulling );
  sphere_mat_node_->set_color(ACG::Vec4f(1.0, 0.0, 0.0, 0.3));
  sphere_mat_node_->enable_blending();
  sphere_mat_node_->disable_alpha_test();
  sphere_mat_node_->enable_backface_culling();

  nodeName = std::string( tr("Selection Plugin: Selection Sphere").toUtf8() );
  sphere_node_ = new ACG::SceneGraph::GlutPrimitiveNode(ACG::SceneGraph::GlutPrimitiveNode::SPHERE, sphere_mat_node_,nodeName);
  sphere_node_->drawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);
  sphere_node_->get_primitive(0).color = ACG::Vec4f(1.0, 0.0, 0.0,0.3);
  sphere_node_->hide();

  sphere_radius_ = 0.1 * PluginFunctions::sceneRadius();

  sphere_selection_ = false;

  nodeName = std::string( tr("Selection Plugin: Lasso Selection Line").toUtf8() );
  line_node_ = new ACG::SceneGraph::LineNode (ACG::SceneGraph::LineNode::PolygonMode,0, nodeName );
  PluginFunctions::addGlobalNode(line_node_);                                              
  line_node_->set_line_width (2.0);
  line_node_->depthFunc (GL_ALWAYS);
  line_node_->setTraverseMode (BaseNode::NodeFirst | BaseNode::SecondPass);
  line_node_->hide();

  //register keys for the plugin
  emit registerKey(Qt::Key_C,      Qt::NoModifier, tr("Clear Selection"));
  emit registerKey(Qt::Key_I,      Qt::NoModifier, tr("Invert Selection"));
  emit registerKey(Qt::Key_Delete, Qt::NoModifier, tr("Delete Selection"));

  //the release event does not contain the modifier
  emit registerKey(Qt::Key_Control, Qt::NoModifier, tr("Deselect instead of Select"), true);
  emit registerKey(Qt::Key_Shift,   Qt::NoModifier,   tr("Switch between source/target Selection"), true);
  //the press event contains the modifier
  emit registerKey(Qt::Key_Control, Qt::ControlModifier, tr("Deselect instead of Select"), true);
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier,   tr("Switch between source/target Selection"), true);

  //different combinations of keyPresses
  emit registerKey(Qt::Key_Shift,   Qt::ShiftModifier | Qt::ControlModifier, tr("Source Deselection"), true);
  emit registerKey(Qt::Key_Control, Qt::ShiftModifier | Qt::ControlModifier, tr("Source Deselection"), true);

  //register keys for the plugin
  emit registerKey(Qt::Key_F8,      Qt::NoModifier, tr("Save Selections"));
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
  emit addHiddenPickMode(FLOOD_FILL_SELECTION);
  
  emit addPickMode(CREATEMESH);

  emit setPickModeMouseTracking(PAINT_SPHERE_SELECTION, true);
  emit setPickModeMouseTracking(LASSO_SELECTION, true);
  emit setPickModeCursor(SURFACE_LASSO_SELECTION, Qt::CrossCursor);

  // CONTEXT MENU
  contextMenu_ = new QMenu(tr("Select"));

  QAction* lastAction;

  lastAction = contextMenu_->addAction( tr("All") );
  lastAction->setToolTip(tr("Select all"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( tr("Clear") );
  lastAction->setToolTip(tr("Clear selection"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( tr("Invert") );
  lastAction->setToolTip(tr("Invert selection"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( tr("Shrink") );
  lastAction->setToolTip(tr("Deselect the boundary of the current selection"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( tr("Grow") );
  lastAction->setToolTip(tr("Extend the selection at their boundary"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( tr("Boundary") );
  lastAction->setToolTip(tr("Select the boundary of the object"));
  lastAction->setStatusTip( lastAction->toolTip() );

  lastAction = contextMenu_->addAction( "Colorize" );
  lastAction->setToolTip("Change the color of selected elements.");
  lastAction->setStatusTip( lastAction->toolTip() );

  emit addContextMenuItem(contextMenu_->menuAction() , DATA_TRIANGLE_MESH , CONTEXTOBJECTMENU );
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_POLY_MESH     , CONTEXTOBJECTMENU );
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_TSPLINE_MESH  , CONTEXTOBJECTMENU );
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  emit addContextMenuItem(contextMenu_->menuAction() , DATA_POLY_LINE     , CONTEXTOBJECTMENU );
#endif
  emit addContextMenuItem(contextMenu_->menuAction() , typeId("BSplineCurve") , CONTEXTOBJECTMENU );

  connect( contextMenu_ , SIGNAL( triggered(QAction*) ),
           this,          SLOT(selectionContextMenu(QAction*)) );



  // TOOLBAR
  toolBar_ = new QToolBar(tr("Selection"));
  emit addToolbar(toolBar_);

  // TOOLBAR - SELECTION TYPES
  toolBarActions_ = new QActionGroup(toolBar_);
  toolBarTypes_ = new QActionGroup(toolBar_);
  toolBarTypes_->setExclusive( true );

  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  objectAction_ = new QAction( QIcon(iconPath + "selection_object.png"), tr("Enable Object Selection"), toolBarTypes_ );
  objectAction_->setCheckable( true );
  objectAction_->setChecked( false );
  toolBar_->addAction( objectAction_ );
  vertexAction_ = new QAction( QIcon(iconPath + "selection_vertex.png"), tr("Enable Vertex Selection"), toolBarTypes_ );
  vertexAction_->setCheckable( true );
  vertexAction_->setChecked( true );
  toolBar_->addAction( vertexAction_ );
  edgeAction_ = new QAction( QIcon(iconPath + "selection_edge.png"), tr("Enable Edge Selection"), toolBarTypes_ );
  edgeAction_->setCheckable( true );
  toolBar_->addAction( edgeAction_ );
  faceAction_ = new QAction( QIcon(iconPath + "selection_face.png"), tr("Enable Face Selection"), toolBarTypes_ );
  faceAction_->setCheckable( true );
  toolBar_->addAction( faceAction_ );
  toolBar_->addSeparator();

  // TOOLBAR - SELECTION ACTIONS
  toggleAction_ = new QAction( QIcon(iconPath + "selection_toggle.png"),
                              tr("<B>Toggle Selection</B><br>Select or deselect clicked elements."), toolBarActions_ );
  toggleAction_->setCheckable( true );
  toolBar_->addAction( toggleAction_ );
  lassoAction_ = new QAction( QIcon(iconPath + "selection_lasso.png"),
                              tr("<B>Lasso Selection</B><br>Draw a Lasso to select elements on the surface."), toolBarActions_ );
  lassoAction_->setCheckable( true );
  toolBar_->addAction( lassoAction_ );
  volumeLassoAction_ = new QAction( QIcon(iconPath + "selection_lasso2.png"),
                              tr("<B>Volume Lasso Selection</B><br>Draw a Lasso to select elements in the drawn volume."), toolBarActions_ );
  volumeLassoAction_->setCheckable( true );
  toolBar_->addAction( volumeLassoAction_ );
  paintSphereAction_ = new QAction( QIcon(iconPath + "selection_paintSphere.png"),
                              tr("<B>Sphere Selection</B><br>Select elements by painting with a sphere."), toolBarActions_ );
  paintSphereAction_->setCheckable( true );
  toolBar_->addAction( paintSphereAction_ );
  boundaryAction_ = new QAction( QIcon(iconPath + "selection_boundary.png"),
                              tr("<B>Closest Boundary</B><br>Select the closest boundary to a clicked point."), toolBarActions_ );
  boundaryAction_->setCheckable( true );
  toolBar_->addAction( boundaryAction_ );
  #ifdef ENABLE_POLYLINE_SUPPORT
  surfaceLassoAction_ = new QAction( QIcon(iconPath + "surface-lasso.png"),
                              tr("<B>Surface Lasso</B><br>Draw a Lasso on the surface."), toolBarActions_ );
  surfaceLassoAction_->setCheckable( true );
  toolBar_->addAction( surfaceLassoAction_ );
  #endif
  connectedAction_ = new QAction( QIcon(iconPath + "selection_connected.png"),
                               tr("<B>Connected Component</B><br>Select the connect component to a clicked element."), toolBarActions_ );
  connectedAction_->setCheckable( true );

  toolBar_->addAction( connectedAction_ );
  floodFillAction_ = new QAction( QIcon(iconPath + "selection_floodFill.png"),
                               tr("<B>Flood Fill</B><br>Select a planar region surrounding the clicked element."), toolBarActions_ );
  floodFillAction_->setCheckable( true );
  toolBar_->addAction( floodFillAction_ );
  
  //createMeshFromSelection
  createMeshFromSelAction_ = new QAction(tr("&Create Mesh from selection"), toolBarActions_);
  createMeshFromSelAction_->setCheckable( true );
  createMeshFromSelAction_->setStatusTip(tr("Create a mesh from the current selection of a target object"));
  createMeshFromSelAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"selection_createMeshSelection.png") );
  
  connect(createMeshFromSelAction_, SIGNAL(triggered()), this, SLOT(slotCreateMeshFromSelection()) );
  
  toolBar_->addAction( createMeshFromSelAction_ );

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
void SelectionPlugin::slotObjectUpdated(int _id, const UpdateType _type){
#ifdef ENABLE_POLYLINE_SUPPORT

  if (waitingForPolyLineSelection_)
    if ( _type == UPDATE_ALL || _type == UPDATE_GEOMETRY ){
    
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
        return;
        break;
    case Qt::Key_F8 :
        saveSelections();
        return;
        break;
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
  if ( PluginFunctions::pickMode() == CONNECTED_COMPONENT_SELECTION) componentSelection(_event); else
  if ( PluginFunctions::pickMode() == FLOOD_FILL_SELECTION)          floodFillSelection(_event); else
  if ( PluginFunctions::pickMode() == CREATEMESH )                   createMeshFromSelection( _event );
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
      
      emit nodeVisibilityChanged( -1 );
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
  floodFillAction_->setChecked(   _mode == FLOOD_FILL_SELECTION );
  createMeshFromSelAction_->setChecked(_mode == CREATEMESH );

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

    if ( PluginFunctions::actionMode() == Viewer::ExamineMode )
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
      emit log(LOGERR,tr("Choose at least one selection type first."));
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
    else if (_action == floodFillAction_)
      PluginFunctions::pickMode( FLOOD_FILL_SELECTION );
    else if (_action == createMeshFromSelAction_)
         PluginFunctions::pickMode( CREATEMESH );
    else if (_action == surfaceLassoAction_){
      waitingForPolyLineSelection_ = true;
      PluginFunctions::pickMode("PolyLine");
    }
  }
}

//******************************************************************************

/** \brief Use current selection to set AREA bits
 */
void SelectionPlugin::slotSetArea(unsigned char _selectionType) {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

#ifdef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH)) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifndef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif


		// Add selection to existing modeling area
		// if corresponding checkbox is checked
		if (tool_->checkAddArea->checkState() == Qt::Checked) {
			IdList list = getModelingVertices(o_it->id());
			selectVertices(o_it->id(), list);
		}

		if (o_it->dataType(DATA_TRIANGLE_MESH)) {
			set_area(PluginFunctions::triMesh(*o_it), _selectionType);
		}
		if (o_it->dataType(DATA_POLY_MESH)) {
			set_area(PluginFunctions::polyMesh(*o_it), _selectionType);
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		if (o_it->dataType(DATA_TSPLINE_MESH)) {
			set_area(PluginFunctions::tsplineMesh(*o_it), _selectionType);
		}
#endif


		emit updatedObject(o_it->id(), UPDATE_SELECTION);
	}

  emit updateView();
}


//******************************************************************************


/** \brief Use current selection to set HANDLE bits
 */
void SelectionPlugin::slotSetHandle(unsigned char _selectionType) {
  PluginFunctions::IteratorRestriction restriction;
  if ( !tool_->restrictOnTargets->isChecked() )
    restriction = PluginFunctions::ALL_OBJECTS;
  else
    restriction = PluginFunctions::TARGET_OBJECTS;

#ifndef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		// Add selection to existing handle area
		// if corresponding checkbox is checked
		if (tool_->checkAddArea->checkState() == Qt::Checked) {
			IdList list = getHandleVertices(o_it->id());
			selectVertices(o_it->id(), list);
		}

		if (o_it->dataType(DATA_TRIANGLE_MESH))
			set_handle(PluginFunctions::triMesh(*o_it), _selectionType);
		if (o_it->dataType(DATA_POLY_MESH))
			set_handle(PluginFunctions::polyMesh(*o_it), _selectionType);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		if (o_it->dataType(DATA_TSPLINE_MESH))
			set_handle(PluginFunctions::tsplineMesh(*o_it), _selectionType);
#endif
		emit updatedObject(o_it->id(), UPDATE_SELECTION);
	}

	emit updateView();
}


//******************************************************************************


/** \brief convert current selection to feature
 */
void SelectionPlugin::slotSetFeatures(unsigned char _selectionType) {
	PluginFunctions::IteratorRestriction restriction;
	if (!tool_->restrictOnTargets->isChecked())
		restriction = PluginFunctions::ALL_OBJECTS;
	else
		restriction = PluginFunctions::TARGET_OBJECTS;

#ifndef ENABLE_TSPLINEMESH_SUPPORT
	for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); o_it
			!= PluginFunctions::objectsEnd(); ++o_it) {
#endif

#ifdef ENABLE_TSPLINEMESH_SUPPORT
	for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH )); o_it
			!= PluginFunctions::objectsEnd(); ++o_it) {
#endif
		if (o_it->dataType(DATA_TRIANGLE_MESH))
			set_features(PluginFunctions::triMesh(*o_it), _selectionType);
		if (o_it->dataType(DATA_POLY_MESH))
			set_features(PluginFunctions::polyMesh(*o_it), _selectionType);

#ifdef ENABLE_TSPLINEMESH_SUPPORT
		if (o_it->dataType(DATA_TSPLINE_MESH))
			set_features(PluginFunctions::tsplineMesh(*o_it), _selectionType);
#endif
		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_handle(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_handle(PluginFunctions::polyMesh(*o_it));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    if ( o_it->dataType( DATA_TSPLINE_MESH ) )
        clear_handle(PluginFunctions::tsplineMesh(*o_it));
#endif
    emit updatedObject(o_it->id(), UPDATE_ALL);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_area(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_area(PluginFunctions::polyMesh(*o_it));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    if ( o_it->dataType( DATA_TSPLINE_MESH ) )
        clear_area(PluginFunctions::tsplineMesh(*o_it));
#endif
    emit updatedObject(o_it->id(), UPDATE_ALL);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH )) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it) {
#endif
    if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
        clear_features(PluginFunctions::triMesh(*o_it));
    if ( o_it->dataType( DATA_POLY_MESH ) )
        clear_features(PluginFunctions::polyMesh(*o_it));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    if ( o_it->dataType( DATA_TSPLINE_MESH ) )
        clear_features(PluginFunctions::tsplineMesh(*o_it));
#endif
    emit updatedObject(o_it->id(), UPDATE_ALL);
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

  DataType types( DATA_TRIANGLE_MESH | DATA_POLY_MESH );

  #ifdef ENABLE_TSPLINEMESH_SUPPORT
    types |= DATA_TSPLINE_MESH;
  #endif

  #ifdef ENABLE_SKELETON_SUPPORT
    types |= DATA_SKELETON;
  #endif

  for ( PluginFunctions::ObjectIterator o_it(restriction, types ); o_it != PluginFunctions::objectsEnd(); ++o_it) {

    if ( o_it->visible() ){

      if ( o_it->dataType( DATA_TRIANGLE_MESH ) )
          deleteSelection(PluginFunctions::triMesh(*o_it));
      if ( o_it->dataType( DATA_POLY_MESH ) )
          deleteSelection(PluginFunctions::polyMesh(*o_it));
      #ifdef ENABLE_TSPLINEMESH_SUPPORT
      if ( o_it->dataType( DATA_TSPLINE_MESH ) )
          deleteSelection(PluginFunctions::tsplineMesh(*o_it));
      #endif
      #ifdef ENABLE_SKELETON_SUPPORT
      if ( o_it->dataType( DATA_SKELETON ) )
          deleteSelection(PluginFunctions::skeleton(*o_it));
      #endif
      
      emit updatedObject(o_it->id(), UPDATE_ALL);
    }
  }
  emit updateView();
}


/** \brief Toggle the selection Restriction
 *
 */
void SelectionPlugin::slotToggleSelectionRestriction(){
  if ( tool_->restrictOnTargets->isChecked() )
      tool_->restrictOnTargets->setText(tr("Select on target objects only"));
  else
      tool_->restrictOnTargets->setText(tr("Select on all objects"));
}

//******************************************************************************

/** \brief CreateMeshFromSelection Button
*
*/
void SelectionPlugin::slotCreateMeshFromSelection( )
{
    PluginFunctions::actionMode( Viewer::PickingMode );
    PluginFunctions::pickMode( CREATEMESH );
}

//******************************************************************************

/** \brief Create Mesh from Selection (triggered by pickMode)
*
* @param _event the mouse event that occured
*/
void SelectionPlugin::createMeshFromSelection(QMouseEvent * _event)
{
    if (_event->type() == QEvent::MouseButtonPress )
    {
        unsigned int     node_idx, target_idx;
        ACG::Vec3d*      sourcePoint3D = 0;
        
        if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                                            _event->pos(),
                                            node_idx,
                                            target_idx,
                                            sourcePoint3D))
        {
            BaseObjectData *obj;
            PluginFunctions::getPickedObject(node_idx, obj);
            createMeshFromSelection( obj->id() );
        }
    }
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
     emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
     return;
   }

   std::vector< int > ids;

   // The objects section should already exist
   QString sectionName = object->name();
   if ( !_ini.section_exists( sectionName ) ) {
     emit log(LOGERR,tr("Cannot find object section id ") + QString::number(_id) + tr(" in saveFile") );
     return;
   }

  _ini.add_entry(sectionName , "VertexSelection" , getVertexSelection( object->id()) );
  _ini.add_entry(sectionName , "EdgeSelection"   , getEdgeSelection  ( object->id()) );

  if ( object->dataType( DATA_POLY_MESH ) || object->dataType( DATA_TRIANGLE_MESH )) {
    _ini.add_entry(sectionName , "FaceSelection"  , getFaceSelection   ( object->id() ) );
    _ini.add_entry(sectionName , "HandleRegion"   , getHandleVertices  ( object->id() ) );
    _ini.add_entry(sectionName , "ModelingRegion" , getModelingVertices( object->id() ) );
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  if ( object->dataType( DATA_TSPLINE_MESH )) {
    _ini.add_entry(sectionName , "FaceSelection"  , getFaceSelection   ( object->id() ) );
    _ini.add_entry(sectionName , "HandleRegion"   , getHandleVertices  ( object->id() ) );
    _ini.add_entry(sectionName , "ModelingRegion" , getModelingVertices( object->id() ) );
  }
#endif

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
    emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
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
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    if ( object->dataType( DATA_TSPLINE_MESH ) ) {
      update_regions(PluginFunctions::tsplineMesh(object));
    }
#endif

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
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    if ( object->dataType( DATA_TSPLINE_MESH ) ) {
      update_regions(PluginFunctions::tsplineMesh(object));
    }
#endif

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

  if ( updated_modeling_regions ) {
    
    emit updatedObject(object->id(), UPDATE_ALL);
    emit updateView();
    
  } else if ( updated_selection ) {

    emit updatedObject(object->id(), UPDATE_SELECTION);
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
		slotSetArea( VERTEX );
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Handle Area")) {
		slotSetHandle( VERTEX );
	}
	else if(sfrom == tr("Vertex Selection") && sto == tr("Feature Area")) {
		slotSetFeatures( VERTEX );
	}
  else if(sfrom == tr("Edge Selection") && sto == tr("Modeling Area")) {
    slotSetArea( EDGE );
  }
  else if(sfrom == tr("Edge Selection") && sto == tr("Handle Area")) {
    slotSetHandle( EDGE );
  }
  else if(sfrom == tr("Edge Selection") && sto == tr("Feature Area")) {
    slotSetFeatures( EDGE );
  }
  else if(sfrom == tr("Face Selection") && sto == tr("Modeling Area")) {
    slotSetArea( FACE );
  }
  else if(sfrom == tr("Face Selection") && sto == tr("Handle Area")) {
    slotSetHandle( FACE );
  }
  else if(sfrom == tr("Face Selection") && sto == tr("Feature Area")) {
    slotSetFeatures( FACE );
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

			// Delete current handle region if
			// checkbox "Add to area" is NOT checked
			if (tool_->checkAddArea->checkState() != Qt::Checked &&
					selectionType_ == VERTEX) {
				clearHandleVertices(o_it->id());
			}

			IdList list = getModelingVertices(o_it->id());
			if(_clearAfter) { clearModelingVertices(o_it->id()); }
			selectHandleVertices(o_it->id(), list);

			emit updatedObject(o_it->id(), UPDATE_ALL);
		}
	}
	else if(_from == HANDLEAREA && _to == AREA) {

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

			// Delete current modeling area if
			// checkbox "Add to area" is NOT checked
			if (tool_->checkAddArea->checkState() != Qt::Checked &&
					selectionType_ == VERTEX) {
				clearModelingVertices(o_it->id());
			}

			IdList list = getHandleVertices(o_it->id());
			if(_clearAfter) { clearHandleVertices(o_it->id()); }
			selectModelingVertices(o_it->id(), list);

			emit updatedObject(o_it->id(), UPDATE_ALL);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertVertexToEdgeSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertVertexToEdgeSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertVertexToEdgeSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearVertexSelection(o_it->id()); }

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertVertexToFaceSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertVertexToFaceSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertVertexToFaceSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearVertexSelection(o_it->id()); }

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertEdgeToVertexSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertEdgeToVertexSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertEdgeToVertexSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearEdgeSelection(o_it->id()); }

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertEdgeToFaceSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertEdgeToFaceSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertEdgeToFaceSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearEdgeSelection(o_it->id()); }

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertFaceToVertexSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertFaceToVertexSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertFaceToVertexSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearFaceSelection(o_it->id()); }
    
		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		if ( o_it->dataType() == DATA_TRIANGLE_MESH ) {
			MeshSelection::convertFaceToEdgeSelection(PluginFunctions::triMesh(o_it));
		}
		else if ( o_it->dataType() == DATA_POLY_MESH ) {
			MeshSelection::convertFaceToEdgeSelection(PluginFunctions::polyMesh(o_it));
		}
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		else if ( o_it->dataType() == DATA_TSPLINE_MESH ) {
			MeshSelection::convertFaceToEdgeSelection(PluginFunctions::tsplineMesh(o_it));
		}
#endif

    if(_unselectAfter) { clearFaceSelection(o_it->id()); }
    
		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

  std::cerr << "clearing vertex selections!" << std::endl;
#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
      std::cerr << "CLEAR VERTEX TSPLINES !" << std::endl;
#endif

		clearVertexSelection(o_it->id());

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		clearEdgeSelection(o_it->id());

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
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

#ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif

		clearFaceSelection(o_it->id());

		emit updatedObject(o_it->id(), UPDATE_SELECTION);
	}

	emit updateView();
}

//******************************************************************************

void SelectionPlugin::saveSelections()
{
	PluginFunctions::IteratorRestriction restriction;
  restriction = PluginFunctions::TARGET_OBJECTS;
  // process all target meshes
  #ifndef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
		for ( PluginFunctions::ObjectIterator o_it(restriction, DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_TSPLINE_MESH ));
			o_it != PluginFunctions::objectsEnd(); ++o_it)   {
#endif
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
    emit log(LOGERR,tr("saveSelection: unable to get object") );
    return;
  }

  if( object->dataType( DATA_TRIANGLE_MESH))
    saveSelection( *PluginFunctions::triMesh(object), _filename);
  else
    if( object->dataType( DATA_POLY_MESH))
      saveSelection( *PluginFunctions::polyMesh(object), _filename);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else
    if( object->dataType( DATA_TSPLINE_MESH))
      saveSelection( *PluginFunctions::tsplineMesh(object), _filename);
#endif
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
	  emit log(LOGERR,tr("saveSelection : Unsupported object Type") );
}

//******************************************************************************

void SelectionPlugin::loadSelection( int _objectId  , QString _filename)
{

  if ( !_filename.isEmpty() ){

    INIFile ini;

    if ( ! ini.connect(_filename,false) ) {
      emit log(LOGERR,tr("Failed to load Selection from file ") + _filename);
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


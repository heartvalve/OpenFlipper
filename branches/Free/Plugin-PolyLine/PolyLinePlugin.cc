//=============================================================================
//
//  CLASS PolyLinePlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include <QtGui>

#include "PolyLinePlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/ManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenFlipper/common/GlobalOptions.hh>


//== IMPLEMENTATION ==========================================================

/// default constructor
PolyLinePlugin::PolyLinePlugin() :
        tool_(0),
        polyLineAction_(0),
        toolBarActions_(0),
        toolbar_(0),
        pickToolbar_(0),
        pickToolBarActions_(0),
        insertAction_(0),
        deleteAction_(0),
        moveAction_(0),
        smartMoveAction_(0),
        mergeAction_(0),
        splitAction_(0),
        cutAction_(0),
        cur_insert_id_(-1),
        cur_polyline_obj_(0),
        cur_move_id_(-1),
        move_point_ref_(0),
        cur_merge_id_(-1),
        smart_move_timer_(0),
        cur_smart_move_obj_(0),
        planeSelect_(0)
{
}


void
PolyLinePlugin::
initializePlugin()
{
  // Initialize the Toolbox
  tool_ = new PolyLineToolbarWidget();
  QSize size(100, 100);
  tool_->resize(size);
  tool_->setObjectName("PolyLineToolbar");

  // connect signals->slots
  connect(tool_->pb_subdivide,SIGNAL(clicked() ),this,SLOT(slot_subdivide()));
  connect(tool_->subdivide_relative,SIGNAL(toggled(bool) ),this,SLOT(slot_subdivide_percent(bool)));
  connect(tool_->pb_decimate,SIGNAL(clicked() ),this,SLOT(slot_decimate()));
  connect(tool_->decimate_relative,SIGNAL(toggled(bool) ),this,SLOT(slot_decimate_percent(bool)));
#ifdef EXTENDED_POLY_LINE
  connect(tool_->pb_resample_on_edges,SIGNAL(clicked() ),this,SLOT(slot_resample_on_edges()));
#else
  tool_->pb_resample_on_edges->setDisabled(true);
#endif
  connect(tool_->pb_smooth,SIGNAL(clicked() ),this,SLOT(slot_smooth()));
  connect(tool_->pb_smooth,SIGNAL(clicked() ),this,SIGNAL(updateView()));
  connect(tool_->pb_project,SIGNAL(clicked() ),this,SLOT(slot_project()));
  connect(tool_->pb_project,SIGNAL(clicked() ),this,SIGNAL(updateView()));
  connect(tool_->pb_smooth_project,SIGNAL(clicked() ),this,SLOT(slot_smooth_project()));
  connect(tool_->pb_smooth_project,SIGNAL(clicked() ),this,SIGNAL(updateView()));

  connect(tool_->rb_insert, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_delete, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_move,   SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_smart_move, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_merge,  SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_split,  SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));

  //add icons
  tool_->rb_insert->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_insert.png") );
  tool_->rb_delete->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_delete.png") );
  tool_->rb_move->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_move.png") );
  tool_->rb_smart_move->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_move.png") );
  tool_->rb_merge->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_merge.png") );
  tool_->rb_split->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_split.png") );
//   tool_->rb_smooth_c0->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_insert.svg") );
//   tool_->rb_smooth_c1->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_c1.svg") );
//   tool_->rb_smooth_c2->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_c2.svg") );


  // Add the toolbox with the given icon
  QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"cut_polyline.png");
  emit addToolbox( tr("PolyLine") , tool_, toolIcon );
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slotMouseEvent( QMouseEvent* _event )
{
  // control modifier is reserved for selcting target
  if (_event->modifiers() & (Qt::ControlModifier))
    return;

  if (PluginFunctions::pickMode() == ("PolyLine") && PluginFunctions::actionMode() == Viewer::PickingMode
      && _event->button() != Qt::RightButton) {
    // handle mouse events depending on current mode
    switch (mode()) {
      case PL_INSERT:
        me_insert(_event);
        break;

      case PL_DELETE:
        me_delete(_event);
        break;

      case PL_MOVE:
        me_move(_event);
        break;

      case PL_SPLIT:
        me_split(_event);
        break;

      case PL_MERGE:
        me_merge(_event);
        break;

      case PL_SMART_MOVE:
        me_smart_move(_event);
        break;

      default:
        break;
    }
  } else if (PluginFunctions::pickMode() == CREATE_CUT_POLYLINE) {
    planeSelect_->slotMouseEvent(_event);
  }
}


//-----------------------------------------------------------------------------

void
PolyLinePlugin::
slotPickModeChanged( const std::string& _mode)
{
  polyLineAction_->setChecked(_mode == "PolyLine");
  cutAction_->setChecked( _mode == CREATE_CUT_POLYLINE );
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
pluginsInitialized()
{
  // Add the required Picking modes (Hidden, controlled only by the buttons)
  emit addHiddenPickMode("PolyLine");
  emit addHiddenPickMode( CREATE_CUT_POLYLINE );
  
  // TOOLBAR
  toolbar_ = new QToolBar(tr("PolyLine Editing"));
  toolbar_->setObjectName("PolyLineEditingToolbar");

  
  toolBarActions_ = new QActionGroup(toolbar_);
  
  // icon which enables the Modeling toolbar
  polyLineAction_ = new QAction(tr("Edit PolyLines"), toolBarActions_);
  polyLineAction_->setStatusTip(tr("Edit and create PolyLines."));
  polyLineAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_insert.png") );
  polyLineAction_->setCheckable(true);
  toolbar_->addAction(polyLineAction_);
  
  // icon for polyline cutting of objects
  cutAction_ = new QAction(tr("&Create polyline at intersection with plane"), this);
  cutAction_->setCheckable( true );
  cutAction_->setStatusTip(tr("Create a polyline by specifying a plane with which the object is then intersected. The polyline will be created at the intersection."));
  cutAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"cut_polyline.png") );
  connect(cutAction_, SIGNAL(triggered()), this, SLOT(slotScissorButton()) );
  toolbar_->addAction(cutAction_);
  
  connect(toolBarActions_, SIGNAL(triggered(QAction*)), this, SLOT(slotSetPolyLineMode(QAction*)) );
  
  emit addToolbar(toolbar_);
  
  // Pick Toolbar
  pickToolbar_ = new QToolBar(tr("PolyLine Editing"));
  pickToolbar_->setObjectName("PolyLine_Editing_Toolbar");
  pickToolbar_->setAttribute(Qt::WA_AlwaysShowToolTips, true);
  pickToolBarActions_ = new QActionGroup(pickToolbar_);
  pickToolBarActions_->setExclusive (true);
  
  insertAction_ = new QAction(tr("Create PolyLine"), pickToolBarActions_);
  insertAction_->setStatusTip(tr("Create a new PolyLine."));
  insertAction_->setToolTip(tr("Create a new PolyLine.\n"
                               "Use <Doubleclick> to finish the line.\n"
                               "Hold <Shift> to close line on finish."));
  insertAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_insert.png") );
  insertAction_->setCheckable(true);
  pickToolbar_->addAction(insertAction_);
  
  deleteAction_ = new QAction(tr("Delete PolyLine"), pickToolBarActions_);
  deleteAction_->setStatusTip(tr("Delete a complete PolyLine."));
  deleteAction_->setToolTip(tr( "Delete a complete PolyLine.\n"
                                "<Click> on the lines you want to delete."));
  deleteAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_delete.png") );
  deleteAction_->setCheckable(true);
  pickToolbar_->addAction(deleteAction_);  

  smartMoveAction_ = new QAction(tr("Move PolyLine"), pickToolBarActions_);
  smartMoveAction_->setStatusTip(tr("Move the PolyLine."));
  smartMoveAction_->setToolTip(tr("Move the polyline."));
  smartMoveAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_move.png") );
  smartMoveAction_->setCheckable(true);
  pickToolbar_->addAction(smartMoveAction_);
  
  mergeAction_ = new QAction(tr("Merge PolyLines"), pickToolBarActions_);
  mergeAction_->setStatusTip(tr("Merge two PolyLines."));
  mergeAction_->setToolTip(tr("Merge two PolyLines.\n"
                              "Drag one endpoint of a PolyLine to the EndPoint of another one.\n"));
  mergeAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_merge.png") );
  mergeAction_->setCheckable(true);
  pickToolbar_->addAction(mergeAction_);  
  
  splitAction_ = new QAction(tr("Split a PolyLine"), pickToolBarActions_);
  splitAction_->setStatusTip(tr("Split a PolyLine at a given point."));
  splitAction_->setToolTip(tr("Split a PolyLine at a given point.\n"
                              "<Click> on the vertex where you want to split the PolyLine and drag it away."));
  splitAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_split.png") );
  splitAction_->setCheckable(true);
  pickToolbar_->addAction(splitAction_);  
  
  pickToolbar_->addSeparator ();
  
  moveAction_ = new QAction(tr("Move PolyLine Vertex"), pickToolBarActions_);
  moveAction_->setStatusTip(tr("Move a Vertex of a PolyLine"));
  moveAction_->setToolTip(tr("Move a single Vertex of a PolyLine."));
  moveAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_move.png") );
  moveAction_->setCheckable(true);
  pickToolbar_->addAction(moveAction_);
  
  emit setPickModeToolbar ("PolyLine", pickToolbar_);
  
  connect(pickToolBarActions_, SIGNAL(triggered(QAction*)), this, SLOT(slotPickToolbarAction(QAction*)) );

  // construct timer
  smart_move_timer_ = new QTimer(this);
  connect(smart_move_timer_, SIGNAL(timeout()), this, SLOT(slot_smart_move_timer()));
  
  // Instantiate plane select object
  planeSelect_ = new QtPlaneSelect( PluginFunctions::viewerProperties().glState() );
  connect( planeSelect_, SIGNAL( signalTriggerCut( ) ), this, SLOT( slotTriggerCutPlaneSelect() ) );
  connect( planeSelect_, SIGNAL( updateViewProxy( ) ), this, SIGNAL( updateView() ) );
}

//------------------------------------------------------------------------------

/** \brief Scissor Button was hit
*
*/
void PolyLinePlugin::slotScissorButton( )
{
    PluginFunctions::actionMode( Viewer::PickingMode );
    PluginFunctions::pickMode( CREATE_CUT_POLYLINE );
}

//-----------------------------------------------------------------------------

/** \brief Generate PolyLine after the cutPlane has been drawn
*
*/
void PolyLinePlugin::slotTriggerCutPlaneSelect( )
{
    using ACG::SceneGraph::LineNode;

  // Iterate over all selected objects
  bool updated = false;
  BaseObjectData* object;
  if (PluginFunctions::getPickedObject(planeSelect_->getNode(), object)) {
    emit log("Cutting object " + object->name());

    if (!(object->dataType(DATA_TRIANGLE_MESH) || object->dataType(DATA_POLY_MESH))) {
      emit log("Only Meshes are supported at the moment ");
      return;
    }

    ACG::Vec3d point = planeSelect_->getSourcePoint();
    ACG::Vec3d normal = planeSelect_->getNormal();

    generatePolyLineFromCut(object->id(), point, normal);

    QString command = "generatePolyLineFromCut(" + QString::number(object->id()) + ",Vector("
        + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + "),Vector("
        + QString::number(normal[0]) + "," + QString::number(normal[1]) + "," + QString::number(normal[2]) + "));";
    emit scriptInfo(command);

    //remove all other targets
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,
        DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); o_it != PluginFunctions::objectsEnd(); ++o_it)
      if (o_it->id() != object->id()) {
        o_it->target(false);
      }
  }

  if (updated)
    emit updateView();
}

//-----------------------------------------------------------------------------

void
PolyLinePlugin::
slot_subdivide()
{
  // get subdivision value
  double max_length = tool_->dsb_subdivide->value();

  // safety catch
  if (max_length == 0.0)
    max_length = 1e-4;

  // iterate over all target polylines
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE);

  for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    // subdivide polyline
    PluginFunctions::polyLineObject(*o_it)->line()->subdivide(max_length);
  }

  // update
  emit updateView();
}


//-----------------------------------------------------------------------------


void PolyLinePlugin::slot_subdivide_percent(bool _checked) {

  // Get subdivision value
  int n_active_pl = 0;
  double total_length = 0;

  // Iterate over all target polylines
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE);

  for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    // Add polyline length
    total_length += PluginFunctions::polyLineObject(*o_it)->line()->length();
    ++n_active_pl;
  }

  double v = total_length / double(n_active_pl);

  if (_checked) {

    // Compute current absolute length to relative portion
    if (n_active_pl > 0) {
      double val_new = tool_->dsb_subdivide->value() / v;
      tool_->dsb_subdivide->setValue(val_new);
    } else {
      emit log(LOGWARN, "Could not find any active polyline!");
    }
  } else {

    // Compute relative portion to absolute value
    if (n_active_pl > 0) {
      double val_new = tool_->dsb_subdivide->value() * v;
      tool_->dsb_subdivide->setValue(val_new);
    } else {
      emit log(LOGWARN, "Could not find any active polyline!");
    }
  }
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_decimate()
{
  // get subdivision value
  double min_length = tool_->dsb_decimate->value();

  // iterate over all target polylines
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE);

  for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    // decimate polyline
    PluginFunctions::polyLineObject(*o_it)->line()->collapse(min_length);
  }

  // update
  emit updateView();
}

//-----------------------------------------------------------------------------


void PolyLinePlugin::slot_decimate_percent(bool _checked)
{

  // Get decimation value
  int n_active_pl = 0;
  double total_length = 0;

  // Iterate over all target polylines
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE);

  for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    // Add polyline length
    total_length += PluginFunctions::polyLineObject(*o_it)->line()->length();
    ++n_active_pl;
  }

  double v = total_length / double(n_active_pl);

  if (_checked) {

    // Compute current absolute length to relative portion
    if (n_active_pl > 0) {
      double val_new = tool_->dsb_subdivide->value() / v;
      tool_->dsb_decimate->setValue(val_new);
    } else {
      emit log(LOGWARN, "Could not find any active polyline!");
    }
  } else {

    // Compute relative portion to absolute value
    if (n_active_pl > 0) {
      double val_new = tool_->dsb_subdivide->value() * v;
      tool_->dsb_decimate->setValue(val_new);
    } else {
      emit log(LOGWARN, "Could not find any active polyline!");
    }
  }
}


//-----------------------------------------------------------------------------

#ifdef EXTENDED_POLY_LINE

void
PolyLinePlugin::
slot_resample_on_edges()
{
  // count target meshes
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_TRIANGLE_MESH );
  unsigned int n_targetMeshes(0);
  for(; o_it != PluginFunctions::objectsEnd(); ++o_it)
    ++n_targetMeshes;

  if( n_targetMeshes != 1)
  {
    std::cerr << "Warning: resample_on_edges needs exactly 1 target mesh! Otherwise no operation is performed! \n";
    return;
  }

  // take first target mesh
  o_it = PluginFunctions::ObjectIterator(PluginFunctions::TARGET_OBJECTS, DATA_TRIANGLE_MESH );

  if(o_it != PluginFunctions::objectsEnd())
  {
    // get pointer to TriMeshObject
    TriMeshObject* tmesh_obj = PluginFunctions::triMeshObject(*o_it);

    // get pointer to mesh
    TriMesh* mesh = tmesh_obj->mesh();

    // get pointer to triangle bsp
    OpenMeshTriangleBSPT< TriMesh >* tbsp = tmesh_obj->requestTriangleBsp();
    //    OpenMeshTriangleBSPT< TriMesh >* tbsp = 0;

    // iterate over all target polylines
    PluginFunctions::ObjectIterator o_it2(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE );

    for (; o_it2 != PluginFunctions::objectsEnd(); ++o_it2)
    {
      std::cerr << "resample " << o_it2->name().toAscii().data() << std::endl;
      // decimate polyline
      PluginFunctions::polyLineObject(*o_it2)->line()->resample_on_mesh_edges( *mesh, tbsp );
    }

    // update
    emit updateView();
  }
}

#endif

//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_smooth()
{
  // iterate over all target polylines
  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE );

  for (  ; o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    for( int i=0; i<tool_->sb_smooth_iter->value(); ++i)
      if( tool_->rb_smooth_c0->isChecked())
	// smooth polyline C0
	PluginFunctions::polyLineObject(*o_it)->line()->smooth_uniform_laplace();
      else
	if( tool_->rb_smooth_c1->isChecked())
	  // smooth polyline C1
	  PluginFunctions::polyLineObject(*o_it)->line()->smooth_uniform_laplace2();
	else
	  if( tool_->rb_smooth_c2->isChecked())
	    // smooth polyline C2
	    PluginFunctions::polyLineObject(*o_it)->line()->smooth_uniform_laplace3();

    emit updatedObject( o_it->id(), UPDATE_GEOMETRY);
  }

}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_smooth( PolyLineObject*& _pol)
{
  // if polyline object is valid
  if (_pol) {
    for (int i = 0; i < tool_->sb_smooth_iter->value(); ++i)
      if (tool_->rb_smooth_c0->isChecked())
        // smooth polyline C0
        _pol->line()->smooth_uniform_laplace();
      else if (tool_->rb_smooth_c1->isChecked())
        // smooth polyline C1
        _pol->line()->smooth_uniform_laplace2();
      else if (tool_->rb_smooth_c2->isChecked())
        // smooth polyline C2
        _pol->line()->smooth_uniform_laplace3();

    emit updatedObject(_pol->id(), UPDATE_GEOMETRY);
  }
}

//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_project()
{
  // create meshe and bsp vectors
  std::vector<TriMesh*> meshes;
  std::vector<OpenMeshTriangleBSPT<TriMesh>*> bsps;

  PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_TRIANGLE_MESH);

  for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    // get pointer to TriMeshObject
    TriMeshObject* tmesh_obj = PluginFunctions::triMeshObject(*o_it);

    // save meshes and bsps
    meshes.push_back(tmesh_obj->mesh());
    //    bsps.push_back(0);
    bsps.push_back(tmesh_obj->requestTriangleBsp());
  }

  // iterate over all target polylines
  PluginFunctions::ObjectIterator o_it2(PluginFunctions::TARGET_OBJECTS, DATA_POLY_LINE);

  for (; o_it2 != PluginFunctions::objectsEnd(); ++o_it2) {
    // project polyline
    PluginFunctions::polyLineObject(*o_it2)->line()->project_to_mesh(meshes, &bsps);
    emit updatedObject(o_it2->id(), UPDATE_GEOMETRY);
  }
}


//-----------------------------------------------------------------------------


void 
PolyLinePlugin::
slot_project( PolyLineObject*& _pol)
{
  // check if polyline ok
  if (_pol) {

    // create meshes and bsp vectors
    std::vector<TriMesh*> meshes;
    std::vector<OpenMeshTriangleBSPT<TriMesh>*> bsps;

    PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_TRIANGLE_MESH);

    for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {

      // get pointer to TriMeshObject
      TriMeshObject* tmesh_obj = PluginFunctions::triMeshObject(*o_it);

      // save meshes and bsps
      meshes.push_back(tmesh_obj->mesh());
      //    bsps.push_back(0);
      bsps.push_back(tmesh_obj->requestTriangleBsp());
    }

    // only project given PolyLine
    _pol->line()->project_to_mesh(meshes, &bsps);
  }

}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_smooth_project()
{
  int smooth_project_iter = tool_->sb_smooth_project_iter->value();

  for (int i = 0, j = 1; i < smooth_project_iter; ++i, ++j) {

    // n smoothing steps
    slot_smooth();

    // projection step
    slot_project();

  }
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_smooth_project(PolyLineObject*& _pol)
{
  int smooth_project_iter = tool_->sb_smooth_project_iter->value();

  for (int i = 0, j = 1; i < smooth_project_iter; ++i, ++j) {

    // n smoothing steps
    slot_smooth(_pol);

    // projection step
    slot_project(_pol);

  }
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
slot_smart_move_timer()
{
  int smooth_project_iter = tool_->sb_smooth_project_iter->value();

  if (smooth_project_iter)
    slot_smooth_project(cur_smart_move_obj_);
  else // only smooth
    slot_smooth(cur_smart_move_obj_);

  emit updateView();
}


//-----------------------------------------------------------------------------


PolyLinePlugin::EditMode
PolyLinePlugin::
mode()
{
  if(tool_ )
  {
    if( tool_->rb_insert->isChecked()         ) return PL_INSERT;
    else if( tool_->rb_delete->isChecked()    ) return PL_DELETE;
    else if( tool_->rb_move->isChecked()      ) return PL_MOVE;
    else if( tool_->rb_split->isChecked()     ) return PL_SPLIT;
    else if( tool_->rb_merge->isChecked()     ) return PL_MERGE;
    else if( tool_->rb_smart_move->isChecked()) return PL_SMART_MOVE;
  }

  return PL_NONE;
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_insert( QMouseEvent* _event )
{
  // Pick position
  unsigned int node_idx, target_idx;
  ACG::Vec3d hit_point;
  if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {

    switch (_event->type()) {
      // insert new point
      case QEvent::MouseButtonPress: {
        // new Polyline?
        if (cur_insert_id_ == -1 || !PluginFunctions::objectRoot()->childExists(cur_insert_id_)) {
          // add new polyline
          emit addEmptyObject(DATA_POLY_LINE, cur_insert_id_);

          // get current polylineobject
          BaseObjectData *obj = 0;
          PluginFunctions::getObject(cur_insert_id_, obj);
          // default: set as target
          obj->target(true);
          // get polyline object
          cur_polyline_obj_ = PluginFunctions::polyLineObject(obj);

          cur_polyline_obj_->materialNode()->set_random_color();
        }

        // add new point
        cur_polyline_obj_->line()->add_point((PolyLine::Point) hit_point);

        // update
        emit updatedObject(cur_insert_id_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

        break;
      }

      // finish polyline
      case QEvent::MouseButtonDblClick: {

        // close polyline
        if (_event->modifiers() & (Qt::ShiftModifier)) {
          cur_polyline_obj_->line()->set_closed(true);
        }

        // update
        emit updatedObject(cur_insert_id_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

        // reset current variables
        cur_insert_id_ = -1;
        cur_polyline_obj_ = 0;

        break;
      }

      default:
        break;
    }
  }
}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_delete( QMouseEvent* _event )
{
  // MousePress ?
  if (_event->type() == QEvent::MouseButtonPress) {

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {

      BaseObjectData* obj = 0;

      if (PluginFunctions::getPickedObject(node_idx, obj))
        // is picked object polyline?
        if (PluginFunctions::polyLineObject(obj)) {
          emit deleteObject(obj->id());
        }
    }

  }

}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_move( QMouseEvent* _event )
{
  // MousePress ? -> get eference point
  if (_event->type() == QEvent::MouseButtonPress) {

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point)) {

      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {
        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
        if (cur_pol) {
          // save references
          cur_move_id_ = cur_pol->id();
          move_point_ref_ = &(cur_pol->line()->point(target_idx));
        }
      }

    }

  }

  // Move ? -> move reference point
  if (_event->type() == QEvent::MouseMove)
    if (move_point_ref_ != 0) {

      unsigned int node_idx, target_idx;
      ACG::Vec3d hit_point;

      // pick
      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {
        (*move_point_ref_) = (PolyLine::Point) hit_point;

        // update
        emit updatedObject(cur_move_id_, UPDATE_GEOMETRY);
      }
    }

  // Release ? -> release reference point
  if (_event->type() == QEvent::MouseButtonRelease)
    move_point_ref_ = 0;


}
//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_split( QMouseEvent* _event )
{
  // MousePress ?
  if (_event->type() == QEvent::MouseButtonPress) {
    // release old references
    move_point_ref_ = 0;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx,
        &hit_point)) {
      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {
        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
        if (cur_pol) {
          // splitting for CLOSED PolyLines
          if (cur_pol->line()->is_closed()) {
            cur_pol->line()->split_closed(target_idx);

            // save references for moving
            cur_move_id_ = cur_pol->id();
            move_point_ref_ = &(cur_pol->line()->point(cur_pol->line()->n_vertices() - 1));

            // emit changed objects
            emit updatedObject(cur_pol->id(), UPDATE_TOPOLOGY | UPDATE_GEOMETRY);
          } else
          // splitting for OPEN PolyLines
          {
            // add new polyline
            int insert_id;
            emit addEmptyObject(DATA_POLY_LINE, insert_id);

            // get current polylineobject
            BaseObjectData *obj2 = 0;

            // get polyline object
            PluginFunctions::getObject(insert_id, obj2);

            // default: mark as target
            obj2->target(true);

            PolyLineObject* pol_obj2 = PluginFunctions::polyLineObject(obj2);

            pol_obj2->materialNode()->set_random_color();

            cur_pol->line()->split(target_idx, *(pol_obj2->line()));

            // save references for moving
            cur_move_id_ = cur_pol->id();
            move_point_ref_ = &(cur_pol->line()->point(cur_pol->line()->n_vertices() - 1));

            // emit changed objects
            emit updatedObject(insert_id, UPDATE_TOPOLOGY | UPDATE_GEOMETRY);
            emit updatedObject(cur_pol->id(), UPDATE_TOPOLOGY | UPDATE_GEOMETRY);
          }

          // update
          emit updateView();
        }
      }
    }
  }

  // Move splitted ? -> move reference point
  if (_event->type() == QEvent::MouseMove)
    if (move_point_ref_ != 0) {

      unsigned int node_idx, target_idx;
      ACG::Vec3d hit_point;
      // pick
      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {

        (*move_point_ref_) = (PolyLine::Point) hit_point;

        // update
        //	emit updatedObject(cur_insert_id_, UPDATE_TOPOLOGY | UPDATE_GEOMETRY);
        emit updateView();
      }
    }

  // Release splitted? -> release reference point
  if (_event->type() == QEvent::MouseButtonRelease) {

    if (cur_move_id_ != -1)
      emit updatedObject(cur_move_id_, UPDATE_TOPOLOGY | UPDATE_GEOMETRY);

    move_point_ref_ = 0;
    cur_move_id_ = -1;

  }

}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_merge( QMouseEvent* _event )
{
  // Mouse PRESS ?
  if (_event->type() == QEvent::MouseButtonPress) {
    // release old references
    move_point_ref_ = 0;
    cur_merge_id_ = -1;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx,
        &hit_point)) {
      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {
        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
        if (cur_pol)
          if (target_idx == cur_pol->line()->n_vertices() - 1 || target_idx == 0) {
            if (target_idx == 0) {
              cur_pol->line()->invert();
              target_idx = cur_pol->line()->n_vertices() - 1;
            }

            // save references
            cur_merge_id_ = cur_pol->id();

            // save reference for moving
            //	    cur_pol->line()->add_point( cur_pol->line()->point( cur_pol->line()->n_vertices()-1));
            move_point_ref_ = &(cur_pol->line()->point(cur_pol->line()->n_vertices() - 1));
          }
      }
    }
  }

  // Move ? -> move reference point
  if (_event->type() == QEvent::MouseMove && cur_merge_id_ != -1)
    if (move_point_ref_ != 0) {
      unsigned int node_idx, target_idx;
      ACG::Vec3d hit_point;
      // pick
      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx,
          &hit_point)) {
        (*move_point_ref_) = (PolyLine::Point) hit_point;

        // update
        //	emit updatedObject(cur_merge_id_);
        emit updateView();
      }
    }

  // Mouse RELEASE ?
  if (_event->type() == QEvent::MouseButtonRelease && cur_merge_id_ != -1) {
    PolyLine::Point p_save;

    // reset move references
    if (move_point_ref_ != 0) {
      // remove intermediate point
      // restore orig polyline
      BaseObjectData *obj = 0;
      PluginFunctions::getObject(cur_merge_id_, obj);
      PolyLineObject* opol = PluginFunctions::polyLineObject(obj);

      // store position if merging fails
      p_save = opol->line()->back();
      opol->line()->resize(opol->line()->n_vertices() - 1);

      // release reference
      move_point_ref_ = 0;
    }

    bool merged = false;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point)) {

      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {

        // is picked object polyline? -> get second polyline
        PolyLineObject* second_pol = PluginFunctions::polyLineObject(obj);

        if (second_pol) {
          // restore first polyline
          BaseObjectData *obj2 = 0;
          PluginFunctions::getObject(cur_merge_id_, obj2);
          PolyLineObject* first_pol = PluginFunctions::polyLineObject(obj2);

          // get idxs
          unsigned int first_idx = first_pol->line()->n_vertices() - 1;
          unsigned int second_idx = target_idx;

          // both polylines open?
          if (first_pol->line()->is_closed() || second_pol->line()->is_closed())
            return;

          bool inv_first(false), inv_second(false);

          // wrong ordering first Polyline?
          if (first_idx == 0) {
            inv_first = true;
            first_idx = first_pol->line()->n_vertices() - 1;
          }

          // wrong ordering second Polyline?
          if (second_idx == second_pol->line()->n_vertices() - 1) {
            inv_second = true;
            second_idx = 0;
          }

          // two endpoints available?
          if (first_idx == first_pol->line()->n_vertices() - 1 && second_idx == 0) {
            // same polyline?
            if (first_pol->id() == second_pol->id()) {
              // simply close line
              first_pol->line()->set_closed(true);
            } else {
              // invert if necessary
              if (inv_first)
                first_pol->line()->invert();
              if (inv_second)
                second_pol->line()->invert();

              // append second polyline to first one
              first_pol->line()->append(*second_pol->line());

              // set flag
              merged = true;

              // delete appended
              emit deleteObject(second_pol->id());
            }

          }
        }
      }

      if (!merged) {
        // remove intermediate point
        // restore orig polyline
        BaseObjectData *obj = 0;
        PluginFunctions::getObject(cur_merge_id_, obj);
        PolyLineObject* opol = PluginFunctions::polyLineObject(obj);

        opol->line()->add_point(p_save);
      }
    }

    // update
    emit updatedObject(cur_merge_id_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

    // release old references
    move_point_ref_ = 0;
    cur_merge_id_ = -1;
  }

}


//-----------------------------------------------------------------------------


void
PolyLinePlugin::
me_smart_move( QMouseEvent* _event )
{
  // MousePress ? -> select vertex and start timer
  if( _event->type() == QEvent::MouseButtonPress)
  {
    unsigned int     node_idx, target_idx;
    ACG::Vec3d       hit_point;
    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),node_idx, target_idx, &hit_point))
    {
      BaseObjectData* obj = 0;
      if ( PluginFunctions::getPickedObject(node_idx, obj) )
      {
	// is picked object polyline?
	PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
	if(cur_pol)
	{
	  // save references
	  cur_smart_move_obj_ = cur_pol; 
	  if( cur_pol->line()->vertex_selections_available())
	  {
	    if ( !(_event->modifiers() & (Qt::ShiftModifier)) )
	      cur_pol->line()->vertex_selection(target_idx) = true;
	    else
	    {
	      cur_pol->line()->vertex_selection(target_idx) = false;
	      emit updateView();
	    }
	  }
	}

	// start timer
	if ( !(_event->modifiers() & (Qt::ShiftModifier)) )
	  smart_move_timer_->start(20);
      }
    }
  }


  // MouseRelease ? -> stop timer
  if( _event->type() == QEvent::MouseButtonRelease)
  {
    smart_move_timer_->stop();
    cur_smart_move_obj_ = NULL;
  }


  // call move event
  me_move( _event);
}

void
PolyLinePlugin::
slotEditModeChanged()
{
  PluginFunctions::pickMode("PolyLine");
  PluginFunctions::actionMode(Viewer::PickingMode);
}

void 
PolyLinePlugin::
slotPickToolbarAction(QAction* _action) {
  if ( _action == insertAction_ ) {
    tool_->rb_insert->setChecked(true);
  } else if ( _action == deleteAction_ ) {
    tool_->rb_delete->setChecked(true);
  } else if ( _action == moveAction_ ) {
    tool_->rb_move->setChecked(true);
  } else if ( _action == smartMoveAction_ ) {
    tool_->rb_smart_move->setChecked(true);
  } else if ( _action == mergeAction_ ) {
    tool_->rb_merge->setChecked(true);
  } else if ( _action == splitAction_ ) {
    tool_->rb_split->setChecked(true);
  }
}

void 
PolyLinePlugin::
slotSetPolyLineMode(QAction* _action) {
  if (_action == polyLineAction_ ){
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("PolyLine");
    
    polyLineAction_->setChecked( true );
  }
}

//-----------------------------------------------------------------------------


void 
PolyLinePlugin::
slotEnablePickMode(QString _name)
{
  std::cerr << "slotEnablePickMode\n";

  PluginFunctions::pickMode("PolyLine");
  PluginFunctions::actionMode(Viewer::PickingMode);

  if(tool_ )
  {
    if(_name == "INSERT")
      tool_->rb_insert->setChecked(true);
    else if(_name == "DELETE")
      tool_->rb_delete->setChecked(true);
    else if(_name == "MOVE")
      tool_->rb_move->setChecked(true);
    else if(_name == "SPLIT")
      tool_->rb_split->setChecked(true);
    else  if(_name == "MERGE")
      tool_->rb_merge->setChecked(true);
    else if( _name == "SMART_MOVE")
      tool_->rb_smart_move->setChecked(true);
  }
}

//-----------------------------------------------------------------------------


Q_EXPORT_PLUGIN2( PolyLinePlugin , PolyLinePlugin );


//=============================================================================
//
//  CLASS PolyLinePlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


#include "PolyLinePlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/ManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/Geometry/bsp/BSPImplT.hh>

namespace {

class GlutPrimitiveNode : public ACG::SceneGraph::GlutPrimitiveNode {

	public:

		GlutPrimitiveNode(PolyLineObject* L, std::string name, int _index = -1)
			: ACG::SceneGraph::GlutPrimitiveNode(ACG::SceneGraph::GlutPrimitiveNode::SPHERE, L->manipulatorNode(), name)
		{
			index = _index;
			line = L;
		}

	public:

		PolyLineObject* line;
		int index;

};

class GlutLineNode : public ACG::SceneGraph::LineNode {

	public:

		GlutLineNode(PolyLineObject* L, std::string name)
			: ACG::SceneGraph::LineNode(LineSegmentsMode, L->manipulatorNode(), name)
		{
			line = L;
		}

	public:
		PolyLineObject* line;

};

}


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
        insertCircleAction_(0),
        insertSplineAction_(0),
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
        create_point_ref_(0),
        createCircle_CurrSelIndex_(-1),
        createCircle_LastSelIndex_(-1),
        moveCircle_SelNode_(0),
        moveCircle_IsLocked(false),
        moveCircle_IsFloating(false),
        createSpline_CurrSelIndex_(-1),
        createSpline_LastSelIndex_(-1),
        moveBezSpline_SelNode_(0),
        moveBezSpline_SelIndex_(-1),
        moveBezSpline_SelSubIndex_(0),
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
  connect(tool_->rb_InsertCircle, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_InsertSpline, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_delete, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_move,   SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_smart_move, SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_merge,  SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));
  connect(tool_->rb_split,  SIGNAL( clicked() ), this, SLOT( slotEditModeChanged() ));

  connect(tool_->sb_CirclePointNum, SIGNAL(valueChanged(int)), this, SLOT(slot_setCirclePointNum(int)));
  connect(tool_->sb_SplineSegNum, SIGNAL(valueChanged(int)), this, SLOT(slot_setSplinePointNum(int)));
  //add icons
  tool_->rb_insert->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_insert.png") );
  tool_->rb_InsertCircle->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_circle.png") );
  tool_->rb_InsertSpline->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "polyline_bezier.png") );
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

      case PL_INSERTCIRCLE:
        me_insertCircle(_event);
        break;

      case PL_INSERTSPLINE:
        me_insertSpline(_event);
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

void PolyLinePlugin::slotKeyEvent(QKeyEvent* event) {
    if (!cur_polyline_obj_ || cur_insert_id_ == -1) return;
    switch (event->key()) {
        case Qt::Key_Return:
            if (PluginFunctions::pickMode() == ("PolyLine") && PluginFunctions::actionMode() == Viewer::PickingMode && mode() == PL_INSERT) {

                cur_polyline_obj_->line()->delete_point(cur_polyline_obj_->line()->n_vertices() - 1);

                if (event->modifiers() & (Qt::ShiftModifier))
                  cur_polyline_obj_->line()->set_closed(true);

                emit updatedObject(cur_insert_id_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

                cur_insert_id_ = -1;
                cur_polyline_obj_ = 0;
                create_point_ref_ = 0;

                clearStatusMessage();
            }
            break;
        default:
            break;
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
  emit setPickModeMouseTracking("PolyLine", true);
  emit addHiddenPickMode( CREATE_CUT_POLYLINE );

  emit registerKey(Qt::Key_Return, Qt::NoModifier, tr("Terminate creation of poly line."), true);
  emit registerKey(Qt::Key_Return, Qt::ShiftModifier, tr("Terminate creation of poly line and create loop."), true);
  
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
  
  insertCircleAction_ = new QAction(tr("Create PolyCircle"), pickToolBarActions_);
  insertCircleAction_->setStatusTip(tr("Create a new PolyCircle."));
  insertCircleAction_->setToolTip(tr("Create a new PolyCircle.\n"
		  	  	  	  	  	  	  	 "<Click> to select the center.\n"
		  	  	  	  	  	  	  	 "Drag to specify the radius."));

  insertCircleAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_circle.png") );
  insertCircleAction_->setCheckable(true);
  pickToolbar_->addAction(insertCircleAction_);

  insertSplineAction_ = new QAction(tr("Create PolySpline"), pickToolBarActions_);
  insertSplineAction_->setStatusTip(tr("Create a new PolySpline."));
  insertSplineAction_->setToolTip(tr("Create a new PolySpline.\n"
		  	  	  	  	  	  	  	 "<Click> to select the points."));

  insertSplineAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"polyline_bezier.png") );
  insertSplineAction_->setCheckable(true);
  pickToolbar_->addAction(insertSplineAction_);

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
  BaseObjectData* object;
  if (PluginFunctions::getPickedObject(planeSelect_->getNode(), object)) {
    emit log("Cutting object " + object->name());

    if (!(object->dataType(DATA_TRIANGLE_MESH) || object->dataType(DATA_POLY_MESH))) {
      emit log("Only Meshes are supported at the moment ");
      return;
    }

    ACG::Vec3d point = planeSelect_->getSourcePoint();
    ACG::Vec3d normal = planeSelect_->getNormal();

    int objectId = generatePolyLineFromCut(object->id(), point, normal);

    QString command = "generatePolyLineFromCut(" + QString::number(object->id()) + ",Vector("
        + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + "),Vector("
        + QString::number(normal[0]) + "," + QString::number(normal[1]) + "," + QString::number(normal[2]) + "));";
    emit scriptInfo(command);

    //remove all other targets
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,
         DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)); o_it != PluginFunctions::objectsEnd(); ++o_it) {
      if (o_it->id() != object->id()) {
        o_it->target(false);
      }
    }

    // If we successfully created the polyline, we can inform the core about it.
    if ( objectId != -1)
      emit updatedObject(objectId,UPDATE_ALL);

  }

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
      std::cerr << "resample " << o_it2->name().toStdString() << std::endl;
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


void
PolyLinePlugin::slotObjectUpdated( int _identifier, const UpdateType &_type )
{
    PolyLineObject* lineObject = 0;
    if(!PluginFunctions::getObject(_identifier, lineObject))
        return;
    PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));
    PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));
    GlutPrimitiveNode* H = 0, *C = 0;
    lineObject->getAdditionalNode(C,  name(), "circle");
    lineObject->getAdditionalNode(H, name(), "control", 0);
    if(circleData && !C) {
        createCircle_createUI(_identifier);
    }
    else if(splineData && !H) {
        createSpline_createUI(_identifier);
    }
}

//-----------------------------------------------------------------------------

PolyLinePlugin::EditMode
PolyLinePlugin::
mode()
{
  if(tool_ )
  {
    if( tool_->rb_insert->isChecked()         ) return PL_INSERT;
    if( tool_->rb_InsertCircle->isChecked()	  ) return PL_INSERTCIRCLE;
    if( tool_->rb_InsertSpline->isChecked()	  ) return PL_INSERTSPLINE;
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
    if (_event->type() == QEvent::MouseMove) {
        if (create_point_ref_) {
            // Pick position
            unsigned int node_idx, target_idx;
            ACG::Vec3d hit_point;
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {
                *create_point_ref_ = (PolyLine::Point) hit_point;

                // update
                emit updatedObject(cur_insert_id_, UPDATE_GEOMETRY);
            }
        }
        return;
    }

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

          cur_polyline_obj_->line()->add_point((PolyLine::Point) hit_point);

          emit showStatusMessage(tr("Double click/Enter to terminate poly line. Use with shift to create loop."));
        }

        // add new point
        cur_polyline_obj_->line()->add_point((PolyLine::Point) hit_point);
        create_point_ref_ = &cur_polyline_obj_->line()->points().back();

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

        if (cur_polyline_obj_->line()->n_vertices() >= 2) {
            const PolyLine::Point &p1 = cur_polyline_obj_->line()->point(cur_polyline_obj_->line()->n_vertices() - 1),
                    &p2 = cur_polyline_obj_->line()->point(cur_polyline_obj_->line()->n_vertices() - 2);

            /*
             * Remove duplicate created as a move sentinel.
             */
            if ((p2 - p1).sqrnorm() < 1e-6) {
                cur_polyline_obj_->line()->delete_point(cur_polyline_obj_->line()->n_vertices() - 1);
            }
        }

        // update
        emit updatedObject(cur_insert_id_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

        // reset current variables
        cur_insert_id_ = -1;
        cur_polyline_obj_ = 0;
        create_point_ref_ = 0;

        clearStatusMessage();

        break;
      }

      default:
        break;
    }
  }
}

//-----------------------------------------------------------------------------

void PolyLinePlugin::
createCircle_createUI(int _polyLineObjectID)
{
    PolyLineObject* lineObject;
    if (!PluginFunctions::getObject(_polyLineObjectID, lineObject) || !lineObject->objectData(CIRCLE_DATA))
        return;
    PolyLineCircleData* lineData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));

    double rad = 0.0;
    TriMeshObject* mesh;
    if(PluginFunctions::getObject(lineData->circleMeshIndex_, mesh)) {
        //use the size of the mesh to compute the handle radii
        ACG::Vec3d bbMin( FLT_MAX, FLT_MAX, FLT_MAX);
        ACG::Vec3d bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
        mesh->boundingBox(bbMin, bbMax);
        rad = 0.005*(bbMax-bbMin).norm();
    }
    else {
        //use the circle radius to guess the handle radii
        double r = std::max(lineData->circleMainRadius_, lineData->circleSideRadius_);
        rad = r / 25.0;
    }

    GlutPrimitiveNode* handle0 = new GlutPrimitiveNode(lineObject, "N_Handle0");
    handle0->get_primitive(0).color = ACG::Vec4f(1,0,0,1);
    handle0->set_size(rad);
    handle0->show();
    handle0->enablePicking(true);
    handle0->set_position(lineData->circleCenter_ + lineData->circleMainAxis_ * lineData->circleMainRadius_);
    lineObject->addAdditionalNode(handle0, name(), "handle0");
    handle0->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);

    GlutPrimitiveNode* handle1 = new GlutPrimitiveNode(lineObject, "N_Handle1");
    handle1->get_primitive(0).color = ACG::Vec4f(0,1,0,1);
    handle1->set_size(rad);
    handle1->show();
    handle1->enablePicking(true);
    handle1->set_position(lineData->circleCenter_ + lineData->circleSideAxis_ * lineData->circleSideRadius_);
    lineObject->addAdditionalNode(handle1, name(), "handle1");
    handle1->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);

    GlutPrimitiveNode* cenNode = new GlutPrimitiveNode(lineObject, "N_Center");
    cenNode->get_primitive(0).color = ACG::Vec4f(0,0,1,1);
    cenNode->set_size(rad);
    cenNode->show();
    cenNode->enablePicking(true);
    cenNode->set_position(lineData->circleCenter_);
    lineObject->addAdditionalNode(cenNode, name(), "circle");
    cenNode->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);

    lineObject->lineNode()->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);

    emit updatedObject(lineObject->id(), UPDATE_ALL);
}

void PolyLinePlugin::
me_insertCircle(QMouseEvent* _event)
{
	TriMeshObject* mesh;
	TriMesh::FaceHandle fh;
	TriMesh::VertexHandle vh;
	ACG::Vec3d hit_point;
	if(!pick_triangle_mesh(_event->pos(), mesh, fh, vh, hit_point) && _event->type() != QEvent::MouseButtonRelease)
	    return;//can't generate a circle in empty space

	if(_event->type() == QEvent::MouseMove && createCircle_CurrSelIndex_ != -1) {
		PolyLineObject* lineObject = 0;
		if(!PluginFunctions::getObject(createCircle_CurrSelIndex_, lineObject) || !lineObject->objectData(CIRCLE_DATA))
			return;
        PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));

		const ACG::Vec3d n = circleData->circleNormal_, x0 = circleData->circleCenter_;
		const double t = ((n | x0) - (n | hit_point)) / n.sqrnorm();
		const ACG::Vec3d onPlane = hit_point + t * n,  d = onPlane - x0;

        circleData->circleMainAxis_ = (onPlane - x0).normalize();
        circleData->circleSideRadius_  = circleData->circleMainRadius_ = d.norm();
        circleData->circleSideAxis_ = (circleData->circleMainAxis_ % n).normalize();

		updatePolyEllipse(lineObject, tool_->sb_CirclePointNum->value());
		updateHandles(lineObject);
	}
	else if(_event->type() == QEvent::MouseButtonPress && createCircle_CurrSelIndex_ == -1) {
		int new_line_id;
	    emit addEmptyObject(DATA_POLY_LINE, new_line_id);
		BaseObjectData *obj = 0;
		PluginFunctions::getObject(new_line_id, obj);
		obj->target(true);
		PolyLineObject* newLine = PluginFunctions::polyLineObject(obj);
		newLine->materialNode()->set_random_color();

		if(!mesh->mesh()->has_face_normals())
			mesh->mesh()->request_face_normals();

        PolyLineCircleData* lineData = new PolyLineCircleData(hit_point, mesh->mesh()->normal(fh), ACG::Vec3d(), ACG::Vec3d(), 0, 0, mesh->id());
        newLine->setObjectData(CIRCLE_DATA, lineData);

		emit updatedObject(new_line_id, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);
		createCircle_LastSelIndex_ = createCircle_CurrSelIndex_ = newLine->id();
		createSpline_LastSelIndex_ = -1;
	}
	else if(_event->type() == QEvent::MouseButtonRelease ) {
	    PolyLineObject* lineObject;
		if(PluginFunctions::getObject(createCircle_CurrSelIndex_, lineObject))
		{
            ACG::SceneGraph::GlutPrimitiveNode* N;
            if(lineObject->getAdditionalNode(N, name(), "handle0"))
                N->enablePicking(true);
		}
		createCircle_CurrSelIndex_ = -1;
	}
}

//-----------------------------------------------------------------------------

void PolyLinePlugin::
createSpline_createUI(int _polyLineObjectID)
{
    PolyLineObject* lineObject;
    if (!PluginFunctions::getObject(_polyLineObjectID, lineObject) || !lineObject->objectData(BEZSPLINE_DATA))
        return;
    PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));

    double rad = 0.0;
    TriMeshObject* mesh;
    if(PluginFunctions::getObject(splineData->meshIndex_, mesh)) {
        //use the size of the mesh to compute the handle radii
        ACG::Vec3d bbMin( FLT_MAX, FLT_MAX, FLT_MAX);
        ACG::Vec3d bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
        mesh->boundingBox(bbMin, bbMax);
        rad = 0.005*(bbMax-bbMin).norm();
    }
    else {
        //use the size of the spline to guess the handle radii
        ACG::Vec3d bbMin( FLT_MAX, FLT_MAX, FLT_MAX);
        ACG::Vec3d bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
        for(unsigned int i = 0; i < splineData->points_.size(); i++) {
            bbMin =  bbMin.minimize(splineData->points_[i].position);
            bbMax =  bbMax.maximize(splineData->points_[i].position);
        }
        rad = (bbMax-bbMin).norm() / 25.0;
    }

    GlutLineNode* lineN = new GlutLineNode(lineObject, "N_Line");
    lineN->show();
    lineN->enablePicking(false);
    lineObject->addAdditionalNode(lineN, name(), "line");
    lineN->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);

    for(unsigned int i = 0; i < splineData->points_.size(); i++) {
        GlutPrimitiveNode* handle0 = new GlutPrimitiveNode(lineObject, "N_Control", i);
        handle0->get_primitive(0).color = ACG::Vec4f(0,1,0,1);
        handle0->set_size(rad);
        handle0->show();
        handle0->set_position(splineData->points_[i].position);
        handle0->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);
        handle0->enablePicking(true);
        lineObject->addAdditionalNode(handle0, name(), "control", i);
    }

    for(unsigned int i = 0; i < splineData->handles_.size(); i++) {
        const PolyLineBezierSplineData::InterpolatePoint& control = splineData->getInterpolatePoint(i);
        const ACG::Vec3d hndlPos = splineData->handles_[i], ctrlPos = control.position;

        GlutPrimitiveNode* handle0 = new GlutPrimitiveNode(lineObject, "N_Handle", i);
        handle0->get_primitive(0).color = ACG::Vec4f(0,0,1,1);
        handle0->set_size(rad * 0.75);
        handle0->show();
        handle0->enablePicking(true);
        handle0->set_position(hndlPos);
        lineObject->addAdditionalNode(handle0, name(), "handle", i);
        handle0->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);

        GlutLineNode* lineN;
        if(lineObject->getAdditionalNode(lineN,  name(), "line")) {
            lineN->add_line(ctrlPos, hndlPos);
            lineN->add_color(ACG::Vec4f(1,0,0,1));
        }
    }
    updatePolyBezierSpline(lineObject, tool_->sb_SplineSegNum->value());
    lineObject->lineNode()->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);

    emit updatedObject(createSpline_CurrSelIndex_, UPDATE_ALL);
}

void PolyLinePlugin::
me_insertSpline(QMouseEvent* _event)
{

	TriMeshObject* mesh;
	TriMesh::FaceHandle fh;
	TriMesh::VertexHandle vh;
	ACG::Vec3d hit_point;

	if(!pick_triangle_mesh(_event->pos(), mesh, fh, vh, hit_point))
		return;//can't generate a circle in empty space

	ACG::Vec3d bbMin( FLT_MAX, FLT_MAX, FLT_MAX);
	ACG::Vec3d bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	mesh->boundingBox(bbMin, bbMax);
	const ACG::Vec3d sizeBB((bbMax-bbMin));

	if(!mesh->mesh()->has_face_normals())
		mesh->mesh()->request_face_normals();

	ACG::Vec3d nor = mesh->mesh()->normal(fh);

	if(_event->type() == QEvent::MouseButtonPress) {

	  if(createSpline_CurrSelIndex_ == -1) {

	    emit addEmptyObject(DATA_POLY_LINE, createSpline_CurrSelIndex_);
	    createSpline_LastSelIndex_ = createSpline_CurrSelIndex_;
	    createCircle_LastSelIndex_ = -1;
	    BaseObjectData *obj = 0;
	    PluginFunctions::getObject(createSpline_CurrSelIndex_, obj);
	    obj->target(true);
	    PolyLineObject* newLine = PluginFunctions::polyLineObject(obj);
	    newLine->materialNode()->set_random_color();
	    newLine->lineNode()->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
	    emit updatedObject(createSpline_CurrSelIndex_, UPDATE_GEOMETRY | UPDATE_TOPOLOGY);

	    PolyLineBezierSplineData* lineData = new PolyLineBezierSplineData(mesh->id());
	    newLine->setObjectData(BEZSPLINE_DATA, lineData);

	    GlutLineNode* lineN = new GlutLineNode(newLine, "N_Line");
	    lineN->show();
	    lineN->enablePicking(false);
	    newLine->addAdditionalNode(lineN, name(), "line");
	    lineN->drawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
	  }

	  ACG::Vec3d insert_Point = hit_point + nor * 0.003 * sizeBB.norm();
	  PolyLineObject* lineObject = 0;

	  if(!PluginFunctions::getObject(createSpline_CurrSelIndex_, lineObject))
	    return;

	  PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));
	  GlutPrimitiveNode* handle0 = new GlutPrimitiveNode(lineObject, "N_Control", splineData->points_.size());
	  handle0->get_primitive(0).color = ACG::Vec4f(0,1,0,1);
	  handle0->set_size(0.005*sizeBB.norm());
	  handle0->show();
	  handle0->set_position(insert_Point);
	  handle0->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);
	  handle0->enablePicking(false);
	  lineObject->addAdditionalNode(handle0, name(), "control", splineData->points_.size());

	  emit updatedObject(createSpline_CurrSelIndex_, UPDATE_ALL);
	  splineData->addInterpolatePoint(insert_Point, nor);

	}
	if(_event->type() == QEvent::MouseButtonDblClick) {

		PolyLineObject* lineObject = 0;

		if(!PluginFunctions::getObject(createSpline_CurrSelIndex_, lineObject))
			return;

		GlutPrimitiveNode* control = 0;
		PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));

		for(unsigned int i = 0; i < splineData->points_.size(); i++) {
			lineObject->getAdditionalNode(control, name(), "control", i);
			control->enablePicking(true);
		}

		if(splineData->finishSpline()) {

			for(unsigned int i = 0; i < splineData->handles_.size(); i++) {
				const PolyLineBezierSplineData::InterpolatePoint& control = splineData->getInterpolatePoint(i);
				const ACG::Vec3d hndlPos = splineData->handles_[i], ctrlPos = control.position;

				GlutPrimitiveNode* handle0 = new GlutPrimitiveNode(lineObject, "N_Handle", i);
				handle0->get_primitive(0).color = ACG::Vec4f(0,0,1,1);
				handle0->set_size(0.004*sizeBB.norm());
				handle0->show();
				handle0->enablePicking(true);
				handle0->set_position(hndlPos);
				lineObject->addAdditionalNode(handle0, name(), "handle", i);
				handle0->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);

				GlutLineNode* lineN;
				if(lineObject->getAdditionalNode(lineN,  name(), "line")) {
					lineN->add_line(ctrlPos, hndlPos);
					lineN->add_color(ACG::Vec4f(1,0,0,1));
				}

				emit updatedObject(createSpline_CurrSelIndex_, UPDATE_ALL);
			}
			updatePolyBezierSpline(lineObject, tool_->sb_SplineSegNum->value());
			createSpline_CurrSelIndex_ = -1;
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

namespace {

bool me_GetMeshHit(QMouseEvent* _event, ACG::SceneGraph::GlutPrimitiveNode* moveCircle_SelNode_, ACG::Vec3d& _hit_point, unsigned int& _node_idx, unsigned int& _targetIdx)
{
    unsigned int ndx;
    moveCircle_SelNode_->enablePicking(false);
    bool hasHit = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), ndx, _targetIdx, &_hit_point);
    moveCircle_SelNode_->enablePicking(true);
    BaseObjectData* obj;
    //if there is no current mesh use the newly found
    if(hasHit && PluginFunctions::getPickedObject(ndx, obj) && _node_idx == std::numeric_limits<unsigned int>::max())
        _node_idx = obj->id();
    return hasHit;
}

void me_UpdateCircleData(ACG::Vec3d _hit_point, ACG::Vec3d _onPlane, ACG::Vec3d _nor, ACG::SceneGraph::GlutPrimitiveNode* _moveCircle_SelNode_, PolyLineCircleData* _lineData, bool _isShift)
{
    if(!_moveCircle_SelNode_->name().compare("N_Center"))
    {
         _lineData->circleNormal_ = _nor;
         _lineData->circleCenter_ = _hit_point;
         _lineData->circleSideAxis_ = (_lineData->circleMainAxis_ % _lineData->circleNormal_).normalize();
         _lineData->circleMainAxis_ = (_lineData->circleNormal_ % _lineData->circleSideAxis_).normalize();
    }
    else
    {
        const double cr = (_onPlane - _lineData->circleCenter_).norm();
        const ACG::Vec3d axisa = (_onPlane - _lineData->circleCenter_).normalize();
        if (!_moveCircle_SelNode_->name().compare("N_Handle0")) {
            const ACG::Vec3d axisb = (axisa % _lineData->circleNormal_).normalize();
            _lineData->circleMainRadius_ = cr;

            if (_isShift)
                _lineData->circleSideRadius_ = cr;

            _lineData->circleMainAxis_ = axisa;
            _lineData->circleSideAxis_ = axisb;
        } else {
            const ACG::Vec3d axisb = (_lineData->circleNormal_ % axisa).normalize();
            _lineData->circleSideRadius_ = cr;

            if (_isShift)
                _lineData->circleMainRadius_ = cr;

            _lineData->circleSideAxis_ = axisa;
            _lineData->circleMainAxis_ = axisb;
        }
    }
}
}

void
PolyLinePlugin::
me_move( QMouseEvent* _event )
{
  if((_event->modifiers() & Qt::ShiftModifier) != Qt::ShiftModifier && moveCircle_IsLocked)//alt was pressed but it isn't anymore
        moveCircle_IsLocked = moveCircle_IsFloating = false;

  // MousePress ? -> get reference point
  if (_event->type() == QEvent::MouseButtonPress) {

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    // this is for picking the handles on a circle or spline
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {
      BaseNode* node = find_node( PluginFunctions::getRootNode(), node_idx );
      GlutPrimitiveNode* glutNode = dynamic_cast<GlutPrimitiveNode*>(node);

      if(glutNode) {
        PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(glutNode->line->objectData(CIRCLE_DATA));
        PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(glutNode->line->objectData(BEZSPLINE_DATA));
        if(circleData) {
          moveCircle_SelNode_        = glutNode;
          createCircle_LastSelIndex_ = createCircle_CurrSelIndex_ = glutNode->line->id();
        }
        if(splineData) {
          moveBezSpline_SelNode_ = glutNode;
          createSpline_LastSelIndex_ = moveBezSpline_SelIndex_ = glutNode->line->id();
          moveBezSpline_SelSubIndex_ = glutNode->index;
        }
      }
    }
    //this is for picking the normal poly lines
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point)) {
        BaseObjectData* obj = 0;
        if (PluginFunctions::getPickedObject(node_idx, obj)) {
          // is picked object polyline?
          PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
          if (cur_pol && !cur_pol->objectData(CIRCLE_DATA) && !cur_pol->objectData(BEZSPLINE_DATA)) {//no vertex dragging on circles!

            // Check if we got a line segment or a vertex
            if ( target_idx >= cur_pol->line()->n_vertices() )
              return;

            // save references
            cur_move_id_ = cur_pol->id();

            move_point_ref_ = &(cur_pol->line()->point(target_idx));
          }
        }
    }
  }

  // Move ? -> move reference point
  if (_event->type() == QEvent::MouseMove){
    if (moveCircle_SelNode_) {
        PolyLineObject* lineObject;
        if (!PluginFunctions::getObject(createCircle_CurrSelIndex_, lineObject) || !lineObject->objectData(CIRCLE_DATA))
            return;
        PolyLineCircleData* lineData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));
        ACG::Vec3d hit_point;
        unsigned int target_idx;
        bool hasHit = me_GetMeshHit(_event, moveCircle_SelNode_, hit_point, lineData->circleMeshIndex_, target_idx);
        if(lineData->circleMeshIndex_ == std::numeric_limits<unsigned int>::max()) return;
        if(!moveCircle_IsLocked && hasHit) {
            moveCircle_IsFloating = false;
            ACG::Vec3d x0 = lineData->circleCenter_, n = lineData->circleNormal_;
            ACG::Vec3d onMesh = hit_point;
            double t = ((n | x0) - (n | onMesh)) / n.sqrnorm();
            ACG::Vec3d onPlane = onMesh + t * n;
            TriMeshObject* mesh;
            if (!PluginFunctions::getObject(lineData->circleMeshIndex_, mesh))
                return;

            me_UpdateCircleData(onMesh, onPlane, mesh->mesh()->normal(mesh->mesh()->face_handle(target_idx)), moveCircle_SelNode_, lineData, (_event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
            if (!moveCircle_SelNode_->name().compare("N_Center")) {
                moveCircle_LastHitPos_ = onMesh;
                if((_event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
                    moveCircle_IsLocked = true;
                    moveCircle_LastHitNor_ = lineData->circleNormal_;
                }
            }
        }
        else
        {
            moveCircle_IsFloating = true;
            ACG::Vec3d cameraPos, cameraDir;
            int l,b,w,h;
            PluginFunctions::viewerProperties(0).glState().get_viewport(l,b,w,h);
            PluginFunctions::viewerProperties(0).glState().viewing_ray(_event->pos().x(), h - 1 - _event->pos().y(), cameraPos, cameraDir);
            //intersect the camera ray with a plane located at moveCircle_LastHitPos_ facing moveCircle_LastHitNor_
            const ACG::Vec3d x0 = !moveCircle_SelNode_->name().compare("N_Center") ? moveCircle_LastHitPos_ : lineData->circleCenter_,
                             //the side handles can only be moved on the normal plane
                             n = !moveCircle_SelNode_->name().compare("N_Center") ? (moveCircle_IsLocked ? moveCircle_LastHitNor_ : PluginFunctions::viewingDirection())
                                                                                  : lineData->circleNormal_;
            const double t = ((x0 | n) - (cameraPos | n)) / (cameraDir | n);
            const ACG::Vec3d onPlane = cameraPos + cameraDir * t;

            me_UpdateCircleData(onPlane, onPlane, n, moveCircle_SelNode_, lineData, (_event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

        }
        updateHandles(lineObject);
        updatePolyEllipse(lineObject, tool_->sb_CirclePointNum->value());
    }
    else if(moveBezSpline_SelNode_) {
      PolyLineObject* lineObject;
      if(!PluginFunctions::getObject(moveBezSpline_SelIndex_, lineObject) || !lineObject->objectData(BEZSPLINE_DATA))
          return;
      PolyLineBezierSplineData* lineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));
      ACG::Vec3d hit_point;
      unsigned int target_idx;
      bool hasHit = me_GetMeshHit(_event, moveBezSpline_SelNode_, hit_point, lineData->meshIndex_, target_idx);
      if(lineData->meshIndex_ == std::numeric_limits<unsigned int>::max()) return;
      if(!moveBezSpline_SelNode_->name().compare("N_Control") && hasHit) {
        TriMeshObject* mesh;
        if (!PluginFunctions::getObject(lineData->meshIndex_, mesh))
            return;
        ACG::Vec3d onMesh = hit_point, onMeshNor = mesh->mesh()->normal(mesh->mesh()->face_handle(target_idx));
        int controlIndex = moveBezSpline_SelSubIndex_;
        ACG::Vec3d oldPos = moveBezSpline_SelNode_->get_position();
        moveBezSpline_SelNode_->set_position(onMesh);
        lineData->points_[controlIndex].position = onMesh;
        lineData->points_[controlIndex].normal   = onMeshNor;
        if(controlIndex) {
            int handleIndex = 2 * controlIndex - 1;
            ACG::Vec3d dir = lineData->handles_[handleIndex] - oldPos, side = dir % onMeshNor, forw = (onMeshNor % side).normalize() * dir.norm();
            ACG::Vec3d point = forw + onMesh;
            lineData->handles_[handleIndex] = point;
        }
        if(controlIndex != ((int)lineData->points_.size() - 1)) {
            int handleIndex = 2 * controlIndex;
            ACG::Vec3d dir = lineData->handles_[handleIndex] - oldPos, side = dir % onMeshNor, forw = (onMeshNor % side).normalize() * dir.norm();
            ACG::Vec3d point = forw + onMesh;
            lineData->handles_[handleIndex] = point;
        }
      }
      else if(!moveBezSpline_SelNode_->name().compare("N_Handle")) {
          int handleIndex = moveBezSpline_SelSubIndex_;
          const PolyLineBezierSplineData::InterpolatePoint& control = lineData->getInterpolatePoint(handleIndex);
          //we don't use the mesh location but instead the location on the normal plane!
          ACG::Vec3d cameraPos, cameraDir;
          int l,b,w,h;
          PluginFunctions::viewerProperties(0).glState().get_viewport(l,b,w,h);
          PluginFunctions::viewerProperties(0).glState().viewing_ray(_event->pos().x(), h - 1 - _event->pos().y(), cameraPos, cameraDir);
          double t = ((control.normal | control.position) - (control.normal | cameraPos)) / (control.normal | cameraDir);
          ACG::Vec3d onPlane = cameraPos + t * cameraDir;

          lineData->handles_[handleIndex] = onPlane;
          if(handleIndex % 2 == 1 && handleIndex != ((int)lineData->handles_.size() - 1)) {
            double dist = (lineData->handles_[handleIndex + 1] - control.position).norm();
            if(_event->modifiers() & Qt::ShiftModifier)
              dist = (onPlane - control.position).norm();
            ACG::Vec3d dir = -(onPlane - control.position).normalize();
            lineData->handles_[handleIndex + 1] = control.position + dir * dist;
          }
          if(handleIndex % 2 == 0 && handleIndex) {
            double dist = (lineData->handles_[handleIndex - 1] - control.position).norm();
            if(_event->modifiers() & Qt::ShiftModifier)
              dist = (onPlane - control.position).norm();
            ACG::Vec3d dir = -(onPlane - control.position).normalize();
            lineData->handles_[handleIndex - 1] = control.position + dir * dist;
          }
      }
      GlutLineNode* lineN;
      if(!lineObject->getAdditionalNode(lineN, name(), "line", 0))
        return;
      updatePolyBezierHandles(lineObject, lineN);
      updatePolyBezierSpline(lineObject, tool_->sb_SplineSegNum->value());
    }
    else if (move_point_ref_ != 0) {
        ACG::Vec3d hit_point;
        unsigned int node_idx, target_idx;
      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point)) {
        (*move_point_ref_) = (PolyLine::Point) hit_point;
        // update
        emit updatedObject(cur_move_id_, UPDATE_GEOMETRY);
      }
    }
  }

  // Release ? -> release reference point
  if (_event->type() == QEvent::MouseButtonRelease) {
      if((_event->modifiers() & Qt::ShiftModifier) != Qt::ShiftModifier) {
          //in case we are not dragging the center and not pressing alt -> project to mesh
         // if(moveCircle_SelNode_ && moveCircle_SelNode_->name().compare("N_Center"))
              moveCircle_IsLocked = moveCircle_IsFloating = false;
          PolyLineObject* lineObject;
          if (PluginFunctions::getObject(createCircle_CurrSelIndex_, lineObject))
          {
              PolyLineCircleData* lineData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));
              moveCircle_IsLocked = false;
              updateHandles(lineObject);
              updatePolyEllipse(lineObject, tool_->sb_CirclePointNum->value());
          }
      }
      move_point_ref_ = 0;
      moveCircle_SelNode_ = 0;
      createCircle_CurrSelIndex_ = -1;
      moveBezSpline_SelNode_ = 0;
      moveBezSpline_SelIndex_ = -1;
  }
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
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {

      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {
        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);

        if (cur_pol) {

          // Check if we got a line segment or a vertex
          if ( target_idx >= cur_pol->line()->n_vertices() )
            return;

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
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {
      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {
        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
        if (cur_pol) {

          // Check if we got a line segment or a vertex
          if ( target_idx >= cur_pol->line()->n_vertices() )
            return;

          if (target_idx == cur_pol->line()->n_vertices() - 1 || target_idx == 0) {
            if (target_idx == 0) {
              cur_pol->line()->invert();
              //target_idx = cur_pol->line()->n_vertices() - 1;
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

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {

      bool merged = false;

      BaseObjectData* obj = 0;

      if (PluginFunctions::getPickedObject(node_idx, obj)) {

        // is picked object polyline? -> get second polyline
        PolyLineObject* second_pol = PluginFunctions::polyLineObject(obj);

        if (second_pol) {

          // Check if we got a line segment or a vertex
          if ( target_idx >= second_pol->line()->n_vertices() )
            return;

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
  if (_event->type() == QEvent::MouseButtonPress) {

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // pick
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {

      BaseObjectData* obj = 0;
      if (PluginFunctions::getPickedObject(node_idx, obj)) {

        // is picked object polyline?
        PolyLineObject* cur_pol = PluginFunctions::polyLineObject(obj);
        if (cur_pol) {

		  cur_polyline_obj_ = cur_pol;

          // Check if we got a line segment or a vertex
          if ( target_idx >= cur_pol->line()->n_vertices() )
            return;

          // save references
          cur_smart_move_obj_ = cur_pol;
          if (cur_pol->line()->vertex_selections_available()) {
            if (!(_event->modifiers() & (Qt::ShiftModifier)))
              cur_pol->line()->vertex_selection(target_idx) = true;
            else {
              cur_pol->line()->vertex_selection(target_idx) = false;
              emit updateView();
            }
          }

        }

        // start timer
        if (!(_event->modifiers() & (Qt::ShiftModifier)))
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

//-----------------------------------------------------------------------------

void
PolyLinePlugin::
slotEditModeChanged()
{
  PluginFunctions::pickMode("PolyLine");
  PluginFunctions::actionMode(Viewer::PickingMode);
}

//-----------------------------------------------------------------------------

void
PolyLinePlugin::
updateHandles(PolyLineObject* _lineObject)
{
  PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(_lineObject->objectData(CIRCLE_DATA) );
  GlutPrimitiveNode* H0, *H1, *C;
  _lineObject->getAdditionalNode(C,  name(), "circle");
  _lineObject->getAdditionalNode(H0, name(), "handle0");
  _lineObject->getAdditionalNode(H1, name(), "handle1");
  ACG::Vec3d h0 = circleData->circleCenter_ + circleData->circleMainAxis_ * circleData->circleMainRadius_,
             h1 = circleData->circleCenter_ + circleData->circleSideAxis_ * circleData->circleSideRadius_;
  if(C)
      C->set_position(circleData->circleCenter_);
  if(H0)
      H0->set_position(moveCircle_IsFloating ? h0 : createCircle_getHit(circleData, h0));
  if(H1)
      H1->set_position(moveCircle_IsFloating ? h1 : createCircle_getHit(circleData, h1));
}

void
PolyLinePlugin::
slot_setCirclePointNum(int i)
{
  PolyLineObject* _lineObject;
  if(createCircle_LastSelIndex_ != -1 && PluginFunctions::getObject(createCircle_LastSelIndex_, _lineObject))
    updatePolyEllipse(_lineObject, i);
}

void
PolyLinePlugin::
slot_setSplinePointNum(int i)
{
  PolyLineObject* _lineObject;
  if(createSpline_LastSelIndex_ != -1 && PluginFunctions::getObject(createSpline_LastSelIndex_, _lineObject))
	  updatePolyBezierSpline(_lineObject, i);
}

//-----------------------------------------------------------------------------

void 
PolyLinePlugin::
slotPickToolbarAction(QAction* _action) {
  if ( _action == insertAction_ ) {
    tool_->rb_insert->setChecked(true);
  } else if ( _action == insertCircleAction_ ) {
	tool_->rb_InsertCircle->setChecked(true);
  } else if ( _action == insertSplineAction_ ) {
	tool_->rb_InsertSpline->setChecked(true);
  }  else if ( _action == deleteAction_ ) {
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

bool
PolyLinePlugin::
pick_triangle_mesh( QPoint mPos,
						TriMeshObject*& _mesh_object_, TriMesh::FaceHandle& _fh, TriMesh::VertexHandle& _vh, ACG::Vec3d& _hitPoint)
{
  // invalidate return values
  _fh = TriMesh::FaceHandle  (-1);
  _vh = TriMesh::VertexHandle(-1);

  unsigned int target_idx = 0, node_idx = 0;
  ACG::Vec3d hit_point;



  if( PluginFunctions::scenegraphPick( ACG::SceneGraph::PICK_FACE, mPos, node_idx, target_idx, &hit_point ) )
  {
    // first check if a sphere was clicked
    BaseObjectData* object(0);
    if( PluginFunctions::getPickedObject( node_idx, object ) )
    {
      if( object->picked( node_idx ) && object->dataType(DATA_TRIANGLE_MESH) )
      {
        // get mesh object
        _mesh_object_ = dynamic_cast<TriMeshObject*>(object);

        // get mesh and face handle
        TriMesh & m = *PluginFunctions::triMesh(object);
        _fh = m.face_handle(target_idx);

        TriMesh::FaceVertexIter fv_it(m,_fh);
        TriMesh::VertexHandle closest = *fv_it;
        float shortest_distance = (m.point(closest) - hit_point).sqrnorm();

        ++fv_it;
        if ( (m.point(*fv_it ) - hit_point).sqrnorm() < shortest_distance ) {
          shortest_distance = (m.point(*fv_it ) - hit_point).sqrnorm();
          closest = *fv_it;
        }

        ++fv_it;
        if ( (m.point(*fv_it ) - hit_point).sqrnorm() < shortest_distance ) {
          // Unnecessary : shortest_distance = (m.point(*fv_it ) - hit_point).sqrnorm();
          closest = *fv_it;
        }

        // stroe closest vh
        _vh = closest;
        _hitPoint = hit_point;

        return true;
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( PolyLinePlugin , PolyLinePlugin );
#endif


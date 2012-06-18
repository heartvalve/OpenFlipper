#include "SkeletonEditingPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>
#include <ObjectTypes/Skeleton/BaseSkin.hh>
#include <ObjectTypes/Skeleton/SkeletonObjectData.hh>
#include <ACG/Geometry/Algorithms.hh>
//--------------------------------------------------------------------------------

/** \brief Default Constructor
 *
 */
SkeletonEditingPlugin::SkeletonEditingPlugin() :
        toolbar_(0),
        toolBarActions_(0),
        skeletonEditingAction_(0),
        pickToolbar_(0),
        pickToolBarActions_(0),
        selectJointAction_(0),
        insertJointAction_(0),
        splitJointAction_(0),
        deleteJointAction_(0),
        moveJointAction_(0),
        transformChildManipAction_(0),
        transformAllManipAction_(0),
        rotateManipAction_(0),
        inverseKinematicAction_(0),

        currentSkeleton_(-1),
        currentJoint_(-1),
        jointPreview_(false),
        transformChildJoints_(false),
        transformAllFrames_(true),
        inverseKinematic_(false),
        dblClick_(false),
        lastRenderer_(""),
        rendererChanged_(false),
        manip_size_(1.0),
        manip_size_modifier_(1.0)

{
}

//--------------------------------------------------------------------------------

/** \brief Destructor
 *
 */
SkeletonEditingPlugin::~SkeletonEditingPlugin()
{

}


/*******************************************************************************
        BaseInterface implementation
 *******************************************************************************/

/** \brief Initialization of the plugin when it is loaded by the core
 *
 */
void SkeletonEditingPlugin::pluginsInitialized() {

  emit addHiddenPickMode("MoveJoints");
  emit addHiddenPickMode("DeleteJoints");
  emit addHiddenPickMode("InsertJoints");
  emit addHiddenPickMode("SplitJoints");
  emit addHiddenPickMode("SelectJoints");
  emit setPickModeMouseTracking ("MoveJoints", true);
  emit setPickModeMouseTracking ("InsertJoints", true);

  //KEYS
  emit registerKey (Qt::Key_Shift, Qt::ShiftModifier, tr("Manipulator rotation"), true);
  emit registerKey (Qt::Key_Shift, Qt::NoModifier, tr("Manipulator rotation"), true);

  //TOOLBAR
  toolbar_ = new QToolBar(tr("Skeleton Editing"));
  toolbar_->setObjectName("Skeleton_Editing_Toolbar");


  toolBarActions_ = new QActionGroup(toolbar_);

  WhatsThisGenerator whatsThisGen("SkeletonEditing");
  skeletonEditingAction_ = new QAction(tr("<B>Skeleton Editing</B><br>Modify the structure of a skeleton"), toolBarActions_);
  skeletonEditingAction_->setStatusTip(tr("Modify the structure of a skeleton."));
  skeletonEditingAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_editing.png") );
  whatsThisGen.setWhatsThis(skeletonEditingAction_,tr("Skeleton Editing"));
  skeletonEditingAction_->setCheckable(true);
  toolbar_->addAction(skeletonEditingAction_);

  connect(toolBarActions_, SIGNAL(triggered(QAction*)), this, SLOT(slotSetEditingMode(QAction*)) );

  emit addToolbar(toolbar_);

  pickToolbar_ = new QToolBar(tr("Skeleton Editing"));
  pickToolbar_->setAttribute(Qt::WA_AlwaysShowToolTips, true);
  pickToolbar_->setObjectName("Skeleton_Editing_Picking_Toolbar");
  pickToolBarActions_ = new QActionGroup(pickToolbar_);
  pickToolBarActions_->setExclusive (false);

  selectJointAction_ = new QAction(tr("<B>Select Joint</B><br>Toggle joint selection"), pickToolBarActions_);
  selectJointAction_->setStatusTip(tr("Toggle the selection state of a joint."));
  selectJointAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_selectJoint.png") );
  selectJointAction_->setCheckable(true);
  whatsThisGen.setWhatsThis(selectJointAction_,tr("Select single joints of the skeleton. You can select multiple joints at once."), "manipulate_joint");
  pickToolbar_->addAction(selectJointAction_);

  insertJointAction_ = new QAction(tr("<B>Insert Joint</B><br>Add a joint to the skeleton"), pickToolBarActions_);
  insertJointAction_->setStatusTip(tr("<DoubleClick> to start a new skeleton and end joint path. <Click> to select parent and position for new joints."));
  insertJointAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_insertJoint.png") );
  whatsThisGen.setWhatsThis(insertJointAction_,tr("Insert new joints. Click on a joint first and then click somewhere else, to add a new joint at the mouseposition. Double-click to finish adding joints."));
  insertJointAction_->setCheckable(true);
  pickToolbar_->addAction(insertJointAction_);

  splitJointAction_ = new QAction(tr("<B>Split Joint</B><br>Add a Joint between two other Joints"), pickToolBarActions_);
  splitJointAction_->setStatusTip(tr("Click on a joint which will be the new child Joint."));
  splitJointAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_splitJoint.png") );
  whatsThisGen.setWhatsThis(splitJointAction_,tr("Click on a joint to split it in the middle."), "manipulate_joint");
  splitJointAction_->setCheckable(true);
  pickToolbar_->addAction(splitJointAction_);

  deleteJointAction_ = new QAction(tr("<B>Delete Joint</B><br>Remove a joint from the skeleton"), pickToolBarActions_);
  deleteJointAction_->setStatusTip(tr("<Press> to select a joint. <Release> to delete it."));
  deleteJointAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_deleteJoint.png") );
  whatsThisGen.setWhatsThis(deleteJointAction_,tr("Delete a joint by clicking on it."), "manipulate_joint");
  deleteJointAction_->setCheckable(true);
  pickToolbar_->addAction(deleteJointAction_);

  moveJointAction_ = new QAction(tr("<B>Transform Joint</B><br>Transform a joint from the skeleton"), pickToolBarActions_);
  moveJointAction_->setStatusTip(tr("Transform a joint from the skeleton."));
  moveJointAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_moveJoint.png") );
  whatsThisGen.setWhatsThis(moveJointAction_,tr("Transform a joint (only possible in an animation). Click on a joint and transform his position and orientation with help of the manipulator"), "manipulate_joint");
  moveJointAction_->setCheckable(true);
  pickToolbar_->addAction(moveJointAction_);

  pickToolbar_->addSeparator();

  rotateManipAction_ = new QAction(tr("Rotate manipulator"), pickToolBarActions_);
  rotateManipAction_->setStatusTip(tr("Rotate manipulator. <Shift>"));
  rotateManipAction_->setToolTip(tr("<B>Rotate manipulator</B><br> Rotates only the manipulator, not the joints. <B>Shift</B>"));
  rotateManipAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_maniprotate.png") );
  whatsThisGen.setWhatsThis(rotateManipAction_,tr("Rotate a joint (possible in an animation). Click on a joint and transform his orientation with help of the manipulator."),"manipulator");
  rotateManipAction_->setCheckable(true);
  pickToolbar_->addAction(rotateManipAction_);
  connect(rotateManipAction_, SIGNAL(toggled(bool)), this, SLOT(slotRotateManipulator(bool)) );

  pickToolbar_->addSeparator();

  transformAllManipAction_ = new QAction(tr(""), pickToolBarActions_);
  transformAllManipAction_->setStatusTip(tr("Apply the relative transformation to all frames of the animation."));
  transformAllManipAction_->setToolTip(tr("<B>Transform whole animation</B><br>Transform all frames of the animation simultaneously."));
  transformAllManipAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_manipAllFrames.png") );
  whatsThisGen.setWhatsThis(transformAllManipAction_,tr("Transform whole animation. Changing will be applied to all frames of the current animation"),"transformation_modi");
  transformAllManipAction_->setCheckable(true);
  transformAllManipAction_->setChecked(transformAllFrames_);
  pickToolbar_->addAction(transformAllManipAction_);

  transformChildManipAction_ = new QAction(tr("Transform Child Joints"), pickToolBarActions_);
  transformChildManipAction_->setStatusTip(tr("Apply joint transformation to child joints as well and thereby rigidly move the whole subtree."));
  transformChildManipAction_->setToolTip(tr("<B>Transform child joints</B><br>Apply joint transformation to all child joints as well."));
  transformChildManipAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_manipChildTransform.png") );
  whatsThisGen.setWhatsThis(transformChildManipAction_,tr("Apply your transformation to all child joints."),"transformation_modi");
  transformChildManipAction_->setCheckable(true);
  transformChildManipAction_->setChecked(transformChildJoints_);
  pickToolbar_->addAction(transformChildManipAction_);

  inverseKinematicAction_ = new QAction(tr("Inverse kinematic"), pickToolBarActions_);
  inverseKinematicAction_->setStatusTip(tr("Move selected joint using inverse kinematic."));
  inverseKinematicAction_->setToolTip(tr("<B>Inverse kinematic</B><br>Move selected joint using inverse kinematic."));
  inverseKinematicAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeleton_inverseKinematic.png") );
  whatsThisGen.setWhatsThis(inverseKinematicAction_,tr("Move a joint to the position only with rotation of the previous joints."),"transformation_modi");
  inverseKinematicAction_->setCheckable(true);
  inverseKinematicAction_->setChecked(inverseKinematic_);
  pickToolbar_->addAction(inverseKinematicAction_);

  connect(pickToolBarActions_, SIGNAL(triggered(QAction*)), this, SLOT(slotPickToolbarAction(QAction*)) );

  emit setPickModeToolbar("MoveJoints", pickToolbar_);
  emit setPickModeToolbar("DeleteJoints", pickToolbar_);
  emit setPickModeToolbar("InsertJoints", pickToolbar_);
  emit setPickModeToolbar("SplitJoints", pickToolbar_);
  emit setPickModeToolbar("SelectJoints", pickToolbar_);

  setDescriptions();
}

//------------------------------------------------------------------------------

void SkeletonEditingPlugin::slotObjectUpdated( int _id, const UpdateType& _type){

  if ( !_type.contains(UPDATE_GEOMETRY) )
    return;

  for (uint i=0; i < activeManipulators_.size(); i++){

    if ( activeManipulators_[i] != _id )
      continue;

    BaseObjectData* obj = 0;

    PluginFunctions::getObject( activeManipulators_[i], obj );

    if (obj != 0 && obj->manipPlaced())
      updateManipulatorPosition(obj);
  }
}

/*******************************************************************************
        ToolBoxInterface implementation
 *******************************************************************************/

void SkeletonEditingPlugin::initializePlugin()
{

}


/*******************************************************************************
        MouseInterface implementation
 *******************************************************************************/

void SkeletonEditingPlugin::slotMouseWheelEvent(QWheelEvent * _event, const std::string & /*_mode*/)
{
  // Skip execution if this is not our pick mode
  if( (PluginFunctions::pickMode() != "MoveJoints") || PluginFunctions::actionMode() != Viewer::PickingMode)
    return;

  // compute the manipulator size modifier based on the mouse wheel change
  manip_size_modifier_ = manip_size_modifier_ - (float)_event->delta() / 120.0 * 0.1;

  // Resize all manipulators based on the modifier on all objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
    o_it->manipulatorNode()->set_size(manip_size_ * manip_size_modifier_);

  // Redraw scene with updated manipulators
  emit updateView();
}

//------------------------------------------------------------------------------

/** \brief checks, if the skeleton can be modified.
 *  a skeleton can be modified, if the active pose is the reference pose
 * @param _event the event that occurred
 */
bool SkeletonEditingPlugin::canModify(QMouseEvent* _event)
{
	//if jointPreview_ is true, we could modify the skeleton
	//so the skeleton is in reference pose
	if (jointPreview_ || (PluginFunctions::pickMode() == ("InsertJoints") && _event->type() == QEvent::MouseButtonDblClick))
		return true;

	// try to select a joint from which the insertion should be started
	unsigned int    node_idx, target_idx;
	ACG::Vec3d      hitPoint;
	BaseObjectData* object;

	//disable picking for anything but skeletons
	for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
		o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

	//perform picking
	bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx,
			target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

	//reenable picking for anything
	for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
		o_it->enablePicking( true );

	if ( successfullyPicked )
	{

		//check if the active pose is the reference pose
		Skeleton* skeleton = PluginFunctions::skeleton( object );

		if ( !skeleton )
			return false;

		if (activePose(PluginFunctions::skeletonObject(object)) == skeleton->referencePose())
			return true;
		else
			return false;
	}
	else
		return false;
}

//------------------------------------------------------------------------------

/** \brief MousePress event occured
 *
 * @param _event the event that occured
 */
void SkeletonEditingPlugin::slotMouseEvent(QMouseEvent* _event) {

  if (PluginFunctions::actionMode() != Viewer::PickingMode)
    return;

  if ( PluginFunctions::pickMode() == ("MoveJoints") )
  	moveJoint(_event);
  else if ( PluginFunctions::pickMode() == ("SelectJoints") )
  	selectJoint(_event);
  else if ( (( PluginFunctions::pickMode() == ("DeleteJoints")) ||
             ( PluginFunctions::pickMode() == ("InsertJoints")) ||
             ( PluginFunctions::pickMode() == ("SplitJoints") ) ) && canModify(_event) )
  {
  	if ( PluginFunctions::pickMode() == ("DeleteJoints") )
  		deleteJoint(_event);
  	else if ( PluginFunctions::pickMode() == ("InsertJoints") )
  		insertJoint(_event);
  	else if (PluginFunctions::pickMode() == ("SplitJoints"))
  		splitJoint(_event);
  }


}

/*******************************************************************************
        KeyInterface implementation
 *******************************************************************************/

void SkeletonEditingPlugin::slotKeyEvent (QKeyEvent* _event)
{
  if (_event->key() == Qt::Key_Shift)
    rotateManipAction_->setChecked(true);
}

//------------------------------------------------------------------------------

void SkeletonEditingPlugin::slotKeyReleaseEvent (QKeyEvent* _event)
{
  if ( _event->key() == Qt::Key_Shift)
    rotateManipAction_->setChecked(false);
}

/*******************************************************************************
        PickingInterface implementation
 *******************************************************************************/

/** \brief slot is called when the pickMode changed
 *
 * @param _mode new pickMode
 */
void SkeletonEditingPlugin::slotPickModeChanged( const std::string& _mode)
{
  if (_mode != "InsertJoints" )
    if ( insertJointAction_->isChecked() )
      cancelJointInsertion();

  moveJointAction_->setChecked(   _mode == "MoveJoints" );
  insertJointAction_->setChecked( _mode == "InsertJoints" );
  deleteJointAction_->setChecked( _mode == "DeleteJoints" );
  selectJointAction_->setChecked( _mode == "SelectJoints" );
  splitJointAction_->setChecked(  _mode == "SplitJoints" );

  skeletonEditingAction_->setChecked( (_mode == "MoveJoints")  ||(_mode == "InsertJoints")
                                    ||(_mode == "DeleteJoints")||(_mode == "SelectJoints")
                                    || (_mode == "SplitJoints"));

  // We left the pickmodes, that are used for skeletonediting
  // If the mode is "", we are in examine mode or anything else
  // But we do not change the renderer than
  if ( (_mode != "")              &&
       (_mode != "MoveJoints")    &&
       (_mode != "InsertJoints")  &&
       (_mode != "DeleteJoints")  &&
       (_mode != "SelectJoints")  &&
       (_mode != "SplitJoints")) {

    // Get the currently active renderer and switch to default one
    QString currentRenderer;
    emit getCurrentRenderer(PluginFunctions::activeExaminer(), currentRenderer);

    // If we still have the Depth peeling renderer, we switch back
    // otherwise the user changed the active renderer and we do not override the setting
    if ( rendererChanged_ && (currentRenderer == "Depth Peeling Renderer" ) ) {
      emit setRenderer(PluginFunctions::activeExaminer(),lastRenderer_);
      rendererChanged_ = false;
    }


  }


  showManipulators();
}

/*******************************************************************************
        SkeletonEditingPlugin implementation
 *******************************************************************************/

void SkeletonEditingPlugin::slotPickToolbarAction(QAction* _action)
{
	if (_action == rotateManipAction_)//in this case, enable the button and keep the rest as it is
		return;

  if (_action != insertJointAction_)
    if ( insertJointAction_->isChecked() )
      cancelJointInsertion();

  if (_action == insertJointAction_){
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("InsertJoints");

  }else if (_action == splitJointAction_){
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("SplitJoints");

  }else if (_action == deleteJointAction_){
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("DeleteJoints");

  } else if (_action == moveJointAction_) {
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("MoveJoints");
  } else if (_action == selectJointAction_) {
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("SelectJoints");

  }else if (_action == transformAllManipAction_)
    transformAllFrames_ = transformAllManipAction_->isChecked();
  else if (_action == transformChildManipAction_)
    transformChildJoints_ = transformChildManipAction_->isChecked();
  else if (_action == inverseKinematicAction_)
  	inverseKinematic_ = inverseKinematicAction_->isChecked();

  moveJointAction_->setChecked( _action == moveJointAction_ );
  insertJointAction_->setChecked( _action == insertJointAction_ );
  deleteJointAction_->setChecked( _action == deleteJointAction_ );
  selectJointAction_->setChecked( _action == selectJointAction_ );
  splitJointAction_->setChecked( _action == splitJointAction_ );
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::slotSetEditingMode(QAction* /*_action*/)
{

  if ( ! rendererChanged_ ) {

    // Get the currently active renderer and switch to default one
    emit getCurrentRenderer(PluginFunctions::activeExaminer(), lastRenderer_);
    emit setRenderer(PluginFunctions::activeExaminer(),"Depth Peeling Renderer");

    // remember, that we changed the active renderer in this plugin
    // to prevent switching, if we did not do it
    rendererChanged_ = true;
  }

  PluginFunctions::actionMode(Viewer::PickingMode);
  PluginFunctions::pickMode("MoveJoints");
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::slotRotateManipulator(bool _toggled)
{
  QtTranslationManipulatorNode::ManipulatorMode mode;

  bool ourPickMode = (PluginFunctions::actionMode() == Viewer::PickingMode)
                   &&( ( PluginFunctions::pickMode() == ("DeleteJoints") )
                     ||( PluginFunctions::pickMode() == ("InsertJoints") )
                     ||( PluginFunctions::pickMode() == ("MoveJoints") )
                     ||( PluginFunctions::pickMode() == ("SelectJoints")
                     ||( PluginFunctions::pickMode() == ("SplitJoints"))));

  if (_toggled && ourPickMode){
    mode = QtTranslationManipulatorNode::LocalRotation;
    PluginFunctions::setViewObjectMarker (&objectMarker_);
  } else {
    mode = QtTranslationManipulatorNode::TranslationRotation;
    PluginFunctions::setViewObjectMarker (PluginFunctions::defaultViewObjectMarker ());
  }

  for (uint i=0; i < activeManipulators_.size(); i++){

    BaseObjectData* obj = 0;

    PluginFunctions::getObject( activeManipulators_[i], obj );

    if (obj != 0 && obj->manipPlaced())
      obj->manipulatorNode()->setMode ( mode );
  }
}

//------------------------------------------------------------------------------

/** \brief Place and show the Manipulator
 *
 * @param _event  mouseEvent that occured
 */
void SkeletonEditingPlugin::placeManip(QMouseEvent * _event) {
    unsigned int node_idx, target_idx;
    OpenMesh::Vec3d hitPoint;
    BaseObjectData* object;

    bool successfullyPicked = false;

    int data = -1;

    //disable picking for anything but skeletons
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

    //perform picking
    successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hitPoint) &&
                         PluginFunctions::getPickedObject(node_idx, object);

    //reenable picking for anything
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( true );

    if ( successfullyPicked && object->dataType(DATA_SKELETON) ) {

      hitPoint = getNearestJoint(PluginFunctions::skeletonObject(object), hitPoint, data);

      PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED | ACG::SceneGraph::DrawModes::WIREFRAME );
    }

    if (successfullyPicked) {

        object->manipPlaced(true);

        if (data != -1)
          object->manipulatorNode()->setData( data );

        object->manipulatorNode()->loadIdentity();
        object->manipulatorNode()->set_center(hitPoint);
        object->manipulatorNode()->set_draw_cylinder(true);
        object->manipulatorNode()->set_autosize(QtTranslationManipulatorNode::Once);
        object->manipulatorNode()->set_size(manip_size_ * manip_size_modifier_);
        object->manipulatorNode()->setMode( QtTranslationManipulatorNode::TranslationRotation );
        object->manipulatorNode()->show();

        // Get the global matrix from the picked joint
        Skeleton* skeleton = PluginFunctions::skeleton( object );
        Skeleton::Joint* pickedJoint = skeleton->joint( data );
        Skeleton::Pose* currentPose = activePose(PluginFunctions::skeletonObject(object));
        const Matrix4x4 pickedGlobalMatrix = currentPose->globalMatrix(pickedJoint->id());

        // Orient the manipulator to be aligned with the joint coordinate system.
        Vector x_axis = pickedGlobalMatrix.transform_vector(Vector(1.0, 0.0, 0.0));
        Vector y_axis = pickedGlobalMatrix.transform_vector(Vector(0.0, 1.0, 0.0));

        object->manipulatorNode()->set_direction(x_axis,y_axis);
        object->manipulatorNode()->enable_rotations(QtTranslationManipulatorNode::ALL_AXIS);

        object->manipulatorNode()->apply_transformation(false);


        // Disconnect a previously connected Signal
        disconnect(object->manipulatorNode() , SIGNAL(manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)),
                   this , SLOT( manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)));

        disconnect(object->manipulatorNode() , SIGNAL(positionChanged(QtTranslationManipulatorNode*)),
                  this , SLOT( ManipulatorPositionChanged(QtTranslationManipulatorNode*)));

        // Reconnect the signals.
        connect(object->manipulatorNode() , SIGNAL(manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)),
                this , SLOT( manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)));

        connect(object->manipulatorNode() , SIGNAL(positionChanged(QtTranslationManipulatorNode*)),
                this , SLOT( ManipulatorPositionChanged(QtTranslationManipulatorNode*)));

        emit updateView();

        bool found = false;

        for (uint i=0; i < activeManipulators_.size(); i++)
          if ( activeManipulators_[i] == object->id() ){
            found = true;
            break;
          }

        if ( !found )
          activeManipulators_.push_back( object->id() );

    }
}
//------------------------------------------------------------------------------
/*
 * \brief move a joint using inverse Kinematic
 * @param dest the new position of the joint
 * @param currentPose the current pose in which the joint is moved
 * @param pickedJoint your picked joint
 * @param rotatbleJoints joints, which can be rotate. It has to be a ordered set in this order (picked,marked]. "picked" is our imaginary -1th element, his parent the 0th element and so on. "marked" is the last element. "marked"s matrix can rotate, but the "marked" will not be translated.
 */
void SkeletonEditingPlugin::inverseKinematic(ACG::Vec3d dest,Skeleton::Pose* currentPose,Skeleton::Joint* pickedJoint, std::vector<Skeleton::Joint*> rotatableJoints)
{
	//Now we can rotate every Joint in rotatableJoints
	ACG::Vec3d pickedJointPos = currentPose->globalMatrix(pickedJoint->id()).transform_point(ACG::Vec3d(0.0,0.0,0.0));
	for(std::size_t i = 5; i && pickedJointPos != dest; --i )
	{

		//iterates through the skeleton from our picked joint to our fixed/root
		for (std::vector<Skeleton::Joint*>::iterator iter = rotatableJoints.begin() ; iter != rotatableJoints.end();  ++iter)
		{
			const unsigned int currentId = (*iter)->id();

			//when we rotate to the false side
			//we are going to compute a new angle
			//it is more precise
			int angleFac = -1;
			bool rightRotation = true;
			unsigned int tries = 0;//to be sure that we are terminate
			do
			{
				//update the position of our picked Joint
				pickedJointPos = currentPose->globalMatrix(pickedJoint->id()).transform_point(ACG::Vec3d(0.0,0.0,0.0));

				//get the position and the matrix of our current joint
				const ACG::Matrix4x4d currentGlobalMatrix = currentPose->globalMatrix(currentId);
				const ACG::Vec3d currentJointPos = currentGlobalMatrix.transform_point(ACG::Vec3d(0.0,0.0,0.0));

				ACG::Vec3d toDest = currentJointPos - dest; //distance from current to Destination
				ACG::Vec3d toPickedJoint = currentJointPos - pickedJointPos;//distance from current to picked Joint

				//get angle of toDest and toPickedJoint
				double theta = (double)angleFac*acos(dot(toPickedJoint ,toDest) / sqrt( toDest.sqrnorm() * toPickedJoint.sqrnorm()));

				if(theta != theta || theta == 0)//test nan
					break;

				//get rotation matrix for theta and our rotation axis
				ACG::Vec3d rotationAxis = cross(toDest,toPickedJoint);
				ACG::Quaterniond quaternion(rotationAxis ,theta);

				ACG::Matrix4x4d changingMatrix = currentGlobalMatrix * quaternion.rotation_matrix();
				ACG::Vec3d currentTranslation = currentPose->globalTranslation( currentId );

				//set position and rotation of our currentJoint, so pickedJoint is moving to our destination
				currentPose->setGlobalMatrix(currentId, changingMatrix, false);

				//////////////todo compute direction of the angle "theta"
				//test new position
				//it is false, when the distance of pickedJoint and destination is far more away then before
				ACG::Vec3d pickedJointPosOld = pickedJointPos;
				pickedJointPos = currentPose->globalMatrix(pickedJoint->id()).transform_point(ACG::Vec3d(0.0,0.0,0.0));
				//compare the distance of the old/new position
				bool rightRotation = !( (pickedJointPos -dest).sqrnorm() > (pickedJointPosOld -dest).sqrnorm());

				if ( !rightRotation )
				{
					//not the right rotation? then rotate back and compute a new angle
					ACG::Quaterniond quaternionBack(rotationAxis ,-theta);
					ACG::Vec3d currentTranslation = currentPose->globalTranslation( currentId );

					//rotate back and compute our angle again
					changingMatrix = currentGlobalMatrix * quaternionBack.rotation_matrix();

					currentPose->setGlobalMatrix(currentId, changingMatrix , false);
					currentPose->setGlobalTranslation(currentId, currentTranslation, false);

					//compute angle again and this time, we try the other side for our rotation
					++tries;
					angleFac *= 1;
				}
			}while( rightRotation && tries <= 5);
		}
	}
}

//------------------------------------------------------------------------------

/** \brief move the object when its manipulator moves
 *
 * @param _node the manipulator node
 * @param _event the mouse event
 */
void SkeletonEditingPlugin::manipulatorMoved( QtTranslationManipulatorNode* _node , QMouseEvent* _event) {

  // React on event only in move mode
  if ( PluginFunctions::pickMode() != "MoveJoints" )
    return;

  OpenFlipper::Options::redrawDisabled( true );

  int objectId = _node->getIdentifier();

  // rotation matrix from the manipulator
  ACG::Matrix4x4d mat;
  mat.identity();
  mat = _node->matrix();

  BaseObjectData* object = 0;
  PluginFunctions::getObject(objectId, object);

  if(inverseKinematic_)
  {
    Skeleton* skeleton = PluginFunctions::skeleton( object );
    SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(object);

    if (!skeleton || !skeletonObj)
    	return;

    Skeleton::Joint* pickedJoint = skeleton->joint( _node->getData().toInt() );

    if (!pickedJoint)
    	return;

    Skeleton::Pose* currentPose = activePose(PluginFunctions::skeletonObject(object));

    const Matrix4x4 pickedGlobalMatrix = currentPose->globalMatrix(pickedJoint->id());

    //get our destination
    const ACG::Vec3d dest = (mat*pickedGlobalMatrix).transform_point( ACG::Vec3d(0.0,0.0,0.0) );

    if (pickedJoint->isRoot())
    {
    	//we have no fixed points, when we pick our root
    	currentPose->setGlobalTranslation(pickedJoint->id(), dest, false);
    }
    else
    {
    	//get all Parent Joints (picked,fixed]
    	std::vector<Skeleton::Joint*> rotatableJoints;

    	Skeleton::Iterator iter = pickedJoint->parent();

    	//go from our picked Joint and find the first fixed/root Joint
    	for (;iter && !iter->selected() && !iter->isRoot(); iter = iter->parent())
    		rotatableJoints.push_back(*iter);

    	if (iter)
    		rotatableJoints.push_back(*iter); //save the fixed/root Joint, because he can rotate
    	else
    		return;

    	inverseKinematic(dest, currentPose, pickedJoint, rotatableJoints);

    	if (transformAllFrames_ && _event->type() == QEvent::MouseButtonRelease)
    		for (std::vector<Skeleton::Joint*>::iterator iter = rotatableJoints.begin() ; iter != rotatableJoints.end();  ++iter)
    		{
    			ACG::Vec3d position = currentPose->globalTranslation((*iter)->id());
    			setJointPosition(skeletonObj,*iter,position);
    		}
    }
    //update
    emit updatedObject(objectId, UPDATE_GEOMETRY);
  }
  else
  	transformJoint( objectId, _node->getData().toInt(), mat );

  // Reset Node
  _node->loadIdentity();
  _node->set_center(mat.transform_point(_node->center()));


  if (_event->type() == QEvent::MouseButtonPress)
    accumMatrix_.identity();

  accumMatrix_ *= mat;

  //only backup on mouseRelease
  if ( (_event->type() == QEvent::MouseButtonRelease) && !accumMatrix_.is_identity() )
    emit createBackup(objectId, "Joint Transformation", UPDATE_GEOMETRY);

  if ( object != 0)
    //check if there is a skin which has to be deformed
    if ( object->hasObjectData(OBJECTDATA_SKELETON) ){

      SkeletonObjectData* skeletonData = reinterpret_cast< SkeletonObjectData* >( object->objectData(OBJECTDATA_SKELETON) );

      AnimationHandle hAni = PluginFunctions::skeletonObject(object)->activePose();

      for (unsigned int i=0; i < skeletonData->skinCount(); i++){
        //deform all attached skin meshes
        int meshId = skeletonData->skin(i);

        BaseObjectData* meshObject = 0;
        PluginFunctions::getObject(meshId, meshObject);

        if (meshObject == 0)
          continue;

        if ( !meshObject->hasObjectData(OBJECTDATA_SKIN) )
          continue;

        BaseSkin* skin = reinterpret_cast< BaseSkin* > ( meshObject->objectData(OBJECTDATA_SKIN) );
        skin->deformSkin( hAni, BaseSkin::M_LBS );

        emit updatedObject(meshObject->id(), UPDATE_GEOMETRY);
      }
    }

  if (_event->type() == QEvent::MouseButtonRelease)
    emit updatedObject(objectId, UPDATE_GEOMETRY);

  OpenFlipper::Options::redrawDisabled( false );
}


//------------------------------------------------------------------------------

/** \brief update object when its manipulator changes position
 *
 * @param _node the manipulator node
 */
void SkeletonEditingPlugin::ManipulatorPositionChanged(QtTranslationManipulatorNode* _node ) {

  // Position has been changed of the manipulator by a direct function
  int objectId = _node->getIdentifier();

  if ( objectId > 0 ){

    BaseObjectData* object;
    PluginFunctions::getObject(objectId,object);

    // Assume that it has a good position now
    object->manipPlaced( true );
  }
}

//------------------------------------------------------------------------------

/** \brief Checks if the manipulators should be visible or not
 *
 */
void SkeletonEditingPlugin::showManipulators(){

  if ( (PluginFunctions::pickMode() == "MoveJoints") && (PluginFunctions::actionMode() == Viewer::PickingMode) ){

    for (uint i=0; i < activeManipulators_.size(); i++){

      BaseObjectData* obj = 0;

      PluginFunctions::getObject( activeManipulators_[i], obj );

      if (obj != 0 && obj->manipPlaced()) {
        obj->manipulatorNode()->show();
        emit nodeVisibilityChanged(obj->id());
      }
    }

  } else {

    for (uint i=0; i < activeManipulators_.size(); i++){

      BaseObjectData* obj = 0;

      PluginFunctions::getObject( activeManipulators_[i], obj );

      if ( obj != 0 ) {
        obj->manipulatorNode()->hide();
        emit nodeVisibilityChanged(obj->id());
      }
    }
  }

  emit updateView();
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::deleteJoint(QMouseEvent* _event)
{
  if ( (_event->type() == QEvent::MouseButtonPress) || (_event->type() == QEvent::MouseButtonRelease) ){
    // only select the joint on mousePress
    unsigned int    node_idx, target_idx;
    ACG::Vec3d      hitPoint;
    BaseObjectData* object;

    //disable picking for anything but skeletons
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

    //perform picking
    bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx,
                                                              target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

    //reenable picking for anything
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( true );

    if ( successfullyPicked ){
      Skeleton* skeleton = PluginFunctions::skeleton( object );

      if ( !skeleton )
        return;

      currentSkeleton_ = object->id();

      Skeleton::Joint* joint = skeleton->joint( target_idx );

      if ( joint != 0 ){

        bool wasSelected = joint->selected();

        //clear selection
        for (Skeleton::Iterator it=skeleton->begin(); it != skeleton->end(); ++it)
          (*it)->setSelected(false);

        currentJoint_ = joint->id();

        if ( _event->type() == QEvent::MouseButtonPress ) //select on press
          joint->setSelected(true);
        else{
          //delete on release
          if ( wasSelected ){

            if (skeleton->jointCount() > 1){
              skeleton->removeJoint(joint);
              PluginFunctions::skeletonObject(object)->updateIndices();
              emit updatedObject(object->id(), UPDATE_ALL);
              emit createBackup(object->id(), "Delete Joint", UPDATE_TOPOLOGY);
            } else
              emit deleteObject( object->id() );
          }
        }
      }
    }
  }

  //make sure the joint is deselected
  if (_event->type() == QEvent::MouseButtonRelease){

    BaseObjectData* baseObject = 0;
    PluginFunctions::getObject(currentSkeleton_, baseObject);

    if (baseObject == 0)
      return;

    Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

    if (skeleton == 0)
      return;

    Skeleton::Joint* joint = skeleton->joint( currentJoint_ );

    if (joint != 0)
      joint->setSelected(false);

    currentSkeleton_ = -1;
    currentJoint_    = -1;
  }
}

//--------------------------------------------------------------------------------
/** \brief split the selected joint. selected joint will be child of new joint
 *
 * @param _event  mouseEvent that occured
 */
void SkeletonEditingPlugin::splitJoint(QMouseEvent* _event)
{
	if ( _event->type() == QEvent::MouseButtonPress )
	{
		if ( jointPreview_ )
			return;

		// try to select a joint from which the insertion should be started
		unsigned int    node_idx, target_idx;
		ACG::Vec3d      hitPoint;
		BaseObjectData* object;

		//disable picking for anything but skeletons
		for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
			o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

		//perform picking
		bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx,
				target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

		//reenable picking for anything
		for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
			o_it->enablePicking( true );

		if ( successfullyPicked )
		{
			Skeleton* skeleton = PluginFunctions::skeleton( object );

			if ( !skeleton )
				return;

			Skeleton::Joint* joint = skeleton->joint( target_idx );

			splitBone(object->id(), joint->id());
		}
	}
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::insertJoint(QMouseEvent* _event)
{

  //----------------------------------------------------------------------
  // handle PRESS events
  //----------------------------------------------------------------------
  if ( _event->type() == QEvent::MouseButtonPress ){

    if ( jointPreview_ )
      return;

    // try to select a joint from which the insertion should be started
    //
    unsigned int    node_idx, target_idx;
    ACG::Vec3d      hitPoint;
    BaseObjectData* object;

    //disable picking for anything but skeletons
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

    //perform picking
    bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx,
                                                              target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

    //reenable picking for anything
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( true );

    if ( successfullyPicked ){
      Skeleton* skeleton = PluginFunctions::skeleton( object );

      if ( !skeleton )
        return;

      currentSkeleton_ = object->id();
      jointPreview_ = false;

      Skeleton::Joint* joint = skeleton->joint( target_idx );

      if ( joint != 0 ){
        //clear selection
        for (Skeleton::Iterator it=skeleton->begin(); it != skeleton->end(); ++it)
          (*it)->setSelected(false);

        currentJoint_ = joint->id();
        joint->setSelected(true);
      }
    }
  }

  //----------------------------------------------------------------------
  // handle DOUBLE CLICK events
  //----------------------------------------------------------------------
  else if ( _event->type() == QEvent::MouseButtonDblClick ){


  	dblClick_ = true;
    // end the path in the skeleton
    if (currentSkeleton_ != -1){
      cancelJointInsertion();
      return;
    }

    // add a new skeleton at this position
    unsigned int    node_idx, target_idx;
    ACG::Vec3d      lastHitPoint(0.0, 0.0, 0.0);

    // first try to pick something
    if( !PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &lastHitPoint) ){
      //if picking fails just unproject at position
      ACG::Vec3d viewCoords   = ACG::Vec3d(_event->pos().x(), PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y(), 0.5);
      lastHitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);
    }


    int newSkeletonID = -1;
    emit addEmptyObject(DATA_SKELETON, newSkeletonID);

    BaseObjectData* baseObject = 0;
    PluginFunctions::getObject(newSkeletonID, baseObject);

    if (baseObject == 0)
      return;

    Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

    if (skeleton == 0)
      return;

    Skeleton::Joint* rootJoint = new Skeleton::Joint(0);
    skeleton->addJoint(0, rootJoint);

    // set joint position
    setJointPosition(PluginFunctions::skeletonObject(baseObject), rootJoint, lastHitPoint);
    emit updatedObject(baseObject->id(), UPDATE_ALL);
    emit createBackup(baseObject->id(), "Add Joints", UPDATE_ALL);

    // add an additional joint which is moved on mousemove
    Skeleton::Joint* tmpJoint = new Skeleton::Joint(rootJoint);
    skeleton->addJoint(rootJoint, tmpJoint);

    // set joint position
    setJointPosition(PluginFunctions::skeletonObject(baseObject), tmpJoint, lastHitPoint);

    currentSkeleton_ = baseObject->id();
    currentJoint_    = tmpJoint->id();
    jointPreview_    = true;

  //----------------------------------------------------------------------
  // handle MOVE events
  //----------------------------------------------------------------------
  } else if ( _event->type() == QEvent::MouseMove ){

    if ( jointPreview_ ){
      //only if jointPreview_ is enabled a joint is under construction
      BaseObjectData* baseObject = 0;
      PluginFunctions::getObject(currentSkeleton_, baseObject);

      if (baseObject == 0)
        return;

      Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

      if (skeleton == 0)
        return;

      Skeleton::Joint* joint = skeleton->joint( currentJoint_ );

      if (joint != 0){

        ACG::Vec3d parentPosition = activePose(PluginFunctions::skeletonObject(baseObject))->globalTranslation(joint->parent()->id());
        ACG::Vec3d parentViewer = PluginFunctions::viewerProperties().glState().project(parentPosition);

        ACG::Vec3d viewCoords   = ACG::Vec3d(_event->pos().x(), PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y(), parentViewer[2]);
        ACG::Vec3d lastHitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);

        // set joint position
        setJointPosition(PluginFunctions::skeletonObject(baseObject), joint, lastHitPoint);
      }
    }

  //----------------------------------------------------------------------
  // handle RELEASE events
  //----------------------------------------------------------------------
  } else if ( _event->type() == QEvent::MouseButtonRelease){

    if (!dblClick_) {

      // CASE 1 : this is a release on a joint from which the insertion should be started
      if ( !jointPreview_ ){
        // in
        unsigned int    node_idx, target_idx;
        ACG::Vec3d      hitPoint;
        BaseObjectData* object;

        //disable picking for anything but skeletons
        for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
          o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

        //perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx,
            target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

        //reenable picking for anything
        for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
          o_it->enablePicking( true );

        if ( successfullyPicked ){
          Skeleton* skeleton = PluginFunctions::skeleton( object );

          if ( !skeleton )
            return;

          currentSkeleton_ = object->id();

          Skeleton::Joint* joint = skeleton->joint( target_idx );

          if ( joint != 0 ){

            if ( joint->selected() ){
              //only if the joint is selected we are sure it was the same joint
              //like on the mouse press
              Skeleton::Joint* tmpJoint = new Skeleton::Joint(joint);
              skeleton->addJoint(joint, tmpJoint);

              currentJoint_ = tmpJoint->id();
              jointPreview_ = true;
            }
          }
        }

      } else {
        // CASE 2 : a joint is already under construction
  			//          so we insert the new joint at the current position
  			BaseObjectData* baseObject = 0;
  			PluginFunctions::getObject(currentSkeleton_, baseObject);

  			if (baseObject == 0)
  				return;

  			Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

  			if (skeleton == 0)
  				return;

  			Skeleton::Joint* joint = skeleton->joint( currentJoint_ );

  			if (joint != 0){

  				ACG::Vec3d parentPosition = activePose(PluginFunctions::skeletonObject(baseObject))->globalTranslation(joint->parent()->id());
  				ACG::Vec3d parentViewer = PluginFunctions::viewerProperties().glState().project(parentPosition);

  				ACG::Vec3d viewCoords   = ACG::Vec3d(_event->pos().x(), PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y(), parentViewer[2]);
  				ACG::Vec3d lastHitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);

  				// set joint position
  				setJointPosition(PluginFunctions::skeletonObject(baseObject), joint, lastHitPoint);
  				emit updatedObject(baseObject->id(), UPDATE_ALL);

  				Skeleton::Joint* tmpJoint = new Skeleton::Joint(joint);
  				skeleton->addJoint(joint, tmpJoint);
  				setJointPosition(PluginFunctions::skeletonObject(baseObject), tmpJoint, lastHitPoint);

  				currentJoint_ = tmpJoint->id();
  			}


  		}
  	} else
  	  dblClick_ = false;
  }


  // keep the joint selection correct
  if ( (_event->type() != QEvent::MouseButtonPress) ){
      BaseObjectData* baseObject = 0;
      PluginFunctions::getObject(currentSkeleton_, baseObject);

      if (baseObject == 0)
        return;

      Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

      if (skeleton == 0)
        return;

      for (Skeleton::Iterator it=skeleton->begin(); it != skeleton->end(); ++it)
        (*it)->setSelected(false);
  }
}

//--------------------------------------------------------------------------------

Skeleton::Pose* SkeletonEditingPlugin::activePose(SkeletonObject* _skeletonObj){

  Skeleton* skeleton = PluginFunctions::skeleton(_skeletonObj);

  if(!skeleton) return NULL;

  AnimationHandle handle = _skeletonObj->skeletonNode()->activePose();

  if ( !handle.isValid() ){
    //no current animation found -> transform the reference Pose
    return skeleton->referencePose();
  } else {

    Skeleton::Animation* animation = skeleton->animation(handle);
    return animation->pose( handle.frame() );
  }
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::setJointPosition(SkeletonObject* _skeletonObj, Skeleton::Joint* _joint, ACG::Vec3d& _position){

  Skeleton* skeleton = PluginFunctions::skeleton(_skeletonObj);

  if (skeleton == 0)
    return;

  //set position in refPose
  Skeleton::Pose* currentPose = activePose(_skeletonObj);
  currentPose->setGlobalTranslation(_joint->id(), _position );

  ACG::Matrix4x4d localMatrix = currentPose->localMatrix(_joint->id());

  //set position in animations
  for (unsigned int a=0; a < skeleton->animationCount(); a++)
    if ( AnimationHandle(a, 0 ).isValid() ){

      AnimationT<ACG::Vec3d> *animation = skeleton->animation( AnimationHandle(a, 0 ) );

      if ( animation != 0){

        //set initial joint translation
        for (int iFrame=0; iFrame < (int)animation->frameCount(); iFrame++) {

          PoseT<ACG::Vec3d>* pose = skeleton->pose( AnimationHandle(a, iFrame ) );

          if (pose != currentPose)
            pose->setLocalMatrix(_joint->id(), localMatrix );
        }
      }
    }

  if (currentPose != skeleton->referencePose())
    skeleton->referencePose()->setLocalMatrix(_joint->id(), localMatrix );
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::cancelJointInsertion(){

  int cSkeleton = currentSkeleton_;
  int cJoint    = currentJoint_;

  currentSkeleton_ = -1;
  currentJoint_    = -1;
  jointPreview_    = false;

  BaseObjectData* baseObject = 0;
  PluginFunctions::getObject(cSkeleton, baseObject);

  if (baseObject == 0)
    return;

  Skeleton* skeleton = PluginFunctions::skeleton( baseObject );

  if (skeleton == 0)
    return;

  Skeleton::Joint* joint = skeleton->joint( cJoint );

  if (joint != 0)
    skeleton->removeJoint(joint);

  emit updatedObject(baseObject->id(), UPDATE_ALL);
  emit createBackup(baseObject->id(), "Add Joints", UPDATE_ALL);
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::moveJoint(QMouseEvent* _event)
{

  if (_event->type() == QEvent::MouseButtonDblClick && _event->button() == Qt::LeftButton){
      placeManip(_event);
  } else {

    // interaction
    ACG::SceneGraph::MouseEventAction action(_event,PluginFunctions::viewerProperties().glState());
    PluginFunctions::traverse(action);
  }
}

//--------------------------------------------------------------------------------

void SkeletonEditingPlugin::selectJoint(QMouseEvent* _event)
{

  if ( _event->type() == QEvent::MouseButtonRelease ){

    unsigned int    node_idx, target_idx;
    ACG::Vec3d      hitPoint;
    BaseObjectData* object;

    //disable picking for anything but skeletons
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( o_it->dataType(DATA_SKELETON) );

    //perform picking
    bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx,
                                                              target_idx, &hitPoint) && PluginFunctions::getPickedObject(node_idx, object);

    //reenable picking for anything
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      o_it->enablePicking( true );

    if ( successfullyPicked ){
      Skeleton* skeleton = PluginFunctions::skeleton( object );

      if ( !skeleton )
        return;

      Skeleton::Joint* joint = skeleton->joint( target_idx );

      if ( joint != 0 ){
        joint->setSelected( !joint->selected() );
        emit updatedObject(object->id(), UPDATE_SELECTION);
      }
    }
  }
}

//--------------------------------------------------------------------------------

/**
 * \brief Get nearest joint to hitPoint (used for snapping)
 */
OpenMesh::Vec3d SkeletonEditingPlugin::getNearestJoint(SkeletonObject* _skeletonObj, OpenMesh::Vec3d &_hitPoint, int& _bestJointID) {

  ACG::Vec3d    bestJoint;
  double        bestDistance = DBL_MAX;

  _bestJointID = -1;

  Skeleton* skeleton = PluginFunctions::skeleton(_skeletonObj);

  Skeleton::Pose* pose = activePose(_skeletonObj);

  //find the nearest joint
  for (unsigned int joint = 0; joint < skeleton->jointCount(); joint++){

    double dist = (_hitPoint - pose->globalTranslation(joint)).sqrnorm();

    if (dist < bestDistance){
      bestJoint     = pose->globalTranslation(joint);
      _bestJointID  = joint;
      bestDistance  = dist;
    }
  }

  return (OpenMesh::Vec3d) bestJoint;
}

//--------------------------------------------------------------------------------

/** \brief make sure the manipulator is positioned on a joint
 *
 */
void SkeletonEditingPlugin::updateManipulatorPosition(BaseObjectData* _skeletonObj){

  int jointID = _skeletonObj->manipulatorNode()->getData().toInt();

  Skeleton* skeleton = PluginFunctions::skeleton(_skeletonObj);

  if (skeleton == 0)
    return;

  if (skeleton->joint(jointID) == 0)
    return;

  Skeleton::Pose* pose = activePose(PluginFunctions::skeletonObject(_skeletonObj));

  if(!pose) return;

  ACG::Vec3d newPos = pose->globalTranslation( jointID );

  _skeletonObj->manipulatorNode()->set_center(newPos);
}

//--------------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( skeletoneditingplugin , SkeletonEditingPlugin );


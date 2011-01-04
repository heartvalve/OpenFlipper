#include "SkeletalAnimationPlugin.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

using namespace std;

//------------------------------------------------------------------------------

/** \brief returns the plugin name
 *
 */
QString SkeletalAnimationPlugin::name()
{
  return "SkeletalAnimation";
}

//------------------------------------------------------------------------------

/** \brief returns a plugin description
 *
 */
QString SkeletalAnimationPlugin::description()
{
  return "Plugin to control skeletal animations";
}

//------------------------------------------------------------------------------

/** \brief initialize the plugin
 *
 */
void SkeletalAnimationPlugin::initializePlugin()
{
  bGuiUpdating_ = false;

  pToolbox_ = new AnimationToolboxWidget();
  QSize size(300, 300);
  pToolbox_->resize(size);

  connect( pToolbox_->pbBindMesh,   SIGNAL(clicked()), this, SLOT(slotBindMesh()) );
  connect( pToolbox_->pbUnbindMesh, SIGNAL(clicked()), this, SLOT(slotUnbindMesh()) );

  connect( pToolbox_->cbAnimation,                SIGNAL(currentIndexChanged(int)), this, SLOT(slotAnimationIndexChanged(int)) );
  connect( pToolbox_->hsFrame,                    SIGNAL(sliderMoved(int)),         this, SLOT(slotFrameChanged(int)) );
  connect( pToolbox_->hsFrame,                    SIGNAL(valueChanged(int)),        this, SLOT(slotFrameChanged(int)) );
  connect( pToolbox_->pbPlay,                     SIGNAL(clicked()),                this, SLOT( playAnimation() ) );
  connect( pToolbox_->pbStop,                     SIGNAL(clicked()),                this, SLOT( stopAnimation() ) );
  connect( pToolbox_->pbPrevFrame,                SIGNAL(clicked()),                this, SLOT( prevFrame() ) );
  connect( pToolbox_->pbNextFrame,                SIGNAL(clicked()),                this, SLOT( nextFrame() ) );
  connect( pToolbox_->sbFPS,                      SIGNAL(valueChanged ( int )),     this, SLOT( changeFPS(int) ) );
  connect( pToolbox_->cbSkipFrames,               SIGNAL(stateChanged(int)),        this, SLOT(slotSkipFramesChanged(int)) );

  pToolbox_->cbMethod->addItem("Linear Blend Skinning");
  pToolbox_->cbMethod->addItem("Dual Quaternion Blend Skinning");

  pToolbox_->cbMethod->setCurrentIndex(0);
  connect( pToolbox_->cbMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMethodChanged(int)) );
  method_ = BaseSkin::M_LBS;

  emit addToolbox( tr("Skeletal Animation") , pToolbox_ );
}

//------------------------------------------------------------------------------

/** \brief final initializations
 *
 */
void SkeletalAnimationPlugin::pluginsInitialized()
{
  emit setDescriptions();
}

//------------------------------------------------------------------------------

/** \brief Plugin gets closed
 *
 */
void SkeletalAnimationPlugin::exit()
{
}

//------------------------------------------------------------------------------

/** \brief clear all occurred
 *
 */
void SkeletalAnimationPlugin::slotAllCleared()
{
  activeSkeletons_.clear();
  activeMeshes_.clear();

  slotAnimationIndexChanged(0);
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object selection changes
 *
 */
void SkeletalAnimationPlugin::slotObjectSelectionChanged(int _id)
{
  checkObjectSelection();
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is loaded
 *
 */
void SkeletalAnimationPlugin::fileOpened( int _id) {
  checkObjectSelection();
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is added
 *
 */
void SkeletalAnimationPlugin::addedEmptyObject(int _id) {
  checkObjectSelection();
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is deleted
 *
 */
void SkeletalAnimationPlugin::objectDeleted(int _id) {
  checkObjectSelection();
}

//------------------------------------------------------------------------------

/** \brief Check source/target selection of objects
 *
 */
void SkeletalAnimationPlugin::checkObjectSelection(){

  activeSkeletons_.clear();
  activeMeshes_.clear();
  
  // get target skeletons
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_SKELETON)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
    activeSkeletons_.push_back( o_it->id() );

  // if no target skeleton there check if there is only one skeleton
  if ( activeSkeletons_.size() == 0){
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_SKELETON)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      activeSkeletons_.push_back( o_it->id() );

    if (activeSkeletons_.size() != 1)
      activeSkeletons_.clear();
  }

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH|DATA_POLY_MESH)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
    activeMeshes_.push_back( o_it->id() );

  // if no target mesh there check if there is only one mesh
  if ( activeMeshes_.size() == 0){
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_TRIANGLE_MESH|DATA_POLY_MESH)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      activeMeshes_.push_back( o_it->id() );

    if (activeMeshes_.size() != 1)
      activeMeshes_.clear();
  }

  UpdateUI();
}

//------------------------------------------------------------------------------

/**
 * @brief Returns the number of frames in the currently active animation
 */
int SkeletalAnimationPlugin::getNumberOfFrames()
{

  // deform the skin(s) using the new method
  for (unsigned int i=0; i < activeSkeletons_.size(); i++){

    //get active skeleton
    BaseObjectData* baseObject = 0;
    PluginFunctions::getObject(activeSkeletons_[i], baseObject);

    if ( baseObject == 0 )
      continue;

    SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(baseObject);
    Skeleton*             skeleton = PluginFunctions::skeleton(skeletonObject);

    if(skeleton != 0 && skeleton->animation( currentAnimationHandle() ) != 0)
      return skeleton->animation( currentAnimationHandle() )->frameCount();
  }

  return 0;
}

//------------------------------------------------------------------------------

/**
 * @brief Displays the given frame from the current animation and updates the view
 */
void SkeletalAnimationPlugin::setFrame(int _iFrame)
{
  pToolbox_->hsFrame->setValue(_iFrame);
}

//------------------------------------------------------------------------------

/**
 * @brief Gets the current frame number
 */
int SkeletalAnimationPlugin::getFrame() {
  return pToolbox_->hsFrame->value();
}

//------------------------------------------------------------------------------

/**
 * @brief Change the frames per second (FPS)
 */
void SkeletalAnimationPlugin::changeFPS(int _fps) {
  pToolbox_->sbFPS->blockSignals(true);
  pToolbox_->sbFPS->setValue(_fps);
  pToolbox_->sbFPS->blockSignals(false);

  animationTimer_.setInterval(1000/_fps);
}

//------------------------------------------------------------------------------

/**
 * @brief Changes the mesh's pose to represent the frame given by the animation handle
 *
 * @param _pSkeletonObject The object holding the skeleton, it is holding the pointer to the skin object
 * @param _hAni A handle of the new pose
 */
void SkeletalAnimationPlugin::UpdateSkin(BaseObjectData *_pSkeletonObject, AnimationHandle &_hAni)
{
  // deform the skin, if attached to the skeleton
  if(_pSkeletonObject->objectData(OBJECTDATA_SKELETON) != 0 &&
      reinterpret_cast< SkeletonObjectDataT<BaseSkin>* >(_pSkeletonObject->objectData(OBJECTDATA_SKELETON)) != 0)
  {
    SkeletonObjectDataT<BaseSkin> *pData = reinterpret_cast< SkeletonObjectDataT<BaseSkin>* >(_pSkeletonObject->objectData(OBJECTDATA_SKELETON));
    BaseObjectData *pMeshObject = NULL;
    PluginFunctions::getObject(pData->pSkinObjectId_, pMeshObject);//pData->pSkinObject_;
    BaseSkin *pSkin = pData->pSkin_;

    if (pMeshObject != NULL && pSkin != NULL) {
      pSkin->DeformSkin(_hAni, method_ );
      if(pMeshObject->dataType(DATA_TRIANGLE_MESH))
        dynamic_cast<TriMeshObject*>(pMeshObject)->updateGeometry();
      else if(pMeshObject->dataType(DATA_POLY_MESH))
        dynamic_cast<PolyMeshObject*>(pMeshObject)->updateGeometry();
      
      emit updatedObject(pMeshObject->id(), UPDATE_GEOMETRY);
    }
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Check activePose if a skeleton was updated
 */
void SkeletalAnimationPlugin::slotObjectUpdated( int _id, const UpdateType _type )
{
  // do nothing during animation
  if ( animationTimer_.isActive() )
    return;
  
  BaseObjectData* obj = 0;
  
  PluginFunctions::getObject(_id, obj);
  
  if ( (obj != 0)  && (obj->dataType(DATA_SKELETON)) ){

    SkeletonObject* sObj = PluginFunctions::skeletonObject(obj);
    
    //check if pose changed
    if ( sObj->activePose() != currentAnimationHandle() ){

      AnimationHandle newHandle = sObj->activePose();

      if ( !newHandle.isValid() ) //refPose
        pToolbox_->cbAnimation->setCurrentIndex( 0 ); 
      else
        pToolbox_->cbAnimation->setCurrentIndex( newHandle.animationIndex()+1 );
    }

    //check if animationCount changed
    if ( (int)PluginFunctions::skeleton(sObj)->animationCount() != pToolbox_->cbAnimation->count()-1 )
      UpdateUI();
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the framework when the animation index changed
 */
void SkeletalAnimationPlugin::slotAnimationIndexChanged(int /*_index*/)
{
  if(bGuiUpdating_)	// do not update while the gui is updating
    return;

  if ( activeSkeletons_.size() == 0){
    pToolbox_->hsFrame->setRange( 0, 0 );
    pToolbox_->hsFrame->setTickInterval(1);
    pToolbox_->hsFrame->setValue(0);

    pToolbox_->cbAnimation->clear();

  } else {

    //get first active skeleton
    BaseObjectData* skelObject = 0;
    PluginFunctions::getObject(activeSkeletons_[0], skelObject);
    
    if ( skelObject == 0 )
      return;
    
    Skeleton* skeleton = PluginFunctions::skeleton(skelObject);
    
    // equip the frame slider with the new range
    AnimationT<ACG::Vec3d> *pAnimation = skeleton->animation(currentAnimationHandle());
    if(pAnimation != 0){
      pToolbox_->hsFrame->setRange( 0, pAnimation->frameCount() - 1 );
      changeFPS( pAnimation->fps() );
    }else
      pToolbox_->hsFrame->setRange( 0, 0 );

    pToolbox_->hsFrame->setTickInterval(1);
    pToolbox_->hsFrame->setValue(0);

    // pass the current frame
    AnimationHandle hAni = currentAnimationHandle();
    dynamic_cast<SkeletonObject*>(skelObject)->setActivePose(hAni);

    emit updatedObject(skelObject->id(), UPDATE_GEOMETRY);

    // update skin if available
    UpdateSkin(skelObject, hAni);

    slotFrameChanged(0);
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the framework when a different frame was selected
 */
void SkeletalAnimationPlugin::slotFrameChanged(int /*_index*/)
{
  // do not update while the gui is updating
  if(bGuiUpdating_)
    return;

  for (unsigned int i=0; i < activeSkeletons_.size(); i++){
    
      //get active skeleton
      BaseObjectData* skelObject = 0;
      PluginFunctions::getObject(activeSkeletons_[i], skelObject);
    
      if ( skelObject == 0 )
        return;
      
      // pass the current frame
      AnimationHandle hAni = currentAnimationHandle();
      dynamic_cast<SkeletonObject*>(skelObject)->setActivePose(hAni);

      // and update the skin if available
      UpdateSkin(skelObject, hAni);

      emit updatedObject(skelObject->id(), UPDATE_GEOMETRY);
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the ui and starts an automatic animation
 */
void SkeletalAnimationPlugin::playAnimation() {
  disconnect(&animationTimer_, 0, 0, 0);
  animationTimer_.stop();
  animationTimer_.setSingleShot(false);
  animationTimer_.setInterval(1000 / pToolbox_->sbFPS->value());
  connect(&animationTimer_,SIGNAL(timeout()),this,SLOT(animate()));
  animationTimer_.start();
  animationTime_.start();
  animationOffset_ = pToolbox_->hsFrame->value();
  
  //The Play button will just toggle between play and pause mode
  connect( pToolbox_->pbPlay, SIGNAL(clicked()), this, SLOT( pauseAnimation() ) );
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the ui and stops the current animation
 */
void SkeletalAnimationPlugin::pauseAnimation() {
  animationTimer_.stop();
  disconnect(&animationTimer_, 0, 0, 0);
  
  connect( pToolbox_->pbPlay, SIGNAL(clicked()), this, SLOT( playAnimation() ) );
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the ui and stops the current animation
 */
void SkeletalAnimationPlugin::stopAnimation() {
  pauseAnimation();
  pToolbox_->hsFrame->setSliderPosition(0);     //Reset the playback
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the ui and goes to previous frame of the current animation
 */
void SkeletalAnimationPlugin::prevFrame() {
  pauseAnimation();
  pToolbox_->hsFrame->setSliderPosition(pToolbox_->hsFrame->sliderPosition() - 1);
}

//------------------------------------------------------------------------------

/**
 * @brief Called by the ui and goes to next frame of the current animation
 */
void SkeletalAnimationPlugin::nextFrame() {
  pauseAnimation();
  pToolbox_->hsFrame->setSliderPosition(pToolbox_->hsFrame->sliderPosition() + 1);
}

//------------------------------------------------------------------------------

/**
 * @brief Iterates the animation
 *
 * If frame skipping is disabled, the next frame will be drawn. Otherwise, the next frame is determined by the
 * time that passed since the last call.
 */
void SkeletalAnimationPlugin::animate()
{
  unsigned long frameCount = getNumberOfFrames();
  if(frameCount <= 1)	// 0: invalid animation, 1: just a pose
  {
    setFrame(0);
    stopAnimation();
    return;
  }
  --frameCount;

  if(pToolbox_->cbSkipFrames->isChecked())
  {
    int fps = pToolbox_->sbFPS->value();
    unsigned long currentFrame = animationOffset_ + (unsigned long)floor(double(animationTime_.elapsed()) / 1000.0 * fps);
    setFrame(currentFrame % frameCount);
  }else{
    setFrame((getFrame() + 1) % frameCount);
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Called as the skip frames check box changes state
 *
 * To prevent the animation playback from jumping, the animation timer is restarted here.
 */
void SkeletalAnimationPlugin::slotSkipFramesChanged(int _state)
{
  if(_state == Qt::Checked)
  {
    // changing to skip frames as necessary
    animationTime_.start();
    animationOffset_ = pToolbox_->hsFrame->value();
  }else{
    // changing to display all frames
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Called as the skin deformation method changed
 *
 * The spherical skin blending appears to be buggy, so it is recommended not to use it.
 */
void SkeletalAnimationPlugin::slotMethodChanged(int _index)
{
  // change the method
  switch(_index)
  {
  case 0:
    method_ = BaseSkin::M_LBS;
    break;
  default:
  case 1:
    method_ = BaseSkin::M_DBS;
    break;
  }

  // deform the skin(s) using the new method
  for (unsigned int i=0; i < activeSkeletons_.size(); i++){

    //get active skeleton
    BaseObjectData* baseObject = 0;
    PluginFunctions::getObject(activeSkeletons_[i], baseObject);

    if ( baseObject == 0 )
      return;

    SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(baseObject);
    AnimationHandle hAni = skeletonObject->skeletonNode()->activePose();

    // and update the skin
    UpdateSkin(skeletonObject, hAni);
  }

  emit updateView();
}

//------------------------------------------------------------------------------

/**
 * @brief Called when the active object changes and the interface needs to be updated
 */
void SkeletalAnimationPlugin::UpdateUI()
{
  if(bGuiUpdating_)	// gui updates object -> object is updated so gui gets updated -> loop forever
    return;

  bGuiUpdating_ = true;

  if( activeSkeletons_.size() > 0 )
  {
    //get first active skeleton
    BaseObjectData* baseObject = 0;
    PluginFunctions::getObject(activeSkeletons_[0], baseObject);

    if ( baseObject == 0 )
      return;

    SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(baseObject);
    Skeleton*          skeleton = PluginFunctions::skeleton(baseObject);

    // update the rigging and skinning group
    // enable UI
    if(skeletonObj->objectData(OBJECTDATA_SKELETON) == 0)
    {
      pToolbox_->pbBindMesh->setEnabled(true);
      pToolbox_->pbUnbindMesh->setEnabled(false);
    }else{
      pToolbox_->pbBindMesh->setEnabled(false);
      pToolbox_->pbUnbindMesh->setEnabled(true);
    }

    // update the Skeleton group
    AnimationHandle hAni = skeletonObj->skeletonNode()->activePose();

    pToolbox_->cbAnimation->setEnabled(true);
    pToolbox_->cbAnimation->clear();

    // create the reference pose
    pToolbox_->cbAnimation->addItem("Reference Pose");

    // create the other poses
    for(unsigned int i = 0; i < skeleton->animationCount(); ++i)
      pToolbox_->cbAnimation->addItem(skeleton->animationName(i).c_str());
    pToolbox_->cbAnimation->setCurrentIndex(hAni.animationIndex() + 1);

    // get the number of frames in the animation
    pToolbox_->hsFrame->setEnabled(true);
    if(skeleton->animation(hAni) != 0)
      pToolbox_->hsFrame->setRange( 0, skeleton->animation(hAni)->frameCount() - 1 );
    pToolbox_->hsFrame->setTickInterval(1);
    pToolbox_->hsFrame->setValue(hAni.frame());

  }else{
    // disable UI
    pToolbox_->cbAnimation->setEnabled(false);
    pToolbox_->cbAnimation->clear();
    pToolbox_->hsFrame->setEnabled(false);
    pToolbox_->hsFrame->setRange(0, 0);
    pToolbox_->hsFrame->setTickInterval(1);

    pToolbox_->pbBindMesh->setEnabled(false);
    pToolbox_->pbUnbindMesh->setEnabled(false);
  }

  bGuiUpdating_ = false;
}

//------------------------------------------------------------------------------

/**
 * @brief Called by Qt as the user is trying to connect a mesh to a skeleton
 */
void SkeletalAnimationPlugin::slotBindMesh()
{
//   // get the selected object and mesh
//   BaseObjectData *pMeshObject = 0, *pSkeletonObject = 0;
// 
//   GetSelectedPair(&pMeshObject, &pSkeletonObject);
//   if(pMeshObject == 0 || pSkeletonObject == 0)
//     return;
// 
//   rig(pMeshObject, pSkeletonObject);
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::rig(BaseObjectData *_skin, BaseObjectData* _skeletonObj) {
  
//   // stop if the skeleton already has a skin
//   if(_skeletonObj->objectData(OBJECTDATA_SKELETON) != 0)
//     return;
// 
//   // get the skeleton and make it prepare the mesh
//   Skeleton *skeleton = dynamic_cast<SkeletonObject*>(_skeletonObj)->skeleton();
// 
//   // prepare the skin template class used to deform the skin
//   SkeletonObjectDataT<BaseSkin> *pData = new SkeletonObjectDataT<BaseSkin>();
//   pData->pSkinObjectId_ = pSkin->id();
// 
//   if(pSkin->dataType(DATA_TRIANGLE_MESH))
//   {
//     pData->pSkin_ = dynamic_cast<BaseSkin*>( new SkinT<TriMesh>(skeleton, PluginFunctions::triMesh(pSkin)) );
// 
//     pData->pSkin_->AttachSkin();
//     pData->pSkin_->MakeInfluenceGroups();
//     emit updatedObject(pSkin->id(), UPDATE_GEOMETRY);
// 
//   }else if(pSkin->dataType(DATA_POLY_MESH)){
//     pData->pSkin_ = dynamic_cast<BaseSkin*>( new SkinT<PolyMesh>(skeleton, PluginFunctions::polyMesh(pSkin)) );
// 
//     pData->pSkin_->AttachSkin();
//     pData->pSkin_->MakeInfluenceGroups();
//     emit updatedObject(pSkin->id(), UPDATE_GEOMETRY);
//   }
// 
//   //add skelSegment Property @TODO remove the skelSegment stuff
//   PropertyHandleT<short unsigned int> propSkeletonSegment;
//   skeleton->add_property(propSkeletonSegment, "Skeleton Segment");
// 
//   //copy boundaries from old object data
//   if ( pSkeleton->hasObjectData(OBJECTDATA_SKELETON) ){
//     SkeletonObjectDataT<BaseSkin>* oldData = reinterpret_cast< SkeletonObjectDataT<BaseSkin>* >(pSkeleton->objectData(OBJECTDATA_SKELETON));
// 
//     pData->boundaryMap_ = oldData->boundaryMap_;
//     pSkeleton->clearObjectData(OBJECTDATA_SKELETON);
//     delete oldData;
//   }
// 
//   pSkeleton->setObjectData(OBJECTDATA_SKELETON, pData);
//   dynamic_cast<BaseObject*>(pSkeleton)->target(true);
//   dynamic_cast<BaseObject*>(pSkeleton)->source(false);
//   
//   //Add a reference to the skin to the skeletonObject's name, so that the user can see the connection in the UI (in the DataControl plugin)
//   pSkeleton->setName(pSkeleton->name() + " (" + pSkin->name() + ")");
// 
//   emit updateView();
//   UpdateUI();
}

//------------------------------------------------------------------------------

/**
 * @brief Called by Qt as the user is trying to unbind a mesh from as a skeleton
 */
void SkeletalAnimationPlugin::slotUnbindMesh()
{
//   // get the selected object and mesh
//   BaseObjectData *pSkeletonObject = 0;
// 
//   // Test if there is a skeleton object selected as target
//   for(PluginFunctions::ObjectIterator it_o(PluginFunctions::TARGET_OBJECTS, DATA_SKELETON);
//     it_o != PluginFunctions::objectsEnd(); ++it_o)
//   {
//     pSkeletonObject = *it_o;
//     break;
//   }
// 
//   unrig(pSkeletonObject);
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::unrig(BaseObjectData *_pSkeleton) {
  
//   if(_pSkeleton == 0 || _pSkeleton->objectData(OBJECTDATA_SKELETON) == 0)
//           return;
// 
//           
//   //Remove the reference to the skin from the skeletonObject's name
//   QStringList parts = _pSkeleton->name().split(" (", QString::KeepEmptyParts);
//   _pSkeleton->setName(parts[0]);
//   
//   // reset the skin it to its former shape
//   SkeletonObjectDataT<BaseSkin> *pData = reinterpret_cast< SkeletonObjectDataT<BaseSkin>* >(_pSkeleton->objectData(OBJECTDATA_SKELETON));
//   BaseSkin *pSkin = pData->pSkin_;
//   Skeleton *pSkeleton = dynamic_cast<SkeletonObject*>(_pSkeleton)->skeleton();
// 
//   pSkin->ReleaseSkin();
// 
//   BaseObjectData *pSkinObject;
//   if (PluginFunctions::getObject(pData->pSkinObjectId_, pSkinObject) && pSkinObject != NULL)
//     emit updatedObject(pData->pSkinObjectId_, UPDATE_GEOMETRY);
// 
//   delete pData;
//   _pSkeleton->clearObjectData(OBJECTDATA_SKELETON);
//   emit updateView();
// 
//   UpdateUI();
}

//------------------------------------------------------------------------------


/**
 * @brief Returns a handle describing the current frame in the active animation
 *
 * This information is derived from the corresponding combobox and slider.
 */
AnimationHandle SkeletalAnimationPlugin::currentAnimationHandle()
{
  int iAnimation = pToolbox_->cbAnimation->currentIndex();

  if(iAnimation == 0)
    return AnimationHandle(); //This will be the reference pose, i.e. an empty animation
  else if(iAnimation > 0)
    return AnimationHandle(iAnimation - 1, pToolbox_->hsFrame->value());
  return AnimationHandle(); // should not happen
}

//------------------------------------------------------------------------------

Q_EXPORT_PLUGIN2(skeletalAnimationplugin, SkeletalAnimationPlugin);

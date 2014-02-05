/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $LastChangedBy$                                                 *
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

#include <QInputDialog>
#include <QMessageBox>

#include "SkeletalAnimationPlugin.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"
#include <ObjectTypes/Skeleton/SkeletonObjectData.hh>
#include <ObjectTypes/Skeleton/SkinT.hh>

#include "AddAnimationDialog.hh"

using namespace std;

//------------------------------------------------------------------------------

/** \brief Constructor
 *
 */
SkeletalAnimationPlugin::SkeletalAnimationPlugin() :
        pToolbox_(0),
        toolIcon_(0),
        bGuiUpdating_(false),
        animationOffset_(0)
{
}

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

  connect( pToolbox_->pbAttachSkin, SIGNAL(clicked()), this, SLOT(slotAttachSkin()) );
  connect( pToolbox_->pbClearSkins, SIGNAL(clicked()), this, SLOT(slotClearSkins()) );

  connect( pToolbox_->cbAnimation,                SIGNAL(currentIndexChanged(int)), this, SLOT(slotAnimationIndexChanged(int)) );
  connect( pToolbox_->hsFrame,                    SIGNAL(sliderMoved(int)),         this, SLOT(slotFrameChanged(int)) );
  connect( pToolbox_->hsFrame,                    SIGNAL(valueChanged(int)),        this, SLOT(slotFrameChanged(int)) );
  connect( pToolbox_->pbPlay,                     SIGNAL(clicked()),                this, SLOT( playAnimation() ) );
  connect( pToolbox_->pbStop,                     SIGNAL(clicked()),                this, SLOT( stopAnimation() ) );
  connect( pToolbox_->pbPrevFrame,                SIGNAL(clicked()),                this, SLOT( prevFrame() ) );
  connect( pToolbox_->pbNextFrame,                SIGNAL(clicked()),                this, SLOT( nextFrame() ) );
  connect( pToolbox_->sbFPS,                      SIGNAL(valueChanged ( int )),     this, SLOT( changeFPS(int) ) );
  connect( pToolbox_->cbSkipFrames,               SIGNAL(stateChanged(int)),        this, SLOT(slotSkipFramesChanged(int)) );
  connect( pToolbox_->pbAddAnimation,             SIGNAL(clicked()),                this, SLOT(slotAddAnimation()) );
  connect( pToolbox_->pbDeleteAnimation,          SIGNAL(clicked()),                this, SLOT(slotDeleteAnimation()) );
  
  connect( pToolbox_->pbEditAnimation, SIGNAL(clicked()), this, SLOT(slotAnimationNameChanged()));

  pToolbox_->pbAddAnimation->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"addAnimation.png") );
  pToolbox_->pbDeleteAnimation->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"deleteAnimation.png") );
  pToolbox_->pbEditAnimation->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"editAnimation.png") );
  
  pToolbox_->cbMethod->addItem("Linear Blend Skinning");
  pToolbox_->cbMethod->addItem("Dual Quaternion Blend Skinning");

  pToolbox_->cbMethod->setCurrentIndex(0);
  connect( pToolbox_->cbMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMethodChanged(int)) );
  method_ = BaseSkin::M_LBS;

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"skeletalAnimation.png");
  emit addToolbox( tr("Skeletal Animation") , pToolbox_, toolIcon_ );
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

  slotAnimationIndexChanged(0);
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object selection changes
 *
 */
void SkeletalAnimationPlugin::slotObjectSelectionChanged(int _id)
{
  checkObjectSelection(_id);
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is loaded
 *
 */
void SkeletalAnimationPlugin::fileOpened( int _id) {
  checkObjectSelection(_id);
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is added
 *
 */
void SkeletalAnimationPlugin::addedEmptyObject(int _id) {

  checkObjectSelection(_id);
}

//------------------------------------------------------------------------------

/** \brief Update ui when the object is deleted
 *
 */
void SkeletalAnimationPlugin::objectDeleted(int _id) {

    checkObjectSelection(_id);

    // Check for skin that is to be cleared
    BaseObjectData* bod = 0;
    if (!PluginFunctions::getObject(_id, bod)) {
        return;
    }

    if (bod->hasObjectData(OBJECTDATA_SKIN)) {

        BaseSkin* baseSkin = 0;

        if (bod->dataType(DATA_TRIANGLE_MESH))
            baseSkin = dynamic_cast<BaseSkin*> (bod->objectData(OBJECTDATA_SKIN));
        else if (bod->dataType(DATA_POLY_MESH))
            baseSkin = dynamic_cast<BaseSkin*> (bod->objectData(OBJECTDATA_SKIN));

        if (baseSkin) {
            BaseObjectData* skeletonObj = PluginFunctions::skeletonObject(baseSkin->skeletonId());

            if(skeletonObj) {
                // Detach skin  from skeleton
                detachSkin(bod, skeletonObj);
            }
        }
    }
}

//------------------------------------------------------------------------------

/** \brief Check source/target selection of objects
 *
 */
void SkeletalAnimationPlugin::checkObjectSelection(const int _objectId){

  BaseObject* object;
  PluginFunctions::getObject(_objectId,object);

  if ( !object ) {
    std::cerr << "SkeletalAnimationPlugin::checkObjectSelection : unable to get object! " << std::endl;
    return;
  }

  if ( object->dataType() == DataType(DATA_SKELETON) ) {
    activeSkeletons_.clear();

    // get target skeletons
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_SKELETON)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
      activeSkeletons_.push_back( o_it->id() );

    // if no target skeleton there check if there is only one skeleton
    if ( activeSkeletons_.empty() ){
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_SKELETON)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
        activeSkeletons_.push_back( o_it->id() );

      if (activeSkeletons_.size() != 1)
        activeSkeletons_.clear();
    }

  }

  UpdateUI();
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::slotAnimationNameChanged() {

    if(pToolbox_->cbAnimation->currentText() == "Reference Pose") {

        QMessageBox::warning(0, "Not editable!", "You cannot change the reference pose's name!");
        return;
    }

    QString newName = QInputDialog::getText(0, tr("Change Animation's Name"), tr("New Name:"),
            QLineEdit::Normal, pToolbox_->cbAnimation->currentText());

    // Set animation's name
    for (unsigned int i=0; i < activeSkeletons_.size(); i++){

        // Get active skeleton
        BaseObjectData* baseObject = 0;
        PluginFunctions::getObject(activeSkeletons_[i], baseObject);

        if ( baseObject == 0 )
          continue;

        SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(baseObject);
        if(!skeletonObject) continue;
        Skeleton*             skeleton = PluginFunctions::skeleton(skeletonObject);
        if(!skeleton) continue;

        AnimationHandle h = skeleton->animationHandle(pToolbox_->cbAnimation->currentText().toStdString());
        if(skeleton != 0 && h.isValid()) {
            skeleton->animation(h)->setName(newName.toStdString());
            skeleton->replaceAnimationName(pToolbox_->cbAnimation->currentText().toStdString(), newName.toStdString());
        } else {
            return;
        }
    }

    pToolbox_->cbAnimation->setItemText(pToolbox_->cbAnimation->currentIndex(), newName);
}

//------------------------------------------------------------------------------

/**
 * @brief Returns the number of frames in the currently active animation
 */
int SkeletalAnimationPlugin::getNumberOfFrames()
{

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
 * @param _skeletonObject The object holding the skeleton, it is holding the pointer to the skin object
 * @param _hAni           A handle of the new animation/pose
 */
void SkeletalAnimationPlugin::UpdateSkins(BaseObjectData *_skeletonObject, AnimationHandle &_hAni)
{
  
  if( !_skeletonObject->hasObjectData(OBJECTDATA_SKELETON) )
    return;

  SkeletonObjectData* skeletonData = dynamic_cast< SkeletonObjectData* >( _skeletonObject->objectData(OBJECTDATA_SKELETON) );
  
  for (unsigned int i=0; i < skeletonData->skinCount(); i++){

    // deform all attached skin meshes
    int meshId = skeletonData->skin(i);

    BaseObjectData* object = 0;
    PluginFunctions::getObject(meshId, object);
    
    if (object == 0)
      continue;

    if ( !object->hasObjectData(OBJECTDATA_SKIN) ){
      emit log(LOGERR, tr("Error: Attached skin mesh has no skin-object-data."));
      continue;
    }
    
    BaseSkin* skin = dynamic_cast< BaseSkin* > ( object->objectData(OBJECTDATA_SKIN) );
    skin->deformSkin(_hAni, method_ );

    emit updatedObject(object->id(), UPDATE_GEOMETRY);
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Check activePose if a skeleton was updated
 */
void SkeletalAnimationPlugin::slotObjectUpdated( int _id, const UpdateType& /*_type*/ )
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
        setComboBoxPosition(newHandle.animationIndex());
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

  if ( activeSkeletons_.empty() ){
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

    // update skins if available
    UpdateSkins(skelObject, hAni);

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
      PluginFunctions::skeletonObject(skelObject)->setActivePose(hAni);

      // and update the skin if available
      UpdateSkins(skelObject, hAni);

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
    UpdateSkins(skeletonObject, hAni);
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

  if( ! activeSkeletons_.empty() )
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
      pToolbox_->pbAttachSkin->setEnabled(true);
      pToolbox_->pbClearSkins->setEnabled(false);
      pToolbox_->skinningBox->setTitle(tr("Attached Skins"));
    }else{
      pToolbox_->pbAttachSkin->setEnabled(true);
      pToolbox_->pbClearSkins->setEnabled(true);

      SkeletonObjectData* skelData = dynamic_cast<SkeletonObjectData*>( skeletonObj->objectData(OBJECTDATA_SKELETON) );
      pToolbox_->skinningBox->setTitle(tr("Attached Skins (Currently: %1)").arg(skelData->skinCount())  );
    }

    // update the Skeleton group
    AnimationHandle hAni = skeletonObj->skeletonNode()->activePose();

    pToolbox_->pbAddAnimation->setEnabled(true);
    pToolbox_->cbAnimation->setEnabled(true);
    pToolbox_->cbAnimation->clear();

    // create the reference pose
    pToolbox_->cbAnimation->addItem("Reference Pose");

    Skeleton::AnimationIterator animations = skeleton->animationsBegin();

    while ( animations ) {
      AnimationHandle anim = *animations;
      pToolbox_->cbAnimation->addItem(skeleton->animationName(anim.animationIndex()).c_str(),QVariant(anim.animationIndex()));
      ++animations;
    }

    setComboBoxPosition(hAni.animationIndex());

    // get the number of frames in the animation
    pToolbox_->hsFrame->setEnabled(true);
    if(skeleton->animation(hAni) != 0)
      pToolbox_->hsFrame->setRange( 0, skeleton->animation(hAni)->frameCount() - 1 );
    pToolbox_->hsFrame->setTickInterval(1);
    pToolbox_->hsFrame->setValue(hAni.frame());

  }else{
    // disable UI
    pToolbox_->pbAddAnimation->setEnabled(false);
    pToolbox_->cbAnimation->setEnabled(false);
    pToolbox_->cbAnimation->clear();
    pToolbox_->hsFrame->setEnabled(false);
    pToolbox_->hsFrame->setRange(0, 0);
    pToolbox_->hsFrame->setTickInterval(1);

    pToolbox_->pbAttachSkin->setEnabled(false);
    pToolbox_->pbClearSkins->setEnabled(false);
    pToolbox_->skinningBox->setTitle(tr("Attached Skins"));
  }

  bGuiUpdating_ = false;
}

//------------------------------------------------------------------------------

/**
 * @brief Called by Qt as the user is trying to connect a mesh to a skeleton
 */
void SkeletalAnimationPlugin::slotAttachSkin()
{
  
  if( activeSkeletons_.size() != 1 ){
    emit log(LOGERR, tr("Cannot bind mesh. Please select only one skeleton."));
    return;
  }

  int meshCount = 0;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH|DATA_POLY_MESH)) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    attachSkin(activeSkeletons_[0], o_it->id());
    meshCount++;
  }
  
  if (meshCount == 0){
    emit log(LOGERR, tr("Cannot bind mesh. Please select at least one mesh as target."));
    return;
  }
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::attachSkin(BaseObjectData* _skin, BaseObjectData* _skeletonObj) {
  
  // get the skeleton and make it prepare the mesh
  Skeleton* skeleton = dynamic_cast<SkeletonObject*>(_skeletonObj)->skeleton();

  //check if mesh is already a skin
  if (_skin->hasObjectData(OBJECTDATA_SKIN) ){
    emit log(LOGERR, tr("Cannot bind mesh as skin. Mesh is already a skin."));
    return;
  }

  // prepare the skin template class used to deform the skin
  SkeletonObjectData* skelData = 0;

  if ( _skeletonObj->hasObjectData(OBJECTDATA_SKELETON) )
    skelData = dynamic_cast< SkeletonObjectData* >(_skeletonObj->objectData(OBJECTDATA_SKELETON));
  else {
    skelData = new SkeletonObjectData();
    _skeletonObj->setObjectData(OBJECTDATA_SKELETON, skelData);
  }

  skelData->addSkin( _skin->id() );

  BaseSkin* baseSkin = 0;

  OpenMesh::VPropHandleT<BaseSkin::SkinWeights> propWeights;
  bool hasSkinWeights = true;

  if(_skin->dataType(DATA_TRIANGLE_MESH)){
    hasSkinWeights = PluginFunctions::triMesh(_skin)->get_property_handle(propWeights, SKIN_WEIGHTS_PROP);
    baseSkin       = dynamic_cast<BaseSkin*>( new TriMeshSkin(skeleton, PluginFunctions::triMesh(_skin),_skeletonObj->id()) );
  }else if(_skin->dataType(DATA_POLY_MESH)){
    hasSkinWeights = PluginFunctions::polyMesh(_skin)->get_property_handle(propWeights, SKIN_WEIGHTS_PROP);
    baseSkin       = dynamic_cast<BaseSkin*>( new PolyMeshSkin(skeleton, PluginFunctions::polyMesh(_skin) ,_skeletonObj->id() ));
  }

  baseSkin->attachSkin();

  if (hasSkinWeights)
    baseSkin->deformSkin(currentAnimationHandle(), method_);

  emit updatedObject(_skin->id(), UPDATE_GEOMETRY);
  _skin->setObjectData(OBJECTDATA_SKIN, baseSkin);

  _skeletonObj->target(true);
  _skeletonObj->source(false);

  if( !hasSkinWeights ){
    //ask if they should be computed automatically
    bool canCompute;
    emit pluginExists("skinningplugin", canCompute);
    
    if (canCompute){
      QMessageBox msgBox;
      msgBox.setText("The mesh is not equipped with skin weights.");
      msgBox.setInformativeText("Do you want to compute them automatically?");
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::Yes);
      int ret = msgBox.exec();

      if (ret == QMessageBox::Yes)
        RPC::callFunction("skinningplugin", "computeSkinWeights");
    }
  }
  
  UpdateUI();
}

//------------------------------------------------------------------------------

/**
 * @brief Called by Qt as the user is trying to unbind a mesh from as a skeleton
 */
void SkeletalAnimationPlugin::slotClearSkins()
{
  for (unsigned int i=0; i < activeSkeletons_.size(); i++)
    clearSkins( activeSkeletons_[i] );
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::clearSkins(BaseObjectData* _skeletonObj) {

  // prepare the skin template class used to deform the skin
  SkeletonObjectData* skelData = 0;

  if ( !_skeletonObj->hasObjectData(OBJECTDATA_SKELETON) )
    return;

  skelData = dynamic_cast< SkeletonObjectData* >(_skeletonObj->objectData(OBJECTDATA_SKELETON));

  for (int i=skelData->skinCount()-1; i >= 0; i--){
    //deform all attached skin meshes
    int meshId = skelData->skin(i);

    BaseObjectData* object = 0;
    PluginFunctions::getObject(meshId, object);
    
    if (object == 0)
      continue;

    detachSkin(object, _skeletonObj);
  }
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::detachSkin(BaseObjectData* _skin, BaseObjectData* _skeletonObj) {

  // get the skeleton and make it prepare the mesh
  Skeleton* skeleton = dynamic_cast<SkeletonObject*>(_skeletonObj)->skeleton();

  if ( !_skeletonObj->hasObjectData(OBJECTDATA_SKELETON) ){
    emit log(LOGERR, tr("Cannot detach skin. Skeleton has no object data."));
    return;
  }

  //first try to remove the skin from the mesh
  if (_skin->hasObjectData(OBJECTDATA_SKIN) ){

    BaseSkin* baseSkin = 0;

    if(_skin->dataType(DATA_TRIANGLE_MESH))
      baseSkin = dynamic_cast<BaseSkin*>( new TriMeshSkin(skeleton, PluginFunctions::triMesh(_skin) ,_skeletonObj->id()));
    else if(_skin->dataType(DATA_POLY_MESH))
      baseSkin = dynamic_cast<BaseSkin*>( new PolyMeshSkin(skeleton, PluginFunctions::polyMesh(_skin) ,_skeletonObj->id()));

    baseSkin->releaseSkin();
    _skin->clearObjectData(OBJECTDATA_SKIN);
    delete baseSkin;

    emit updatedObject(_skin->id(), UPDATE_GEOMETRY);
  }

  // then remove the skin from the skeleton data
  SkeletonObjectData* skelData = dynamic_cast< SkeletonObjectData* >(_skeletonObj->objectData(OBJECTDATA_SKELETON));

  skelData->removeSkin( _skin->id() );
  
  //remove the objectData if all skins are removed
  if ( skelData->skinCount() == 0 ){
    _skeletonObj->clearObjectData(OBJECTDATA_SKELETON);
    delete skelData;
  }

  UpdateUI();
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::slotAddAnimation()
{
  
  if( activeSkeletons_.size() != 1 ){
    emit log(LOGERR, tr("Cannot add animation. Please select only one skeleton."));
    return;
  }

  AddAnimationDialog dialog;
  dialog.animationName->selectAll();
  dialog.animationName->setFocus();
  
  if ( dialog.exec() == QDialog::Accepted ){
    if ( dialog.animationName->text() == "" ){
      emit log(LOGERR, tr("Cannot add animation with empty name"));
      return;
    }

    BaseObjectData* obj = 0;

    PluginFunctions::getObject(activeSkeletons_[0], obj);

    if (obj == 0){
      emit log(LOGERR, tr("Unable to get object"));
      return;
    }

    SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

    if (skeletonObj == 0){
      emit log(LOGERR, tr("Unable to get skeletonObject"));
      return;
    }

    Skeleton* skeleton = PluginFunctions::skeleton(obj);

    std::string stdName = dialog.animationName->text().toStdString();

    if ( skeleton->animation(stdName) != 0 ){
      emit log(LOGERR, tr("Animation with this name already exists"));
      return;
    }

    FrameAnimationT<ACG::Vec3d>* animation = new FrameAnimationT<ACG::Vec3d>(skeleton, dialog.frames->value());
    AnimationHandle handle = skeleton->addAnimation(stdName, animation);

    //init pose to refPose
    for (unsigned int i=0; i < skeleton->animation(handle)->frameCount(); i++){
      handle.setFrame(i);
      Skeleton::Pose* pose = skeleton->pose(handle);

      for (unsigned int j=0; j < skeleton->jointCount(); j++)
        pose->setGlobalMatrix(j, skeleton->referencePose()->globalMatrix(j) );
    }

    emit updatedObject(activeSkeletons_[0], UPDATE_ALL);

    //select the new animation
    setComboBoxPosition(handle.animationIndex());
  }
}

//------------------------------------------------------------------------------

void SkeletalAnimationPlugin::slotDeleteAnimation()
{
  int iAnimation = pToolbox_->cbAnimation->currentIndex();
  unsigned int animationIndex = pToolbox_->cbAnimation->itemData(iAnimation).toUInt();

  if ( iAnimation == 0 ) {
    emit log(LOGERR,"Reference pose could never be removed!");
  } else {
    pToolbox_->cbAnimation->removeItem(iAnimation);

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_SKELETON)) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

      SkeletonObject* skeletonObject = dynamic_cast<SkeletonObject*>(*o_it);
      Skeleton*             skeleton = PluginFunctions::skeleton(skeletonObject);

      skeleton->removeAnimation(AnimationHandle(animationIndex));

    }

    UpdateUI();
  }
}

//------------------------------------------------------------------------------

/**
 * @brief Returns a handle describing the current frame in the active animation
 *
 * This information is derived from the corresponding combo box and slider.
 */
AnimationHandle SkeletalAnimationPlugin::currentAnimationHandle()
{
  int iAnimation = pToolbox_->cbAnimation->currentIndex();
  unsigned int animationId = pToolbox_->cbAnimation->itemData(iAnimation).toUInt();
  
  if(iAnimation == 0)
    return AnimationHandle(); //This will be the reference pose, i.e. an empty animation
  else if(iAnimation > 0)
    return AnimationHandle(animationId, pToolbox_->hsFrame->value());
  return AnimationHandle(); // should not happen
}

//------------------------------------------------------------------------------

/**
 * @brief Sets the animations combo box to the right entry
 *
 */
void SkeletalAnimationPlugin::setComboBoxPosition(unsigned int _animationIndex)
{
  for ( int i = 0 ; i <  pToolbox_->cbAnimation->count(); ++i ) {
    unsigned int animationId = pToolbox_->cbAnimation->itemData(i).toUInt();

    if ( animationId == _animationIndex ) {
      pToolbox_->cbAnimation->setCurrentIndex(i);
      return;
    }

  }

}

//------------------------------------------------------------------------------

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(skeletalAnimationplugin, SkeletalAnimationPlugin);
#endif



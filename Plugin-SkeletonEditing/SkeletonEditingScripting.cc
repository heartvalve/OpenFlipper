#include "SkeletonEditingPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ObjectTypes/Skeleton/Helper/SkeletonTransform.hh>
#include <ACG/Geometry/Algorithms.hh>

//------------------------------------------------------------------------------

/** \brief Set Descriptions for Scripting Slots
 *
 */
void SkeletonEditingPlugin::setDescriptions() {

  emit setSlotDescription("splitBone(int,int)",
      tr("insert a joint in the middle of a bone."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of tail joint")).split(","));

  emit
      setSlotDescription(
          "addJoint(int,int,Vector)",
          tr("add a joint to the skeleton."),
          QString(tr("objectId,jointId,Vector")).split(","),
          QString(
              tr(
                  "ID of an object,ID of parent joint,Position for the new joint")).split(
              ","));

  emit setSlotDescription("deleteJoint(int,int)",
      tr("delete a joint from the skeleton."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of a joint")).split(","));

  emit setSlotDescription(
      "transformJoint(int,int,Matrix4x4)",
      tr("transform a joint with a matrix."),
      QString(tr("objectId,jointId,Matrix")).split(","),
      QString(tr("ID of an object,ID of a joint,transformation matrix")).split(
          ","));

  emit setSlotDescription("globalMatrix(int,int)",
      tr("get the global matrix of a joint in the active pose."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of a joint")).split(","));

  emit setSlotDescription("localMatrix(int,int)",
      tr("get the local matrix of a joint in the active pose."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of a joint")).split(","));

  emit setSlotDescription("globalTranslation(int,int)",
      tr("get the global translation of a joint in the active pose."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of a joint")).split(","));

  emit setSlotDescription("localTranslation(int,int)",
      tr("get the local translation of a joint in the active pose."),
      QString(tr("objectId,jointId")).split(","),
      QString(tr("ID of an object,ID of a joint")).split(","));

  emit setSlotDescription("animationCount(int)",
      tr("get the number of animations the skeleton has."),
      QString(tr("objectId")).split(","),
      QString(tr("ID of an object")).split(","));

  emit setSlotDescription("frameCount(int,int)",
      tr("get the number of frames a given animation has."),
      QString(tr("objectId,animationIndex")).split(","),
      QString(tr("ID of an object,Index of an animation")).split(","));

  emit setSlotDescription("activeAnimation(int)",
      tr("get the animation which is currently active."),
      QString(tr("objectId")).split(","),
      QString(tr("ID of an object")).split(","));

  emit setSlotDescription("activeFrame(int)",
      tr("get the frame which is currently active"),
      QString(tr("objectId")).split(","),
      QString(tr("ID of an object")).split(","));

  emit
      setSlotDescription(
          "setActivePose(int,int,int)",
          tr("set the active pose of the skeleton."),
          QString(tr("objectId,animationIndex,frame")).split(","),
          QString(tr("ID of an object,Index of an animation,Index of a frame")).split(
              ","));

  emit
      setSlotDescription(
          "addAnimation(int,QString,int)",
          tr("add an animation to the skeleton."),
          QString(tr("objectId,AnimationName,frameCount")).split(","),
          QString(
              tr(
                  "ID of an object,name for the animation,number of frames the animation should have")).split(
              ","));
}

//------------------------------------------------------------------------------

/// insert a joint in the middle of a bone given by its (unique) tailJoint
void SkeletonEditingPlugin::splitBone(int _objectId, int _tailJoint) {

  BaseObjectData* baseObject = 0;
  PluginFunctions::getObject(_objectId, baseObject);

  if (baseObject == 0)
    return;

  Skeleton* skeleton = PluginFunctions::skeleton(baseObject);

  if (skeleton == 0)
    return;

  Skeleton::Joint* tailJoint = skeleton->joint(_tailJoint);

  if (tailJoint == 0) {
    emit log(
        LOGERR,
        tr("Cannot split bone. Unable to find joint with id ")
            + QString::number(_tailJoint));
    return;
  }

  Skeleton::Joint* headJoint = tailJoint->parent();

  //add the new joint
  Skeleton::Joint* jointNew = new Skeleton::Joint(headJoint);
  skeleton->addJoint(headJoint, jointNew);
  tailJoint->setParent(jointNew, *skeleton);

  //set position in refPose
  Skeleton::Pose* refPose = skeleton->referencePose();
  refPose->setGlobalTranslation(
      jointNew->id(),
      0.5 * refPose->globalTranslation(headJoint->id()) + 0.5
          * refPose->globalTranslation(tailJoint->id()));

  //set position in animations
  for (unsigned int a = 0; a < skeleton->animationCount(); a++)
    if (AnimationHandle(a, 0).isValid()) {

      AnimationT<ACG::Vec3d> *animation = skeleton->animation(
          AnimationHandle(a, 0));

      if (animation != 0) {

        //set initial joint translation
        for (int iFrame = 0; iFrame < (int) animation->frameCount(); iFrame++) {

          PoseT<ACG::Vec3d>* pose = skeleton->pose(AnimationHandle(a, iFrame));

          pose->setGlobalMatrix(jointNew->id(),
              pose->globalMatrix(headJoint->id()));
          pose->setGlobalTranslation(
              jointNew->id(),
              0.5 * pose->globalTranslation(headJoint->id()) + 0.5
                  * pose->globalTranslation(tailJoint->id()));
        }
      }
    }

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo("splitBone( ObjectId, " + QString::number(_tailJoint) + " )");

  // Create backup
  emit createBackup(_objectId, "Split Bone", UPDATE_TOPOLOGY);
}

//------------------------------------------------------------------------------

/// add joint to the skeleton
void SkeletonEditingPlugin::addJoint(int _objectId, int _parent,
    Vector _position) {

  BaseObjectData* baseObject = 0;
  PluginFunctions::getObject(_objectId, baseObject);

  if (baseObject == 0)
    return;

  Skeleton* skeleton = PluginFunctions::skeleton(baseObject);

  if (skeleton == 0)
    return;

  Skeleton::Joint* parent = skeleton->joint(_parent);

  if (parent == 0) {
    emit log(
        LOGERR,
        tr("Cannot add joint. Unable to find joint with id ")
            + QString::number(_parent));
    return;
  }

  //add the new joint
  Skeleton::Joint* jointNew = new Skeleton::Joint(parent);
  skeleton->addJoint(parent, jointNew);

  //set the position

  setJointPosition(PluginFunctions::skeletonObject(baseObject), jointNew,
      _position);
  emit updatedObject(_objectId, UPDATE_ALL);

  emit scriptInfo(
      "addJoint( ObjectId, " + QString::number(_parent) + ", Vector("
          + QString::number(_position[0]) + "," + QString::number(_position[1])
          + "," + QString::number(_position[2]) + ") )");

  // Create backup
  emit createBackup(_objectId, "Add Joint", UPDATE_TOPOLOGY);
}

//------------------------------------------------------------------------------

/// delete joint from the skeleton
void SkeletonEditingPlugin::deleteJoint(int _objectId, int _jointId) {

  BaseObjectData* baseObject = 0;
  PluginFunctions::getObject(_objectId, baseObject);

  if (baseObject == 0)
    return;

  Skeleton* skeleton = PluginFunctions::skeleton(baseObject);

  if (skeleton == 0)
    return;

  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(
        LOGERR,
        tr("Cannot Remove joint. Unable to find joint with id ")
            + QString::number(_jointId));
    return;
  }

  skeleton->removeJoint(joint);

  emit scriptInfo("deleteJoint( ObjectId, " + QString::number(_jointId) + " )");

  // Create backup
  emit createBackup(_objectId, "Delete Joint", UPDATE_TOPOLOGY);

  emit updatedObject(_objectId, UPDATE_ALL);
}

//------------------------------------------------------------------------------

/// transform selected joint with given matrix
void SkeletonEditingPlugin::transformJoint(int _objectId, int _jointId,
    Matrix4x4 _matrix) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return;
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);

  if (skeleton == 0) {
    emit log(LOGERR, tr("Unable to get skeleton"));
    return;
  }

  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(LOGERR, tr("Unable to get joint"));
    return;
  }

  bool recursiveJointTransformation = transformChildJoints_;

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  Skeleton::Pose* activePose;

  if (!handle.isValid()) { // reference pose
    activePose = skeleton->referencePose();
  } else { // animation pose

    activePose = skeleton->animation(handle.animationIndex())->pose(
        handle.frame());

    //always transform children otherwise only the local coordsys is rotated
    recursiveJointTransformation = true;

    // translation changes the skeleton structure
    // this is only allowed in refPose therefore delete translation
    Matrix4x4 mat = _matrix;
    mat(0, 3) = 0.0;
    mat(1, 3) = 0.0;
    mat(2, 3) = 0.0;
    if (mat.is_identity())
      _matrix = mat;
  }

  SkeletonTransform transformer(*skeleton);

  if (handle.isValid()) { // animation pose

    //we are in an animation pose -> only rotation allowed
    transformer.rotateJoint(joint, activePose, _matrix, transformAllFrames_);

    //update the skin
    bool exists = false;

    emit functionExists("skeletalanimation", "updateSkin()", exists);

    if (exists)
      RPC::callFunction("skeletalanimation", "updateSkin");

  } else { // reference pose

    // full transformation
    if (_matrix(0, 0) != 1 || _matrix(1, 1) != 1 || _matrix(2, 2) != 1
        || _matrix(0, 1) != 0 || _matrix(0, 2) != 0 || _matrix(1, 0) != 0
        || _matrix(1, 2) != 0 || _matrix(2, 0) != 0 || _matrix(2, 1) != 0) {
      // apply full transformation
      transformer.transformJoint(joint, _matrix, !recursiveJointTransformation);

    } else { // translation only

      if (_matrix(0, 3) == 0 && _matrix(1, 3) == 0 && _matrix(2, 3) == 0) // no translation
        return;

      if (joint->isRoot()) {

        /*
         * Change translation of root joint.
         */

        transformer.translateJoint(joint, ACG::Vec3d(_matrix(0, 3), _matrix(1, 3), _matrix(2, 3)), !transformChildJoints_);

      } else {

        /*
         * Change translation of non-root joint.
         *
         * In these cases, the rotation of the coordinate frame of the parent
         * joint (unless it is not a branching joint) as well as the
         * the rotation of the coordinate frame of the current joint have to be
         * corrected in the following way:
         *
         * We compute the rotation of the affected joints' coordinate system
         * such that the local(!) translation of the respective child
         * joint remains constant. So, in 2D, consider configuration
         * A --> B --> C, where C is a child of B is a child of A.
         * If C has translation (2, 0) and B has translation (3,5), then, after
         * global translation of B, the coordinate frames of A and B have to be rotated
         * such that the joint axes remain constant, so B lies on the line (0,0) + x*(3,5)
         * with respect to A's coordinate system and C lies somewhere on the line (0,0) + y*(2,0)
         * with respect to B's coordinate system.
         *
         */

        // init params
        bool parentIsNotBranch = (joint->parent()->size() == 1);
        bool hasOneChild = (joint->size() == 1);
        ACG::Vec3d oldParentAxis(0.0, 0.0, 0.0);
        ACG::Vec3d oldJointAxis(0.0, 0.0, 0.0);
        ACG::Vec3d transParentAxis(0.0, 0.0, 0.0);
        ACG::Vec3d transJointAxis(0.0, 0.0, 0.0);
        ACG::Vec3d parentRotAxis(0.0, 0.0, 0.0);
        ACG::Vec3d jointRotAxis(0.0, 0.0, 0.0);
        double parentRotAngle = 0.0;
        double jointRotAngle = 0.0;

        // get the original parent axis: parent joint -----> current joint
        oldParentAxis = activePose->localTranslation(_jointId);
        // get the original joint axis: current joint -----> child joint
        if (hasOneChild)
          oldJointAxis = activePose->localTranslation(joint->child(0)->id());

        // store the joint axes of all animations before the translation of the current joint
        // so that the rotations can be calculated for the animation poses
        std::vector<ACG::Vec3d> oldAnimJoint, oldAnimParent;
        for (unsigned int a = 0; a < skeleton->animationCount(); ++a) {
          for (unsigned int iFrame = 0; iFrame < skeleton->animation(a)->frameCount(); iFrame++) {
            Skeleton::Pose* pose = skeleton->animation(a)->pose(iFrame);

            if (hasOneChild)
              oldAnimJoint.push_back(pose->localTranslation(joint->child(0)->id()));

            oldAnimParent.push_back(pose->localTranslation(_jointId));
          }
        }

        // translate the joint
        transformer.translateJoint(joint, ACG::Vec3d(_matrix(0, 3), _matrix(1, 3), _matrix(2, 3)), !transformChildJoints_);

        // get translated parent axis
        transParentAxis = activePose->localTranslation(_jointId);

        if (hasOneChild) {
          // get translated joint axis
          transJointAxis = activePose->localTranslation(joint->child(0)->id());
        }

        // now calculate the rotation that has to occur for a translation of the joint:

        // get the rotation axis and angle between the old and new parent axis
        if (!ACG::Geometry::rotationOfTwoVectors<double>(oldParentAxis, transParentAxis, parentRotAxis, parentRotAngle))
          return;

        // get rotation axis and angle between old and new joint axis
        if (hasOneChild) {
          if (!ACG::Geometry::rotationOfTwoVectors<double>(oldJointAxis, transJointAxis, jointRotAxis, jointRotAngle))
            return;
        }

        if (parentIsNotBranch) {
          // parent rotation matrix
          ACG::GLMatrixT<double> parentRotMatrix;
          parentRotMatrix.identity();
          parentRotMatrix.rotate(parentRotAngle, parentRotAxis);

          // apply rotation to parent joint
          ACG::Matrix4x4d localParent = activePose->localMatrix(joint->parent()->id());
          localParent *= parentRotMatrix;
          activePose->setLocalMatrix(joint->parent()->id(), localParent, false);
        }

        if (hasOneChild) {
          // joint rotation matrix
          ACG::GLMatrixT<double> jointRotMatrix;
          jointRotMatrix.identity();
          jointRotMatrix.rotate(jointRotAngle, jointRotAxis);

          // apply current joint
          ACG::Matrix4x4d localJoint = activePose->localMatrix(joint->id());
          localJoint *= jointRotMatrix;
          activePose->setLocalMatrix(joint->id(), localJoint, false);
        }

        // apply rotations to animation
        std::vector<ACG::Vec3d>::iterator jointIt, parentIt;
        jointIt = oldAnimJoint.begin();
        parentIt = oldAnimParent.begin();
        for (unsigned int a = 0; a < skeleton->animationCount(); ++a) {
          for (unsigned int iFrame = 0; iFrame < skeleton->animation(a)->frameCount(); iFrame++) {
            Skeleton::Pose* pose = skeleton->animation(a)->pose(iFrame);

            // only apply rotation to parent joint if it is not a branch
            if (parentIsNotBranch) {

              // get the rotation axis and angle between the old and new parent axis
              ACG::Vec3d translatedParent = pose->localTranslation(_jointId);
              ACG::Vec3d parentRotAxis(0.0, 0.0, 0.0);
              double parentRotAngle = 0.0;
              if (!ACG::Geometry::rotationOfTwoVectors<double>(*parentIt, translatedParent, parentRotAxis,
                  parentRotAngle))
                return;

              // parent rotation matrix
              ACG::GLMatrixT<double> parentRotMatrix;
              parentRotMatrix.identity();
              parentRotMatrix.rotate(parentRotAngle, parentRotAxis);

              // apply rotation to parent joint
              ACG::Matrix4x4d parentMat = pose->localMatrix(joint->parent()->id());
              parentMat *= parentRotMatrix;
              pose->setLocalMatrix(joint->parent()->id(), parentMat, false);

              ++parentIt;
            }

            if (hasOneChild) {
              // get rotation axis and angle between old and new joint axis
              ACG::Vec3d translatedAxis = pose->localTranslation(joint->child(0)->id());
              ACG::Vec3d jointRotAxis(0.0, 0.0, 0.0);
              double jointRotAngle = 0.0;

              if (!ACG::Geometry::rotationOfTwoVectors<double>(*jointIt, translatedAxis, jointRotAxis, jointRotAngle))
                return;

              // joint rotation matrix
              ACG::GLMatrixT<double> jointRotMatrix;
              jointRotMatrix.identity();
              jointRotMatrix.rotate(jointRotAngle, jointRotAxis);

              // apply rotation to current joint
              ACG::Matrix4x4d localMat = pose->localMatrix(joint->id());
              localMat *= jointRotMatrix;
              pose->setLocalMatrix(joint->id(), localMat, false);

              ++jointIt;
            }
          }
        }
      }
    }
  }

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      matString += " , " + QString::number(_matrix(i, j));

  matString = matString.right(matString.length() - 3);

  emit scriptInfo(
      "transformJoint( ObjectId, " + QString::number(_jointId) + ", Matrix4x4("
          + matString + " ) )");

  // Create backup if there was a change
  // the backup is only created when the slot is called via scripting (sender == 0)
  if (!_matrix.is_identity() && (sender() == 0))
    emit createBackup(_objectId, "Joint Transformation", UPDATE_GEOMETRY);
}

//------------------------------------------------------------------------------

/// get global matrix of a joint in the active pose
Matrix4x4 SkeletonEditingPlugin::globalMatrix(int _objectId, int _jointId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return Matrix4x4();
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return Matrix4x4();
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);
  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(LOGERR, tr("Unable to get joint"));
    return Matrix4x4();
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  Skeleton::Pose* activePose;

  if (!handle.isValid())
    activePose = skeleton->referencePose();
  else
    activePose = skeleton->animation(handle.animationIndex())->pose(
        handle.frame());

  emit
      scriptInfo("globalMatrix( ObjectId, " + QString::number(_jointId) + " )");
  return activePose->globalMatrix(joint->id());
}

//------------------------------------------------------------------------------

/// get local matrix of a joint in the active pose
Matrix4x4 SkeletonEditingPlugin::localMatrix(int _objectId, int _jointId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return Matrix4x4();
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return Matrix4x4();
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);
  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(LOGERR, tr("Unable to get joint"));
    return Matrix4x4();
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  Skeleton::Pose* activePose;

  if (!handle.isValid())
    activePose = skeleton->referencePose();
  else
    activePose = skeleton->animation(handle.animationIndex())->pose(
        handle.frame());

  emit scriptInfo("localMatrix( ObjectId, " + QString::number(_jointId) + " )");
  return activePose->localMatrix(joint->id());
}

//------------------------------------------------------------------------------

/// get global translation of a joint in the active pose
Vector SkeletonEditingPlugin::globalTranslation(int _objectId, int _jointId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return Vector();
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);
  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(LOGERR, tr("Unable to get joint"));
    return Vector();
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  Skeleton::Pose* activePose;

  if (!handle.isValid())
    activePose = skeleton->referencePose();
  else
    activePose = skeleton->animation(handle.animationIndex())->pose(
        handle.frame());

  emit scriptInfo(
      "globalTranslation( ObjectId, " + QString::number(_jointId) + " )");
  return activePose->globalTranslation(joint->id());
}

//------------------------------------------------------------------------------

/// get local translation of a joint in the active pose
Vector SkeletonEditingPlugin::localTranslation(int _objectId, int _jointId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return Vector();
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);
  Skeleton::Joint* joint = skeleton->joint(_jointId);

  if (joint == 0) {
    emit log(LOGERR, tr("Unable to get joint"));
    return Vector();
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  Skeleton::Pose* activePose;

  if (!handle.isValid())
    activePose = skeleton->referencePose();
  else
    activePose = skeleton->animation(handle.animationIndex())->pose(
        handle.frame());

  emit scriptInfo(
      "localTranslation( ObjectId, " + QString::number(_jointId) + " )");
  return activePose->localTranslation(joint->id());
}

//------------------------------------------------------------------------------

/// get the number of animations
int SkeletonEditingPlugin::animationCount(int _objectId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return -1;
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);

  emit scriptInfo("animationCount( ObjectId )");
  return skeleton->animationCount();
}

//------------------------------------------------------------------------------

/// get the number of frames
int SkeletonEditingPlugin::frameCount(int _objectId, int _animationIndex) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return -1;
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);

  if ((_animationIndex < 0) || (_animationIndex
      > (int) skeleton->animationCount())) {
    emit log(LOGERR, tr("Invalid animationIndex"));
    return -1;
  }

  emit scriptInfo(
      "frameCount( ObjectId, " + QString::number(_animationIndex) + " )");
  return skeleton->animation(_animationIndex)->frameCount();
}

//------------------------------------------------------------------------------

/// get active animation
int SkeletonEditingPlugin::activeAnimation(int _objectId) {

  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return -1;
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  emit scriptInfo("activeAnimation( ObjectId )");
  return handle.animationIndex();
}

//------------------------------------------------------------------------------

/// get active frame
int SkeletonEditingPlugin::activeFrame(int _objectId) {
  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return -1;
  }

  AnimationHandle handle = skeletonObj->skeletonNode()->activePose();

  emit scriptInfo("activeFrame( ObjectId )");
  return handle.frame();
}

//------------------------------------------------------------------------------

/// set active pose
void SkeletonEditingPlugin::setActivePose(int _objectId, int _animationIndex,
    int _frame) {
  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return;
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);

  if ((_animationIndex < 0) || (_animationIndex
      > (int) skeleton->animationCount())) {
    emit log(LOGERR, tr("Invalid animationIndex"));
    return;
  }

  if ((_frame < 0) || (_frame
      > (int) skeleton->animation(_animationIndex)->frameCount())) {
    emit log(LOGERR, tr("Invalid frame"));
    return;
  }

  skeletonObj->skeletonNode()->setActivePose(
      AnimationHandle(_animationIndex, _frame));
  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo(
      "setActivePose( ObjectId, " + QString::number(_animationIndex) + ", "
          + QString::number(_frame) + ")");
}

//------------------------------------------------------------------------------

/// add animation
void SkeletonEditingPlugin::addAnimation(int _objectId, QString _name,
    int _frames) {
  BaseObjectData* obj = 0;

  PluginFunctions::getObject(_objectId, obj);

  if (obj == 0) {
    emit log(LOGERR, tr("Unable to get object"));
    return;
  }

  SkeletonObject* skeletonObj = PluginFunctions::skeletonObject(obj);

  if (skeletonObj == 0) {
    emit log(LOGERR, tr("Unable to get skeletonObject"));
    return;
  }

  Skeleton* skeleton = PluginFunctions::skeleton(obj);

  if (_frames <= 0) {
    emit log(LOGERR, tr("Invalid frame count"));
    return;
  }

  std::string stdName = _name.toStdString();

  if (skeleton->animation(stdName) != 0) {
    emit log(LOGERR, tr("Animation with this name already exists"));
    return;
  }

  FrameAnimationT<ACG::Vec3d>* animation = new FrameAnimationT<ACG::Vec3d> (
      skeleton, _frames);
  AnimationHandle handle = skeleton->addAnimation(stdName, animation);

  //init pose to refPose
  for (unsigned int i = 0; i < skeleton->animation(handle)->frameCount(); i++) {
    handle.setFrame(i);
    Skeleton::Pose* pose = skeleton->pose(handle);

    for (unsigned int j = 0; j < skeleton->jointCount(); j++)
      pose->setGlobalMatrix(j, skeleton->referencePose()->globalMatrix(j));
  }

  emit updatedObject(_objectId, UPDATE_ALL);
  emit scriptInfo(
      "addAnimation( ObjectId, " + _name + ", " + QString::number(_frames)
          + ")");

  // Create backup
  emit createBackup(_objectId, "Add Animation", UPDATE_ALL);
}

//------------------------------------------------------------------------------

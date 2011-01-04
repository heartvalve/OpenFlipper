#include "SkeletalAnimationPlugin.hh"

void SkeletalAnimationPlugin::setDescriptions(){
  
  emit setSlotDescription("attachSkin(int,int)",tr("Attaches a mesh to a skeleton as skin. Each are identified by their objectIds"),
                          QString(tr("SkeletonObjectId,SkinObjectId")).split(","),
                          QString(tr("id of the skeleton object, id of the skin object")).split(","));

  emit setSlotDescription("detachSkin(int)",tr("Removes a skeleton's skin"),
                          QStringList(tr("SkeletonObjectId")), QStringList(tr("ID of the skeleton object")));
}

bool SkeletalAnimationPlugin::attachSkin(int skeletonId, int skinId) {
//   BaseObjectData* skeleton = NULL;
//   if (!PluginFunctions::getObject(skeletonId, skeleton)) {
//     emit log(LOGERR, "Could not retrieve skeleton");
//     return false;
//   }
//   
//   BaseObjectData* skin = NULL;
//   if (!PluginFunctions::getObject(skinId, skin)) {
//     emit log(LOGERR, "Could not retrieve skin");
//     return false;
//   }
//   
//   rig(skin, skeleton);
//   
  return true;
}

bool SkeletalAnimationPlugin::detachSkin(int skeletonId) {
//   BaseObjectData* skeleton = NULL;
//   if (!PluginFunctions::getObject(skeletonId, skeleton)) {
//     emit log(LOGERR, "Could not retrieve skeleton");
//     return false;
//   }
//   
//   unrig(skeleton);
//   
  return true;
}

void SkeletalAnimationPlugin::updateSkin(){
  
//   BaseObjectData *pMeshObject = 0, *pSkeletonObject = 0;
//   GetSelectedPair(&pMeshObject, &pSkeletonObject);
// 
//   if (pSkeletonObject != 0){
//     AnimationHandle hAni = PluginFunctions::skeletonObject(pSkeletonObject)->skeletonNode()->getActivePose();
//     UpdateSkin(pSkeletonObject, hAni);
//   }
}
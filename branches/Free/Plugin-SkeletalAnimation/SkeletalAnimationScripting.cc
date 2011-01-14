/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#include "SkeletalAnimationPlugin.hh"

void SkeletalAnimationPlugin::setDescriptions(){
  
  emit setSlotDescription("attachSkin(int,int)",tr("Attaches a mesh to a skeleton as skin. Each are identified by their objectIds"),
                          QString(tr("SkeletonObjectId,SkinObjectId")).split(","),
                          QString(tr("id of the skeleton object, id of the skin object")).split(","));

  emit setSlotDescription("detachSkin(int)",tr("Removes a skeleton's skin"),
                          QStringList(tr("SkeletonObjectId")), QStringList(tr("ID of the skeleton object")));
}

bool SkeletalAnimationPlugin::attachSkin(int /*skeletonId*/, int /*skinId*/) {
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

bool SkeletalAnimationPlugin::detachSkin(int /*skeletonId*/) {
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
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
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "MovePlugin.hh"
#include <ACG/QtScenegraph/QtManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//***********************************************************************************

void MovePlugin::hideManipulator() {
  
  QVariant contextObject = contextActionHide_->data();
  int nodeId = contextObject.toInt();
  
  if ( nodeId == -1)
    return;
  
  // Get Node
  ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode(), nodeId );
  
  ACG::SceneGraph::QtTranslationManipulatorNode* mNode;
  mNode = dynamic_cast<ACG::SceneGraph::QtTranslationManipulatorNode*>(node);
  
  if(mNode == 0) {
    // Not a manipulator node
    return;
  }

  int objectId = mNode->getIdentifier();

  BaseObjectData* obj;
  if ( ! PluginFunctions::getObject(objectId,obj) )
    return;
  
  // Disconnect its signals to the plugin
  disconnect(obj->manipulatorNode() , SIGNAL(manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)),
             this , SLOT( manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)));
             
  disconnect(obj->manipulatorNode() , SIGNAL(positionChanged(QtTranslationManipulatorNode*)),
             this , SLOT( ManipulatorPositionChanged(QtTranslationManipulatorNode*)));    
  
  obj->manipPlaced(false);
  mNode->hide();
  
  emit nodeVisibilityChanged(obj->id());
  
}

void MovePlugin::showProps(){

  QVariant contextObject = contextAction_->data();
  int nodeId = contextObject.toInt();

  if (nodeId == -1)
    return;

  // Get Node
  ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node(PluginFunctions::getSceneGraphRootNode(), nodeId);

  ACG::SceneGraph::QtTranslationManipulatorNode* mNode;
  mNode = dynamic_cast<ACG::SceneGraph::QtTranslationManipulatorNode*>(node);

  if (mNode == 0) {
    // Not a manipulator node
    return;
  }

  int objectId = mNode->getIdentifier();

  BaseObjectData* obj;
  if (!PluginFunctions::getObject(objectId, obj))
    return;

  // Check if the widget has been created and show it.
  movePropsWidget* pW = getDialogWidget(obj);
  if (pW != 0) {
    pW->show();
    pW->raise();
    return;
  }
    
  pW = new movePropsWidget(obj->id());
  pW->setWindowTitle(QString((mNode->name()).c_str()));

  connect(pW->posButton, SIGNAL(clicked() ), this, SLOT(slotSetPosition()));
  connect(pW->axisAButton, SIGNAL(clicked() ), this, SLOT(slotToggleAxisA()));
  connect(pW->axisBButton, SIGNAL(clicked() ), this, SLOT(slotToggleAxisB()));
  connect(pW->dirButton, SIGNAL(clicked() ), this, SLOT(slotSetDirection()));

  connect(pW->transButton, SIGNAL(clicked() ), this, SLOT(slotTranslation()));
  connect(pW->rotButton, SIGNAL(clicked() ), this, SLOT(slotRotate()));
  connect(pW->scaleButton, SIGNAL(clicked() ), this, SLOT(slotScale()));

  connect(pW->projectTangentButton, SIGNAL(clicked() ), this, SLOT(slotProjectToTangentPlane()));
  connect(pW->moveManipToCOG, SIGNAL(clicked() ), this, SLOT(slotMoveManipToCOG()));

  connect(pW->selectionRadioButton, SIGNAL(clicked()), this, SLOT(slotEnableSelectionMode()));
  connect(pW->objectRadioButton, SIGNAL(clicked()), this, SLOT(slotEnableObjectMode()));

  // Values
  BaseObjectData* object;
  if (PluginFunctions::getObject(lastActiveManipulator_, object)) {
    if (object->manipulatorNode()->visible()) {
      const OpenMesh::Vec3d pos = object->manipulatorNode()->center();

      QString num;

      num = QString::number(pos[0]);
      pW->posx->setText(num);
      num = QString::number(pos[1]);
      pW->posy->setText(num);
      num = QString::number(pos[2]);
      pW->posz->setText(num);

      OpenMesh::Vec3d direction = object->manipulatorNode()->directionX();
      num = QString::number(direction[0]);
      pW->dirxx->setText(num);
      num = QString::number(direction[1]);
      pW->dirxy->setText(num);
      num = QString::number(direction[2]);
      pW->dirxz->setText(num);

      direction = object->manipulatorNode()->directionY();
      num = QString::number(direction[0]);
      pW->diryx->setText(num);
      num = QString::number(direction[1]);
      pW->diryy->setText(num);
      num = QString::number(direction[2]);
      pW->diryz->setText(num);

      direction = object->manipulatorNode()->directionZ();
      num = QString::number(direction[0]);
      pW->dirzx->setText(num);
      num = QString::number(direction[1]);
      pW->dirzy->setText(num);
      num = QString::number(direction[2]);
      pW->dirzz->setText(num);
    }
  }

  setPickModeProps(pW,PluginFunctions::pickMode());

  pW->show();
  propsWindows_.append(pW);
}

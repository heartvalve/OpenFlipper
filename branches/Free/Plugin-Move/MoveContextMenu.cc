//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 4795 $
//   $Author: moebius $
//   $Date: 2009-02-12 10:05:59 +0100 (Thu, 12 Feb 2009) $
//
//=============================================================================

#include "MovePlugin.hh"
#include <ACG/QtScenegraph/QtManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//***********************************************************************************

// ContextMenu Action triggered
void MovePlugin::moveContextMenu(QAction* _action) {
  QVariant contextObject = _action->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  if ( _action->text() == "Set properties") 
    showProps( objectId );
}

//***********************************************************************************

void MovePlugin::showProps( int objectId ){
    // Get Node
    ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode(), objectId );
    
    ACG::SceneGraph::QtTranslationManipulatorNode* mNode;
    mNode = dynamic_cast<ACG::SceneGraph::QtTranslationManipulatorNode*>(node);
    
    if(mNode == 0) {
	// Not a manipulator node
	return;
    }
    
    int meshID = mNode->getIdentifier();
    
    ACG::SceneGraph::BaseNode* mesh = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode(), meshID );
    
    propsWindow_->setWindowTitle(QString((mesh->name()).c_str()));
    
    connect(propsWindow_->posButton,SIGNAL(clicked() ),this,SLOT(slotSetPosition()));
    connect(propsWindow_->axisAButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisA()));
    connect(propsWindow_->axisBButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisB()));
    connect(propsWindow_->dirButton,SIGNAL(clicked() ),this,SLOT(slotSetDirection()));
    connect(propsWindow_->transButton,SIGNAL(clicked() ),this,SLOT(slotTranslation()));

    connect(propsWindow_->rotButton,SIGNAL(clicked() ),this,SLOT(slotRotate()));
    connect(propsWindow_->scaleButton,SIGNAL(clicked() ),this,SLOT(slotScale()));

    connect(propsWindow_->projectTangentButton,SIGNAL(clicked() ),this,SLOT(slotProjectToTangentPlane()));
    connect(propsWindow_->moveManipToCOG,SIGNAL(clicked() ),this,SLOT(slotMoveManipToCOG()));
    
    // Values
    BaseObjectData* object;
    if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
	if (  object->manipulatorNode()->visible() ) {
	    const TriMesh::Point pos = object->manipulatorNode()->center();

	    QString num;
	
	    num = QString::number(pos[0]); propsWindow_->posx->setText(num);
	    num = QString::number(pos[1]); propsWindow_->posy->setText(num);
	    num = QString::number(pos[2]); propsWindow_->posz->setText(num);

	    TriMesh::Point direction = object->manipulatorNode()->directionX();
	    num = QString::number(direction[0]); propsWindow_->dirxx->setText(num);
	    num = QString::number(direction[1]); propsWindow_->dirxy->setText(num);
	    num = QString::number(direction[2]); propsWindow_->dirxz->setText(num);
  
	    direction = object->manipulatorNode()->directionY();
	    num = QString::number(direction[0]); propsWindow_->diryx->setText(num);
	    num = QString::number(direction[1]); propsWindow_->diryy->setText(num);
	    num = QString::number(direction[2]); propsWindow_->diryz->setText(num);
  
	    direction = object->manipulatorNode()->directionZ();
	    num = QString::number(direction[0]); propsWindow_->dirzx->setText(num);
	    num = QString::number(direction[1]); propsWindow_->dirzy->setText(num);
	    num = QString::number(direction[2]); propsWindow_->dirzz->setText(num);
	
	}
    }

    propsWindow_->show();
}
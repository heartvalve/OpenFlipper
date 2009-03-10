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

void MovePlugin::showProps(){
    
    QVariant contextObject = contextAction_->data();
    int objectId = contextObject.toInt();
  
    if ( objectId == -1)
	return;
  
    // Get Node
    ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node( PluginFunctions::getSceneGraphRootNode(), objectId );
    
    ACG::SceneGraph::QtTranslationManipulatorNode* mNode;
    mNode = dynamic_cast<ACG::SceneGraph::QtTranslationManipulatorNode*>(node);
    
    if(mNode == 0) {
	// Not a manipulator node
	return;
    }
    
    int meshID = mNode->getIdentifier();

    BaseObjectData* obj;
    if ( ! PluginFunctions::getObject(meshID,obj) )
        return;
    
    movePropsWidget* pW = new movePropsWidget(obj);
    pW->setWindowTitle(QString((mNode->name()).c_str()));
    
    connect(pW->posButton,SIGNAL(clicked() ),this,SLOT(slotSetPosition()));
    connect(pW->axisAButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisA()));
    connect(pW->axisBButton,SIGNAL(clicked() ),this,SLOT(slotToggleAxisB()));
    connect(pW->dirButton,SIGNAL(clicked() ),this,SLOT(slotSetDirection()));
    connect(pW->transButton,SIGNAL(clicked() ),this,SLOT(slotTranslation()));

    connect(pW->rotButton,SIGNAL(clicked() ),this,SLOT(slotRotate()));
    connect(pW->scaleButton,SIGNAL(clicked() ),this,SLOT(slotScale()));

    connect(pW->projectTangentButton,SIGNAL(clicked() ),this,SLOT(slotProjectToTangentPlane()));
    connect(pW->moveManipToCOG,SIGNAL(clicked() ),this,SLOT(slotMoveManipToCOG()));
    
    // Values
    BaseObjectData* object;
    if ( PluginFunctions::getObject(lastActiveManipulator_ , object) ) {
	if (  object->manipulatorNode()->visible() ) {
	    const TriMesh::Point pos = object->manipulatorNode()->center();

	    QString num;
	
	    num = QString::number(pos[0]); pW->posx->setText(num);
	    num = QString::number(pos[1]); pW->posy->setText(num);
	    num = QString::number(pos[2]); pW->posz->setText(num);

	    TriMesh::Point direction = object->manipulatorNode()->directionX();
	    num = QString::number(direction[0]); pW->dirxx->setText(num);
	    num = QString::number(direction[1]); pW->dirxy->setText(num);
	    num = QString::number(direction[2]); pW->dirxz->setText(num);
  
	    direction = object->manipulatorNode()->directionY();
	    num = QString::number(direction[0]); pW->diryx->setText(num);
	    num = QString::number(direction[1]); pW->diryy->setText(num);
	    num = QString::number(direction[2]); pW->diryz->setText(num);
  
	    direction = object->manipulatorNode()->directionZ();
	    num = QString::number(direction[0]); pW->dirzx->setText(num);
	    num = QString::number(direction[1]); pW->dirzy->setText(num);
	    num = QString::number(direction[2]); pW->dirzz->setText(num);
	
	}
    }

    pW->show();
    propsWindows_.append(pW);
}
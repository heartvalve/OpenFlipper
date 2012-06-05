/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define SKELETONOBJECT_C

//== INCLUDES =================================================================

#include <string>
#include <sstream>
#include <ACG/Scenegraph/TextNode.hh>
#include <ACG/Scenegraph/LineNode.hh>
#include <OpenFlipper/common/Types.hh>
#include "Skeleton.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for Skeleton Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The skeleton is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addSkeleton instead. ( see Types.hh::DataType )
*/
SkeletonObject::SkeletonObject( ) :
  BaseObjectData( ),
  skeleton_(NULL),
  skeletonNode_(NULL)
{
  setDataType(DATA_SKELETON);
  init();
}

//-----------------------------------------------------------------------------

SkeletonObject::SkeletonObject( const SkeletonObject &_other ) :
  BaseObjectData(_other)
{
  init(_other.skeleton_);

  setName(name());
}

//-----------------------------------------------------------------------------

/** Destructor for PolyLine Objects. The destructor deletes the Line and all
*  Scenegraph nodes associated with the Line or the object.
*/
SkeletonObject::~SkeletonObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the skeleton below and
  // they do something with the skeleton in the destructor of their
  // perObjectData.
  deleteData();

  // Delete the Skeleton only, if this object contains a mesh
  if ( skeleton_ != NULL)  {
    delete skeleton_;
    skeleton_ = NULL;
  } else {
    std::cerr << "Destructor error : Skeleton already deleted" << std::endl;
  }

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  skeletonNode_    = NULL;
}

//-----------------------------------------------------------------------------

/** Cleanup Function for Line Objects. Deletes the contents of the whole object and
* calls SkeletonObject::init afterwards.
*/
void SkeletonObject::cleanup() {
  // Delete the Skeleton only, if this object contains a line
  if ( skeleton_ != NULL)  {
    delete skeleton_;
    skeleton_ = NULL;
  } else {
    std::cerr << "Cleanup error : Skeleton already deleted" << std::endl;
  }

  BaseObjectData::cleanup();

  skeletonNode_   = NULL;

  setDataType(DATA_SKELETON);

  init();

}

//-----------------------------------------------------------------------------

/** This function initalizes the line object. It creates the scenegraph nodes,
*  the line.
*/
void SkeletonObject::init() {
  skeleton_ = new Skeleton();

  if ( materialNode() == NULL)
    std::cerr << "Error when creating Skeleton Object! materialNode is NULL!" << std::endl;

  skeletonNode_    = new ACG::SceneGraph::SkeletonNodeT< Skeleton >(*skeleton_, materialNode() , "NEW SkeletonNode");
  skeletonNode_->drawMode( ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED | ACG::SceneGraph::DrawModes::POINTS );
  
  //set defaults for the material
  materialNode()->set_color(ACG::Vec4f(0.654, 0.8, 1.0, 1.0));
  materialNode()->set_point_size(8.0);
  materialNode()->set_line_width(3.0);
  materialNode()->set_round_points(true);

}

//-----------------------------------------------------------------------------

void SkeletonObject::init(Skeleton *_pSkeleton)
{
  skeleton_ = new Skeleton(*_pSkeleton);

  if(materialNode() == NULL)
    std::cerr << "Error when creating Skeleton Object! materialNode is NULL!" << std::endl;

  skeletonNode_ = new ACG::SceneGraph::SkeletonNodeT<Skeleton>(*skeleton_, materialNode(), "NEW SkeletonNode");
  skeletonNode_->drawMode( ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED | ACG::SceneGraph::DrawModes::POINTS );

  //set defaults for the material
  materialNode()->set_color(ACG::Vec4f(0.654, 0.8, 1.0, 1.0));
  materialNode()->set_point_size(8.0);
  materialNode()->set_line_width(3.0);
  materialNode()->set_round_points(true);
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void SkeletonObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("SkeletonNode for Skeleton "     + _name.toUtf8() );
  skeletonNode_->name( nodename );
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects line.
* @return Pointer to the line
*/
Skeleton* SkeletonObject::skeleton() {
  return skeleton_;
}

// ===============================================================================
// Visualization
// ===============================================================================

ACG::SceneGraph::SkeletonNodeT< Skeleton >* SkeletonObject::skeletonNode() {
  return skeletonNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString SkeletonObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_SKELETON ) )
    output += "Object Contains a Skeleton : ";

	output += QString::number( skeleton()->jointCount()) + " joints.\n";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraph_pick this function can be used to
* check if the skeletonNode of the object has been picked.
*
* @param _node_idx Index of the picked skeleton node
* @return bool if the skeletonNode of this object is the picking target.
*/
bool SkeletonObject::picked( uint _node_idx ) {
  return ( _node_idx == skeletonNode_->id() );
}

//-----------------------------------------------------------------------------

void SkeletonObject::enablePicking( bool _enable ) {
  skeletonNode_->enablePicking( _enable );
}

//-----------------------------------------------------------------------------

bool SkeletonObject::pickingEnabled() {
  return skeletonNode_->pickingEnabled();
}

//-----------------------------------------------------------------------------

AnimationHandle SkeletonObject::activePose() {
  return skeletonNode()->activePose();
}

//-----------------------------------------------------------------------------

/**
 * @brief Call this to set the active pose
 *
 * This method notifies the skeleton node about the change. Call this method, do not call the
 * SkeletonNodeT::setActivePose method directly.
 *
 * @param _hAni The new pose; both animation and frame index are used
 */
void SkeletonObject::setActivePose(const AnimationHandle &_hAni)
{
  skeletonNode()->setActivePose(_hAni);

  updateIndices(_hAni);
  updateMotionPath(_hAni);
}

//-----------------------------------------------------------------------------

/**
 * @brief Updates the joint index text node positions
 *
 * Reads the current animation and frame from the skeleton node.
 */
void SkeletonObject::updateIndices()
{
  updateIndices(skeletonNode()->activePose());
}

//-----------------------------------------------------------------------------

/**
 * @brief Updates the joint index text node positions
 *
 * Does not add missing indices. Call SkeletonObject::showIndices in that case.
 *
 * @param _hAni A handle to the active animation and frame
 */
void SkeletonObject::updateIndices(const AnimationHandle &_hAni)
{
  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Text material node"))
    return;

  // update the position of existing nodes
  PoseT<OpenMesh::Vec3d> *pose = skeleton_->pose(_hAni);
  for(unsigned int i = 0; i < skeleton_->jointCount(); ++i)
  {
    stringstream buf;
    string nameTransformNode;

    buf << "TextNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(!getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
      continue;

    pTransNode->loadIdentity();
    pTransNode->translate(pose->globalTranslation(i));
  }

  // find and prune redundant nodes
  unsigned int i = skeleton_->jointCount();
  bool bAdditionalNodes = true;
  do{
    stringstream buf;
    string nameTransformNode;

    buf << "TextNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      removeAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
      ++i;
      continue;
    }
    bAdditionalNodes = false;
  }while(bAdditionalNodes);
}

//-----------------------------------------------------------------------------

/**
 * @brief Shows or hides the indices
 *
 * The first call will also create the indices. After the first call, the material nodes visibility is changed.
 *
 * @param _bVisible true if the indices are to be visible, false otherwise
 */
void SkeletonObject::showIndices(bool _bVisible)
{
  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Text material node"))
  {
    pMatNode = new ACG::SceneGraph::MaterialNode(baseNode(), "Text material node");
    addAdditionalNode(pMatNode, "SkeletonPlugin", "Text material node");
  }
  
  pMatNode->set_color(ACG::Vec4f(0, 0, 0, 1.0));
  if(_bVisible)
    pMatNode->show();
  else
    pMatNode->set_status(ACG::SceneGraph::BaseNode::HideSubtree);

  // find and prune redundant nodes
  unsigned int i = skeleton_->jointCount();
  bool bAdditionalNodes = true;
  do{
    stringstream buf;
    string nameTransformNode;

    buf << "TextNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      removeAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
      ++i;
      continue;
    }
    bAdditionalNodes = false;
  }while(bAdditionalNodes);

  // add or update nodes
  PoseT<OpenMesh::Vec3d>* ref = skeleton_->referencePose();
  for(unsigned int i = 0; i < skeleton_->jointCount(); ++i)
  {
    stringstream buf;
    string nameJoint, nameTextNode, nameTransformNode, nameSubMatNode;

    buf.str("");
    buf << i;
    nameJoint = buf.str();

    buf.str("");
    buf << "TextNode " << i;
    nameTextNode = buf.str();

    buf << " Transform";
    nameTransformNode = buf.str();

    buf << " Material";
    nameSubMatNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(!getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      pTransNode = new ACG::SceneGraph::TransformNode(pMatNode, nameTransformNode.c_str());
      addAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
    }
    pTransNode->loadIdentity();
    pTransNode->translate(ref->globalTranslation(i));

    ACG::SceneGraph::MaterialNode *pSubMatNode;
    if(!getAdditionalNode(pSubMatNode, "SkeletonPlugin", nameSubMatNode.c_str()))
    {
      pSubMatNode = new ACG::SceneGraph::MaterialNode(pTransNode, nameSubMatNode.c_str());
      addAdditionalNode(pSubMatNode, "SkeletonPlugin", nameSubMatNode.c_str());
    }
    //     pSubMatNode->set_color(skeleton_->joint(i)->color());

    ACG::SceneGraph::TextNode *pTextNode = NULL;
    if(!getAdditionalNode(pTextNode, "SkeletonPlugin", nameTextNode.c_str()))
    {
      pTextNode = new ACG::SceneGraph::TextNode(pSubMatNode,
                                                nameTextNode.c_str(),
                                                ACG::SceneGraph::TextNode::SCREEN_ALIGNED);
          addAdditionalNode(pTextNode, "SkeletonPlugin", nameTextNode.c_str());
    }
    pTextNode->setText("  " + nameJoint);
    pTextNode->setSize(50);
    pTextNode->multipassNodeSetActive(8, true);
  }
  
  //update the indices with the current animationhandle
  updateIndices( skeletonNode()->activePose() );
}

//-----------------------------------------------------------------------------

bool SkeletonObject::indicesVisible()
{
  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Text material node"))
    return false;
  
  return ( pMatNode->status() == ACG::SceneGraph::BaseNode::Active );
}

//-----------------------------------------------------------------------------

bool SkeletonObject::motionPathVisible()
{
  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Motion material node"))
    return false;
  
  return ( pMatNode->status() == ACG::SceneGraph::BaseNode::Active );
}

//-----------------------------------------------------------------------------

/**
 * @brief Updates the node that visualizes the local space of joint motions
 */
void SkeletonObject::updateMotionPath()
{
  updateMotionPath(skeletonNode()->activePose());
}

//-----------------------------------------------------------------------------

/**
 * @brief Updates the node that visualizes the local space of joint motions
 *
 * Does not update the motion space itself. Only moves it to the current local frame.
 * Call SkeletonObject::showMotionPath if the motion space should be updated
 *
 * @param _hAni A handle to the active animation and frame
 */
void SkeletonObject::updateMotionPath(const AnimationHandle &_hAni)
{

  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Motion material node"))
    return;

  // update the position of existing nodes
  PoseT<OpenMesh::Vec3d>* pose = skeleton_->pose(_hAni);

  for(unsigned int i = 0; i < skeleton_->jointCount(); ++i)
  {

    stringstream buf;
    string nameTransformNode;

    buf << "LineNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(!getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
      continue;

    pTransNode->loadIdentity();
    pTransNode->translate( pose->globalTranslation(i) );
  }

  // find and prune redundant nodes
  unsigned int i = skeleton_->jointCount();
  bool bAdditionalNodes = true;
  do{
    stringstream buf;
    string nameTransformNode;

    buf << "LineNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      removeAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
      ++i;
      continue;
    }
    bAdditionalNodes = false;
  }while(bAdditionalNodes);
}

//-----------------------------------------------------------------------------

/**
 * @brief Shows or hides the local motion space for a joint
 *
 * The first call will also create the corresponding nodes
 *
 * @param _visible true if the motion space is to be visible, false otherwise
 */
void SkeletonObject::showMotionPath(bool _visible)
{
  ACG::SceneGraph::MaterialNode *pMatNode;
  if(!getAdditionalNode(pMatNode, "SkeletonPlugin", "Motion material node"))
  {
    pMatNode = new ACG::SceneGraph::MaterialNode(baseNode(), "Motion material node");
    addAdditionalNode(pMatNode, "SkeletonPlugin", "Motion material node");
  }
  pMatNode->set_color(ACG::Vec4f(0, 0, 0, 1.0));

  if(_visible)
    pMatNode->show();
  else
    pMatNode->set_status(ACG::SceneGraph::BaseNode::HideSubtree);

  // find and prune redundant nodes
  unsigned int i = skeleton_->jointCount();
  bool bAdditionalNodes = true;
  do{
    stringstream buf;
    string nameTransformNode;

    buf << "LineNode " << i << " Transform";
    nameTransformNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      removeAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
      ++i;
      continue;
    }
    bAdditionalNodes = false;
  }while(bAdditionalNodes);


  // add or update nodes
  PoseT<OpenMesh::Vec3d>* ref = skeleton_->referencePose();

  for(unsigned int i = 0; i < skeleton_->jointCount(); ++i)
  {
    stringstream buf;
    string nameJoint, nameLineNode, nameTransformNode, nameSubMatNode;

    buf.str("");
    buf << i;
    nameJoint = buf.str();

    buf.str("");
    buf << "LineNode " << i;
    nameLineNode = buf.str();

    buf << " Transform";
    nameTransformNode = buf.str();

    buf << " Material";
    nameSubMatNode = buf.str();

    ACG::SceneGraph::TransformNode *pTransNode;
    if(!getAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str()))
    {
      pTransNode = new ACG::SceneGraph::TransformNode(pMatNode, nameTransformNode.c_str());
      addAdditionalNode(pTransNode, "SkeletonPlugin", nameTransformNode.c_str());
    }
    pTransNode->loadIdentity();
    ACG::GLMatrixd mat( ref->globalMatrix(i).get_raw_data() );
    pTransNode->scale(mat); //this is just a hack to set the global matrix

    ACG::SceneGraph::LineNode *pLineNode = NULL;
    if(!getAdditionalNode(pLineNode, "SkeletonPlugin", nameLineNode.c_str()))
    {
      pLineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, pTransNode, nameLineNode.c_str());
      addAdditionalNode(pLineNode, "SkeletonPlugin", nameLineNode.c_str());
    }
    pLineNode->clear();

    Skeleton::Joint* joint = skeleton_->joint(i);

    if ( joint->size() > 0 ){

      for (uint c=0; c < joint->size(); c++ ){
        Skeleton::Joint* child = joint->child(c);

        ACG::Vec3d globTrans = ref->localTranslation(child->id());

        //get animation
        AnimationT<ACG::Vec3d>* animation = 0;

        for (uint a = 0; a < skeleton_->animationCount(); a++){

          AnimationHandle animHandle = AnimationHandle(a, 0 );
          animation = skeleton_->animation( animHandle );

          if (animation->name() == "")
            continue;

          ACG::Vec3d lastPos(0.0, 0.0, 0.0);

          // every frame of that animation
          for(unsigned int k = 0; k < animation->frameCount(); ++k){
            animHandle.setFrame(k);
            Skeleton::Pose* pose = skeleton_->pose( animHandle );

            ACG::Matrix4x4d globalInv = pose->globalMatrix( joint->id() );
            globalInv.invert();

            ACG::Matrix4x4d local   = pose->globalMatrix(child->id());
            ACG::Matrix4x4d unified = pose->unifiedMatrix( joint->id() );

            ACG::Vec3d trans = pose->globalTranslation(child->id()) - pose->globalTranslation(joint->id());
            trans *= 0.5;

            pLineNode->add_line( ACG::Vec3d(0.0, 0.0, 0.0), trans );
            pLineNode->add_color( ACG::Vec3uc(180,255,180));

            pLineNode->add_line( lastPos, trans );
            pLineNode->add_color( ACG::Vec3uc(110,160,110));

            lastPos = trans;
          }
        }

        ACG::Vec3d trans = ref->globalTranslation(child->id())-ref->globalTranslation(joint->id());
        trans *= 0.6;

        pLineNode->add_line( ACG::Vec3d(0.0, 0.0, 0.0), trans);
        pLineNode->add_color( ACG::Vec3uc(255,0,0));
      }
    }
  }
  
  //update the indices with the current animationhandle
  updateMotionPath( skeletonNode_->activePose() );
}

//-----------------------------------------------------------------------------

BaseObject* SkeletonObject::copy()
{
  SkeletonObject *pObject = new SkeletonObject(*this);
  return dynamic_cast<BaseObject*>(pObject);
}

//=============================================================================


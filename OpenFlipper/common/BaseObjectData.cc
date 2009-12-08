/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  MyTypes
//
//=============================================================================

//== INCLUDES =================================================================

#include "Types.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ACG/Scenegraph/SceneGraph.hh>
#include <OpenFlipper/common/BaseObjectCore.hh>

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

BaseObjectData::BaseObjectData(const BaseObjectData& _object)
  : BaseObject(_object),
    path_("."),
    manipPlaced_(false),
    rootNode_(_object.rootNode_),
    separatorNode_(0),
    manipulatorNode_(0),
    materialNode_(0),
    boundingBoxNode_(0),
    stencilRefNode_(0)
{
  // We have to create our own visualization nodes as we are a new object
  init();
  
  getObjectManager()->objectCreated(id());
}

BaseObjectData::BaseObjectData( SeparatorNode* _rootNode ) :
  BaseObject(),
  path_("."),
  manipPlaced_(false),
  rootNode_(_rootNode),
  separatorNode_(0),
  manipulatorNode_(0),
  materialNode_(0),
  boundingBoxNode_(0),
  stencilRefNode_(0)
{
  init();
  
  getObjectManager()->objectCreated(id());
}

BaseObjectData::~BaseObjectData() {
  if ( separatorNode_ != 0 ) {
    separatorNode_->delete_subtree();
  }

}

void BaseObjectData::cleanup() {
  path_       = ".";

  // Delete everything below the seperator node on top of the object. This will remove the complete subtree.
  if ( separatorNode_ == 0 )
    std::cerr << "cleanup : separatorNode_ is already 0" << std::endl;
  else {
    separatorNode_->delete_subtree();
    separatorNode_   = 0;
    manipulatorNode_ = 0;
    materialNode_    = 0;
    boundingBoxNode_ = 0;
    additionalNodes_.clear();
  }

  BaseObject::cleanup();

  BaseObjectData::init();
}

void BaseObjectData::init() {
  // Create seperatorNode for Object only if it does not exist.
  if ( separatorNode_ == 0 )
    separatorNode_       = new SeparatorNode((BaseNode*)rootNode_,"NEW Object");
  else
    std::cerr << "Separator Node already exists. this should not happen!" << std::endl;

  if ( manipulatorNode_ == 0 ) {
    manipulatorNode_      = new QtTranslationManipulatorNode(baseNode(),"NEW ManipulatorNode");

    // Bind this manipulator to the current object
    manipulatorNode_->setIdentifier(id());
    manipulatorNode_->set_status( ACG::SceneGraph::TranslationManipulatorNode::HideNode );
  }
  else
    std::cerr << "Manipulator Node already exists. this should not happen!" << std::endl;
  if ( boundingBoxNode_ == 0)
  {
    boundingBoxNode_      = new BoundingBoxNode(manipulatorNode(),  "New Bounding Box");
    boundingBoxNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
  }
  if ( stencilRefNode_ == 0 )
  {
    stencilRefNode_       = new StencilRefNode(boundingBoxNode(),  "New Stencil Reference");
    stencilRefNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
  }
  if ( materialNode_ == 0 )
    materialNode_         = new MaterialNode(stencilRefNode(),  "New Material");
}


// ===============================================================================
// Name and path Handling
// ===============================================================================

void BaseObjectData::setFromFileName(QString _filename ) {
  QString str = _filename;
  path_ = str.section(OpenFlipper::Options::dirSeparator(),0,-2);
  setName(str.section(OpenFlipper::Options::dirSeparator(),-1));
}

void BaseObjectData::setName( QString _name ) {
  BaseObject::setName( _name );

  std::string nodename = std::string("SeparatorNode for object " + _name.toUtf8());
  separatorNode_->name( nodename );

  nodename = std::string("ManipulatorNode for object " + _name.toUtf8());
  manipulatorNode_->name( nodename );

  nodename = std::string("BoundingBoxNode for object " + _name.toUtf8());
  boundingBoxNode_->name( nodename );

  nodename = std::string(_name.toUtf8() + "'s Material" );
  materialNode_->name( nodename );

  nodename = std::string("StencilRefNode for object " + _name.toUtf8());
  stencilRefNode_->name( nodename );
}


QString BaseObjectData::path(){
  return path_;
}

void BaseObjectData::path(QString _path ) {
  path_ = _path;
}

// ===============================================================================
// Object visualization
// ===============================================================================

void BaseObjectData::show() {
  separatorNode_->set_status( ACG::SceneGraph::BaseNode::Active  );
  visible_ = true;
}

void BaseObjectData::hide() {
  separatorNode_->set_status( ACG::SceneGraph::BaseNode::HideSubtree );
  visible_ = false;
}

bool BaseObjectData::visible(){
  return visible_;
}

void BaseObjectData::visible(bool _visible) {

  if (_visible)
    separatorNode_->set_status( ACG::SceneGraph::BaseNode::Active  );
  else
    separatorNode_->set_status( ACG::SceneGraph::BaseNode::HideSubtree );

  visible_ = _visible;
}

SeparatorNode* BaseObjectData::baseNode() {
  return separatorNode_;
}

QtTranslationManipulatorNode* BaseObjectData::manipulatorNode() {
  return manipulatorNode_;
}

ACG::SceneGraph::ShaderNode* BaseObjectData::shaderNode() {
  return 0;
}

MaterialNode* BaseObjectData::materialNode() {
  return materialNode_;
}

BoundingBoxNode* BaseObjectData::boundingBoxNode() {
  return boundingBoxNode_;
}

StencilRefNode* BaseObjectData::stencilRefNode() {
  return stencilRefNode_;
}


void BaseObjectData::setBaseColor(ACG::Vec4f _color) {
  materialNode_->set_base_color(_color);
}

bool BaseObjectData::manipPlaced() {
  return manipPlaced_;
}

void BaseObjectData::manipPlaced( bool _placed ) {
  manipPlaced_ = _placed;
}

void BaseObjectData::getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax){

  // Single pass action, as the bounding box is not influenced by multipass traversal
  ACG::SceneGraph::BoundingBoxAction act;
  ACG::SceneGraph::traverse(separatorNode_, act);


  bbmin = (ACG::Vec3d) act.bbMin();
  bbmax = (ACG::Vec3d) act.bbMax();
}

// ===============================================================================
// Picking
// ===============================================================================

bool BaseObjectData::picked( uint /* _node_idx */ ) {
 return false;
}

void BaseObjectData::enablePicking( bool /*_enable*/ ) {
}

bool BaseObjectData::pickingEnabled() {
  return true;
}

// ===============================================================================
// Content Nodes
// ===============================================================================
void BaseObjectData::update()
{

}

// ===============================================================================
// Additional Nodes
// ===============================================================================

bool BaseObjectData::hasAdditionalNode(QString _pluginName, QString _nodeName , int _id )
{
  QString index;
  index.setNum(_id);
  QString searchname = _pluginName+"#"+_nodeName+"#"+index;

  for ( uint i =0 ; i < additionalNodes_.size() ; ++i ) {
    if (additionalNodes_[i].second == searchname )
      return true;
  }

  return false;
}


//=============================================================================

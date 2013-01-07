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

//== INCLUDES =================================================================

#include "Types.hh"
#include <ACG/Scenegraph/SceneGraph.hh>
#include <OpenFlipper/common/BaseObjectCore.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <QDir>



//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

BaseObjectData::BaseObjectData(const BaseObjectData& _object)
  : BaseObject(_object),
    manipPlaced_(false),
    rootNode_(_object.rootNode_),
    separatorNode_(0),
    manipulatorNode_(0),
    materialNode_(0),
    boundingBoxNode_(0),
    stencilRefNode_(0)
{
  /// We have to create our own visualization nodes as we are a new object
  initializeScenegraphNodes();
}

BaseObjectData::BaseObjectData() :
  BaseObject(),
  manipPlaced_(false),
  rootNode_( dynamic_cast< ACG::SceneGraph::SeparatorNode* > (PluginFunctions::getRootNode()) ),
  separatorNode_(0),
  manipulatorNode_(0),
  materialNode_(0),
  boundingBoxNode_(0),
  stencilRefNode_(0)
{
  initializeScenegraphNodes();
}

BaseObjectData::~BaseObjectData() {
  if ( separatorNode_ != 0 ) {
    separatorNode_->delete_subtree();
  }

}

void BaseObjectData::cleanup() {

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

  initializeScenegraphNodes();
}

void BaseObjectData::initializeScenegraphNodes() {
  // Create seperatorNode for Object only if it does not exist.
  if ( separatorNode_ == 0 )
    separatorNode_       = new SeparatorNode((BaseNode*)rootNode_,"NEW Object");
  else
    std::cerr << "Separator Node already exists. this should not happen!" << std::endl;

  if ( manipulatorNode_ == 0 ) {
    manipulatorNode_      = new QtTranslationManipulatorNode(baseNode(),"NEW ManipulatorNode");

    // Bind this manipulator to the current object
    manipulatorNode_->setIdentifier(id());
    manipulatorNode_->set_status( ACG::SceneGraph::TranslationManipulatorNode::Active );
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
  if ( materialNode_ == 0 ) {
    materialNode_         = new MaterialNode(stencilRefNode(),  "New Material");
    
    QColor color;
    
    if ( OpenFlipper::Options::randomDefaultColor() ){

      QColor bckgrnd = OpenFlipperSettings().value("Core/Gui/glViewer/defaultBackgroundColor").value<QColor>();
      int diff;
      
      do{
        color.setRgb(rand()%255, rand()%255, rand()%255);
        
        diff = (bckgrnd.red()   - color.red())  *(bckgrnd.red()   - color.red())
              +(bckgrnd.green() - color.green())*(bckgrnd.green() - color.green())
              +(bckgrnd.blue()  - color.blue()) *(bckgrnd.blue()  - color.blue());
      } while (diff < 70000);
    }
    else{
      color = OpenFlipper::Options::defaultColor();
    }
    
    ACG::Vec4f colorV;
    colorV[0] = color.redF();
    colorV[1] = color.greenF();
    colorV[2] = color.blueF();
    colorV[3] = color.alphaF();
    
    materialNode_->set_base_color(colorV);
    materialNode_->set_color(colorV);
    
    // We take the brightest color component for overlays to get best contrast
    materialNode_->set_overlay_color(materialNode_->specular_color());
  }
}


// ===============================================================================
// Object visualization
// ===============================================================================

void BaseObjectData::show() {
  if ( !visible_ ) {
    separatorNode_->set_status( ACG::SceneGraph::BaseNode::Active  );
    visible_ = true;
    
    emit visibilityChanged( id() );
  }
}

void BaseObjectData::hide() {
  if ( visible_ ) {
    separatorNode_->set_status( ACG::SceneGraph::BaseNode::HideSubtree );
    visible_ = false;
    
    emit visibilityChanged( id() );
  }
}

bool BaseObjectData::visible(){
  return visible_;
}

void BaseObjectData::visible(bool _visible) {

  if ( visible_ != _visible ) {
    
    if (_visible)
      separatorNode_->set_status( ACG::SceneGraph::BaseNode::Active  );
    else
      separatorNode_->set_status( ACG::SceneGraph::BaseNode::HideSubtree );

    visible_ = _visible;
    
    emit visibilityChanged( id() );
  }
}

SeparatorNode* BaseObjectData::baseNode() {
  return separatorNode_;
}

bool BaseObjectData::hasNode(BaseNode* _node) {

  QList< BaseNode* > list;
  list.push_back(separatorNode_);
  
  BaseNode* currentNode;
  while (!list.empty()) {
    currentNode = list.front();
    
    if ( currentNode->find(_node) == currentNode->childrenEnd() ) {
      for ( BaseNode::ChildIter child = currentNode->childrenBegin() ; child != currentNode->childrenEnd(); ++child )
        list.push_back(*child);
    } else 
      return true;
      
    list.pop_front();
  }
  
  return false;
  
}

BaseNode* BaseObjectData::primaryNode() {
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

void BaseObjectData::setObjectDrawMode(const ACG::SceneGraph::DrawModes::DrawMode _mode, bool _force) {
    
  // Set draw mode for this object
  ACG::SceneGraph::SetDrawModesAction actionActive(_mode, _force);
  
  // Traverse scenegraph in order to set new draw mode
  ACG::SceneGraph::traverse(primaryNode(), actionActive);
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
void BaseObjectData::update(UpdateType _type )
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

void BaseObjectData::setName(QString _name ) {
  BaseObject::setName(_name);

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


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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  Plugin Functions
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>

#include "PluginFunctions.hh"

namespace PluginFunctions {

 /** \brief DONT USE DIRECTLY!!
  *
  * This Pointer will be used to access the data trees root in the plugin. Never use it directly as
 *  the internal Structure will change. Use the Access functions instead */
static BaseObject* objectRoot_;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is internally used to acces the examiner widget in the main apllication
 */
static std::vector< ACG::QtWidgets::QtExaminerViewer* > examiner_widgets_;

/// TODO : Remove this variable and implement multiView correctly here
static ACG::QtWidgets::QtExaminerViewer*  examiner_widget_;

static unsigned int activeExaminer_ = 0;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is internally used to access the scenegraphs root node
 */
static SeparatorNode* root_node_;

static SeparatorNode* sceneGraph_root_node_;

void setDataRoot( BaseObject* _root ) {
   objectRoot_ = _root;
}

void set_examiner( std::vector< ACG::QtWidgets::QtExaminerViewer* > _examiner_widgets ) {
   PluginFunctions::examiner_widgets_ = _examiner_widgets;
   PluginFunctions::examiner_widget_ =  examiner_widgets_[0];
}

void setActiveExaminer( unsigned int _id ) {
  activeExaminer_ = _id;
}

void set_rootNode( SeparatorNode* _root_node ) {
   PluginFunctions::root_node_ = _root_node;
}

void set_sceneGraphRootNode( SeparatorNode* _root_node ) {
   PluginFunctions::sceneGraph_root_node_ = _root_node;
}

bool get_picked_object(uint _node_idx , BaseObjectData*& _object) {
  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if ( o_it->picked( _node_idx ) ) {
      _object = *o_it;
      return true;
    }
  }
  return false;
}


bool get_source_identifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if ( o_it->source() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

bool get_target_identifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if ( o_it->target() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

// ===============================================================================
// Get objects
// ===============================================================================

bool get_object(  int _identifier , BaseObject*& _object ) {

  if ( _identifier == -1 )
    return false;

  _object = objectRoot_->childExists( _identifier );
  return ( _object != 0 );
}

bool get_object(  int _identifier , BaseObjectData*& _object ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  _object = dynamic_cast< BaseObjectData* >(object);
  return ( _object != 0 );
}

// ===============================================================================
// ===============================================================================


bool deleteObject( int _id ) {

  if ( _id == -1 )
    return false;

  // get the node
  BaseObject* object = objectRoot_->childExists(_id);

  if ( !object || object == objectRoot() ) {
    std::cerr << "Error while deleting object, does not exist!!" << std::endl;
    return false;
  }

  // remove the whole subtree below this item
  object->deleteSubtree();

  // remove the item itself from the parent
  object->parent()->removeChild(object);

  // delete it
  delete object;

  return true;
}

void deleteAll( ) {
  objectRoot()->deleteSubtree();
}

int copyObject( int _id ) {
  if ( _id == -1 )
    return -1;

  // get the node
  BaseObject* object = objectRoot_->childExists(_id);

  if ( !object ) {
    std::cerr << "Error while copying object ... not found" << std::endl;
    return -1;
  }

  // remove the whole subtree below this item
  BaseObject* copy = object->copy();

  if ( copy == 0 ) {
    std::cerr << "Unable to create a copy of the object." << std::endl;
  }

  // Integrate into object tree
  copy->setParent( object->parent() );
  if ( object->parent() )
    object->parent()->appendChild(copy);
  else
    std::cerr << "Unable to add copy to object list" << std::endl;

  return copy->id();
}

bool object_exists(  int _identifier ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  return ( object != 0 );
}

//===============================================================================

bool get_all_meshes( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

bool get_all_object_identifiers( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

void setBackColor( OpenMesh::Vec4f _color) {
  examiner_widgets_[activeExaminer_]->backgroundColor(_color);
}

QPoint mapToGlobal(QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapToGlobal(_point);
}

QPoint mapToLocal( QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapFromGlobal(_point);
}

/** Set the draw Mode of the examiner widget.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 */
void setDrawMode( unsigned int _mode ) {
  for ( uint i = 0 ; i < examiner_widgets_.size() ; ++i ) {
    examiner_widgets_[i]->drawMode(_mode);
    examiner_widgets_[i]->updateGL();
  }
}

/** Get the current draw Mode of the examiner widget.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 */
unsigned int drawMode( ) {
  return examiner_widget_->drawMode();
}

bool scenegraph_pick( ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

   return examiner_widgets_[activeExaminer_]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}

bool scenegraph_pick( unsigned int _examiner, ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return false;
  }
  return examiner_widgets_[_examiner]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}


//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( ACG::SceneGraph::MouseEventAction  &_action ) {
   ACG::SceneGraph::traverse(PluginFunctions::examiner_widgets_[activeExaminer_]->sceneGraph(),
                             _action,PluginFunctions::examiner_widgets_[activeExaminer_]->glState() );
}

//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse(  unsigned int _examiner, ACG::SceneGraph::MouseEventAction  &_action ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return;
  }

  ACG::SceneGraph::traverse(PluginFunctions::examiner_widgets_[_examiner]->sceneGraph(), _action,PluginFunctions::examiner_widgets_[_examiner]->glState() );
}


const std::string & pickMode () {
   return examiner_widget_->pickMode();
}

void pickMode ( std::string _mode) {
  for ( uint i = 0 ; i < examiner_widgets_.size() ; ++i )
    examiner_widgets_[i]->pickMode(_mode);
}

void pickMode ( unsigned int _examiner, std::string _mode) {
  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return;
  }

  examiner_widgets_[_examiner]->pickMode(_mode);
}

void actionMode ( ACG::QtWidgets::QtBaseViewer::ActionMode _mode) {
  examiner_widget_->actionMode(_mode);
}

ACG::GLState&  glState() {
   return examiner_widget_->glState();
}

void getCurrentViewImage(QImage& _image) {
  examiner_widget_->copyToImage( _image );
}

ACG::QtWidgets::QtBaseViewer::ActionMode actionMode() {
   return examiner_widget_->actionMode();
}

void perspectiveProjection() {
  examiner_widget_->perspectiveProjection();
}

void orthographicProjection() {
  examiner_widget_->orthographicProjection();
}

void allowRotation(bool _mode) {
  examiner_widget_->allowRotation(_mode);
}

void viewingDirection(const ACG::Vec3d &_dir, const ACG::Vec3d &_up) {
  examiner_widget_->viewingDirection(_dir,_up);
}


void setScenePos(const ACG::Vec3d& _center, double _radius) {
  examiner_widget_->setScenePos( _center, _radius );
}

void setScenePos(const ACG::Vec3d& _center) {
  examiner_widget_->setScenePos( _center, examiner_widget_->scene_radius() );
}

const ACG::Vec3d& sceneCenter() {
   return examiner_widget_->scene_center();
}

double sceneRadius() {
   return examiner_widget_->scene_radius();
}

void translate( const ACG::Vec3d &_vector ) {
  examiner_widget_->translate(_vector);
}

void rotate(const ACG::Vec3d&  _axis,
            double             _angle,
            const ACG::Vec3d&  _center)
{
  examiner_widget_->rotate(_axis,_angle,_center);
}

void viewHome() {
  examiner_widget_->home();
}

void viewAll() {
  examiner_widget_->viewAll();
}

ACG::Vec3d viewingDirection() {
  return examiner_widget_->glState().viewing_direction();
}

ACG::Vec3d eyePos() {
  return examiner_widget_->glState().eye();
}

ACG::Vec3d upVector() {
  return examiner_widget_->glState().up();
}


ACG::SceneGraph::BaseNode* getSceneGraphRootNode() {
   return PluginFunctions::sceneGraph_root_node_;
}

ACG::SceneGraph::BaseNode* getRootNode() {
   return PluginFunctions::root_node_;
}

void addNode(ACG::SceneGraph::BaseNode* _node){
  if (PluginFunctions::root_node_)
    _node->set_parent( PluginFunctions::root_node_ );
}

void addGlobalNode(ACG::SceneGraph::BaseNode* _node){
  if (PluginFunctions::sceneGraph_root_node_){
    //set node as new parent for root's children
    while( PluginFunctions::sceneGraph_root_node_->nChildren() > 0 ){
      ACG::SceneGraph::BaseNode* child = *(PluginFunctions::sceneGraph_root_node_->childrenBegin());
      child->set_parent( _node );
    }

    _node->set_parent( PluginFunctions::sceneGraph_root_node_ );
  }
}

int object_count() {

  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    ++count;
  }

  return ( count );

}

int target_count() {

  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    ++count;
  }

  return ( count );
}

int source_count() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    ++count;
  }

  return ( count );
}

int visible_count() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    if ( o_it->visible() )
    ++count;
  }

  return ( count );

}


/** Returns a vector containing all currently available objects
 *
 * @param _objects vector of all objects
 */
void get_all_objects( std::vector < BaseObjectData*>& _objects ) {

   _objects.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)  {
    _objects.push_back( *o_it );
  }

}

///   Fly to point and viewing direction (animated).
void   flyTo (const ACG::Vec3d &_position, const ACG::Vec3d &_center, double _time) {
   examiner_widget_->flyTo(_position,_center,_time);
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

BaseObjectData* baseObjectData( BaseObject* _object ){
  if ( _object == 0 )
    return 0;

  return dynamic_cast< BaseObjectData* >(_object);
}

// ===============================================================================
// Get the root of the object structure
// ===============================================================================
BaseObject*& objectRoot() {
  return (objectRoot_);
}

}

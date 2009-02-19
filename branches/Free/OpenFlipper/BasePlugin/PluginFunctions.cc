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
#include "PluginFunctionsCore.hh"

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
static std::vector< glViewer* > examiner_widgets_;

/** \brief DONT USE DIRECTLY!!
 *
 * This vector is used internally to access properties of all viewers
 */
static std::vector< Viewer::ViewerProperties* > viewerProperties_;

/// TODO : Remove this variable and implement multiView correctly here
static glViewer*  examiner_widget_;

static unsigned int activeExaminer_ = 0;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is internally used to access the scenegraphs root node
 */
static SeparatorNode* root_node_;

static SeparatorNode* sceneGraph_root_node_;

static Viewer::ViewerProperties dummyProperties;

void setDataRoot( BaseObject* _root ) {
   objectRoot_ = _root;
}

void setViewers( std::vector< glViewer* > _viewerWidgets ) {
   PluginFunctions::examiner_widgets_ = _viewerWidgets;
   PluginFunctions::examiner_widget_ =  examiner_widgets_[0];
}

void setViewerProperties( std::vector< Viewer::ViewerProperties* > _viewerProperties ) {
   PluginFunctions::viewerProperties_ = _viewerProperties;
}

void setActiveExaminer( const unsigned int _id ) {
  activeExaminer_ = _id;
}

glViewer* viewer(int  _viewerId ) {
   if ( _viewerId < 0 || _viewerId >= (int)examiner_widgets_.size() ) {
     std::cerr << "Requested unknown viewer with id : " << _viewerId << std::endl;
     return examiner_widgets_[activeExaminer()];
   }

   return( examiner_widgets_[_viewerId] );
}

unsigned int activeExaminer( ) {
  return activeExaminer_;
}

void setRootNode( SeparatorNode* _root_node ) {
   PluginFunctions::root_node_ = _root_node;
}

void setSceneGraphRootNode( SeparatorNode* _root_node ) {
   PluginFunctions::sceneGraph_root_node_ = _root_node;
}

bool getPickedObject(const unsigned int _node_idx , BaseObjectData*& _object) {
  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->picked( _node_idx ) ) {
      _object = *o_it;
      return true;
    }
  }
  return false;
}


bool getSourceIdentifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->source() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

bool getTargetIdentifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->target() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  const int _identifier , BaseObject*& _object ) {

  if ( _identifier == -1 )
    return false;

  _object = objectRoot_->childExists( _identifier );
  return ( _object != 0 );
}

bool getObject(  const int _identifier , BaseObjectData*& _object ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  _object = dynamic_cast< BaseObjectData* >(object);
  return ( _object != 0 );
}

// ===============================================================================
// ===============================================================================


bool deleteObject( const int _id ) {

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

int copyObject( const int _id ) {
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

bool object_exists(  const int _identifier ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  return ( object != 0 );
}

//===============================================================================

bool getAllMeshes( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

bool get_all_object_identifiers( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

void setBackColor( OpenMesh::Vec4f _color) {
  for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
    PluginFunctions::viewerProperties(i).backgroundColor(_color);
}

QPoint mapToGlobal(const QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapToGlobal(_point);
}

QPoint mapToLocal( const QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapFromGlobal(_point);
}

void setDrawMode( const unsigned int _mode , int _viewer) {

  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->drawMode(_mode);
    examiner_widgets_[activeExaminer_]->updateGL();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i ) {
      examiner_widgets_[i]->drawMode(_mode);
      examiner_widgets_[i]->updateGL();
    }
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() ) {
    examiner_widgets_[_viewer]->drawMode(_mode);
    examiner_widgets_[_viewer]->updateGL();
  }
  else
    std::cerr << "Requested illegal viewer for setting DrawMode!!" << std::endl;

}

/** Get the current draw Mode of the examiner widget.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 */
unsigned int drawMode( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return examiner_widgets_[activeExaminer_]->drawMode();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get viewing direction!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return examiner_widgets_[_viewer]->drawMode();
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;

  return examiner_widgets_[activeExaminer_]->drawMode();
}

bool scenegraphPick( ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

   return examiner_widgets_[activeExaminer_]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}

bool scenegraphPick( const unsigned int _examiner, ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return false;
  }
  return examiner_widgets_[_examiner]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}


//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( ACG::SceneGraph::MouseEventAction  &_action ) {
   ACG::SceneGraph::traverse(sceneGraph_root_node_,
                             _action,viewerProperties().glState() );
}

//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( const unsigned int _examiner, ACG::SceneGraph::MouseEventAction  &_action ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return;
  }

  ACG::SceneGraph::traverse(sceneGraph_root_node_, _action,viewerProperties(_examiner).glState() );
}


const std::string & pickMode () {
  // No seperate draw modes available all should have the same so take first
  return examiner_widgets_[0]->pickMode();
}

void pickMode ( std::string _mode) {
  for ( uint i = 0 ; i < examiner_widgets_.size() ; ++i )
    examiner_widgets_[i]->pickMode(_mode);
}

void pickMode ( const unsigned int _examiner, std::string _mode) {
  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return;
  }

  examiner_widgets_[_examiner]->pickMode(_mode);
}

void actionMode ( Viewer::ActionMode _mode) {

  for ( uint i = 0 ; i < examiner_widgets_.size() ; ++i )
    viewerProperties(i).actionMode(_mode);
}

void getCurrentViewImage(QImage& _image) {
  examiner_widget_->copyToImage( _image );
}

Viewer::ActionMode actionMode() {
   return viewerProperties(activeExaminer_).actionMode();
}

Viewer::ViewerProperties& viewerProperties(int _id) {
  if ( _id >= (int)viewerProperties_.size() ) {
    std::cerr << " Error, requested properties for non-existing Viewer!" << std::endl;
    return dummyProperties;
  }

  if ( _id == -1 )
    _id = activeExaminer_;

  return ( *viewerProperties_[_id] );

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

void setMainGLContext() {
  examiner_widget_->makeCurrent();
}

void viewingDirection(const ACG::Vec3d &_dir, const ACG::Vec3d &_up , int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->viewingDirection(_dir,_up);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->viewingDirection(_dir,_up);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->viewingDirection(_dir,_up);
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;
}


void setScenePos(const ACG::Vec3d& _center,const double _radius, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->setScenePos( _center, _radius );
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->setScenePos( _center, _radius );
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->setScenePos( _center, _radius );
  else
    std::cerr << "Requested illegal viewer for setScenePos!!" << std::endl;
  examiner_widgets_[activeExaminer_];
}

void setScenePos(const ACG::Vec3d& _center, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->setScenePos( _center, examiner_widgets_[activeExaminer_]->scene_radius() );
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->setScenePos( _center, examiner_widgets_[i]->scene_radius() );
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->setScenePos( _center, examiner_widgets_[_viewer]->scene_radius() );
  else
    std::cerr << "Requested illegal viewer for setScenePos!!" << std::endl;
}

const ACG::Vec3d& sceneCenter( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return examiner_widgets_[activeExaminer_]->scene_center();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get viewing direction!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return examiner_widgets_[_viewer]->scene_center();
  else
    std::cerr << "Requested illegal viewer for sceneCenter!!" << std::endl;

  return examiner_widgets_[activeExaminer_]->scene_center();
}

double sceneRadius() {
   return examiner_widgets_[activeExaminer_]->scene_radius();
}

void translate( const ACG::Vec3d &_vector , int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->translate(_vector);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->translate(_vector);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->translate(_vector);
  else
    std::cerr << "Requested illegal viewer for translate!!" << std::endl;
}

void rotate(const ACG::Vec3d&  _axis,
            const double       _angle,
            const ACG::Vec3d&  _center,
            int                _viewer )
{
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->rotate(_axis,_angle,_center);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->rotate(_axis,_angle,_center);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->rotate(_axis,_angle,_center);
  else
    std::cerr << "Requested illegal viewer for rotate!!" << std::endl;
}

void viewHome(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->home();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->home();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->home();
  else
    std::cerr << "Requested illegal viewer for viewHome!!" << std::endl;
}

void viewAll(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->viewAll();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->viewAll();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->viewAll();
  else
    std::cerr << "Requested illegal viewer for viewAll!!" << std::endl;
}

ACG::Vec3d viewingDirection(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().viewing_direction();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get viewing direction!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().viewing_direction();
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;

  return viewerProperties().glState().viewing_direction();
}

ACG::Vec3d eyePos(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().eye();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get eyePos!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().eye();
  else
    std::cerr << "Requested illegal viewer for eyePos!!" << std::endl;

  return viewerProperties().glState().eye();
}

ACG::Vec3d upVector(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().up();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get up vector!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().up();
  else
    std::cerr << "Requested illegal viewer for up vector!!" << std::endl;

  return viewerProperties().glState().up();
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

int objectCount() {

  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    ++count;
  }

  return ( count );

}

int targetCount() {

  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    ++count;
  }

  return ( count );
}

int sourceCount() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    ++count;
  }

  return ( count );
}

int visibleCount() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
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
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _objects.push_back( *o_it );
  }

}

///   Fly to point and viewing direction (animated).
void   flyTo (const ACG::Vec3d &_position, const ACG::Vec3d &_center, double _time) {
   examiner_widgets_[activeExaminer_]->flyTo(_position,_center,_time);
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

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
static ACG::QtWidgets::QtExaminerViewer* examiner_widget_;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is internally used to access the scenegraphs root node
 */
static SeparatorNode* root_node_;

void setDataRoot( BaseObject* _root ) {
   objectRoot_ = _root;
}

void set_examiner( ACG::QtWidgets::QtExaminerViewer* _examiner_widget ) {
   PluginFunctions::examiner_widget_ = _examiner_widget;
}

void set_rootNode( SeparatorNode* _root_node ) {
   PluginFunctions::root_node_ = _root_node;
}

bool get_source_meshes( std::vector<TriMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if (! o_it->source() )
      continue;
    _meshes.push_back ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() );
  }

  return (_meshes.size() > 0 );
}

bool get_source_meshes( std::vector<PolyMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_POLY_MESH) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if (! o_it->source() )
      continue;
    _meshes.push_back ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() );
  }

  return (_meshes.size() > 0 );
}


bool get_target_meshes( std::vector<TriMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_TRIANGLE_MESH) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if (! o_it->target() )
      continue;
    if ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() )
      _meshes.push_back ( dynamic_cast< TriMeshObject* >( *o_it )->mesh() );
  }

  return (_meshes.size() > 0 );
}

bool get_target_meshes( std::vector<PolyMesh*>& _meshes  ) {
  _meshes.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_POLY_MESH) ; o_it != PluginFunctions::objects_end(); ++o_it) {
    if (! o_it->target() )
      continue;
    if ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() )
      _meshes.push_back ( dynamic_cast< PolyMeshObject* >( *o_it )->mesh() );
  }

  return (_meshes.size() > 0 );
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

bool get_object(  int _identifier , TriMeshObject*& _object ) {
  
  if ( _identifier == -1 )
    return false;
  
  BaseObject* object = objectRoot_->childExists( _identifier );
  _object = dynamic_cast< TriMeshObject* >(object);
  return ( _object != 0 );
}

bool get_object(  int _identifier , PolyMeshObject*& _object ) {
  
  if ( _identifier == -1 )
    return false;
  
  BaseObject* object = objectRoot_->childExists( _identifier );
  _object = dynamic_cast< PolyMeshObject* >(object);
  return ( _object != 0 );
}

// ===============================================================================
// ===============================================================================


bool get_mesh(  int _identifier , TriMesh*& _mesh ) {
  
  if ( _identifier == -1 )
    return false;
  
  BaseObject* object = objectRoot_->childExists( _identifier );

  // Unable to find object
  if ( object == 0)
    return false;

  TriMeshObject* triangleMeshObject = dynamic_cast< TriMeshObject* > (object);

  // Object is not a triangle mesh
  if ( triangleMeshObject == 0)
    return false;

  _mesh = triangleMeshObject->mesh();
  return true;
}

bool get_mesh(  int _identifier , PolyMesh*& _mesh ) {
  
  if ( _identifier == -1 )
    return false;
  
  BaseObject* object = objectRoot_->childExists( _identifier );

  // Unable to find object
  if ( object == 0)
    return false;

  PolyMeshObject* polyMeshObject = dynamic_cast< PolyMeshObject* > (object);

  // Object is not a triangle mesh
  if ( polyMeshObject == 0)
    return false;

  _mesh = polyMeshObject->mesh();
  return true;
}

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
  examiner_widget_->backgroundColor(_color);
}

QPoint mapToGlobal(QPoint _point ) {
  return examiner_widget_->glMapToGlobal(_point);
}

QPoint mapToLocal( QPoint _point ) {
  return examiner_widget_->glMapFromGlobal(_point);
}

/** Set the draw Mode of the examiner widget.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 */
void setDrawMode( unsigned int _mode ) {
  examiner_widget_->drawMode(_mode);
  examiner_widget_->updateGL();
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
   return examiner_widget_->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}


//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( ACG::SceneGraph::MouseEventAction  &_action ) {
   ACG::SceneGraph::traverse(PluginFunctions::examiner_widget_->sceneGraph(), _action,PluginFunctions::examiner_widget_->glState() );
}

const std::string & pickMode () {
   return examiner_widget_->pickMode();
}

void pickMode ( std::string _mode) {
  examiner_widget_->pickMode(_mode);
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


ACG::SceneGraph::BaseNode* getRootNode() {
   return PluginFunctions::root_node_;
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
void   flyTo (const TriMesh::Point &_position, const TriMesh::Point &_center, double _time) {
   examiner_widget_->flyTo((ACG::Vec3d)_position,(ACG::Vec3d)_center,_time);
}





ObjectIterator::ObjectIterator( IteratorRestriction _restriction , DataType _dataType) {

  // Initialize with invalid pos
  pos_ = 0;

  // Store the restriction for the operator ( Source/Target )
  restriction_ = _restriction;

  // Store the requested DataType
  dataType_ = _dataType;

  // Start at the root Node
  BaseObject* currentPos = objectRoot_->next();

  // Go through the tree and stop at the root node or if we found a baseObjectData Object
  while ( (currentPos != objectRoot_) && !dynamic_cast<BaseObjectData* > (currentPos)  )
    currentPos = currentPos->next();

  if (currentPos == objectRoot_)
    return;


  while ( (currentPos != objectRoot_) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // Return only the right dataType
    if ( _dataType != DATA_ALL )
      if ( ! (currentPos->dataType( dataType_ ) ) ) {
        currentPos = currentPos->next();
        continue;
      }

    // found a valid object
    pos_ = dynamic_cast<BaseObjectData* > (currentPos);
    break;
  }
}

ObjectIterator::ObjectIterator(BaseObjectData* pos, IteratorRestriction _restriction , DataType _data)
{
   restriction_ = _restriction;
   pos_         = pos;
   dataType_    = _data;
};


bool ObjectIterator::operator==( const ObjectIterator& _rhs) {
   std::cerr << "==" << std::endl;
   return ( _rhs.pos_ == pos_ );
}

bool ObjectIterator::operator!=( const ObjectIterator& _rhs) {
   return ( _rhs.pos_ != pos_ );
}

ObjectIterator& ObjectIterator::operator=( const ObjectIterator& _rhs) {
   pos_ = _rhs.pos_;
   return *this;
}


ObjectIterator::pointer ObjectIterator::operator->(){
   return pos_;
}

ObjectIterator& ObjectIterator::operator++() {

  // Convert our pointer to the basic one
  BaseObject* currentPos = dynamic_cast< BaseObject* >(pos_);

  // Get the next element in the tree
  currentPos = currentPos->next();

  while ( (currentPos != objectRoot_) ) {

    // Return only target objects if requested
    if ( (restriction_ == TARGET_OBJECTS) && (! currentPos->target() ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // Return only source objects if requested
    if ( (restriction_ == SOURCE_OBJECTS) && (! currentPos->source() ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // Return only the right dataType
    if ( ! (currentPos->dataType( dataType_ ) ) ) {
      currentPos = currentPos->next();
      continue;
    }

    // found a valid object
    pos_ = dynamic_cast<BaseObjectData* > (currentPos);
    return *this;
  }

  // No valid object found
  pos_ = 0;
  return *this;
}

ObjectIterator& ObjectIterator::operator--() {
   std::cerr << "TODO :--" << std::endl;
   return *this;
}

/** This operator returns a pointer to the current object the iterator
 * points to.
 *
 * @return Pointer to the current ObjectData
 */
BaseObjectData* ObjectIterator::operator*() {
   return pos_;
}

/// Return Iterator to Object End
ObjectIterator objects_end() {
   return ObjectIterator(0);
};


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

BaseObjectData* baseObjectData( BaseObject* _object ){
  if ( _object == 0 )
    return 0;
  
  return dynamic_cast< BaseObjectData* >(_object);
}

TriMesh* triMesh( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;
  
  if ( _object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMeshObject* object = dynamic_cast< TriMeshObject* >(_object);
    return object->mesh();
  } else
    return NULL;
}

PolyMesh* polyMesh( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;
  
  if ( _object->dataType(DATA_POLY_MESH) ) {
    PolyMeshObject* object = dynamic_cast< PolyMeshObject* >(_object);
    return object->mesh();
  } else
    return NULL;
}

TriMeshObject* triMeshObject( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;
  
  if ( ! _object->dataType(DATA_TRIANGLE_MESH) )
    return NULL;
  return dynamic_cast< TriMeshObject* >( _object );
}

PolyMeshObject* polyMeshObject( BaseObjectData* _object ) {
  if ( _object == 0 )
    return 0;
  
  if ( ! _object->dataType(DATA_POLY_MESH) )
    return NULL;
  return dynamic_cast< PolyMeshObject* >( _object );
}



// ===============================================================================
// Get the root of the object structure
// ===============================================================================
BaseObject* objectRoot() {
  return (objectRoot_);
}

}

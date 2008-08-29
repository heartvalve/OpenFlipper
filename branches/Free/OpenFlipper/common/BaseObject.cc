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
//  MyTypes
//
//=============================================================================

#define BASEOBJECT_C


//== INCLUDES =================================================================

#include "Types.hh"

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** This counter is used to provide unique object ids. Every time a object is created
 * the counter is incremented and a new objectId given to the object. DONT TOUCH IT.
 */
static int idGenerator = 1;


BaseObject::BaseObject(BaseObject* _parent) :
  id_(-1),
  persistentId_(-1),
  objectType_(DATA_NONE),
  target_(false),
  source_(false),
  visible_(true),
  parentItem_(_parent),
  name_("NONAME")
{
  id_ = idGenerator;
  ++ idGenerator;
}

BaseObject::~BaseObject() {

  deleteData();

}


// ===============================================================================
// Object Identification
// ===============================================================================

int BaseObject::id() {
  return id_;
}

int BaseObject::persistentId() {
  return persistentId_;
}

void BaseObject::persistentId( int _id ) {
  persistentId_ = _id;
}


// ===============================================================================
// Data
// ===============================================================================

void BaseObject::cleanup() {
  persistentId_ = -1;
  objectType_ = DATA_NONE;

  target_     = false;
  source_     = false;
  visible_    = true;
  name_       = "NONAME";
}

// ===============================================================================
// Data Type Handling
// ===============================================================================

bool BaseObject::dataType(DataType _type) {
  return (objectType_ & _type);
}

DataType BaseObject::dataType() {
  return BaseObject::objectType_;
}

void BaseObject::setDataType(DataType _type) {
  if ( objectType_ != DATA_NONE )
    std::cerr << "BaseObect : overwriting data type" << std::endl;
  objectType_ = _type;
}

// ===============================================================================
// Object Information
// ===============================================================================


QString BaseObject::getObjectinfo() {
  QString output;

  output += "Info for Object with id " + QString::number(id()) +"\n";
  output += "Object is : ";
  if ( target() )
    output += "target ";
  if ( source() )
    output += " source";

  if ( visible() )
    output += " visible";
  else
    output += " invisible";

  output +="\n";

  return output;
}

void BaseObject::printObjectInfo() {
  std::cout << getObjectinfo().toStdString();
}


// ===============================================================================
// source/target Handling
// ===============================================================================

bool BaseObject::target() {
  return target_;
}

void BaseObject::target(bool _target) {
  target_= _target;
}

bool BaseObject::source() {
  return source_;
}

void BaseObject::source(bool _source) {
  source_ = _source;
}

// ===============================================================================
// Object visualization
// ===============================================================================

bool BaseObject::visible() {
  return visible_;
}

// ===============================================================================
// ===============================================================================
// Tree Structure :
// ===============================================================================
// ===============================================================================

BaseObject* BaseObject::next() {
  // Visit child item of this node
  if ( childItems_.size() > 0 ) {
     return childItems_[0];
  }

  // No Child Item so visit the next child of the parentItem_
  if ( parentItem_ ) {

    BaseObject* parentPointer = parentItem_;
    BaseObject* thisPointer   = this;

    // while we are not at the root node
    while ( parentPointer ) {

      // If there is an unvisited child of the parent, return this one
      if ( parentPointer->childCount() > ( thisPointer->row() + 1) ) {
        return parentPointer->childItems_[ thisPointer->row() + 1 ];
      }

      // Go to the next level
      thisPointer   = parentPointer;
      parentPointer = parentPointer->parentItem_;

    }

    return thisPointer;
  }

  return this;

}

int BaseObject::level() {
  int level = 0;
  BaseObject* current = this;

  // Go up and count the levels to the root node
  while ( current->parent() != 0 ) {
    level++;
    current = current->parent();
  }

  return level;
}

// ===============================================================================
// Parent
// ===============================================================================

int BaseObject::row() const
{
    if (parentItem_)
        return parentItem_->childItems_.indexOf(const_cast<BaseObject*>(this));

    return 0;
}

BaseObject* BaseObject::parent()
{
  return parentItem_;
}

/// Set the parent pointer
void BaseObject::setParent(BaseObject* _parent) {
  parentItem_ = _parent;
}


// ===============================================================================
// Children
// ===============================================================================

void BaseObject::appendChild(BaseObject *item)
{
    childItems_.append(item);
}

BaseObject *BaseObject::child(int row)
{
    return childItems_.value(row);
}

int BaseObject::childCount() const
{
    return childItems_.count();
}

BaseObject* BaseObject::childExists(int _objectId) {

  // Check if this object has the requested id
  if ( id_ == _objectId )
    return this;

  // search in children
  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    BaseObject* tmp = childItems_[i]->childExists(_objectId);
    if ( tmp != 0)
      return tmp;
  }

  return 0;
}

BaseObject* BaseObject::childExists(QString _name) {

  // Check if this object has the requested id
  if ( name() == _name )
    return this;

  // search in children
  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    BaseObject* tmp = childItems_[i]->childExists(_name);
    if ( tmp != 0)
      return tmp;
  }

  return 0;
}

void BaseObject::removeChild( BaseObject* _item ) {

  bool found = false;
  QList<BaseObject*>::iterator i;
  for (i = childItems_.begin(); i != childItems_.end(); ++i) {
     if ( *i == _item ) {
        found = true;
        break;
     }
  }

  if ( !found ) {
    std::cerr << "Illegal remove request" << std::endl;
    return;
  }

  childItems_.erase(i);
}

QList< BaseObject* > BaseObject::getLeafs() {

  QList< BaseObject* > items;

  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    items = items + childItems_[i]->getLeafs();
  }

  // If we are a leave...
  if ( childCount() == 0 )
    items.push_back(this);

  return items;
}

void BaseObject::deleteSubtree() {

  // call function for all children of this node
  for ( int i = 0 ; i < childItems_.size(); ++i) {

    // remove the subtree recursively
    childItems_[i]->deleteSubtree();

    // delete child
    delete childItems_[i];
  }

  // clear the array
  childItems_.clear();
}


// TreeItem* TreeItem::inConsistent() {
//
//   BaseObjectData* object;
//
//   // If id is -1 this is a group or rootitem so dont stop here
//   if ( (objectId_ != -1) && !PluginFunctions::get_object(objectId_,object)  )
//     return this;
//
//   for ( int i = 0 ; i < childItems.size(); ++i ) {
//     TreeItem* tmp = childItems[i]->inConsistent();
//     if ( tmp != 0)
//       return tmp;
//   }
//
//   return 0;
// }

// ===============================================================================
// Grouping
// ===============================================================================
int BaseObject::group() {
  // Skip root node
  if ( parent() == 0 )
    return -1;

  // Dont count root node as a group
  if ( parent()->parent() == 0 )
    return -1;

  // Only consider groups
  if ( !parent()->dataType(DATA_GROUP) )
    return -1;

  // Get the group id
  return ( parent()->id() );

}

bool BaseObject::isGroup() {
//   return ( (childItems_.size() > 0) || dataType(DATA_GROUP) ) ;
  return ( dataType(DATA_GROUP) ) ;
};


bool BaseObject::isInGroup( int _id ) {
  BaseObject* current = this;

  // Go up and check for the group id
  do {

    // Check if we found the id
    if ( current->id() == _id )
      return true;

    // Move on to parent object
    current = current->parent();
  } while ( current != 0 );

  return false;
}

bool BaseObject::isInGroup( QString _name ) {
  BaseObject* current = this;

  // Go up and check for the group name
  do {

    // Check if this object is a group and if it has the serach name
    if ( current->dataType( DATA_GROUP ) && (current->name() == _name) )
      return true;

    // Move on to parent object
    current = current->parent();

  } while ( current != 0 );

  return false;
}

std::vector< int > BaseObject::getGroupIds() {
  std::vector< int > groups;

  BaseObject* current = this;

  // Go up and collect all groups in the given order
  do {

    // collect only groups
    if ( current->dataType( DATA_GROUP ) )
      // Skip the root Object
      if ( current->parent() != 0 )
        groups.push_back( current->id() );

    // Move on to parent object
    current = current->parent();
  } while ( current != 0 );

  return groups;
}

QStringList BaseObject::getGroupNames() {
  QStringList groups;

  BaseObject* current = this;

  // Go up and collect all groups in the given order
  do {

    // collect only groups
    if ( current->dataType( DATA_GROUP ) )
      // Skip the root Object
      if ( current->parent() != 0 )
        groups.push_back( current->name() );

    // Move on to parent object
    current = current->parent();
  } while ( current != 0 );

  return groups;
}

// ===============================================================================
// Name
// ===============================================================================
QString BaseObject::name() {
  return name_;
}

void BaseObject::setName(QString _name ) {
  name_ = _name;
}

// ===============================================================================
// Content
// ===============================================================================
void BaseObject::update() {
}

void BaseObject::dumpTree() {

  // Add spaces to visualize level
  for ( int i = 0 ; i < level() ; ++i  )
    std::cerr << "   ";

  std::cerr << "Node ";
  std::cerr << std::string(name().toAscii());

  std::cerr << " with id : ";
  std::cerr << id();

  // Write the type of this Object
  std::cerr << " and type ";
  if ( dataType() == DATA_GROUP )
    std::cerr << "Group" << std::endl;
  else if ( dataType() == DATA_TRIANGLE_MESH )
    std::cerr << "Triangle Mesh" << std::endl;
  else if ( dataType() == DATA_POLY_MESH )
    std::cerr << "Poly Mesh" << std::endl;
  else if ( dataType() == DATA_POLY_LINE )
    std::cerr << "Poly Line" << std::endl;
  else if ( dataType() == DATA_POINTS )
    std::cerr << "Points" << std::endl;
  else if ( dataType() == DATA_VOLUME )
    std::cerr << "Volume" << std::endl;
  else
    std::cerr << int( dataType() ) << std::endl;

  // call function for all children of this node
  for ( int i = 0 ; i < childItems_.size(); ++i)
    childItems_[i]->dumpTree();

}


// ===============================================================================
// per Object data functions
// ===============================================================================

void
BaseObject::
setObjectData( QString _dataName , PerObjectData* _data ) {
  dataMap_.insert( _dataName, _data );
}

void
BaseObject::
clearObjectData( QString _dataName ) {
  if (dataMap_.contains(_dataName))
    dataMap_.remove(_dataName);
}


bool
BaseObject::
hasObjectData( QString _dataName )
{
  return dataMap_.contains(_dataName);
}


PerObjectData*
BaseObject::
objectData( QString _dataName ) {
  if (dataMap_.contains(_dataName))
    return dataMap_.value(_dataName);
  else
    return 0;
}

void
BaseObject::
deleteData() {

  QMapIterator<QString, PerObjectData* > i(dataMap_);
  while (i.hasNext()) {
      i.next();
      delete i.value();
  }

  dataMap_.clear();

}





//=============================================================================

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

#define BASEOBJECT_C


//== INCLUDES =================================================================

#include "BaseObjectCore.hh"
#include "Types.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//== TYPEDEFS =================================================================

//== Variables =================================================================

static ObjectManager objectManager_;

//== CLASS DEFINITION =========================================================

/** This counter is used to provide unique object ids. Every time a object is created
 * the counter is incremented and a new objectId given to the object. DONT TOUCH IT.
 */
static int idGenerator = 1;

BaseObject::BaseObject(const BaseObject& _object) :
  QObject() 
{
  id_           = idGenerator;
  ++idGenerator;
  persistentId_ = _object.persistentId_;
  objectType_   = _object.objectType_;
  flags_ = _object.flags_;
  visible_ = _object.visible_;
  parentItem_ = 0;
  childItems_.clear();
  name_ = "Copy of " + _object.name_;

  ///@todo Maybe copy per Object Data
  dataMap_.clear();
  
  // If the pointer is 0 then we are creating the objectroot
  if ( PluginFunctions::objectRoot() ) {
    setParent(PluginFunctions::objectRoot());
    PluginFunctions::objectRoot()->appendChild(this);
  }
  
  objectManager_.objectCreated(id());
}

BaseObject::BaseObject(BaseObject* _parent) :
  QObject() ,
  id_(-1),
  persistentId_(-1),
  objectType_(DATA_UNKNOWN),
  flags_(),
  visible_(true),
  parentItem_(_parent),
  name_("NONAME")
{
  id_ = idGenerator;
  ++ idGenerator;
  
  if ( PluginFunctions::objectRoot() ) {
    PluginFunctions::objectRoot()->dumpTree();
  }
  
  // If the pointer is 0 then something went wrong
  if ( _parent ) {
    
    _parent->appendChild(this);
    
  } else {
    
    if ( PluginFunctions::objectRoot() ) {
      setParent(PluginFunctions::objectRoot());
      PluginFunctions::objectRoot()->appendChild(this);
    }
    
  }
  
  objectManager_.objectCreated(id());
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
  objectType_ = DATA_UNKNOWN;

  flags_.clear();

  visible_    = true;
  name_       = "NONAME";
}

// ===============================================================================
// Data Type Handling
// ===============================================================================

bool BaseObject::dataType(DataType _type) {
  if ( _type == DATA_ALL ) {
    return true;
  }

  return ( objectType_ & _type);
}

DataType BaseObject::dataType() {
  return BaseObject::objectType_;
}

void BaseObject::setDataType(DataType _type) {
  if ( objectType_ != DATA_UNKNOWN )
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
// flag Handling
// ===============================================================================

bool BaseObject::target() {
  return flag("target");
}

void BaseObject::target(bool _target) {
  setFlag("target", _target);
}

bool BaseObject::source() {
  return flag("source");
}

void BaseObject::source(bool _source) {
  setFlag("source", _source);
}

bool BaseObject::flag(QString _flag)
{
  return flags_.contains(_flag);
}

void BaseObject::setFlag(QString _flag, bool _set)
{
  bool emitted = false;
  
  if (flags_.contains(_flag))
  {
    if (!_set) {
      flags_.removeAll(_flag);
      emit objectSelectionChanged(id());
      emitted = true;
    }
  }
  else
  {
    if (_set) {
      flags_ << _flag;
      emit objectSelectionChanged(id());
      emitted = true;
    }
  }
  
  //always emit if its a group
  if ( !emitted && isGroup() )
    emit objectSelectionChanged(id());
}

QStringList BaseObject::flags()
{
  return flags_;
}

// ===============================================================================
// Object visualization
// ===============================================================================

bool BaseObject::visible() {
  return visible_;
}

void BaseObject::visible(bool _visible) {
  // Only do something if this is really a change
  if (  visible_ != _visible ) {
    visible_ = _visible;
    
    emit visibilityChanged( id() );
    
  } else {
    
    //always emit if its a group
    if ( isGroup() )
      emit visibilityChanged( id() );
  }
}

// ===============================================================================
// ===============================================================================
// Tree Structure :
// ===============================================================================
// ===============================================================================

BaseObject* BaseObject::last() {
  //indexOf

//   // Visit child item of this node
//   if ( childItems_.size() > 0 ) {
//      return childItems_[0];
//   }
//
//   // No Child Item so visit the next child of the parentItem_
//   if ( parentItem_ ) {
//
//     BaseObject* parentPointer = parentItem_;
//     BaseObject* thisPointer   = this;
//
//     // while we are not at the root node
//     while ( parentPointer ) {
//
//       // If there is an unvisited child of the parent, return this one
//       if ( parentPointer->childCount() > ( thisPointer->row() + 1) ) {
//         return parentPointer->childItems_[ thisPointer->row() + 1 ];
//       }
//
//       // Go to the next level
//       thisPointer   = parentPointer;
//       parentPointer = parentPointer->parentItem_;
//
//     }
//
//     return thisPointer;
//   }
//
//   return this;
  std::cerr << "Last not implemented yet! " << std::endl;
  return 0;

}

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

// ===============================================================================
// ===============================================================================

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
  if ( !childItems_.contains(item) )
    childItems_.append(item);
  else 
    std::cerr << "Warning! Trying to append a child twice! Remove the append calls from your File plugin!" << std::endl;
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
//   if ( (objectId_ != -1) && !PluginFunctions::getObject(objectId_,object)  )
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
  std::cerr << " and type " << typeName(dataType()).toStdString()  <<  std::endl;

  // call function for all children of this node
  for ( int i = 0 ; i < childItems_.size(); ++i)
    childItems_[i]->dumpTree();

}

BaseObject* BaseObject::copy() {
  std::cerr << "Copy not supported by this Object" << std::endl;
  return  0;
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



ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
  
}
    
void ObjectManager::objectCreated(int _objectId) 
{
  emit newObject(_objectId);
}

ObjectManager* getObjectManager() {
  return &objectManager_;
}




//=============================================================================

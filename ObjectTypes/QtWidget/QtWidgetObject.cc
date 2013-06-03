 
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
//  QtWidget Object
//
//=============================================================================

//== INCLUDES =================================================================
#include "QtWidgetObject.hh"
#include "QtWidget.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================
QtWidgetObject::QtWidgetObject( ) :
  BaseObjectData(),
  widgetNode_(NULL)
{
  setDataType(DATA_QT_WIDGET);
  setTypeIcon(DATA_QT_WIDGET,"QtWidgetType.png");
  init(NULL);
}

/**
 * Copy Constructor - generates a copy of the given object
 */
QtWidgetObject::QtWidgetObject(const QtWidgetObject & _object) :
  BaseObjectData(_object),
  widgetNode_(NULL)
{
  init(_object.widget());
  setName( name() );
}

/** Destructor for Light Objects. The destructor deletes the light and all
*  Scenegraph nodes associated with the Light or the object.
*/
QtWidgetObject::~QtWidgetObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the light below and
  // they do something with the light in the destructor of their
  // perObjectData.
  deleteData();

  // Move children to parent
  BaseNode* parent = static_cast<BaseNode*>(widgetNode_)->parent();

  // First, collect all children as the iterator will get invalid if we delete while iterating!
  std::vector< BaseNode*> children;
  for (BaseNode::ChildIter cIt=widgetNode_->childrenBegin(); cIt!=widgetNode_->childrenEnd(); ++cIt)
    children.push_back( (*cIt) );

  // Move the children
  for (unsigned int i = 0 ; i < children.size(); ++i )
    children[i]->set_parent(parent);

  // Delete the scenegraph node
  delete widgetNode_;
}

/** Cleanup Function for Light Objects. Deletes the contents of the whole object and
* calls QtWidgetObject::init afterwards.
*/
void QtWidgetObject::cleanup() {

  BaseObjectData::cleanup();

  widgetNode_   = NULL;

  setDataType(DATA_QT_WIDGET);
  setTypeIcon(DATA_QT_WIDGET,"QtWidgetType.png");

}

/**
 * Generate a copy
 */
BaseObject* QtWidgetObject::copy() {
    QtWidgetObject* object = new QtWidgetObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initializes the light object. It creates the scenegraph nodes.
*/
void QtWidgetObject::init(QWidget* _widget) {

  widgetNode_ = new QtWidgetNode( _widget ,materialNode() , "QtWidgetNode");
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void QtWidgetObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("Qt WidgetNode "     + _name.toUtf8() );
  widgetNode_->name( nodename );
}

// ===============================================================================
// Visualization
// ===============================================================================

QtWidgetNode* QtWidgetObject::qtWidgetNode() {
  return widgetNode_;
}


bool QtWidgetObject::hasNode(BaseNode* _node) {
 if ( BaseObjectData::hasNode(_node) )
   return true;

 if ( _node == widgetNode_ )
   return true;

 return false;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString QtWidgetObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_QT_WIDGET ) )
    output += "Qt Accessible Name: ";
    output += widgetNode_->widget()->accessibleName();
    output += "\n";
  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the lightNode of the object has been picked.
*
* @param _node_idx Index of the picked QtWidget node
* @return bool if the widgetNode of this object is the picking target.
*/
bool QtWidgetObject::picked( uint _node_idx ) {
  return ( _node_idx == widgetNode_->id() );
}

void QtWidgetObject::enablePicking( bool _enable ) {
  widgetNode_->enablePicking( _enable );
}

bool QtWidgetObject::pickingEnabled() {
  return widgetNode_->pickingEnabled();
}

void QtWidgetObject::show() {
  if ( !visible_ ) {
    BaseObjectData::show();
    visible_ = true;

    static_cast<BaseNode*>(widgetNode_)->show();

    emit visibilityChanged( id() );
  }
}

void QtWidgetObject::hide() {
  if ( visible_ ) {
    BaseObjectData::hide();
    visible_ = false;
    static_cast<BaseNode*>(widgetNode_)->hide();

    emit visibilityChanged( id() );
  }
}

void QtWidgetObject::visible(bool _visible) {
  if ( _visible )
    show();
  else
    hide();
}

bool QtWidgetObject::visible() {
  return visible_;
}


//=============================================================================


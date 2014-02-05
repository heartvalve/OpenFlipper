/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
//  Plane Object
//
//=============================================================================

#define PLANEOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include "Coordsys.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for Coordinate system Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The Coordinate system is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You don't need to create an object of this type manually. Use
*  the functions from LoadSaveInterface ( addEmpty() )
*/
CoordsysObject::CoordsysObject( ) :
  BaseObjectData( ),
  coordsysNode_(NULL)
{
  setDataType(DATA_COORDSYS);
  init();
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
CoordsysObject::CoordsysObject(const CoordsysObject & _object) :
  BaseObjectData(_object)
{

    init(_object.coordsysNode_);

    setName( name() );
}

/** Destructor for Coordsys Objects. The destructor deletes the Line and all
*  Scenegraph nodes associated with the Coordinate System or the object.
*/
CoordsysObject::~CoordsysObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the coordsys below and
  // they do something with the coordsys in the destructor of their
  // perObjectData.
  deleteData();

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  coordsysNode_    = NULL;
}

/** Cleanup Function for Coordsys Objects. Deletes the contents of the whole object and
* calls CoordsysObject::init afterwards.
*/
void CoordsysObject::cleanup() {

  BaseObjectData::cleanup();

  coordsysNode_   = NULL;

  setDataType(DATA_COORDSYS);
  setTypeIcon(DATA_COORDSYS,"CoordsysType.png");

  init();

}

/**
 * Generate a copy
 */
BaseObject* CoordsysObject::copy() {
  CoordsysObject* object = new CoordsysObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the coordsys object. It creates the scenegraph nodes.
*/
void CoordsysObject::init(CoordsysNode* _coordsys) {

  if ( materialNode() == NULL)
    std::cerr << "Error when creating Coordsys Object! materialNode is NULL!" << std::endl;

  coordsysNode_ = new CoordsysNode( materialNode() , "NEW CoordsysNode");

  if (_coordsys){
    coordsysNode_->position( _coordsys->position() );
    coordsysNode_->size( _coordsys->size() );
  } else {
    coordsysNode_->position( ACG::Vec3d(0.0, 0.0, 0.0) );
    coordsysNode_->size( 1.0 );
  }
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void CoordsysObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("CoordsysNode for Coordinate system "     + _name.toUtf8() );
  coordsysNode_->name( nodename );
}

// ===============================================================================
// Visualization
// ===============================================================================

CoordsysNode* CoordsysObject::coordsysNode() {
  return coordsysNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString CoordsysObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_COORDSYS ) )
    output += "Object Contains a coordinate system : ";

  ACG::Vec3d pos  = coordsysNode_->position();
  double     size = coordsysNode_->size();

  output += " Position ( " + QString::number(pos[0]) + ", " + QString::number(pos[1]) + ", " + QString::number(pos[2]) + ")";
  output += " Size ( "+ QString::number(size) + ")";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the coordsysNode of the object has been picked.
*
* @param _node_idx Index of the picked coordsys node
* @return bool if the coordsys node of this object is the picking target.
*/
bool CoordsysObject::picked( uint _node_idx ) {
  return ( _node_idx == coordsysNode_->id() );
}

void CoordsysObject::enablePicking( bool _enable ) {
  coordsysNode_->enablePicking( _enable );
}

bool CoordsysObject::pickingEnabled() {
  return coordsysNode_->pickingEnabled();
}

//=============================================================================


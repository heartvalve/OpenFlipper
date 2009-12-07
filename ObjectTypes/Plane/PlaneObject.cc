//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define PLANEOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include "Plane.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for Plane Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The plane is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addPlane instead. ( see Types.hh::DataType )
*/
PlaneObject::PlaneObject( SeparatorNode* _rootNode ) :
  BaseObjectData(_rootNode ),
  planeNode_(NULL)
{
  setDataType(DATA_PLANE);
  setTypeIcon(DATA_PLANE,"PlaneType.png");
  init();
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
PlaneObject::PlaneObject(const PlaneObject & _object) :
  BaseObjectData(_object)
{

    init(_object.planeNode_);

    setName( name() );
}

/** Destructor for Plane Objects. The destructor deletes the Line and all
*  Scenegraph nodes associated with the Plane or the object.
*/
PlaneObject::~PlaneObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the plane below and
  // they do something with the plane in the destructor of their
  // perObjectData.
  deleteData();

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  planeNode_    = NULL;
}

/** Cleanup Function for Plane Objects. Deletes the contents of the whole object and
* calls PlaneObject::init afterwards.
*/
void PlaneObject::cleanup() {

  BaseObjectData::cleanup();

  planeNode_   = NULL;

  setDataType(DATA_PLANE);
  setTypeIcon(DATA_PLANE,"PlaneType.png");

  init();

}

/**
 * Generate a copy
 */
BaseObject* PlaneObject::copy() {
    PlaneObject* object = new PlaneObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the plane object. It creates the scenegraph nodes.
*/
void PlaneObject::init(PlaneNode* _plane) {

  if ( materialNode() == NULL)
    std::cerr << "Error when creating Plane Object! materialNode is NULL!" << std::endl;

  planeNode_ = new PlaneNode( materialNode() , "NEW PlaneNode");

  if (_plane){
    planeNode_->setPosition( _plane->position(), _plane->normal() );
    planeNode_->setSize( _plane->xDirection().norm(), _plane->yDirection().norm() );
  } else {
    planeNode_->setPosition( ACG::Vec3f(0.0, 0.0, 0.0), ACG::Vec3f(0.0, 1.0, 0.0) );
    planeNode_->setSize( 5.0, 5.0 );
  }
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void PlaneObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("PlaneNode for Plane "     + _name.toUtf8() );
  planeNode_->name( nodename );
}

// ===============================================================================
// Visualization
// ===============================================================================

PlaneNode* PlaneObject::planeNode() {
  return planeNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString PlaneObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_PLANE ) )
    output += "Object Contains Plane : ";

  ACG::Vec3f pos = planeNode_->position();
  ACG::Vec3f nor = planeNode_->normal();

  output += " Position ( " + QString::number(pos[0]) + ", " + QString::number(pos[1]) + ", " + QString::number(pos[2]) + ")";
  output += " Normal ( " + QString::number(nor[0]) + ", " + QString::number(nor[1]) + ", " + QString::number(nor[2]) + ")";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the planeNode of the object has been picked.
*
* @param _node_idx Index of the picked plane node
* @return bool if the planeNode of this object is the picking target.
*/
bool PlaneObject::picked( uint _node_idx ) {
  return ( _node_idx == planeNode_->id() );
}

void PlaneObject::enablePicking( bool _enable ) {
  planeNode_->enablePicking( _enable );
}

bool PlaneObject::pickingEnabled() {
  return planeNode_->pickingEnabled();
}

//=============================================================================


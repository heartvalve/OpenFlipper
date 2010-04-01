//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define LIGHTOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include "Light.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for Light Objects. 
*  You dont need to create an object of this type manually. 
*/
LightObject::LightObject( ) :
  BaseObjectData( ),
  lightNode_(NULL)
{
  setDataType(DATA_LIGHT);
  setTypeIcon(DATA_LIGHT,"LightType.png");
  init();
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
LightObject::LightObject(const LightObject & _object) :
  BaseObjectData(_object)
{

    init(_object.lightNode_);

    setName( name() );
}

/** Destructor for Light Objects. The destructor deletes the light and all
*  Scenegraph nodes associated with the Light or the object.
*/
LightObject::~LightObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the light below and
  // they do something with the light in the destructor of their
  // perObjectData.
  deleteData();

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  lightNode_    = NULL;
}

/** Cleanup Function for Light Objects. Deletes the contents of the whole object and
* calls LightObject::init afterwards.
*/
void LightObject::cleanup() {

  BaseObjectData::cleanup();

  lightNode_   = NULL;

  setDataType(DATA_LIGHT);
  setTypeIcon(DATA_LIGHT,"LightType.png");

  init();

}

/**
 * Generate a copy
 */
BaseObject* LightObject::copy() {
    LightObject* object = new LightObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the light object. It creates the scenegraph nodes.
*/
void LightObject::init(LightNode* _light) {
///\TODO Add Light Node here
  /*
  planeNode_ = new LightNode( materialNode() , "NEW LightNode");

  if (_plane){
    planeNode_->setPosition( _plane->position(), _plane->normal() );
    planeNode_->setSize( _plane->xDirection().norm(), _plane->yDirection().norm() );
  } else {
    planeNode_->setPosition( ACG::Vec3f(0.0, 0.0, 0.0), ACG::Vec3f(0.0, 1.0, 0.0) );
    planeNode_->setSize( 5.0, 5.0 );
  }
  */
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void LightObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("LightNode for Light "     + _name.toUtf8() );
  lightNode_->name( nodename );
}

// ===============================================================================
// Visualization
// ===============================================================================

LightNode* LightObject::lightNode() {
  return lightNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString LightObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_LIGHT ) )
    output += "Object Contains Light : ";

  ///\TODO Output info
//   ACG::Vec3f pos = lightNode_->position();
//   ACG::Vec3f nor = lightNode_->normal();

//   output += " Position ( " + QString::number(pos[0]) + ", " + QString::number(pos[1]) + ", " + QString::number(pos[2]) + ")";
//   output += " Normal ( " + QString::number(nor[0]) + ", " + QString::number(nor[1]) + ", " + QString::number(nor[2]) + ")";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the lightNode of the object has been picked.
*
* @param _node_idx Index of the picked light node
* @return bool if the lightNode of this object is the picking target.
*/
bool LightObject::picked( uint _node_idx ) {
  return ( _node_idx == lightNode_->id() );
}

void LightObject::enablePicking( bool _enable ) {
  lightNode_->enablePicking( _enable );
}

bool LightObject::pickingEnabled() {
  return lightNode_->pickingEnabled();
}

//=============================================================================


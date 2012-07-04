//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define POLYLINEOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include "PolyLine.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for PolyLine Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The line is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addPolyLine instead. ( see Types.hh::DataType )
*/
PolyLineObject::PolyLineObject() :
  BaseObjectData( ),
  line_(NULL),
  lineNode_(NULL)
{
  setDataType(DATA_POLY_LINE);
  init();
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
PolyLineObject::PolyLineObject(const PolyLineObject & _object) :
  BaseObjectData(_object)
{

    init(_object.line_);

    setName( name() );
}

/** Destructor for PolyLine Objects. The destructor deletes the Line and all
*  Scenegraph nodes associated with the Line or the object.
*/
PolyLineObject::~PolyLineObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the line below and
  // they do something with the polyline in the destructor of their
  // perObjectData.
  deleteData();

  // Delete the Mesh only, if this object contains a mesh
  if ( line_ != NULL)  {
    delete line_;
    line_ = NULL;
  } else {
    std::cerr << "Destructor error : Line already deleted" << std::endl;
  }

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  lineNode_    = NULL;
}

/** Cleanup Function for Line Objects. Deletes the contents of the whole object and
* calls PolyLineObject::init afterwards.
*/
void PolyLineObject::cleanup() {
  // Delete the Line only, if this object contains a line
  if ( line_ != NULL)  {
    delete line_;
    line_ = NULL;
  } else {
    std::cerr << "Cleanup error : Line already deleted" << std::endl;
  }

  BaseObjectData::cleanup();

  lineNode_   = NULL;

  setDataType(DATA_POLY_LINE);

  init();

}

/**
 * Generate a copy
 */
BaseObject* PolyLineObject::copy() {
    PolyLineObject* object = new PolyLineObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the line object. It creates the scenegraph nodes,
*  the line.
*/
void PolyLineObject::init(PolyLine* _line) {

  if (_line == 0)
    line_ = new PolyLine();
  else
    line_ = new PolyLine(*_line);

  // request default properties
  line()->request_vertex_selections();
  line()->request_edge_selections();
  line()->request_vertex_vhandles();
  line()->request_vertex_ehandles();
  line()->request_vertex_fhandles();


  if ( materialNode() == NULL)
    std::cerr << "Error when creating Line Object! materialNode is NULL!" << std::endl;

  lineNode_    = new ACG::SceneGraph::PolyLineNodeT< PolyLine >(*line_, materialNode() , "NEW LineNode");
  
  // Set default material of the polyLine
  materialNode()->set_random_color();
  materialNode()->set_line_width(3.0);
  materialNode()->set_point_size(8.0);
  materialNode()->set_round_points(true);

}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void PolyLineObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("LineNode for Line "     + _name.toUtf8() );
  lineNode_->name( nodename );
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects line.
* @return Pointer to the line
*/
PolyLine* PolyLineObject::line() {
  return line_;
}

// ===============================================================================
// Visualization
// ===============================================================================

ACG::SceneGraph::PolyLineNodeT< PolyLine >* PolyLineObject::lineNode() {
  return lineNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString PolyLineObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_POLY_LINE ) )
    output += "Object Contains PolyLine : ";

  output += QString::number( line()->n_vertices() ) + " vertices, ";
  output += QString::number( line()->n_edges() ) += " edges and is ";
  if ( line()->is_closed() )
    output += "closed.\n";
  else
    output += "open.\n";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the polyLineNode of the object has been picked.
*
* @param _node_idx Index of the picked line node
* @return bool if the lineNode of this object is the picking target.
*/
bool PolyLineObject::picked( uint _node_idx ) {
  return ( _node_idx == lineNode_->id() );
}

void PolyLineObject::enablePicking( bool _enable ) {
  lineNode_->enablePicking( _enable );
}

bool PolyLineObject::pickingEnabled() {
  return lineNode_->pickingEnabled();
}

//=============================================================================


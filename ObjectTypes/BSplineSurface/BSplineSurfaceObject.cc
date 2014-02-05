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
// BSplineSurface object type - Implementation
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#define BSPLINESURFACEOBJECT_C


//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/Types.hh>
#include "BSplineSurface.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for BSpline Surface Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The spline surface is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addBSplineSurface instead. ( see Types.hh::DataType )
*/
BSplineSurfaceObject::BSplineSurfaceObject() :
  BaseObjectData(),
  splineSurface_(NULL),
  splineSurfaceNode_(NULL),
  shaderNode_(0),
  textureNode_(0)

{
  setDataType(DATA_BSPLINE_SURFACE);
  init();
}

//-----------------------------------------------------------------------------

/**
 * Copy Constructor - generates a copy of the given object
 */
BSplineSurfaceObject::BSplineSurfaceObject(const BSplineSurfaceObject & _object) :
    BaseObjectData(_object)
{
  init(_object.splineSurface_);
  setName( name() );
}

//-----------------------------------------------------------------------------

/** Destructor for BSpline Surface Objects. The destructor deletes the spline surface and all
*  Scenegraph nodes associated with the surface or the object.
*/
BSplineSurfaceObject::~BSplineSurfaceObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the curve below and
  // they do something with the splineCurve in the destructor of their
  // perObjectData.
  deleteData();

  // Delete the Mesh only, if this object contains a mesh
  if ( splineSurface_ != NULL)  {
    delete splineSurface_;
    splineSurface_ = NULL;
  } else {
    std::cerr << "Destructor error : Spline surface already deleted" << std::endl;
  }

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  splineSurfaceNode_ = NULL;
  textureNode_       = 0;
  shaderNode_        = 0;
}

//-----------------------------------------------------------------------------

/** Cleanup Function for spline surface Objects. Deletes the contents of the whole object and
* calls BSplineSurfaceObject::init afterwards.
*/
void BSplineSurfaceObject::cleanup() {
  // Delete the spline surface only, if this object contains a surface
  if ( splineSurface_ != NULL)  {
    delete splineSurface_;
    splineSurface_ = NULL;
  } else {
    std::cerr << "Cleanup error : Spline surface already deleted" << std::endl;
  }

  BaseObjectData::cleanup();

  splineSurfaceNode_ = NULL;
  textureNode_       = 0;
  shaderNode_        = 0;

  setDataType(DATA_BSPLINE_SURFACE);

  init();
}

//-----------------------------------------------------------------------------

/**
 * Generate a copy
 */
BaseObject* BSplineSurfaceObject::copy()
{
  BSplineSurfaceObject* object = new BSplineSurfaceObject(*this);
  return dynamic_cast< BaseObject* >(object);
}

//-----------------------------------------------------------------------------

/** This function initalizes the spline object. It creates the scenegraph nodes,
 *  the spline.
 */
void BSplineSurfaceObject::init(BSplineSurface* _surface) {

  if (_surface == 0)
    splineSurface_ = new BSplineSurface();
  else
    splineSurface_ = new BSplineSurface(*_surface);

  // request default properties
  splineSurface()->request_controlpoint_selections();
  splineSurface()->request_edge_selections();
  // request selection property for both knotvectors
  splineSurface()->get_knotvector_m_ref()->request_selections();
  splineSurface()->get_knotvector_n_ref()->request_selections();

  if ( materialNode() == NULL)
    std::cerr << "Error when creating BSplineSurface Object! materialNode is NULL!" << std::endl;

  textureNode_       = new ACG::SceneGraph::EnvMapNode(materialNode(),"NEW TextureNode for ", true, GL_LINEAR_MIPMAP_LINEAR );
  shaderNode_        = new ACG::SceneGraph::ShaderNode(textureNode_ , "NEW ShaderNode for ");
  splineSurfaceNode_ = new ACG::SceneGraph::BSplineSurfaceNodeT< BSplineSurface >(*splineSurface_, shaderNode_ , "NEW BSplineSurfaceNode");

//   shaderNode_        = new ACG::SceneGraph::ShaderNode(materialNode() , "NEW ShaderNode for ");
//   splineSurfaceNode_ = new ACG::SceneGraph::BSplineSurfaceNodeT< BSplineSurface >(*splineSurface_, shaderNode_ , "NEW BSplineSurfaceNode");

  QString shaderDir = OpenFlipper::Options::shaderDirStr() +
                      OpenFlipper::Options::dirSeparator();

  std::string shaderDirectory = std::string( shaderDir.toUtf8() );
  shaderNode_->setShaderDir( shaderDirectory );
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void BSplineSurfaceObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("BSplineSurfaceNode for BSpline surface " + _name.toUtf8() );
  splineSurfaceNode_->name( nodename );

  nodename = std::string("TextureNode for BSpline surface " + _name.toUtf8() );
  textureNode_->name( nodename );

  nodename = std::string("ShaderNode for BSpline surface " + _name.toUtf8() );
  shaderNode_->name( nodename );
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects spline surface.
* @return Pointer to the spline surface
*/
BSplineSurface* BSplineSurfaceObject::splineSurface() {
  return splineSurface_;
}

//-----------------------------------------------------------------------------

/// Update the whole Object (Selection,Topology,...)
void 
BSplineSurfaceObject::
update(UpdateType _type)
{
  if ( _type.contains(UPDATE_ALL))
  {
    // mark textures as invalid
    splineSurfaceNode()->cpSelectionTextureValid(false);
    splineSurfaceNode()->knotSelectionTextureValid(false);
  }
  else 
  {
    if (_type.contains(UPDATE_GEOMETRY) || _type.contains(UPDATE_SELECTION)) 
    {
      // mark textures as invalid
      splineSurfaceNode()->cpSelectionTextureValid(false);
      splineSurfaceNode()->knotSelectionTextureValid(false);
    }
    else if (_type.contains(UPDATE_SELECTION_VERTICES))
      splineSurfaceNode()->cpSelectionTextureValid(false);
    else if (_type.contains(UPDATE_SELECTION_KNOTS))
      splineSurfaceNode()->knotSelectionTextureValid(false);
  }
}

// ===============================================================================
// Visualization
// ===============================================================================

/** Returns a pointer to the bspline surface node
 * @return Pointer to the bspline surface node
 */
ACG::SceneGraph::BSplineSurfaceNodeT< BSplineSurface >* BSplineSurfaceObject::splineSurfaceNode() {
  return splineSurfaceNode_;
}


/** Returns a pointer to the shader node
 * @return Pointer to the shader node
 */
ACG::SceneGraph::ShaderNode * BSplineSurfaceObject::shaderNode() {
  return shaderNode_;
}


/** Returns a pointer to the texture node
 * @return Pointer to the texture node
 */
ACG::SceneGraph::EnvMapNode* BSplineSurfaceObject::textureNode() {
  return textureNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString BSplineSurfaceObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_BSPLINE_SURFACE ) )
    output += "Object Contains BSpline Surface : ";
  else{
    output += "Error: Object Contains NO BSpline Surface!";
    output += "========================================================================\n";
    return output;
  }

  output += QString::number( splineSurface()->n_control_points_m() ) + " control points in m direction, ";
  output += QString::number( splineSurface()->n_control_points_n() ) + " control points in n direction, ";
  output += QString::number( splineSurface()->n_knots_m() ) += " knots in m direction";
  output += QString::number( splineSurface()->n_knots_n() ) += " knots in n direction.\n";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the bsplineSurface of the object has been picked.
*
* @param _node_idx Index of the picked spline surface node
* @return bool if the splineSurfaceNode of this object is the picking target.
*/
bool BSplineSurfaceObject::picked( uint _node_idx ) {
  return ( _node_idx == splineSurfaceNode_->id() );
}

//-----------------------------------------------------------------------------

void BSplineSurfaceObject::enablePicking( bool _enable ) {
  splineSurfaceNode_->enablePicking( _enable );
  textureNode_->enablePicking( _enable );
  shaderNode_->enablePicking( _enable );
}

//-----------------------------------------------------------------------------

bool BSplineSurfaceObject::pickingEnabled() {
  return splineSurfaceNode_->pickingEnabled();
}

//=============================================================================
//=============================================================================

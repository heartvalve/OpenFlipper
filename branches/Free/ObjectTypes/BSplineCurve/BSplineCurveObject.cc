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
// BSplineCurve object type - Implementation
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define BSPLINECURVEOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "BSplineCurve.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for BSpline Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The spline is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addBSplineCurve instead. ( see Types.hh::DataType )
*/
BSplineCurveObject::BSplineCurveObject() :
  BaseObjectData(),
  splineCurve_(NULL),
  splineCurveNode_(NULL)
{
  setDataType(DATA_BSPLINE_CURVE);
  init();
}

//-----------------------------------------------------------------------------

/**
 * Copy Constructor - generates a copy of the given object
 */
BSplineCurveObject::BSplineCurveObject(const BSplineCurveObject & _object) :
BaseObjectData(_object)
{
  init(_object.splineCurve_);
  setName( name() );
}

//-----------------------------------------------------------------------------

/** Destructor for BSpline Objects. The destructor deletes the spline and all
*  Scenegraph nodes associated with the spline or the object.
*/
BSplineCurveObject::~BSplineCurveObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the curve below and
  // they do something with the splineCurve in the destructor of their
  // perObjectData.
  deleteData();

  // Delete the Mesh only, if this object contains a mesh
  if ( splineCurve_ != NULL)  {
    delete splineCurve_;
    splineCurve_ = NULL;
  } else {
    std::cerr << "Destructor error : Spline curve already deleted" << std::endl;
  }

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  splineCurveNode_    = NULL;
}

//-----------------------------------------------------------------------------

/** Cleanup Function for spline Objects. Deletes the contents of the whole object and
* calls BSplineObject::init afterwards.
*/
void BSplineCurveObject::cleanup()
{
  // Delete the spline only, if this object contains a spline
  if ( splineCurve_ != NULL)  {
    delete splineCurve_;
    splineCurve_ = NULL;
  } else {
    std::cerr << "Cleanup error : Spline already deleted" << std::endl;
  }

  BaseObjectData::cleanup();

  splineCurveNode_   = NULL;

  setDataType(DATA_BSPLINE_CURVE);

  init();

}

//-----------------------------------------------------------------------------

/**
 * Generate a copy
 */
BaseObject* BSplineCurveObject::copy()
{
  BSplineCurveObject* object = new BSplineCurveObject(*this);
  return dynamic_cast< BaseObject* >(object);
}

//-----------------------------------------------------------------------------

/** This function initalizes the spline object. It creates the scenegraph nodes,
*  the spline.
*/
void BSplineCurveObject::init(BSplineCurve* _curve) {

  if (_curve == 0)
    splineCurve_ = new BSplineCurve();
  else
    splineCurve_ = new BSplineCurve(*_curve);

  // request selection properties for control polygon
  splineCurve()->request_controlpoint_selections();
  splineCurve()->request_edge_selections();
  // request selection property for knotvector
  splineCurve()->get_knotvector_ref()->request_selections();

  if ( materialNode() == NULL)
    std::cerr << "Error when creating BSplineCurve Object! materialNode is NULL!" << std::endl;

  splineCurveNode_ = new ACG::SceneGraph::BSplineCurveNodeT< BSplineCurve >(*splineCurve_, materialNode() , "NEW BSplineCurveNode");

  materialNode()->set_color(ACG::Vec4f(178.0f/255.0f, 34.0f/255.0f, 34.0f/255.0f, 1.0f));
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void BSplineCurveObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("BSplineCurveNode for BSpline curves " + _name.toUtf8() );
  splineCurveNode_->name( nodename );
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects spline.
* @return Pointer to the spline
*/
BSplineCurve* BSplineCurveObject::splineCurve() {
  return splineCurve_;
}

//-----------------------------------------------------------------------------

/// Update the whole Object (Selection,Topology,...)
void 
BSplineCurveObject::
update(UpdateType _type)
{
  if ( _type.contains(UPDATE_ALL))
  {
    splineCurveNode()->updateGeometry();
    
    // mark textures as invalid
    splineCurveNode()->cpSelectionTextureValid(false);
    splineCurveNode()->knotSelectionTextureValid(false);
  }
  else 
  {
    if (_type.contains(UPDATE_GEOMETRY) || _type.contains(UPDATE_SELECTION)) 
    {
      splineCurveNode()->updateGeometry();
      
      // mark textures as invalid
      splineCurveNode()->cpSelectionTextureValid(false);
      splineCurveNode()->knotSelectionTextureValid(false);
    }
    else if (_type.contains(UPDATE_SELECTION_VERTICES))
    {
      splineCurveNode()->updateGeometry();
      splineCurveNode()->cpSelectionTextureValid(false);
    }
    else if (_type.contains(UPDATE_SELECTION_KNOTS))
    {
      splineCurveNode()->updateGeometry();
      splineCurveNode()->knotSelectionTextureValid(false);
    }
  }
}


// ===============================================================================
// Visualization
// ===============================================================================

ACG::SceneGraph::BSplineCurveNodeT< BSplineCurve >* BSplineCurveObject::splineCurveNode() {
  return splineCurveNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString BSplineCurveObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_BSPLINE_CURVE ) )
    output += "Object Contains BSpline Curve : ";
  else{
    output += "Error: Object Contains NO BSpline Curve!";
    output += "========================================================================\n";
    return output;
  }

  output += QString::number( splineCurve()->n_control_points() ) + " control points, ";
  output += QString::number( splineCurve()->n_knots() ) += " knots.\n";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the bsplineNode of the object has been picked.
*
* @param _node_idx Index of the picked spline node
* @return bool if the splineNode of this object is the picking target.
*/
bool BSplineCurveObject::picked( uint _node_idx ) {
  return ( _node_idx == splineCurveNode_->id() );
}

//-----------------------------------------------------------------------------

void BSplineCurveObject::enablePicking( bool _enable ) {
  splineCurveNode_->enablePicking( _enable );
}

//-----------------------------------------------------------------------------

bool BSplineCurveObject::pickingEnabled() {
  return splineCurveNode_->pickingEnabled();
}

//=============================================================================

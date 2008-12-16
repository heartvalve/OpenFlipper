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
//  Types
//
//=============================================================================

/**
 * \file Types.hh
 * This File contains the basic available datatypes in the Framework.
 */


#ifndef TYPES_HH
#define TYPES_HH

#include "GlobalDefines.hh"
#include <limits.h>


//== Global Typedefs  =================================================================

/** \brief Field describing the type of Objects.
 *
 *  This is an enumerator defining the supported data types. To decide which object
 *  type an object has, get the dataType from it.
 */

/// None of the other Objects
const unsigned int DATA_NONE          = 0;

/// Items used for Grouping
const unsigned int DATA_GROUP         = 1;

/// Point based objects
const unsigned int DATA_POINTS        = 2;

/// PolyLine objects
const unsigned int DATA_POLY_LINE     = 4;

/// Triangle Mesh objects
const unsigned int DATA_TRIANGLE_MESH = 8;

/// Poly Mesh Objects
const unsigned int DATA_POLY_MESH     = 16;

/// BSpline objects
const unsigned int DATA_BSPLINE_CURVE = 32;

/// Volumetric objects
const unsigned int DATA_VOLUME        = 64;

/// BSpline Surface objects
const unsigned int DATA_BSPLINE_SURFACE = 128;

/// Skeletons
const unsigned int DATA_SKELETON       = 256;

/// Gis Data
const unsigned int DATA_GIS            = 512;


/// Identifier for all available objects
const unsigned int DATA_ALL           = UINT_MAX;

typedef unsigned int DataType;

// TODO: Make types runtime addable

//== INCLUDES =================================================================

#include "BaseObject.hh"
#include "GroupObject.hh"
#include "BaseObjectData.hh"
#include "MeshObjectT.hh"

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
#include "BSplineSurface/BSplineSurfaceObject.hh"
#endif

//== TYPEDEFS =================================================================

/// Standard Type for 3d Vector used for scripting
typedef ACG::Vec3d Vector;
/// Standard Type for id Lists used for scripting
typedef std::vector< int > idList;
/// Standard Type for a 4x4 Matrix used for scripting
typedef ACG::Matrix4x4d Matrix4x4;

Q_DECLARE_METATYPE(idList);
Q_DECLARE_METATYPE(QVector< int >);
Q_DECLARE_METATYPE(Vector);
Q_DECLARE_METATYPE(Matrix4x4);

/// Vector Type containing the Data Objects
typedef std::vector<BaseObjectData*> DataContainer;

/// Typedef for a Meshobject containing a triangle mesh
typedef MeshObject< TriMesh , DATA_TRIANGLE_MESH > TriMeshObject;

/// Typedef for a Meshobject containing a poly mesh
typedef MeshObject< PolyMesh , DATA_POLY_MESH >    PolyMeshObject;

//== Functions for adding dataTypes =========================================================

/// Adds a datatype and returns the id for the new type
DataType addDataType(QString _name);

/// Given a dataType Identifier string this function will return the id of the datatype
DataType typeId(QString _name);

//=============================================================================
#endif // TYPES_HH defined
//=============================================================================

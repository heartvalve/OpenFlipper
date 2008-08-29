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


//== Global Typedefs  =================================================================

/** \brief Field describing the type of Objects.
 *
 *  This is an enumerator defining the supported data types. To decide which object
 *  type an object has, get the dataType from it.
 */
enum DataType {
   /// Empty Objects (Should not be used)
   DATA_NONE            = 0 ,
   /// Items used for Grouping
   DATA_GROUP           = 1 ,
   /// Point based objects
   DATA_POINTS          = 1 << 1,
   /// PolyLine objects
   DATA_POLY_LINE       = 1 << 2,
   /// Triangle Mesh objects
   DATA_TRIANGLE_MESH   = 1 << 3,
   /// Poly Mesh Objects
   DATA_POLY_MESH       = 1 << 4,
   /// Volumetric objects
   DATA_VOLUME          = 1 << 5,
   /// BSpline objects
   DATA_BSPLINE_CURVE   = 1 << 6,
   /// BSpline Surface objects
   DATA_BSPLINE_SURFACE = 1 << 7,
   /// Skeletons
   DATA_SKELETON        = 1 << 8,
};

/// Datatype matching all available types
const DataType DATA_ALL = DataType( DATA_POINTS | DATA_TRIANGLE_MESH | DATA_POLY_MESH | DATA_VOLUME | DATA_POLY_LINE | DATA_BSPLINE_CURVE | DATA_BSPLINE_SURFACE);

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

//== CLASS DEFINITION =========================================================

//=============================================================================
#endif // TYPES_HH defined
//=============================================================================

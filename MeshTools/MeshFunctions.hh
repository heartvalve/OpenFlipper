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
//
//=============================================================================


#ifndef MESHFUNCTIONS_HH
#define MESHFUNCTIONS_HH


/*! \file MeshFunctions.hh
    \brief Functions for modifying a Mesh
    
    General file with template functions doing modifications on a given Mesh 
    (e.g smooth, reposition,... )
*/

//== INCLUDES =================================================================

#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

/// Namespace providing different Mesh editing functions
namespace MeshFunctions {

//== DEFINITIONS ==============================================================

/**
    get one boundary at the vertex
    @param _mesh Mesh
    @param _vh   Vertex handle of one vertex of the boundary
    @param _boundary Coords and vertex handles of the boundary
  */
template < typename MeshT , typename VectorT >
bool get_boundary(MeshT& _mesh, 
                               typename MeshT::VertexHandle _vh, 
                               std::vector< std::pair< VectorT , typename MeshT::VertexHandle > >& _boundary);  

/**
    get boundary of a mesh (assumes that there is only one boundary!!
    @param _mesh Mesh
    @param _boundary Coords and vertex handles of the boundary
    @return Found a boundary?
  */
template < typename MeshT , typename VectorT >
bool get_boundary(MeshT& _mesh, 
                               std::vector< std::pair< VectorT , 
                               typename MeshT::VertexHandle > >& _boundary);  

/**
    function to smooth a boundary of a given mesh by moving each vertex to the 
    mean Position of its adjazent verticies  
    @param _mesh Mesh to work on
    @param _vh Vertex handle on the boundary 
*/ 
template < typename MeshT , typename VectorT >
void smooth_boundary(MeshT& _mesh , 
                     typename MeshT::VertexHandle _vh);

/**
    Checks for two faces if they are adjazent
    @param _mesh Mesh
    @param _fh1  First Face 
    @param _fh2  Second Face
*/
template < typename MeshT >
bool neighbour(const MeshT& _mesh , 
                         const typename MeshT::FaceHandle _fh1 , 
                         const typename MeshT::FaceHandle _fh2 );

/**
   Checks if plane cuts the face
   @param _porigin Planes origin
   @param _pnormal Plane normal
   @param _mesh    Mesh
   @param _fh      Facehandle of face
   @return triangle cut by plane?
*/
template < typename MeshT , typename VectorT >
bool
cut_face(const VectorT& _porigin, 
               const VectorT& _pnormal, 
               const MeshT& _mesh, 
               const typename MeshT::FaceHandle& _fh);
         
/** Get the area of a mesh ( sum over all triangle areas)
    @param _mesh Mesh to calculate area 
    @return Area spanned by the mesh
    */
template < typename MeshT >
double 
calc_area( const MeshT& _mesh);

/** Get the sum of the angles around a vertex
    @param _mesh Mesh to work on
    @param _vh Vertex
    @return Sum of angles around the vertex
    */
template < typename MeshT >
double 
calc_angle_around( const MeshT& _mesh , const typename MeshT::VertexHandle _vh);

/**
 * Transform geometry of the mesh using the specified
 * transformation matrix.
 * @param _matrix The transformation matrix
 * @param _mesh The mesh that is to be transformed
 */
template< typename MeshT >
void transformMesh(ACG::Matrix4x4d _matrix , MeshT& _mesh);

/**
 * Transform handle vertices only
 * @param _matrix The transformation matrix
 * @param _mesh The mesh that is to be transformed
 */
template< typename MeshT >
void transformHandleVertices(ACG::Matrix4x4d _matrix , MeshT& _mesh);

//=============================================================================
} // MeshFunctions Namespace 
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MESHFUNCTIONS_C)
#define MESHFUNCTIONS_TEMPLATES
#include "MeshFunctions.cc"
#endif
//=============================================================================
#endif // MESHFUNCTIONS_HH defined
//=============================================================================


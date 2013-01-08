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
//
//=============================================================================


#ifndef MESHSELECTION_HH
#define MESHSELECTION_HH


/*! \file MeshSelectionT.hh
    \brief Functions for selection on a mesh

*/

//== INCLUDES =================================================================

#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace MeshSelection {

//== DEFINITIONS ==============================================================

//===========================================================================
/** @name Vertex Selection
* @{ */
//===========================================================================

/// Select given vertices of a mesh
template< typename MeshT >
inline
void selectVertices(MeshT* _mesh, const std::vector< int >& _vertices);

/// Unselect given vertices of a mesh
template< typename MeshT >
inline
void unselectVertices(MeshT* _mesh, const std::vector< int >& _vertices);

/// Select all vertices of a mesh
template< typename MeshT >
inline
void selectAllVertices(MeshT* _mesh);

/// Set all vertices to unselected
template< typename MeshT >
inline
void clearVertexSelection(MeshT* _mesh);

/// invert vertex selection
template< typename MeshT >
inline
void invertVertexSelection(MeshT* _mesh);

/** Select all vertices of the mesh which are boundary vertices
 */
template< typename MeshT >
inline
void selectBoundaryVertices(MeshT* _mesh);

/** shrink the vertex selection
 */
template< typename MeshT >
inline
void shrinkVertexSelection(MeshT* _mesh) ;

/** grow the vertex selection
 */
template< typename MeshT >
inline
void growVertexSelection(MeshT* _mesh);

/** Get the current vertex selection
 */
template< typename MeshT >
inline
std::vector< int > getVertexSelection(MeshT* _mesh);

/** Get the current vertex selection
 * @param _invert if true : vector has been inverted to save mem
 */
template< typename MeshT >
inline
std::vector< int > getVertexSelection(MeshT* _mesh, bool& _invert);

/** Select all vertices belonging to the same boundary as _vh
 *
 * This function gets one vertex that has to be on a boundary.
 * Than the function will follow the boundary and mark all vertices
 * at it as selected.
 *
 * @param _vh handle of a boundary vertex
 */
template< typename MeshT >
inline
void selectBoundaryVertices(MeshT* _mesh, const typename MeshT::VertexHandle& _vh);

/**
 * Select for each vertex in _vertices all incident edges
 * @param _vertices List of vertices to be converted
 */
template< typename MeshT >
inline
void convertVertexToEdgeSelection(MeshT* _mesh, const std::vector< int >& _vertices);

/**
 * For each selected vertex select all incident edges
 */
template< typename MeshT >
inline
void convertVertexToEdgeSelection(MeshT* _mesh);

/**
 * Select for each vertex in _vertices all incident halfedges
 * @param _vertices List of vertices to be converted
 */
template< typename MeshT >
inline
void convertVertexToHalfedgeSelection(MeshT* _mesh, const std::vector< int >& _vertices);

/**
 * For each selected vertex select all incident halfedges
 */
template< typename MeshT >
inline
void convertVertexToHalfedgeSelection(MeshT* _mesh);

/**
 * Select for each vertex in _vertices all adjacent faces
 * @param _vertices List of vertices to be converted
 */
template< typename MeshT >
inline
void convertVertexToFaceSelection(MeshT* _mesh, const std::vector< int >& _vertices);

/**
 * For each selected vertex select all adjacent faces
 */
template< typename MeshT >
inline
void convertVertexToFaceSelection(MeshT* _mesh);

/**
 * Convert vertex selection to feature selection
 */
template< typename MeshT >
inline
void convertVertexSelectionToFeatureVertices(MeshT* _mesh);

/**
 * Convert feature selection to vertex selection
 */
template< typename MeshT >
inline
void convertFeatureVerticesToVertexSelection(MeshT* _mesh);

/**
 * Clear all features
 */
template< typename MeshT >
inline
void clearFeatureVertices(MeshT* _mesh);

/** @} */

//===========================================================================
/** @name Modeling Regions
* @{ */
//===========================================================================

/** Set the area bit for all defined vertices
 * @param _vertices The vertices belonging to the modeling area
 * @param _type Bit to be changed ( Normally Modeling area is OpenMesh::Attributes::UNUSED << 1 )
 * */
template< typename MeshT >
inline
void setArea(MeshT* _mesh, const std::vector< int >& _vertices , unsigned int _type, bool _state);

/** Reset Modeling Status for vertices
 *
 * */
template< typename MeshT >
inline
void setArea(MeshT* _mesh , unsigned int _type, bool _state);

/** Get the current vertex selection
 */
template< typename MeshT >
inline
std::vector< int > getArea(MeshT* _mesh, unsigned int _type);

/** Get the current vertex selection
 * @param _invert if true : vector has been inverted to save mem
 */
template< typename MeshT >
inline
std::vector< int > getArea(MeshT* _mesh, unsigned int _type , bool& _invert);

/** @} */

//===========================================================================
/** @name Edge Selection
* @{ */
//===========================================================================

/// Select given edges of a mesh
template< typename MeshT >
inline
void selectEdges(MeshT* _mesh, const std::vector< int >& _edges);

/// Unselect given edges of a mesh
template< typename MeshT >
inline
void unselectEdges(MeshT* _mesh, const std::vector< int >& _edges);

/// Select all edges of a mesh
template< typename MeshT >
inline
void selectAllEdges(MeshT* _mesh);

///  Set all edges to unselected
template< typename MeshT >
inline
void clearEdgeSelection(MeshT* _mesh);

/// Invert Edge selection
template< typename MeshT >
inline
void invertEdgeSelection(MeshT* _mesh);

/// Select all boundary edges of a mesh
template< typename MeshT >
void selectBoundaryEdges(MeshT* _mesh);

/** Get the current edge selection
 */
template< typename MeshT >
inline
std::vector< int > getEdgeSelection(MeshT* _mesh);

/** Get the current edge selection
 * @param _invert if true : vector has been inverted to save mem
 */
template< typename MeshT >
inline
std::vector< int > getEdgeSelection(MeshT* _mesh, bool& _invert);

/**
 * Select for each edge in _edges all incident vertices
 * @param _edges List of edges to be converted
 */
template< typename MeshT >
inline
void convertEdgeToVertexSelection(MeshT* _mesh, const std::vector< int >& _edges);

/**
 * For each selected edge select all incident vertices
 */
template< typename MeshT >
inline
void convertEdgeToVertexSelection(MeshT* _mesh);

/**
 * Select for each edge in _edges all adjacent faces
 * @param _edges List of edges to be converted
 */
template< typename MeshT >
inline
void convertEdgeToFaceSelection(MeshT* _mesh, const std::vector< int >& _edges);

/**
 * For each selected edge select all adjacent faces
 */
template< typename MeshT >
inline
void convertEdgeToFaceSelection(MeshT* _mesh);

/**
 * For each selected edge select all halfedges
 */
template< typename MeshT >
inline
void convertEdgeToHalfedgeSelection(MeshT* _mesh);

/**
 * Convert edge selection to feature selection
 */
template< typename MeshT >
inline
void convertEdgeSelectionToFeatureEdges(MeshT* _mesh);

/**
 * Convert feature selection to edge selection
 */
template< typename MeshT >
inline
void convertFeatureEdgesToEdgeSelection(MeshT* _mesh);

/**
 * Clear all features
 */
template< typename MeshT >
inline
void clearFeatureEdges(MeshT* _mesh);


/** @} */

//===========================================================================
/** @name Halfedge Selection
* @{ */
//===========================================================================

/// Select given edges of a mesh
template< typename MeshT >
inline
void selectHalfedges(MeshT* _mesh, const std::vector< int >& _halfedges);

/// Unselect given edges of a mesh
template< typename MeshT >
inline
void unselectHalfedges(MeshT* _mesh, const std::vector< int >& _halfedges);

/// Select all edges of a mesh
template< typename MeshT >
inline
void selectAllHalfedges(MeshT* _mesh);

///  Set all edges to unselected
template< typename MeshT >
inline
void clearHalfedgeSelection(MeshT* _mesh);

/// Invert Edge selection
template< typename MeshT >
inline
void invertHalfedgeSelection(MeshT* _mesh);

/// Select all boundary edges of a mesh
template< typename MeshT >
void selectBoundaryHalfedges(MeshT* _mesh);

/** Get the current edge selection
 */
template< typename MeshT >
inline
std::vector< int > getHalfedgeSelection(MeshT* _mesh);

/**
 * For each halfedge select all incident vertices
 */
template< typename MeshT >
inline
void convertHalfedgeToVertexSelection(MeshT* _mesh);

/**
 * For each halfedge select all edges
 */
template< typename MeshT >
inline
void convertHalfedgeToEdgeSelection(MeshT* _mesh);

/**
 * For each halfedge select all incident faces
 */
template< typename MeshT >
inline
void convertHalfedgeToFaceSelection(MeshT* _mesh);

//===========================================================================
/** @name Face Selection
* @{ */
//===========================================================================

/// Select given faces of a mesh
template< typename MeshT >
inline
void selectFaces(MeshT* _mesh, const std::vector< int >& _faces );

/// Unselect given faces of a mesh
template< typename MeshT >
inline
void unselectFaces(MeshT* _mesh, const std::vector< int >& _faces );

/// Select all faces of a mesh
template< typename MeshT >
inline
void selectAllFaces(MeshT* _mesh);

/// Set all faces to unselected
template< typename MeshT >
inline
void clearFaceSelection(MeshT* _mesh);

/// Invert face selection
template< typename MeshT >
inline
void invertFaceSelection(MeshT* _mesh);

/// Select all boundary faces of a mesh
template< typename MeshT >
void selectBoundaryFaces(MeshT* _mesh);

/** \brief Shrink Face selection
 *
 * Deselects all faces which are adjacent to a boundary vertex of the original selection
 */
template< typename MeshT >
inline
void shrinkFaceSelection(MeshT* _mesh);

/** \brief Grow Face selection
 *
 * Selects all faces which are adjacent to a vertex of a already selected face.
 */
template< typename MeshT >
inline
void growFaceSelection(MeshT* _mesh);

/** Get the current face selection
 */
template< typename MeshT >
inline
std::vector< int > getFaceSelection(MeshT* _mesh);

/** Get the current face selection
 * @param _invert if true : vector has been inverted to save mem
 */
template< typename MeshT >
inline
std::vector< int > getFaceSelection(MeshT* _mesh, bool& _invert);

/**
 * Select for each face in _faces all adjacent vertices
 * @param _faces List of faces to be converted
 */
template< typename MeshT >
inline
void convertFaceToVertexSelection(MeshT* _mesh, const std::vector< int >& _faces);

/**
 * For each selected face select all adjacent vertices
 */
template< typename MeshT >
inline
void convertFaceToVertexSelection(MeshT* _mesh);

/**
 * For each selected face select all adjacent edges
 */
template< typename MeshT >
inline
void convertFaceToEdgeSelection(MeshT* _mesh);

/**
 * For each selected face select all incident halfedges
 */
template< typename MeshT >
inline
void convertFaceToHalfedgeSelection(MeshT* _mesh);

/**
 * Convert face selection to feature selection
 */
template< typename MeshT >
inline
void convertFaceSelectionToFeatureFaces(MeshT* _mesh);

/**
 * Convert feature selection to edge selection
 */
template< typename MeshT >
inline
void convertFeatureFacesToFaceSelection(MeshT* _mesh);

/**
 * Clear all features
 */
template< typename MeshT >
inline
void clearFeatureFaces(MeshT* _mesh);

/** @} */

//=============================================================================
} // MeshSelection Namespace
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MESHSELECTION_C)
#define MESHSELECTION_TEMPLATES
#include "MeshSelectionT.cc"
#endif
//=============================================================================
#endif // MESHSELECTION_HH defined
//=============================================================================


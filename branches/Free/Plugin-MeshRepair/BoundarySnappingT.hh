/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


#ifndef BOUNDARYSNAPPING_HH
#define BOUNDARYSNAPPING_HH

//=============================================================================
//
//  CLASS BoundarySnappingT
//
//=============================================================================

//== INCLUDES =================================================================

//== NAMESPACE ================================================================

//== CLASS DEFINITION =========================================================

/** \brief Snaps selected vertices at boundaries
 *
 * Snaps selected boundary vertices together if they are closer than the given
 * distance. No new vertices will be introduced on either edge, so they are just
 * snapped to existing ones.
 *
 */
template<class MeshT>
class BoundarySnappingT {

public:

  BoundarySnappingT(MeshT& _mesh );

  /** \brief snaps boundary vertices
   *
   * snaps selected boundary vertices where the vertices
   * distance is not greater than the given distance
   *
   * @param _epsilon max Distance between 2 boundary vertices
   *
   */
  void snap(double _epsilon);

private:

  MeshT& mesh_;

};

#if defined(INCLUDE_TEMPLATES) && !defined(BOUNDARYSNAPPING_CC)
#define BOUNDARYSNAPPING_TEMPLATES
#include "BoundarySnappingT.cc"
#endif

#endif


/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
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
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//================================================================
//
//  SplatCloudSelection
//
//================================================================


#ifndef SPLATCLOUDSELECTION_HH
#define SPLATCLOUDSELECTION_HH


/*! \file SplatCloudSelection.hh
    \brief Functions for selection on a SplatCloud

*/

//== INCLUDES =================================================================


#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <vector>


//== NAMESPACES ===============================================================


namespace SplatCloudSelection {


//== DEFINITIONS ==============================================================


//===========================================================================
/** @name Vertex Selection
* @{ */
//===========================================================================

void             selectVertices       (       SplatCloud *_splatCloud, const std::vector<int> &_vertices ); //!< Select given vertices.
void             unselectVertices     (       SplatCloud *_splatCloud, const std::vector<int> &_vertices ); //!< Unselect given vertices.
void             selectAllVertices    (       SplatCloud *_splatCloud                                    ); //!< Select all vertices.
void             clearVertexSelection (       SplatCloud *_splatCloud                                    ); //!< Unselect all vertices.
void             invertVertexSelection(       SplatCloud *_splatCloud                                    ); //!< Invert vertex selection.
std::vector<int> getVertexSelection   ( const SplatCloud *_splatCloud                                    ); //!< Get current vertex selection.
std::vector<int> getVertexSelection   ( const SplatCloud *_splatCloud, bool &_inverted                   ); //!< Get current vertex selection. If _inverted is true, vector is inverted to save memory.

/** @} */


//=============================================================================


} // namespace SplatCloudSelection


//=============================================================================


#endif // SPLATCLOUDSELECTION_HH

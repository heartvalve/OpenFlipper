/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS ICP
//
//=============================================================================


#ifndef ICP_HH
#define ICP_HH

/*! \file ICP.hh
    \brief Classes for ICP Algorithm
    
    Classes for doing Principal component analysis
*/

//== INCLUDES =================================================================

#include <gmm/gmm.h>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

/// Namespace for ICP
namespace ICP {
 
//== CLASS DEFINITION =========================================================
  
/** \brief Compute rigid transformation from first point set to second point set
   * 
   * Compute ICP Parameters ( No iteration is done ) Points are unchanged, only parameters are computed.
   * @param _points1 first set of points
   * @param _points2 second set of points
   * @param  _cog1 center of gravity first point set
   * @param  _cog2 center of gravity second point set
   * @param _scale1 variance of first point set
   * @param _scale1 variance of second point set
   * @param _rotation Rotation between point sets ( rotation(_points1) -> _points2
   * 
*/
template < typename VectorT , typename QuaternionT >
void icp(std::vector< VectorT >& _points1 , std::vector< VectorT >& _points2  , VectorT& _cog1 ,  VectorT& _cog2 , double& _scale1 , double& _scale2 , QuaternionT& _rotation );

//=============================================================================
} //namespace ICP
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ICP_C)
#define ICP_TEMPLATES
#include "ICP.cc"
#endif
//=============================================================================
#endif // ICP_HH defined
//=============================================================================


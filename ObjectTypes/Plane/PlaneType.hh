/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: -1 $                                                         *
 *   $Author: $                                                      *
 *   $Date: $                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS Plane
//
//=============================================================================

#ifndef PLANE_HH
#define PLANE_HH

//== INCLUDES =================================================================


#include <ACG/Math/Matrix4x4T.hh>
#include <ACG/Math/VectorT.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================


/** \class PlaneNode PlaneNode.hh <ACG/Scenegraph/PlaneNode.hh>

    PlaneNode renders a plane

**/

class DLLEXPORT Plane {

public:

  /** \brief Set plane
   *
   * @param _position   One point on the plane. Will be used as corner point point for rendering in the PlaneNode
   * @param _xDirection Vector pointing in planes x direction
   * @param _yDirection Vector pointing in planes y direction
   */
  void setPlane(const ACG::Vec3f& _position, const ACG::Vec3f& _xDirection, const ACG::Vec3f& );

   /** \brief Set plane with given normal and one point
    *
    * @param _position One point on the plane. Will be used as corner point for rendering in the PlaneNode
    * @param _normal   Plane normal
    */
  void setPlane(const ACG::Vec3f & _position, const ACG::Vec3f & _normal);

  /** \brief Set plane size
   *
   * Scales the plane such that the x and y direction vectors have the given lengths
   *
   * @param _xDirection Size in x direction
   * @param _yDirection Size in y direction
   */
  void setSize(double _xDirection, double _yDirection);

  /** \brief Transform the plane with given matrix
   *
   *
   * @param _mat Transformation matrix.
   */
  void transform(const ACG::Matrix4x4d & _mat);

public:

  ACG::Vec3f position;
  ACG::Vec3f normal;
  ACG::Vec3f xDirection;
  ACG::Vec3f yDirection;

};

//=============================================================================
#endif // PLANE_HH defined
//=============================================================================

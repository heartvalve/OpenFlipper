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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Wed, 26 Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS BSPImplT
//
//=============================================================================

#ifndef BSPIMPLT_HH
#define BSPIMPLT_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/Geometry/VectorT.hh>


//== CLASS DEFINITION =========================================================
#include <vector>
#include <ostream>

template <class BSPCore>
class BSPImplT : public BSPCore
{
public: //---------------------------------------------------------------------

  typedef typename BSPCore::Traits      Traits;
  typedef typename BSPCore::Handle      Handle;
  typedef typename BSPCore::Point       Point;
  typedef typename BSPCore::Scalar      Scalar;
  typedef typename BSPCore::Node        Node;
  typedef typename BSPCore::Handles     Handles;
  typedef typename BSPCore::HandleIter  HandleIter;
  

public: //---------------------------------------------------------------------

  BSPImplT(const Traits& _traits, const Scalar _infinity = std::numeric_limits<Scalar>::infinity()) :
      BSPCore(_traits),
      infinity_(_infinity) {}
  ~BSPImplT() {}


  /// Store nearest neighbor information
  struct NearestNeighbor
  {
    NearestNeighbor() {}
    NearestNeighbor(Handle _h, Scalar _d) : handle(_h), dist(_d) {}
    Handle  handle;
    Scalar  dist;
  };
  
  /// Store nearest neighbor information
  typedef  std::vector< std::pair<Handle,Scalar> > RayCollision;

  /// Return handle of the nearest neighbor face
  NearestNeighbor nearest(const Point& _p) const;
  
  /** \brief intersect mesh with ray
   *
   * This function shots a ray through the mesh and collects all intersected triangles and
   * the handle of the closest face ( non-directional, so no matter of the ray direction, the
   * closest face handle is returned in either direction)
   *
   * @param _p Start point of the ray
   * @param _r Ray direction
   * @return   Collision information
   */
  RayCollision raycollision (const Point& _p, const Point& _r) const;
  
  /** \brief intersect mesh with ray
   *
   * This function shots a ray through the mesh and collects all intersected triangles and
   * the handle of the closest face ( directional, so the ray direction is taken into account!).
   *
   * Only hits with a distance > 0.0 to the point p will be collected (_p will be skipped!)
   *
   * @param _p Start point of the ray
   * @param _r Ray direction
   * @return   Collision information
   */
  RayCollision directionalRaycollision (const Point& _p, const Point& _r) const;


private: //---------------------------------------------------------------------


  /// Store nearest neighbor information
  struct NearestNeighborData
  {
    Point   ref;
    Scalar  dist;
    Handle  nearest;

    friend std::ostream &operator<< (std::ostream &stream, const NearestNeighborData &data) {
        stream << "[NearestNeghborData instance. ref: " << data.ref << ", dist: " << data.dist << ", nearest: " << data.nearest.idx() << "]";
        return stream;
    }
  };
  
  /// Store ray collide information
  struct RayCollisionData
  {
    Point   ref;
    Point   ray;
    RayCollision hit_handles;
  };


  // Recursive part of nearest()
  void _nearest(Node* _node, NearestNeighborData& _data) const;
  
  /**  \brief recursive part of raycollision()
   *
   * @param _node The current node in the tree
   * @param _data Data pointer, used to collect the collision information
   */
  void _raycollision_non_directional(Node* _node, RayCollisionData& _data) const;

  /**  \brief recursive part of directionalRaycollision()
   *
   * @param _node The current node in the tree
   * @param _data Data pointer, used to collect the collision information
   */
  void _raycollision_directional(Node* _node, RayCollisionData& _data) const;

  template<typename T,typename U>
  struct less_pair_second: public std::binary_function<T,U,bool> {
      bool operator()(const std::pair<T,U> &left, const std::pair<T,U> &right) {
          return (fabs(left.second) < fabs(right.second));
      }
  };

  const Scalar infinity_;

};

//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(BSPIMPLT_C)
#  define BSPIMPLT_TEMPLATES
#  include "BSPImplT.cc"
#endif
//=============================================================================
#endif // BSPIMPLT_HH defined
//=============================================================================

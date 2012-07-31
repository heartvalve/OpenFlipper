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

#define BSPIMPLT_C

//== INCLUDES =================================================================


#include "BSPImplT.hh"
#include <float.h>


//== CLASS DEFINITION =========================================================
#include <vector>
#include <stdexcept>
#include <limits>

template <class BSPCore>
typename BSPImplT<BSPCore>::NearestNeighbor
BSPImplT<BSPCore>::
nearest(const Point& _p) const
{
  NearestNeighborData  data;
  data.ref  = _p;
  data.dist = std::numeric_limits<Scalar>::infinity();
  if (this->root_ == 0)
    throw std::runtime_error("It seems like the BSP hasn't been built, yet. Did you call build(...)?");
  _nearest(this->root_, data);
  return NearestNeighbor(data.nearest, sqrt(data.dist));
}


//-----------------------------------------------------------------------------


template <class BSPCore>
void
BSPImplT<BSPCore>::
_nearest(Node* _node, NearestNeighborData& _data) const
{
  // terminal node
  if (!_node->left_child_)
  {
    Scalar dist;

    for (HandleIter it=_node->begin(); it!=_node->end(); ++it)
    {
      dist = this->traits_.sqrdist(*it, _data.ref);
      if (dist < _data.dist)
      {
        _data.dist = dist;
        _data.nearest = *it;
      }
    }
  }

  // non-terminal node
  else
  {
    Scalar dist = _node->plane_.distance(_data.ref);

    if (dist > 0.0)
    {
      _nearest(_node->left_child_, _data);
      if (dist*dist < _data.dist)
        _nearest(_node->right_child_, _data);
    }
    else
    {
      _nearest(_node->right_child_, _data);
      if (dist*dist < _data.dist)
        _nearest(_node->left_child_, _data);
    }    
  }
}

//-----------------------------------------------------------------------------

template <class BSPCore>
typename BSPImplT<BSPCore>::RayCollision
BSPImplT<BSPCore>::
raycollision(const Point& _p, const Point& _r) const
{
  // Prepare the struct for returning the data
  RayCollisionData  data;
  data.ref  = _p;
  data.dist = FLT_MAX;
  data.ray  = _r;
  data.hit_vertices.clear();
  
  _raycollision_non_directional(this->root_, data);
  return RayCollision(data.nearest, data.dist, data.hit_vertices);
}

template <class BSPCore>
typename BSPImplT<BSPCore>::RayCollision
BSPImplT<BSPCore>::
directionalRaycollision(const Point& _p, const Point& _r) const {

  // Prepare the struct for returning the data
  RayCollisionData  data;
  data.ref  = _p;
  data.dist = FLT_MAX;
  data.ray  = _r;
  data.hit_vertices.clear();

  _raycollision_directional(this->root_, data);
  return RayCollision(data.nearest, data.dist, data.hit_vertices);

}


//-----------------------------------------------------------------------------


template <class BSPCore>
void
BSPImplT<BSPCore>::
_raycollision_non_directional(Node* _node, RayCollisionData& _data) const
{
  // terminal node
  if (!_node->left_child_)
  {
    Scalar dist;
    Point v0, v1, v2;
    Scalar u, v;

    for (HandleIter it=_node->begin(); it!=_node->end(); ++it)
    {
      this->traits_.points(*it, v0, v1, v2);
      if (ACG::Geometry::triangleIntersection(_data.ref, _data.ray, v0, v1, v2, dist, u, v)) {

        _data.hit_vertices.push_back(*it);

        // face intersects with ray. But is it closer than any that we have found so far?
        if ( fabs(dist) < _data.dist)
        {
          _data.dist    = fabs(dist);
          _data.nearest = *it;
        }
      }
    }
  }

  // non-terminal node
  else
  {
    Scalar tmin, tmax;
    bool used = false;
    if ( _node->left_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->left_child_->bb_min, _node->left_child_->bb_max, tmin, tmax)) {
      _raycollision_non_directional(_node->left_child_, _data);
    }
    if ( _node->right_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->right_child_->bb_min, _node->right_child_->bb_max, tmin, tmax)) {
      _raycollision_non_directional(_node->right_child_, _data);
    }
  }
}

//-----------------------------------------------------------------------------


template <class BSPCore>
void
BSPImplT<BSPCore>::
_raycollision_directional(Node* _node, RayCollisionData& _data) const
{
  // terminal node
  if (!_node->left_child_)
  {
    Scalar dist;
    Point v0, v1, v2;
    Scalar u, v;

    for (HandleIter it=_node->begin(); it!=_node->end(); ++it)
    {
      this->traits_.points(*it, v0, v1, v2);
      if (ACG::Geometry::triangleIntersection(_data.ref, _data.ray, v0, v1, v2, dist, u, v)) {

        // We shoot into direction of the ray
        // The first intersection should not be on the starting point
        if ( dist <= 0.0 )
          continue;

        _data.hit_vertices.push_back(*it);

        // face intersects with ray. But is it closer than any that we have found so far?
        // Note as we rely on the direction of the hit, so we will never get negative directions here
        if ( dist < _data.dist)
        {
          _data.dist    = fabs(dist);
          _data.nearest = *it;
        }
      }
    }
  }

  // non-terminal node
  else
  {
    Scalar tmin, tmax;
    bool used = false;
    if ( _node->left_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->left_child_->bb_min, _node->left_child_->bb_max, tmin, tmax)) {
      _raycollision_directional(_node->left_child_, _data);
    }
    if ( _node->right_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->right_child_->bb_min, _node->right_child_->bb_max, tmin, tmax)) {
      _raycollision_directional(_node->right_child_, _data);
    }
  }
}


//=============================================================================

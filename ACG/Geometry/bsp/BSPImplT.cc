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
  data.dist = infinity_;
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
    Scalar dist(0);
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
  data.ray  = _r;
  data.hit_handles.clear();

  _raycollision_non_directional(this->root_, data);

  std::sort(data.hit_handles.begin(), data.hit_handles.end(), less_pair_second<Handle,Scalar>());
  return RayCollision(data.hit_handles);
}

template <class BSPCore>
typename BSPImplT<BSPCore>::RayCollision
BSPImplT<BSPCore>::
directionalRaycollision(const Point& _p, const Point& _r) const {

  // Prepare the struct for returning the data
  RayCollisionData  data;
  data.ref  = _p;
  data.ray  = _r;
  data.hit_handles.clear();

  _raycollision_directional(this->root_, data);

  std::sort(data.hit_handles.begin(), data.hit_handles.end(), less_pair_second<Handle,Scalar>());
  return RayCollision(data.hit_handles);

}

template <class BSPCore>
typename BSPImplT<BSPCore>::RayCollision
BSPImplT<BSPCore>::
nearestRaycollision(const Point& _p, const Point& _r) const {

  // Prepare the struct for returning the data
  RayCollisionData  data;
  data.ref  = _p;
  data.ray  = _r;
  data.hit_handles.clear();

  _raycollision_nearest_directional(this->root_, data);

  return RayCollision(data.hit_handles);
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
        // face intersects with ray.
        _data.hit_handles.push_back(std::pair<Handle,Scalar>(*it,dist));
      }
    }
  }

  // non-terminal node
  else
  {
    Scalar tmin, tmax;
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
        if (dist > 0.0){
          _data.hit_handles.push_back(std::pair<Handle,Scalar>(*it,dist));
        }
      }
    }
  }

  // non-terminal node
  else
  {
    Scalar tmin, tmax;
    if ( _node->left_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->left_child_->bb_min, _node->left_child_->bb_max, tmin, tmax)) {
      _raycollision_directional(_node->left_child_, _data);
    }
    if ( _node->right_child_ && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, _node->right_child_->bb_min, _node->right_child_->bb_max, tmin, tmax)) {
      _raycollision_directional(_node->right_child_, _data);
    }
  }
}

//-----------------------------------------------------------------------------


template <class BSPCore>
void
BSPImplT<BSPCore>::
_raycollision_nearest_directional(Node* _node, RayCollisionData& _data) const
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
        if (dist > 0.0){
          _data.hit_handles.push_back(std::pair<Handle,Scalar>(*it, dist));
        }
      }
    }
		// only return the closest hit
		if(!_data.hit_handles.empty()) {
			std::partial_sort(_data.hit_handles.begin(), _data.hit_handles.begin() + 1, _data.hit_handles.end(), less_pair_second<Handle, Scalar>());
			_data.hit_handles.resize(1);
		}
  }

  // non-terminal node
  else
	{
		// determine order of traversal
		Node* first_node = _node->left_child_, *second_node = _node->right_child_;
		if (!_node->plane_(_data.ref)) {
			std::swap(first_node, second_node);
		}

		Scalar tmin, tmax;
		if ( first_node && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, first_node->bb_min, first_node->bb_max, tmin, tmax) ) {
			_raycollision_nearest_directional(first_node, _data);
		}
		// if the second node is further away than the closeset hit skip it
		Scalar dist = ACG::NumLimitsT<Scalar>::max();
		if(!_data.hit_handles.empty()) {
			dist = _data.hit_handles.front().second;
		}
		if ( second_node && ACG::Geometry::axisAlignedBBIntersection( _data.ref, _data.ray, second_node->bb_min, second_node->bb_max, tmin, tmax) && (tmin < dist) ) {
			_raycollision_nearest_directional(second_node, _data);
		}
	}
}


//=============================================================================

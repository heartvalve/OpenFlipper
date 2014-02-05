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
//  CLASS KnotvectorT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#define ACG_KNOTVECTORT_C

#include "KnotvectorT.hh"

namespace ACG {

template < typename Scalar >
KnotvectorT<Scalar>::
KnotvectorT()
  : ref_count_selections_(0)
{
  knotvectorType_ = UNIFORM_INTERPOL;
}

//-----------------------------------------------------------------------------

template < typename Scalar >
KnotvectorT<Scalar>::
KnotvectorT( const KnotvectorT& _knotvec )
{
  knots_ = _knotvec.knots_;

  // properties
  selections_ = _knotvec.selections_;

  // property reference counter
  ref_count_selections_ = _knotvec.ref_count_selections_;

  knotvectorType_ = _knotvec.knotvectorType_;

  num_control_points_ = _knotvec.num_control_points_;
  spline_degree_      = _knotvec.spline_degree_;
}

//-----------------------------------------------------------------------------

template< typename Scalar >
KnotvectorT<Scalar>::
~KnotvectorT()
{
}

//-----------------------------------------------------------------------------

template <typename Scalar >
void
KnotvectorT<Scalar>::
setType(KnotvectorType _type)
{
  knotvectorType_ = _type;

  if ( (spline_degree_ != 0) && (num_control_points_ != 0))
    createKnots(spline_degree_, num_control_points_);
}

//-----------------------------------------------------------------------------

template <typename Scalar >
void
KnotvectorT<Scalar>::
createKnots(unsigned int _splineDeg, unsigned int _dim)
{
  num_control_points_ = _dim;
  spline_degree_ = _splineDeg;

  if (knotvectorType_ == UNIFORM)
    createUniformKnots(_splineDeg, _dim);
  else if (knotvectorType_ == UNIFORM_INTERPOL)
    createUniformInterpolatingKnots(_splineDeg, _dim);
};

//-----------------------------------------------------------------------------

template <typename Scalar >
void
KnotvectorT<Scalar>::
addKnot(Scalar _knot)
{
  knots_.push_back(_knot);

  // add available property
  if( selections_available())
    selections_.push_back( false);
}

//-----------------------------------------------------------------------------

template <typename Scalar >
void
KnotvectorT<Scalar>::
insertKnot(unsigned int _index, const Scalar _knot)
{
  assert(_index < knots_.size());
  knots_.insert(knots_.begin()+_index, _knot);

  // add available property
  if( selections_available())
    selections_.insert(selections_.begin()+_index, false);
}

//-----------------------------------------------------------------------------

template< typename Scalar >
void
KnotvectorT<Scalar>::
deleteKnot(unsigned int _index)
{
  assert(_index < knots_.size());
  knots_.erase(knots_.begin()+_index);

  if( selections_available())
    selections_.erase(selections_.begin()+_index);
}

//-----------------------------------------------------------------------------

template< typename Scalar >
void
KnotvectorT<Scalar>::
setKnotvector(const std::vector< Scalar >& _knots)
{
  knots_ = _knots;

  selections_.clear();
  if( selections_available())
    selections_.resize(knots_.size(), false);
}


//-----------------------------------------------------------------------------


template< typename Scalar >
void
KnotvectorT<Scalar>::
createUniformInterpolatingKnots(unsigned int _splineDeg, unsigned int _dim)
{
  knots_.clear();

  float last=0.0;

  for (unsigned int i = 0; i < _splineDeg; i++)
    knots_.push_back(0);  // p+1

  for (int i = 0; i < (int)_dim - (int)_splineDeg + 1; i++) {
    knots_.push_back(i);
    last=i;
  }

  for (unsigned int i = 0; i < _splineDeg; i++)
    knots_.push_back(last); // p+1

  selections_.clear();
  if( selections_available())
    selections_.resize(knots_.size(), false);

//   std::cout << "Added " << knots_.size() << " interpolating knots (deg = " << p << ", dim = " << n+1 << ")" << std::endl;
//   for (unsigned int i = 0; i < knots_.size(); ++i)
//     std::cout << knots_[i] << ", " << std::flush;
//   std::cout << std::endl;
}

//-----------------------------------------------------------------------------

template< typename Scalar >
void
KnotvectorT<Scalar>::
createUniformKnots(unsigned int _splineDeg, unsigned int _dim)
{
  knots_.clear();

  // number of required knots
  int k = _dim + _splineDeg + 1;

  for ( int i = 0; i < k; ++i )
    knots_.push_back(i);

  selections_.clear();
  if( selections_available())
    selections_.resize(knots_.size(), false);

//   std::cout << "Added " << knots_.size() << " uniform knots (deg = " << _splineDeg << ", dim = " << _dim << ")" << std::endl;
//   for (unsigned int i = 0; i < knots_.size(); ++i)
//     std::cout << knots_[i] << ", " << std::flush;
//   std::cout << std::endl;
}


//-----------------------------------------------------------------------------


template< typename Scalar >
template <class PropT>
void
KnotvectorT<Scalar>::
request_prop( unsigned int& _ref_count, PropT& _prop)
{
  if(_ref_count == 0)
  {
    _ref_count = 1;
    _prop.resize(size());
  }
  else ++_ref_count;
}


//-----------------------------------------------------------------------------

template< typename Scalar >
template <class PropT>
void
KnotvectorT<Scalar>::
release_prop( unsigned int& _ref_count, PropT& _prop)
{
  if( _ref_count <= 1)
  {
    _ref_count = 0;
    _prop.clear();
  }
  else --_ref_count;
}


}

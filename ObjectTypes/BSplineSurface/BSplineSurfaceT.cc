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
//  CLASS BSplineSurfaceT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define BSPLINESURFACE_BSPLINESURFACET_C

//== INCLUDES =================================================================

#include <OpenMesh/Core/Geometry/VectorT.hh>

#include <iostream>
#include <fstream>

#include "BSplineSurfaceT.hh"

#include <cfloat>
#include <ACG/Geometry/Algorithms.hh>

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================

template <class PointT>
BSplineSurfaceT<PointT>::
BSplineSurfaceT( unsigned int _degm, unsigned int _degn )
: dimm_(0),
  dimn_(0),
  degree_m_(_degm),
  degree_n_(_degn),
  ref_count_cpselections_(0),
  ref_count_eselections_(0)
{
}

//-----------------------------------------------------------------------------

template <class PointT>
BSplineSurfaceT<PointT>::
BSplineSurfaceT( const BSplineSurfaceT& _surface )
{
  //copy control points
  control_net_ = _surface.control_net_;

  //copy knotvectors
  knotvector_m_ = _surface.knotvector_m_;
  knotvector_n_ = _surface.knotvector_n_;

  degree_m_     = _surface.degree_m_;
  degree_n_     = _surface.degree_n_;

  // copy properties
  cpselections_ = _surface.cpselections_;
  eselections_  = _surface.eselections_;

  // copy property reference counter
  ref_count_cpselections_ = _surface.ref_count_cpselections_;
  ref_count_eselections_  = _surface.ref_count_eselections_;
}

//-----------------------------------------------------------------------------

template <class PointT>
template <class PropT>
void
BSplineSurfaceT<PointT>::
request_prop( unsigned int& _ref_count, PropT& _prop)
{
  if(_ref_count == 0)
  {
    _ref_count = 1;

    // always use vertex size!!!
    _prop.resize(n_control_points_m());
    for (unsigned int i = 0; i < _prop.size(); ++i)
      _prop[i].resize(n_control_points_n());
  }
  else ++_ref_count;
}

//-----------------------------------------------------------------------------

template <class PointT>
template <class PropT>
void
BSplineSurfaceT<PointT>::
release_prop( unsigned int& _ref_count, PropT& _prop)
{
  if( _ref_count <= 1)
  {
    _ref_count = 0;
    _prop.clear();
  }
  else --_ref_count;
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
resize(unsigned int _m, unsigned int _n)
{
  control_net_.resize(_m);

  for (unsigned int i = 0; i < control_net_.size(); ++i)
    control_net_[i].resize(_n);

  dimm_ = _m;
  dimn_ = _n;

  // resize cpselections
  cpselections_.resize(_m);
  for (unsigned int i = 0; i < cpselections_.size(); ++i)
    cpselections_[i].resize(_n);

  // resize eselections
  eselections_.resize(_m);
  for (unsigned int i = 0; i < eselections_.size(); ++i)
    eselections_[i].resize(_n);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
reset_control_net()
{
  control_net_.clear();

  // reset properties
  cpselections_.clear();
  eselections_.clear();
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
createKnots()
{
  knotvector_m_.createKnots(degree_m_, dimm_);
  knotvector_n_.createKnots(degree_n_, dimn_);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
set_degree(unsigned int _degm, unsigned int _degn)
{
  degree_m_ = _degm;
  degree_n_ = _degn;
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
add_vector_m(const std::vector< Point> & _control_polygon)
{
  insert_vector_m(_control_polygon, dimm_);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
add_vector_n(const std::vector< Point> & _control_polygon)
{
  insert_vector_n(_control_polygon, dimn_);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
insert_vector_m(const std::vector< Point> & _control_polygon, unsigned int _m)
{
  std::cout << "insert_vector_m of size " << _control_polygon.size() << " at m = " << _m << std::endl;

  assert(_m <= dimm_);
  if (dimn_ == 0)
    dimn_ =_control_polygon.size();

  assert(_control_polygon.size() == dimn_);

  resize(dimm_ + 1, dimn_);

  control_net_.insert(control_net_.begin() + _m, _control_polygon);
  control_net_.pop_back(); // TODO check
  std::cout << "control_net_: " << control_net_.size() << " x " << control_net_[control_net_.size()-1].size() << std::endl;

  // resize property net cpselection
  std::vector<unsigned char> dummy(_control_polygon.size(), 0);
  cpselections_.insert(cpselections_.begin() + _m, dummy);
  cpselections_.pop_back();
  std::cout << "cpselections_: " << cpselections_.size() << " x " << cpselections_[cpselections_.size()-1].size() << std::endl;

  // resize property net eselection
  eselections_.insert(eselections_.begin() + _m, dummy);
  eselections_.pop_back();
  std::cout << "eselections_: " << eselections_.size() << " x " << eselections_[eselections_.size()-1].size() << std::endl;
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
insert_vector_n(const std::vector< Point> & _control_polygon, unsigned int _n)
{
  assert(_n <= dimn_);
  if (dimm_ == 0)
    dimm_ = _control_polygon.size();

  assert(_control_polygon.size() == dimm_);

  resize(dimm_, dimn_+1);

  for (unsigned int i = 0; i < dimm_; ++i)
  {
    control_net_[i].insert(control_net_[i].begin() + _n, _control_polygon[i]);
    control_net_[i].pop_back();
  }

  // resize property net cpselection
  for (unsigned int i = 0; i < dimm_; ++i)
  {
    cpselections_[i].insert(cpselections_[i].begin() + _n, 0);
    cpselections_[i].pop_back();
  }
  // resize property net eselection
  for (unsigned int i = 0; i < dimm_; ++i)
  {
    eselections_[i].insert(eselections_[i].begin() + _n, 0);
    eselections_[i].pop_back();
  }
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
delete_vector_m(unsigned int _m)
{
  assert(_m < dimm_);
  
  /// \todo Improve deletion routine for control points and knots!

  if(control_net_.begin() + _m < control_net_.end())
      control_net_.erase(control_net_.begin() + _m);

  resize(dimm_-1, dimn_);

  // erase from properties
  if(cpselections_.begin() + _m < cpselections_.end())
      cpselections_.erase(cpselections_.begin() + _m);
  
  if(eselections_.begin() + _m < eselections_.end())
      eselections_.erase(eselections_.begin() + _m);
  
  // Now rebuild knot vectors
  createKnots();
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
delete_vector_n(unsigned int _n)
{
  assert(_n < dimn_);
  
  /// \todo: Improve deletion routine for control points and knots!

  for (unsigned int i = 0; i < control_net_.size(); ++i) {
      if(control_net_[i].begin() + _n < control_net_[i].end())
          control_net_[i].erase(control_net_[i].begin() + _n);
  }

  resize(dimm_, dimn_-1);

  // erase from properties
  for (unsigned int i = 0; i < cpselections_.size(); ++i)
      if(cpselections_[i].begin() + _n < cpselections_[i].end())
          cpselections_[i].erase(cpselections_[i].begin() + _n);

  for (unsigned int i = 0; i < eselections_.size(); ++i)
      if(eselections_[i].begin() + _n < eselections_[i].end())
          eselections_[i].erase(eselections_[i].begin() + _n);
      
  // Now rebuild knot vectors
  createKnots();
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
get_vector_m(std::vector< Point> & _control_polygon, unsigned int _m)
{
  assert(_m < dimm_);
  _control_polygon = control_net_[_m];
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
get_vector_n(std::vector< Point> & _control_polygon, unsigned int _n)
{
  assert(_n < dimn_);
  _control_polygon.resize(dimm_);

  for (unsigned int i = 0; i < dimm_; ++i)
    _control_polygon[i] = control_net_[i][_n];
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
set_knots_m(std::vector< Scalar > _knots)
{
  // set the knotvector
  knotvector_m_.setKnotvector(_knots);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
set_knots_n(std::vector< Scalar > _knots)
{
  // set the knotvector
  knotvector_n_.setKnotvector(_knots);
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
insert_knot_m(double _u)
{
  // span and interval i,i+1
  Vec2i span = spanm(_u);
  Vec2i interval = interval_m(_u);

  // create new knot vector
  Knotvector newknotvecu( get_knotvector_m() );
  newknotvecu.insertKnot(interval[1], _u);

  // alphas
  std::vector<double> alpha;
  for( int i = span[0]; i < span[1]; ++i)
  {
    double a(knotvector_m_.getKnot(i+1));
    double b(knotvector_m_.getKnot(i+degree_m_+1));
    alpha.push_back((_u-a)/(b-a));
  }
  knotvector_m_ = newknotvecu;

  // new control net
  ControlNet oldcpts(control_net_);

  resize(n_control_points_m()+1, n_control_points_n());

  for( int i = 0; i < n_control_points_m(); ++i) // for all v rows
  {
    if( i <= span[0])
      control_net_[i] = oldcpts[i];
    else if( i <= span[1])
      for( unsigned int j = 0; j < n_control_points_n(); ++j)
      {
        control_net_[i][j] = oldcpts[i-1][j]*(1.0-alpha[i-span[0]-1])+oldcpts[i][j]*alpha[i-span[0]-1];
      }
    else
      control_net_[i] = oldcpts[i-1];
  }
}

//-----------------------------------------------------------------------------

template <class PointT>
void
BSplineSurfaceT<PointT>::
insert_knot_n(double _v)
{
  // span and interval i,i+1
  Vec2i span = spann(_v);
  Vec2i interval = interval_n(_v);

  // create new knot vector
  Knotvector newknotvecv( get_knotvector_n() );
  newknotvecv.insertKnot(interval[1], _v);

  // alphas
  std::vector<double> alpha;
  for( int i = span[0]; i < span[1]; ++i)
  {
    double a(knotvector_n_.getKnot(i+1));
    double b(knotvector_n_.getKnot(i+degree_n_+1));
    alpha.push_back((_v-a)/(b-a));
  }
  knotvector_n_ = newknotvecv;

  // new control net
  ControlNet oldcpts(control_net_);

  resize(n_control_points_m(), n_control_points_n()+1);

  for( int i = 0; i < n_control_points_n(); ++i) // for all v rows
  {
    if( i <= span[0])
      for( unsigned int j = 0; j < n_control_points_m(); ++j)
        control_net_[j][i] = oldcpts[j][i];
    else if( i <= span[1])
      for( unsigned int j = 0; j < n_control_points_m(); ++j)
      {
        control_net_[j][i] = oldcpts[j][i-1]*(1.0-alpha[i-span[0]-1])+oldcpts[j][i]*alpha[i-span[0]-1];
      }
    else
      for( unsigned int j = 0; j < n_control_points_m(); ++j)
        control_net_[j][i] = oldcpts[j][i-1];
  }
}

//-----------------------------------------------------------------------------

template <class PointT>
PointT
BSplineSurfaceT<PointT>::
surfacePoint(double _u, double _v)
{
  double epsilon = 0.0000001;

  if (_u > upperu() && _u < upperu()+epsilon)
    _u = upperu();

  if (_v > upperv() && _v < upperv()+epsilon)
    _v = upperv();

  assert(_u >= loweru() && _u <= upperu());
  assert(_v >= lowerv() && _v <= upperv());

  int pm = degree_m();
  int pn = degree_n();

  Point point = Point(0.0, 0.0, 0.0);

  Vec2i span_m(spanm(_u));
  Vec2i span_n(spann(_v));

  for (int i = 0; i <  (int)n_control_points_m(); ++i)
    for (int j = 0; j <  (int)n_control_points_n(); ++j)
      point += control_net_[i][j] * basisFunction(knotvector_m_, i, pm, _u) * basisFunction(knotvector_n_, j, pn, _v);

  return point;
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
basisFunction(Knotvector & _knotvector, int _i, int _n, double _t)
{
  int m = _knotvector.size() - 1;

  // Mansfield Cox deBoor recursion
  if ((_i==0 && _t== _knotvector(0)) || (_i==m-_n-1 && _t==_knotvector(m)))
    return 1.0;

  if (_n == 0) {
    if (_t >= _knotvector(_i) && _t < _knotvector(_i+1))
      return 1.0;
    else
      return 0.0;
  }

  double Nin1 = basisFunction(_knotvector, _i, _n-1, _t);
  double Nin2 = basisFunction(_knotvector, _i+1, _n-1, _t);

  double fac1 = 0;
//   if ((_knotvector(_i+_n) - _knotvector(_i)) > 0.000001 )
  if ((_knotvector(_i+_n) - _knotvector(_i)) != 0)
    fac1 = (_t - _knotvector(_i)) / (_knotvector(_i+_n) - _knotvector(_i)) ;

  double fac2 = 0;
//   if ( (_knotvector(_i+1+_n) - _knotvector(_i+1)) > 0.000001 )
  if ( (_knotvector(_i+1+_n) - _knotvector(_i+1)) != 0 )
    fac2 = (_knotvector(_i+1+_n) - _t) / (_knotvector(_i+1+_n) - _knotvector(_i+1));

//   std::cout << "Nin1 = " << Nin1 << ", Nin2 = " << Nin2 << ", fac1 = " << fac1 << ", fac2 = " << fac2 << std::endl;

  return (fac1*Nin1 + fac2*Nin2);
}

//-----------------------------------------------------------------------------

template <class PointT>
PointT
BSplineSurfaceT<PointT>::
derivativeSurfacePoint(double _u, double _v, int _derm, int _dern)
{
  assert(_u >= loweru() && _u <= upperu());
  assert(_v >= lowerv() && _v <= upperv());

  int pn = degree_n();
  int pm = degree_m();

  Point point(0,0,0);

  Vec2i span_m(spanm(_u));
  Vec2i span_n(spann(_v));


//   for (int i = 0; i < n_control_points_m(); i++)
//     for (int j = 0; j < n_control_points_n(); j++)
//       point += control_net_[i][j] * derivativeBasisFunction(knotvector_m_, i, pm, _u, _derm)
//                                   * derivativeBasisFunction(knotvector_n_, j, pn, _v, _dern);

  for (int i = span_m[0]; i <= span_m[1]; i++)
    for (int j = span_n[0]; j <= span_n[1]; j++)
      point += control_net_[i][j] * derivativeBasisFunction(knotvector_m_, i, pm, _u, _derm)
                                  * derivativeBasisFunction(knotvector_n_, j, pn, _v, _dern);

  return point;
}

//-----------------------------------------------------------------------------

template <class PointT>
PointT
BSplineSurfaceT<PointT>::
normalSurfacePoint(double _u, double _v)
{
  assert(_u >= loweru() && _u <= upperu());
  assert(_v >= lowerv() && _v <= upperv());

  Point derivu( derivativeSurfacePoint(_u,_v,1,0));
  Point derivv( derivativeSurfacePoint(_u,_v,0,1));

  Point normal( (derivu%derivv).normalize());

  return normal;
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
derivativeBasisFunction(Knotvector & _knotvector, int _i, int _n, double _t, int _der)
{
  if (_der == 0)
    return basisFunction(_knotvector, _i, _n, _t);

  double Nin1 = derivativeBasisFunction(_knotvector, _i,   _n-1, _t, _der-1);
  double Nin2 = derivativeBasisFunction(_knotvector, _i+1, _n-1, _t, _der-1);

  double fac1 = 0;
  if ( (_knotvector(_i+_n)-_knotvector(_i)) !=0 )
    fac1 = double(_n) / (_knotvector(_i+_n)-_knotvector(_i));

  double fac2 = 0;
  if ( (_knotvector(_i+_n+1)-_knotvector(_i+1)) !=0 )
    fac2 = double(_n) / (_knotvector(_i+_n+1)-_knotvector(_i+1));

  return (fac1*Nin1 - fac2*Nin2);
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
loweru()
{
  return knotvector_m_(degree_m());
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
upperu() {
  return knotvector_m_(knotvector_m_.size() - 1 - degree_m());
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
lowerv()
{
  return knotvector_n_(degree_n());
}

//-----------------------------------------------------------------------------

template <class PointT>
typename BSplineSurfaceT<PointT>::Scalar
BSplineSurfaceT<PointT>::
upperv() {
  return knotvector_n_(knotvector_n_.size() - 1 - degree_n());
}

//-----------------------------------------------------------------------------

template <class PointT>
ACG::Vec2i
BSplineSurfaceT<PointT>::
spanm(double _t)
{
  unsigned int i(0);

//   if (i > knotvector_m_.size() - degree_m() - 1)
//     i = degree_m();

  if (_t >= upperu())
    i = dimm_ - 1;
  else
  {
    while (_t >= knotvector_m_(i)) i++;
    while (_t < knotvector_m_(i))  i--;
  }

  return ACG::Vec2i(i-degree_m(), i);
}

//-----------------------------------------------------------------------------

template <class PointT>
ACG::Vec2i
BSplineSurfaceT<PointT>::
spann(double _t)
{
  unsigned int i(0);

//   if (i > knotvector_n_.size() - degree_n() - 1)
//     i = degree_n();

  if (_t >= upperv())
    i = dimn_-1;
  else
  {
    while (_t >= knotvector_n_(i)) i++;
    while (_t < knotvector_n_(i))  i--;
  }

  return Vec2i(i-degree_n(), i);
}

//-----------------------------------------------------------------------------

template <class PointT>
ACG::Vec2i
BSplineSurfaceT<PointT>::
interval_m(double _t)
{
  Vec2i interval = Vec2i(-1, -1);

  unsigned int i(0);

  if (_t >= upperu())
    i = dimm_-1;
  else
  {
    while (_t >= knotvector_m_(i)) i++;
    while (_t <  knotvector_m_(i)) i--;
  }

  return Vec2i(i, i+1);
}

//-----------------------------------------------------------------------------

template <class PointT>
ACG::Vec2i
BSplineSurfaceT<PointT>::
interval_n(double _t)
{
  Vec2i interval = Vec2i(-1, -1);

  unsigned int i(0);

  if (_t >= upperv())
    i = dimn_-1;
  else
  {
    while (_t >= knotvector_n_(i)) i++;
    while (_t <  knotvector_n_(i)) i--;
  }
  
  return Vec2i(i, i+1);
}

//-----------------------------------------------------------------------------

//=============================================================================
} // namespace ACG
//=============================================================================

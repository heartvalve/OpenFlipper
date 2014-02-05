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
//  CLASS BSplineSurfaceT
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef BSPLINESURFACET_HH
#define BSPLINESURFACET_HH


//== INCLUDES =================================================================
#include <vector>
#include <iostream>

#include <ACG/Math/VectorT.hh>

#include <ObjectTypes/Knotvector/KnotvectorT.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class BSplineSurfaceT BSplineSurfaceT.hh <ObjectTypes/BSplineSurface/BSplineSurfaceT.hh>

  Brief Description.

  A more elaborate description follows.
 */

template <class PointT>
class BSplineSurfaceT
{
public:

  // internal relevant Types
  typedef PointT                      Point;
  typedef typename Point::value_type  Scalar;
  typedef typename std::vector< std::vector< Point > > ControlNet;
  typedef typename std::vector< std::vector< unsigned char > > PropertyNet;

  /** \brief Constructor
   *
   * @param _degm Degree in m direction
   * @param _degn Degree in n direction
   */
  BSplineSurfaceT(unsigned int _degm = 3, unsigned int _degn = 3);

  /** \brief Copy Constructor
   *
   * @param _surface Original Survare
   */
  BSplineSurfaceT(const BSplineSurfaceT& _surface);

  /// Destructor
  ~BSplineSurfaceT() {};

  /** \brief Resizes the spline struct.
   *
   * You usually shoudn't have to care about this I will probably make it private...
   * @param _m size in m direction
   * @param _n size in n direction
   */
  void resize(unsigned int _m, unsigned int _n);

  /// get the knotvector in m direction of the bspline surface
  std::vector< Scalar >& get_knots_m() {return knotvector_m_.getKnotvector();};
  /// get the knotvector in m direction of the bspline surface
  std::vector< Scalar >& get_knots_n() {return knotvector_n_.getKnotvector();};

  /// Get the knotvector in m direction
  Knotvector get_knotvector_m(){return knotvector_m_;};
  /// Get the knotvector in n direction
  Knotvector get_knotvector_n(){return knotvector_n_;};

  /// Get a reference to the knotvector in m direction
  Knotvector * get_knotvector_m_ref(){return &knotvector_m_;};
  /// Get a reference to the knotvector in n direction
  Knotvector * get_knotvector_n_ref(){return &knotvector_n_;};

  /** \brief Get knot i in m direction
   *
   * @param _i Knot index
   * @return Knot i
   */
  Scalar get_knot_m(int _i) {return knotvector_m_(_i);};

  /** \brief Get knot i in n direction
   *
   * @param _i Knot index
   * @return Knot i
   */
  Scalar get_knot_n(int _i) {return knotvector_n_(_i);};

  /** \brief Set the knotvector of the bspline surface in m direction
   *
   * @param _knots The new knots
   */
  void set_knots_m(std::vector< Scalar > _knots);

  /** \brief Set the knotvector of the bspline surface in n direction
     *
     * @param _knots The new knots
     */
  void set_knots_n(std::vector< Scalar > _knots);

  /** \brief Insert a knot i in m direction without changing the surface
   *
   * @param _t Parameter value
   */
  void insert_knot_m(double _t);

  /** \brief Insert a knot i in n direction without changing the surface
   *
   * @param _t Parameter value
   */
  void insert_knot_n(double _t);

  /// Creates interpolating knotvectors 0...0, 1, 2, ..., n...n
  void createKnots();

  /** \brief Sets the degree of the spline surface
   *
   * @param _degm Degree in m direction
   * @param _degn Degree in n direction
   */
  void set_degree(unsigned int _degm, unsigned int _degn);


  /// Returns the spline degree in m direction
  int degree_m() const {return degree_m_;}
  /// Returns the spline degree in n direction
  int degree_n() const {return degree_n_;}

  /// Returns the number of controlpoints in m direction
  unsigned int n_control_points_m() const {return dimm_;}
  /// Returns the number of controlpoints in n direction
  unsigned int n_control_points_n() const {return dimn_;}

  /// Returns the number of knots in m direction
  unsigned int n_knots_m(){return knotvector_m_.size();};
  /// Returns the number of knots in n direction
  unsigned int n_knots_n(){return knotvector_n_.size();};


  /// Clears the control net
  void reset_control_net();

  /** \brief Returns a reference to the control point (m, n)
   * @param _m, _n the control point index
   */
  Point& get_control_point(unsigned int _m, unsigned int _n)
  {
    assert (_m < dimm_ && _n < dimn_);
    return control_net_[_m][_n];
  }

  /** \brief Returns a reference to the control point (m, n)
   *
   * @param _m, _n the control point index
   */
  inline Point& operator()(unsigned int _m, unsigned int _n)
  {
    assert (_m < dimm_ && _n < dimn_);
    return control_net_[_m][_n];
  }

  /** \brief Returns a const reference to the control point (m, n)
   *
   * @param _m, _n the control point index
   */
  inline const Point& operator()(unsigned int _m, unsigned int _n) const
  {
    assert (_m < dimm_ && _n < dimn_);
    return control_net_[_m][_n];
  }

  /** \brief Returns a reference to the control point (m, n)
   *
   * @param _param the control point index
   */
  inline Point& operator()(Vec2i _param)
  {
    assert (0 <= _param[0] < dimm_ && 0 <= _param[1] < dimn_);
    return control_net_[_param[0]][_param[1]];
  }

  /** \brief Returns a const reference to the control point (m, n)
   *
   * @param _param the control point index
   */
  inline const Point& operator()(Vec2i _param) const
  {
    assert (0 <= _param[0] < dimm_ && 0 <= _param[1] < dimn_);
    return control_net_[_param[0]][_param[1]];
  }


  /** \brief Returns an n control point vector
   *
   * @param _m the m'th pointvector
   * @param _control_polygon the returned pointvector
   */
  void get_vector_m(std::vector< Point> & _control_polygon, unsigned int _m);

  /** \brief Returns an m ctrPointVector
   *
   * @param _n the n'th pointvector
   * @param _control_polygon the returned pointvector
   */
  void get_vector_n(std::vector< Point> & _control_polygon, unsigned int _n);


  /** \brief Adds a control point n-vector
   *
   * @param _control_polygon Additional control point vector
   */
  void add_vector_m(const std::vector< Point> & _control_polygon);

  /** \brief Adds a control point m-vector
   *
   * @param _control_polygon Additional control point vector
   */
  void add_vector_n(const std::vector< Point> & _control_polygon);


  /** \brief Inserts an n control point vector
   *
   * @param _m the position in m direction
   * @param _control_polygon the pointvector to be inserted
   */
  void insert_vector_m(const std::vector< Point> & _control_polygon, unsigned int _m);

  /** \brief Inserts an m control point vector
   * @param _n the position in n direction
   * @param _control_polygon the pointvector to be inserted
   */
  void insert_vector_n(const std::vector< Point> & _control_polygon, unsigned int _n);

  /** \brief Deletes an n control point vector
   *
   * @param _m the m'th pointvector
   */
  void delete_vector_m(unsigned int _m);

  /** \brief Deletes an m control point vector
   *
   * @param _n the n'th pointvector
   */
  void delete_vector_n(unsigned int _n);


  /** \brief Evaluates a spline surface at parameters _u and _v
   *
   * @param _u the parameter in u direction
   * @param _v the parameter in v direction
   * \return the surface point
  */
  Point surfacePoint(double _u, double _v);


  /** \brief Returns the _derm'th derivative of a spline surface
   *
   * @param _u the parameter in u direction
   * @param _v the parameter in v direction
   * @param _derm the _derm'th derivative in m
   * @param _dern the _dern'th derivative in n
   * \return the derivative
   */
  Point derivativeSurfacePoint(double _u, double _v, int _derm, int _dern);

  /** \brief Returns the normal of a spline surface
   *
   * @param _u the parameter in u direction
   * @param _v the parameter in v direction
   * \return the normal
   */
  Point normalSurfacePoint(double _u, double _v);
public:

  /** \brief A Spline Basis Function
   *
   * @param _knotvector the knotvector
   * @param _i the i'th basis function
   * @param _n the spline degree
   * @param _t the parameter
   * Returns N_i^p(_u)
  */
  Scalar basisFunction(Knotvector & _knotvector, int _i, int _n, double _t);

  /** \brief Derivative of a Spline Basis Function
   *
   * @param _knotvector the knotvector
   * @param _i the i'th basis function
   * @param _n the spline degree
   * @param _t the parameter
   * @param _der the _der'th derivative
   */
  Scalar derivativeBasisFunction(Knotvector & _knotvector, int _i, int _n, double _t, int _der);

  /** \brief Returns the basis functions which are unequal to zero at parameter u
   *
   * @param _t the parameter
   */
  ACG::Vec2i spanm(double _t);

  /** \brief Returns the basis functions which are unequal to zero at parameter v
   *
   * @param _t the parameter
   */
  ACG::Vec2i spann(double _t);

  /** \brief Returns the index of the knots u and u+1 such that t in [u, u+1)
   *
   * @param _t the parameter
   */
  ACG::Vec2i interval_m(double _t);

  /** \brief  Returns the index of the knots v and v+1 such that t in [v, v+1)
   *
   * @param _t the parameter
   */
  ACG::Vec2i interval_n(double _t);


public:

  /// Returns the lower u parameter
  Scalar loweru();
  /// Returns the upper u parameter
  Scalar upperu();

  /// Returns the lower v parameter
  Scalar lowerv();
  /// Returns the upper v parameter
  Scalar upperv();


public :

  /// request control point selection property
  void request_controlpoint_selections() { request_prop( ref_count_cpselections_, cpselections_);}
  /// request edge selection property
  void request_edge_selections()         { request_prop( ref_count_eselections_, eselections_);}

  /// release control point selection property
  void release_controlpoint_selections() { release_prop( ref_count_cpselections_, cpselections_);}
  /// release edge selection property
  void release_edge_selections()         { release_prop( ref_count_eselections_, eselections_);}

  /// Check if control point selection property is available
  bool controlpoint_selections_available() const {return bool(ref_count_cpselections_);}
  /// Check if edge selection property is available
  bool edge_selections_available()         const {return bool(ref_count_eselections_);}


  // property access ( no range or availability check! )
        unsigned char& controlpoint_selection(unsigned int _i, unsigned int _j)       {return cpselections_[_i][_j];}
  const unsigned char& controlpoint_selection(unsigned int _i, unsigned int _j) const {return cpselections_[_i][_j];}

//         unsigned char& controlpoint_selection(unsigned int _idx)       {return cpselections_[_idx];}
//   const unsigned char& controlpoint_selection(unsigned int _idx) const {return cpselections_[_idx];}

        unsigned char& edge_selection(unsigned int _i, unsigned int _j)       {return eselections_[_i][_j];}
  const unsigned char& edge_selection(unsigned int _i, unsigned int _j) const {return eselections_[_i][_j];}

//  /// acces with row / col indices
//         unsigned char& controlpoint_selection(unsigned int _i, unsigned int _j)       {int idx = _i * dimn_ + _j; return cpselections_[idx];}
//   const unsigned char& controlpoint_selection(unsigned int _i, unsigned int _j) const {int idx = _i * dimn_ + _j; return cpselections_[idx];}

//  /// access with global idx
//         unsigned char& controlpoint_selection(unsigned int _idx)       {return cpselections_[_idx];}
//   const unsigned char& controlpoint_selection(unsigned int _idx) const {return cpselections_[_idx];}

//         unsigned char& edge_selection(unsigned int _i, unsigned int _j)       {int idx = _i * dimn_ + _j; return eselections_[idx];}
//   const unsigned char& edge_selection(unsigned int _i, unsigned int _j) const {int idx = _i * dimn_ + _j; return eselections_[idx];}

  // Wrapper for selection functions
  void select_controlpoint(unsigned int _iIdx, unsigned int _jIdx) { controlpoint_selection(_iIdx, _jIdx) = 1; };
  void deselect_controlpoint(unsigned int _iIdx, unsigned int _jIdx) { controlpoint_selection(_iIdx, _jIdx) = 0; };
  
  bool controlpoint_selected(unsigned int _iIdx, unsigned int _jIdx) const { return (controlpoint_selection(_iIdx, _jIdx) == 1); };


private:

  template <class PropT>
  void request_prop( unsigned int& _ref_count, PropT& _prop);

  template <class PropT>
  void release_prop( unsigned int& _ref_count, PropT& _prop);

private:

  unsigned int dimm_;     ///< number of control points in m direction
  unsigned int dimn_;     ///< number of control points in n direction
  unsigned int degree_m_; ///< Spline degree in m direction
  unsigned int degree_n_; ///< Spline degree in n direction

  Knotvector knotvector_m_; ///< Knotvector in m direction
  Knotvector knotvector_n_; ///< Knotvector in n direction

  ControlNet control_net_;


private: // private properties

  // ############################### Standard Property Handling #############################

//   // list of vertex properties
//   std::vector< std::vector<unsigned char> > vselections_;
//   // list of edge properties
//   std::vector< std::vector<unsigned char> > eselections_;

  /// list of control point  properties
//   std::vector<unsigned char> cpselections_;
  PropertyNet cpselections_;

  /// list of edge properties
//   std::vector<unsigned char> eselections_;
  PropertyNet eselections_;

  // property reference counter
  unsigned int ref_count_cpselections_;
  unsigned int ref_count_eselections_;

};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(BSPLINESURFACE_BSPLINESURFACET_C)
#define BSPLINESURFACE_BSPLINESURFACET_TEMPLATES
#include "BSplineSurfaceT.cc"
#endif
//=============================================================================
#endif // ACG_BSPLINESURFACET_HH defined
//=============================================================================


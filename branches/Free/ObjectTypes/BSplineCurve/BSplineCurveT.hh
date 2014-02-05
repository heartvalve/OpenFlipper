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
//  CLASS BSplineCurveT
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef BSPLINECURVET_HH
#define BSPLINECURVET_HH


//== INCLUDES =================================================================
#include <vector>
#include <iostream>

#include <ACG/Math/VectorT.hh>

#include <ObjectTypes/Knotvector/KnotvectorT.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class BSplineCurveT BSplineCurveT.hh <ACG/.../BSplineCurveT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class PointT>
class BSplineCurveT
{
public:

  // internal relevant Types
  typedef PointT                      Point;
  typedef typename Point::value_type  Scalar;

  /// Constructor
  BSplineCurveT(unsigned int _degree = 3);

  /// Copy Constructor
  BSplineCurveT(const BSplineCurveT& _curve);

  /// Destructor
  ~BSplineCurveT() {}

  /// get the knotvector of the bspline curve
  std::vector< Scalar >& get_knots() {return knotvector_.getKnotvector();};

  /// set the knotvector of the bspline curve
  void set_knots(std::vector< Scalar > _knots);

  /// get a reference to the knotvector
  Knotvector * get_knotvector_ref(){return &knotvector_;};

  /// set type of knotvector
  void set_knotvector_type(Knotvector::KnotvectorType _type) {knotvector_.setType(_type);};

  /// get knot i
  Scalar get_knot(int _i) {return knotvector_(_i);};

  /// get control point i
  Point& get_control_point(int _i) {return control_polygon_[_i];};

  /// add a control point
  void add_control_point(const Point& _cp);

  /// insert a control point at given index
  void insert_control_point(int _idx, const Point& _cp);

  /// delete control point at given index
  void delete_control_point(int _idx);

  /// reset a control point
  void set_control_point(int _idx, const Point& _cp);

  /// set whole control polygon
  void set_control_polygon(std::vector< Point> & _control_polygon);

  /// Clears the control polygon
  void reset_control_polygon();

  /// print information string
  void print() const;

  /**
   * Evaluates a spline curve at parameter _u
   * \param _u the parameter
   * \return the curve point
   */
  Point curvePoint( Scalar _u );

  /**
   * Returns the _derm'th derivative of a spline curve
   * \param _u the parameter
   * \param _der the _derm'th derivative
   * \return the derivative
   */
  Point derivativeCurvePoint(Scalar _u, unsigned int _der);

  /**
   * Evaluates the curve at parameter u using deBoor algorithm.
   * \param _u the parameter
   * \return triangular array of control points
   */
  std::vector<Point> deBoorAlgorithm(double _u);

  /// Inserts a new knot at parameter u
  void insertKnot(double _u);

  /// Returns the number of control points
  unsigned int n_control_points() const {return control_polygon_.size();};

  /// Returns the number of knots
  unsigned int n_knots() { return knotvector_.size(); };

  /// Returns the spline degree
  unsigned int degree() {return degree_;};

  /// Sets the spline degree
  void set_degree(unsigned int _degree) {degree_ = _degree;};

  /**
   * returns the basis functions which are unequal to zero at parameter t
   * \param _t the parameter
   */
  ACG::Vec2i span(double _t);

  /**
   * returns the index of the knots u and u+1 such that t in [u, u+1)
   * \param _t the parameter
   */
  ACG::Vec2i interval(double _t);
  
  void autocompute_knotvector(bool _auto) {autocompute_knotvector_ = _auto;};

  void fixNumberOfControlPoints(bool _fix) {fix_number_control_points_ = _fix;};

  /** \brief projected
   *
   * \todo This is a hack actually. review
   */
  bool projected() {return fix_number_control_points_;};

  /// Reverses the curve
  void reverse();


public:

  /**
   * A Spline Basis Function
   * \param _i the i'th basis function
   * \param _n the spline degree
   * \param _t the parameter
   * Returns N_i^p(_u)
   */
  Scalar basisFunction(int _i, int _n, Scalar _t);

  /**
   * Derivative of a Spline Basis Function
   * \param _i the i'th basis function
   * \param _n the spline degree
   * \param _t the parameter
   * \param _der the _der'th derivative
   */
  Scalar derivativeBasisFunction(int _i, int _n, Scalar _t, int _der);


private: // private functions

  /// Returns the lower parameter
  Scalar lower();

  /// Returns the upper parameter
  Scalar upper();


public :

  // request properties
  void request_controlpoint_selections() { request_prop( ref_count_cpselections_, cpselections_);}
  void request_edge_selections()   { request_prop( ref_count_eselections_, eselections_);}

  // release properties
  void release_controlpoint_selections() { release_prop( ref_count_cpselections_, cpselections_);}
  void release_edge_selections()   { release_prop( ref_count_eselections_, eselections_);}

  // property availability
  bool controlpoint_selections_available() const {return bool(ref_count_cpselections_);}
  bool edge_selections_available()   const {return bool(ref_count_eselections_);}


  // property access ( no range or availability check! )
  unsigned char& controlpoint_selection(unsigned int _i){
    assert(_i < n_control_points());
    assert(controlpoint_selections_available());
    return cpselections_[_i];
  }
  const unsigned char& controlpoint_selection(unsigned int _i) const {
    assert(_i < n_control_points());
    assert(controlpoint_selections_available());
    return cpselections_[_i];
  }

  unsigned char& edge_selection(unsigned int _i) {
    assert(edge_selections_available());
    return eselections_[_i];
  }
  const unsigned char& edge_selection(unsigned int _i) const {
    assert(edge_selections_available());
    return eselections_[_i];
  }
  
  // Wrapper for selection functions
  void select_controlpoint(unsigned int _pIdx) { controlpoint_selection(_pIdx) = 1; };
  void deselect_controlpoint(unsigned int _pIdx) { controlpoint_selection(_pIdx) = 0; };
  
  bool controlpoint_selected(unsigned int _pIdx) const { return (controlpoint_selection(_pIdx) == 1); };
  
  void select_edge(unsigned int _pIdx) { edge_selection(_pIdx) = 1; };
  void deselect_edge(unsigned int _pIdx) { edge_selection(_pIdx) = 0; };
  
  bool edge_selected(unsigned int _pIdx) const { return (edge_selection(_pIdx) == 1); };

private:

  template <class PropT>
  void request_prop( unsigned int& _ref_count, PropT& _prop);

  template <class PropT>
  void release_prop( unsigned int& _ref_count, PropT& _prop);


private: // private objects

  std::vector<Point> control_polygon_;

  Knotvector knotvector_;

  unsigned int degree_;

  bool autocompute_knotvector_;

  bool fix_number_control_points_;

private: // private properties

  // ############################### Standard Property Handling #############################

  // list of vertex properties
  std::vector<unsigned char>   cpselections_;

  // list of edge properties
  std::vector<unsigned char>   eselections_;

  // property reference counter
  unsigned int ref_count_cpselections_;
  unsigned int ref_count_eselections_;

};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(BSPLINECURVE_BSPLINECURVET_C)
#define BSPLINECURVE_BSPLINECURVET_TEMPLATES
#include "BSplineCurveT.cc"
#endif
//=============================================================================
#endif // ACG_BSPLINECURVET_HH defined
//=============================================================================


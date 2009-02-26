//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS BezierCurveT
//
//=============================================================================


#ifndef ACG_BEZIERCURVE_HH
#define ACG_BEZIERCURVE_HH


//== INCLUDES =================================================================


#include "VectorT.hh"
#include <vector>


//== NAMESPACES  ==============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================


/** Bezier curve. Derived from std::vector<Vector<Scalar, Dimension>>.
 */

template <class Point>
class BezierCurveT : public std::vector<Point>
{
public:

  typedef typename Point::value_type  Scalar;
  typedef BezierCurveT<Point>         Self;
  typedef std::vector<Point>          Base;


  /// constructor
  BezierCurveT() {}

  /// destructor
  ~BezierCurveT() {}


  /// return degree (= size()-1)
  unsigned int degree() const { return Base::size()-1; }


  /// evaluate curve at parameter _t using deCasteljau
  Point operator()(Scalar _t) const;


  /** subdivide curve at parameter _t, store the two resulting
      curves in _curve0, _curve1
  */
  void subdivide(Scalar _t, Self& _curve0, Self& _curve1) const;

};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_BEZIERCURVE_C)
#define ACG_BEZIERCURVE_TEMPLATES
#include "BezierCurveT.cc"
#endif
//=============================================================================
#endif // ACG_BEZIERCURVE_HH defined
//=============================================================================


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
//  CLASS KnotvectorT
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_KNOTVECTORT_HH
#define ACG_KNOTVECTORT_HH

//== INCLUDES =================================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/**
 * A knotvector class for use with splines
 */
template< typename Scalar >
class KnotvectorT {

public:

  enum KnotvectorType {
    UNIFORM_INTERPOL = 0,
    UNIFORM = 1,
  };

  /// Constructor
  KnotvectorT();

  /// Copy Constructor
  KnotvectorT(const KnotvectorT& _knotvec);

  /// Destructor
  ~KnotvectorT();

  void setType(KnotvectorType _type);

  void createKnots(unsigned int _splineDeg, unsigned int _dim);

  inline std::vector< Scalar >& getKnotvector() {return knots_;}

  inline unsigned int size() {return knots_.size();}

  inline Scalar getKnot(unsigned int _index) {
    assert(_index < knots_.size());
    return knots_.at(_index);
  }

  void setKnotvector(const std::vector< Scalar >& _knots);

  void resize(unsigned int _size) {knots_.resize(_size);}

  void insertKnot(unsigned int _index, const Scalar _knot);

  void addKnot(Scalar _knot);

  inline void setKnot(unsigned int _index, Scalar _knot) {
    assert(_index < knots_.size());
    knots_.at(_index) = _knot;
  }

  void deleteKnot(unsigned int _index);


  /** \brief returns a reference to the _index'th knot
   *
   * @param _index The knot index
   */
  inline Scalar & operator()(unsigned int _index) {
    assert (_index < knots_.size());
    return knots_[_index];
  }

  /** \brief returns a const reference to the _index'th knot
   *
   * @param _index The knot index
   */
  inline const Scalar & operator()(unsigned int _index) const {
    assert (_index < knots_.size());
    return knots_[_index];
  }

  void clear() {knots_.clear();};



public :  // selection handling

  // request properties
  void request_selections() { request_prop( ref_count_selections_, selections_);}

  // release properties
  void release_selections() { release_prop( ref_count_selections_, selections_);}

  // property availability
  bool selections_available() const {return bool(ref_count_selections_);}

  // property access ( no range or availability check! )
  unsigned char& selection(unsigned int _i) {
    assert(_i < selections_.size());
    assert(selections_available());
    return selections_[_i];
  }
  const unsigned char& selection(unsigned int _i) const {
    assert(_i < selections_.size());
    assert(selections_available());
    return selections_[_i];
  }
  
  // Wrapper for selection functions
  void select(unsigned int _pIdx) { selection(_pIdx) = 1; };
  void deselect(unsigned int _pIdx) { selection(_pIdx) = 0; };
  
  bool selected(unsigned int _pIdx) const { return (selection(_pIdx) == 1); };


private:

  template <class PropT>
  void request_prop( unsigned int& _ref_count, PropT& _prop);

  template <class PropT>
  void release_prop( unsigned int& _ref_count, PropT& _prop);

  // ############################### Standard Property Handling #############################

  // properties
  std::vector<unsigned char>   selections_;

  // property reference counter
  unsigned int ref_count_selections_;

private:

  std::vector<Scalar> knots_;

  KnotvectorType knotvectorType_;

  void createUniformInterpolatingKnots(unsigned int _splineDeg, unsigned int _dim);

  void createUniformKnots(unsigned int _splineDeg, unsigned int _dim);

  unsigned int num_control_points_;
  unsigned int spline_degree_;

};

typedef  ACG::KnotvectorT< double > Knotvector;


template< typename Scalar >
inline std::ostream& operator<<(std::ostream& _stream, const KnotvectorT< Scalar >& _knotvector) {

  KnotvectorT< Scalar > knotvector = _knotvector;
  for (unsigned int i = 0; i < knotvector.size(); i++)
    _stream << knotvector(i) << " ";

  return _stream;
}


template< typename Scalar >
inline std::istream& operator>>(std::istream& _is, KnotvectorT< Scalar >& _knotvector) {

  unsigned int size(0);
  _is >> size;
  _knotvector.resize(size);
  for (unsigned int i = 0; i < size; i++)
    _is >> _knotvector(i);

  return _is;
}


}

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_KNOTVECTORT_C)
#define ACG_KNOTVECTORT_TEMPLATES
#include "KnotvectorT.cc"
#endif
//=============================================================================
#endif // ACG_KNOTVECTORT_HH defined
//=============================================================================

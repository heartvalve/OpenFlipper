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
//  CLASS PCA
//
//=============================================================================


#ifndef PCA_HH
#define PCA_HH

/*! \file PCA.hh
    \brief Classes for doing Principal component analysis
    
    Classes for doing Principal component analysis
*/

//== INCLUDES =================================================================

#include <gmm/gmm.h>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

/// Namespace for principal Component Analysis
namespace Pca {
 
//== CLASS DEFINITION =========================================================
	      
/** \class PCA PCA.hh </PCA.hh>
    \brief Class for principal component Analysis

    Class for principal component Analysis
*/

template < typename VectorT >
class PCA
{
  typedef gmm::dense_matrix<double>  Matrix;
  typedef std::vector< double >      Vector;

public:
   
  /// Constructor
  PCA() {}
  
  /** Extended constructor
    Parameters : see pca()
  */
  PCA( std::vector< VectorT >& _points, VectorT& _first , VectorT& _second , VectorT& _third);
 
  /// Destructor
  ~PCA();
  
  /**
    Compute center of gravity for a vector of points
    @param _points set of points
  */
  inline VectorT center_of_gravity(const std::vector< VectorT >& _points );
  
  /**
    Compute the principal component analysys for a vector of points
    @param _points set of points
    @param _first  First main axis
    @param _second second main axis
    @param _third  third main axis
  */
  void pca(std::vector< VectorT >& _points , VectorT& _first , VectorT& _second , VectorT& _third);

  bool SymRightEigenproblem( Matrix &_mat_A, Matrix & _mat_VR,
                             std::vector< double > & _vec_EV );
private:

};


//=============================================================================
} //namespace Pca
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(PCA_C)
#define PCA_TEMPLATES
#include "PCA.cc"
#endif
//=============================================================================
#endif // PCA_HH defined
//=============================================================================


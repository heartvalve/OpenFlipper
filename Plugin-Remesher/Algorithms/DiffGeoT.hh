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
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  CLASS DiffGeoT
//
//=============================================================================


#ifndef DIFFGEO_HH
#define DIFFGEO_HH


//== INCLUDES =================================================================

#include <OpenMesh/Core/Utils/Property.hh>


//== NAMESPACES ===============================================================

namespace Remeshing {

//== CLASS DEFINITION =========================================================


template <class Mesh>
class DiffGeoT
{
public:

  typedef typename Mesh::Scalar        Scalar;
  typedef typename Mesh::VertexHandle  VertexHandle;

   
  DiffGeoT(Mesh& _mesh);
  ~DiffGeoT();


  void compute(unsigned int _post_smoothing_iters=0);

  void compute_edge_weights();
  void compute_area();
  void compute_gauss_curvature();
  void compute_mean_curvature();
  void post_smoothing(unsigned int _iters);


  Scalar compute_area(VertexHandle _vh) const;


  Scalar area(VertexHandle _vh) const { 
    return mesh_.property(area_, _vh);
  }

  Scalar gauss_curvature(VertexHandle _vh) const {
    return mesh_.property(gauss_curvature_, _vh);
  }

  Scalar mean_curvature(VertexHandle _vh) const { 
    return mesh_.property(mean_curvature_, _vh);
  }

  Scalar min_curvature(VertexHandle _vh) const { 
    const Scalar zero(0.0);
    Scalar H = mean_curvature(_vh);
    Scalar K = gauss_curvature(_vh);
    return H - sqrt(std::max(zero, H*H-K));
  }
  
  Scalar max_curvature(VertexHandle _vh) const { 
    const Scalar zero(0.0);
    Scalar H = mean_curvature(_vh);
    Scalar K = gauss_curvature(_vh);
    return H + sqrt(std::max(zero, H*H-K));
  }


private:

  Mesh&  mesh_;

  OpenMesh::VPropHandleT<Scalar>   area_;
  OpenMesh::VPropHandleT<Scalar>   gauss_curvature_;
  OpenMesh::VPropHandleT<Scalar>   mean_curvature_;
  OpenMesh::EPropHandleT<Scalar>   edge_weight_;

  bool weights_computed_, area_computed_;
};


//=============================================================================
} // namespace Remeshing
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(DIFFGEOT_C)
#define DIFFGEO_TEMPLATES
#include "DiffGeoT.cc"
#endif
//=============================================================================
#endif // DIFFGEO_HH defined
//=============================================================================


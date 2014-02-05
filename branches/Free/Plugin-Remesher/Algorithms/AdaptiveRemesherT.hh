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
//  CLASS AdaptiveRemesherT
//
//=============================================================================


#ifndef ADAPTIVE_REMESHERT_HH
#define ADAPTIVE_REMESHERT_HH


//== INCLUDES =================================================================

#include "BaseRemesherT.hh"
#include "DiffGeoT.hh"
#include <OpenMesh/Core/Utils/Property.hh>

#include "../ProgressEmitter.hh"

//== NAMESPACES ===============================================================

namespace Remeshing {


//== CLASS DEFINITION =========================================================


template <class Mesh>
class AdaptiveRemesherT : public BaseRemesherT<Mesh>
{
  typedef typename BaseRemesherT<Mesh>::Selection Selection;
public:

  typedef BaseRemesherT<Mesh>          Base;
  typedef typename Mesh::Scalar        Scalar;
  typedef typename Mesh::Point         Point;
  typedef typename Mesh::EdgeHandle    EdgeHandle;
  typedef typename Mesh::VertexHandle  VertexHandle;


  AdaptiveRemesherT(Mesh& _mesh, ProgressEmitter* _progress = NULL) : Base(_mesh, _progress) {}

  void remesh(Scalar        _error,
              Scalar        _min_edge_length,
              Scalar        _max_edge_length,
              unsigned int  _iters,
              bool          _use_projection = true,
              Selection     _selection=BaseRemesherT<Mesh>::VERTEX_SELECTION);



protected:

  virtual void init_reference();
  virtual void project_to_reference(VertexHandle _vh) const;

  void compute_curvature(Mesh& _mesh, OpenMesh::VPropHandleT<Scalar> _ph);

  virtual bool is_too_long  (VertexHandle _v0, VertexHandle _v1) const;
  virtual bool is_too_short (VertexHandle _v0, VertexHandle _v1) const;


protected:

  Scalar  error_, emax_, emin_;

  OpenMesh::VPropHandleT<Scalar>  refcurv_, curvature_;
};


//=============================================================================
} // namespace Remeshing
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ADAPTIVE_REMESHERT_C)
#define ADAPTIVE_REMESHERT_TEMPLATES
#include "AdaptiveRemesherT.cc"
#endif
//=============================================================================
#endif // ADAPTIVE_REMESHERT_HH defined
//=============================================================================


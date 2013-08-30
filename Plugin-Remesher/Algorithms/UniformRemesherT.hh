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
*   $Date$                    *
*                                                                            *
\*===========================================================================*/
//=============================================================================
//
//  CLASS UniformRemesherT
//
//=============================================================================


#ifndef UNIFORM_REMESHERT_HH
#define UNIFORM_REMESHERT_HH


//== INCLUDES =================================================================

#include "BaseRemesherT.hh"

#include "../ProgressEmitter.hh"

//== NAMESPACES ===============================================================

namespace Remeshing {


//== CLASS DEFINITION =========================================================


template <class Mesh>
class UniformRemesherT : public BaseRemesherT<Mesh>
{
  typedef typename BaseRemesherT<Mesh>::Selection Selection;
public:

  typedef BaseRemesherT<Mesh>          Base;
  typedef typename Mesh::Scalar        Scalar;
  typedef typename Mesh::Point         Point;
  typedef typename Mesh::EdgeHandle    EdgeHandle;
  typedef typename Mesh::VertexHandle  VertexHandle;


  UniformRemesherT(Mesh& _mesh, ProgressEmitter* _progress = NULL) : Base(_mesh, _progress) {}


  void remesh(Scalar        _edge_length,
	      unsigned int  _iters,
	      unsigned int  _area_iters,
	      bool          _use_projection = true,
	      Selection     _selection = BaseRemesherT<Mesh>::VERTEX_SELECTION)
  {
//     emin_ = 4.0/5.0 * _edge_length;  sqr_emin_ = emin_ * emin_;
//     emax_ = 4.0/3.0 * _edge_length;  sqr_emax_ = emax_ * emax_;
    emin_ = 0.7 * _edge_length;  sqr_emin_ = emin_ * emin_;
    emax_ = 1.4 * _edge_length;  sqr_emax_ = emax_ * emax_;
    Base::remesh(_iters, _area_iters, _use_projection,_selection);
  }



protected:

  virtual bool is_too_long  (VertexHandle _v0, VertexHandle _v1) const
  {
    return (Base::mesh_.point(_v0) - 
	    Base::mesh_.point(_v1)).sqrnorm() > sqr_emax_;
  }

  virtual bool is_too_short (VertexHandle _v0, VertexHandle _v1) const
  {
    return (Base::mesh_.point(_v0) - 
	    Base::mesh_.point(_v1)).sqrnorm() < sqr_emin_;
  }


protected:

  Scalar  emax_, sqr_emax_, emin_, sqr_emin_;
};


//=============================================================================
} // namespace Remeshing
//=============================================================================
#endif // UNIFORM_REMESHERT_HH defined
//=============================================================================


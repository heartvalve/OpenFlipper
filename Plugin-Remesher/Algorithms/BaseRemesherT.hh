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
//  CLASS BaseRemesherT
//
//=============================================================================


#ifndef BASE_REMESHERT_HH
#define BASE_REMESHERT_HH

/**
 *  BaseRemesher implements a modified version of a remesher approach presented by Mario Botsch and Leif Kobbelt in
 *  "A Remeshing Approach to Multiresolution Modeling" published at Symposium on Geometry Processing 2004 p. 189-196
 *  Paper can found at: http://www.graphics.rwth-aachen.de/publications/0000/37/
 */


//== INCLUDES =================================================================

#include <OpenMesh/Core/Utils/Property.hh>
#include <ACG/Geometry/bsp/TriangleBSPT.hh>

#include "../ProgressEmitter.hh"

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace Remeshing {


//== CLASS DEFINITION =========================================================


template <class Mesh>
class BaseRemesherT
{
public:

  enum Selection
  {
    VERTEX_SELECTION,
    FACE_SELECTION
  };

  typedef typename Mesh::Scalar        Scalar;
  typedef typename Mesh::Point         Point;
  typedef typename Mesh::EdgeHandle    EdgeHandle;
  typedef typename Mesh::VertexHandle  VertexHandle;


  BaseRemesherT(Mesh& _mesh, ProgressEmitter* _progress = NULL);
  virtual ~BaseRemesherT();

  void remesh(unsigned int  _iters,
              unsigned int  _area_iters,
              bool          _use_projection = true,
              Selection     _selection=VERTEX_SELECTION);



protected:

  /// prepare for remeshing only selected vertices (if no vertex was selected, remesh whole mesh)
  void prepare_vertex_selection();
  /// prepare for remeshing only vertices which are fully surrounded by selected faces (if no face was selected, remesh whole mesh)
  void prepare_face_selection();
  void remeshh(unsigned int _iters, unsigned int _aiters, bool _proj);
  void cleanup();

  virtual void init_reference();
  virtual void delete_reference();
  virtual void project_to_reference(VertexHandle _vh) const;

  void split_long_edges();
  void collapse_short_edges();
  void flip_edges();
  void tangential_smoothing(bool _use_projection);
  void balanace_area(unsigned int _iters, bool _use_projection);
  void remove_caps();

  virtual bool is_too_long  (VertexHandle _v0, VertexHandle _v1) const = 0;
  virtual bool is_too_short (VertexHandle _v0, VertexHandle _v1) const = 0;


  
protected:

  typedef OpenMeshTriangleBSPT<Mesh>  BSP;

  Mesh&  mesh_;
  Mesh*  refmesh_;
  BSP*   bsp_;
  bool   nothing_selected_;

  OpenMesh::VPropHandleT<int>     valences_;
  OpenMesh::VPropHandleT<Point>   update_;
  OpenMesh::VPropHandleT<Scalar>  area_;

  ProgressEmitter* progress_;
};


//=============================================================================
} // namespace Remeshing
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(BASE_REMESHERT_C)
#define BASE_REMESHERT_TEMPLATES
#include "BaseRemesherT.cc"
#endif
//=============================================================================
#endif // BASE_REMESHERT_HH defined
//=============================================================================


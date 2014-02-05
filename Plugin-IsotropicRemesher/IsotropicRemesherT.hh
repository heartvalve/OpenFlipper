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

#ifndef ISOTROPICREMESHER_HH
#define ISOTROPICREMESHER_HH
#include "ProgressEmitter.hh"


template< class MeshT >
class IsotropicRemesher{

public:

  IsotropicRemesher(ProgressEmitter* _prgEmt = NULL) : prgEmt_(_prgEmt)  {};
  ~IsotropicRemesher() {};

  void remesh( MeshT& _mesh, const double _targetEdgeLength );

private:

  void splitLongEdges( MeshT& _mesh, const double _maxEdgeLength );

  void collapseShortEdges( MeshT& _mesh, const double _minEdgeLength, const double _maxEdgeLength );

  void equalizeValences( MeshT& _mesh );

  inline
  int targetValence( MeshT& _mesh, const typename MeshT::VertexHandle& _vh );

  void tangentialRelaxation( MeshT& _mesh );

  template <class SpatialSearchT>
  typename MeshT::Point
  findNearestPoint(const MeshT&                   _mesh,
                   const typename MeshT::Point&   _point,
                   typename MeshT::FaceHandle&    _fh,
                   SpatialSearchT*                _ssearch,
                   double*                        _dbest);

  OpenMeshTriangleBSPT< MeshT >* getTriangleBSP(MeshT& _mesh);

  template< class SpatialSearchT >
  void projectToSurface( MeshT& _mesh, MeshT& _original, SpatialSearchT* _ssearch );

  inline
  bool isBoundary( MeshT& _mesh, const typename MeshT::VertexHandle& _vh);
  
  inline
  bool isFeature( MeshT& _mesh, const typename MeshT::VertexHandle& _vh);
  
  private:
    ProgressEmitter* prgEmt_;
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ISOTROPICREMESHER_C)
#define ISOTROPICREMESHER_TEMPLATES
#include "IsotropicRemesherT.cc"
#endif
//=============================================================================
#endif // ISOTROPICREMESHER_HH defined
//=============================================================================

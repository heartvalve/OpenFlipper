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

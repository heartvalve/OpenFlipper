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


#define ISOTROPICREMESHER_C

#include "IsotropicRemesherT.hh"

#include <ACG/Geometry/Algorithms.hh>

// -------------------- BSP
#include <ACG/Geometry/bsp/TriangleBSPT.hh>

/// do the remeshing
template< class MeshT >
void IsotropicRemesher< MeshT >::remesh( MeshT& _mesh, const double _targetEdgeLength )
{
  const double low  = (4.0 / 5.0) * _targetEdgeLength;
  const double high = (4.0 / 3.0) * _targetEdgeLength;

  MeshT meshCopy = _mesh;
  OpenMeshTriangleBSPT< MeshT >* triangleBSP = getTriangleBSP(meshCopy);

  for (int i=0; i < 10; i++){
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 0);
// std::cerr << "Iteration = " << i << std::endl;
    splitLongEdges(_mesh, high);
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 2);
    
// std::cerr << "collapse" << std::endl;
    collapseShortEdges(_mesh, low, high);
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 4);
    
// std::cerr << "equal" << std::endl;
    equalizeValences(_mesh);
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 6);
    
// std::cerr << "relax" << std::endl;
    tangentialRelaxation(_mesh);
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 8);
    
// std::cerr << "project" << std::endl;
    projectToSurface(_mesh, meshCopy, triangleBSP);
    if (prgEmt_)
      prgEmt_->sendProgressSignal(10*i + 10);
  }

  delete triangleBSP;
}

template< class MeshT >
OpenMeshTriangleBSPT< MeshT >* IsotropicRemesher< MeshT >::getTriangleBSP(MeshT& _mesh)
{
  // create Triangle BSP
  OpenMeshTriangleBSPT< MeshT >* triangle_bsp = new OpenMeshTriangleBSPT< MeshT >( _mesh );

  // build Triangle BSP
  triangle_bsp->reserve(_mesh.n_faces());

  typename MeshT::FIter f_it  = _mesh.faces_begin();
  typename MeshT::FIter f_end = _mesh.faces_end();

  for (; f_it!=f_end; ++f_it)
    triangle_bsp->push_back( f_it.handle() );

  triangle_bsp->build(10, 100);

  // return pointer to triangle bsp
  return triangle_bsp;
}

/// performs edge splits until all edges are shorter than the threshold
template< class MeshT >
void IsotropicRemesher< MeshT >::splitLongEdges( MeshT& _mesh, const double _maxEdgeLength )
{

  const double _maxEdgeLengthSqr = _maxEdgeLength * _maxEdgeLength;

  typename MeshT::EdgeIter e_it;
  typename MeshT::EdgeIter e_end = _mesh.edges_end();

  //iterate over all edges
  for (e_it = _mesh.edges_begin(); e_it != e_end; ++e_it){
    const typename MeshT::HalfedgeHandle & hh = _mesh.halfedge_handle( e_it, 0 );

    const typename MeshT::VertexHandle & v0 = _mesh.from_vertex_handle(hh);
    const typename MeshT::VertexHandle & v1 = _mesh.to_vertex_handle(hh);

    typename MeshT::Point vec = _mesh.point(v1) - _mesh.point(v0);

    //edge to long?
    if ( vec.sqrnorm() > _maxEdgeLengthSqr ){

      const typename MeshT::Point midPoint = _mesh.point(v0) + ( 0.5 * vec );

      //split at midpoint
      typename MeshT::VertexHandle vh = _mesh.add_vertex( midPoint );
      
      bool hadFeature = _mesh.status(e_it).feature();
      
      _mesh.split(e_it, vh);
      
      if ( hadFeature ){
        
        typename MeshT::VOHIter vh_it;
        for (vh_it = _mesh.voh_iter(vh); vh_it; ++vh_it)
          if ( _mesh.to_vertex_handle(vh_it.handle()) == v0 || _mesh.to_vertex_handle(vh_it.handle()) == v1 )
            _mesh.status( _mesh.edge_handle( vh_it.handle() ) ).set_feature( true ); 
      }
    }
  }
}

/// collapse edges shorter than minEdgeLength if collapsing doesn't result in new edge longer than maxEdgeLength
template< class MeshT >
void IsotropicRemesher< MeshT >::collapseShortEdges( MeshT& _mesh, const double _minEdgeLength, const double _maxEdgeLength )
{
  const double _minEdgeLengthSqr = _minEdgeLength * _minEdgeLength;
  const double _maxEdgeLengthSqr = _maxEdgeLength * _maxEdgeLength;

  //add checked property
  OpenMesh::EPropHandleT< bool > checked;
  if ( !_mesh.get_property_handle(checked, "Checked Property") )
    _mesh.add_property(checked,"Checked Property" );

  //init property
  typename MeshT::ConstEdgeIter e_it;
  typename MeshT::ConstEdgeIter e_end = _mesh.edges_end();

  for (e_it = _mesh.edges_begin(); e_it != e_end; ++e_it)
    _mesh.property(checked, e_it) = false;


  bool finished = false;

  while( !finished ){

    finished = true;

    for (e_it = _mesh.edges_begin(); e_it != _mesh.edges_end() ; ++e_it){
    
        if ( _mesh.property(checked, e_it) ) 
          continue;
      
        _mesh.property(checked, e_it) = true;

        const typename MeshT::HalfedgeHandle & hh = _mesh.halfedge_handle( e_it, 0 );
      
        const typename MeshT::VertexHandle & v0 = _mesh.from_vertex_handle(hh);
        const typename MeshT::VertexHandle & v1 = _mesh.to_vertex_handle(hh);

        const typename MeshT::Point vec = _mesh.point(v1) - _mesh.point(v0);

        const double edgeLength = vec.sqrnorm();

        // don't try to collapse edges that have length 0
        if ( edgeLength < DBL_EPSILON )
          continue;
    
        //edge too short?
        if ( edgeLength < _minEdgeLengthSqr ){
    
          //check if the collapse is ok
          const typename MeshT::Point & B = _mesh.point(v1);

          bool collapse_ok = true;
    
          for(typename MeshT::VOHIter vh_it = _mesh.voh_iter(v0); vh_it; ++vh_it)
            if ( (( B - _mesh.point( _mesh.to_vertex_handle(vh_it.handle()) ) ).sqrnorm() > _maxEdgeLengthSqr )
                 || ( _mesh.status( _mesh.edge_handle( vh_it.handle() ) ).feature())
                 || ( _mesh.is_boundary( _mesh.edge_handle( vh_it.handle() ) ) )){
              collapse_ok = false;
              break;
            }
  
          if( collapse_ok && _mesh.is_collapse_ok(hh) ) {
            _mesh.collapse( hh );

            finished = false;
          }

        } 
    }

  }

  _mesh.remove_property(checked);

  _mesh.garbage_collection();
}

template< class MeshT >
void IsotropicRemesher< MeshT >::equalizeValences( MeshT& _mesh )
{

  typename MeshT::EdgeIter e_it;
  typename MeshT::EdgeIter e_end = _mesh.edges_end();

  for (e_it = _mesh.edges_sbegin(); e_it != e_end; ++e_it){

    if ( !_mesh.is_flip_ok(e_it) ) continue;
    if ( _mesh.status( e_it ).feature() ) continue;
    

    const typename MeshT::HalfedgeHandle & h0 = _mesh.halfedge_handle( e_it, 0 );
    const typename MeshT::HalfedgeHandle & h1 = _mesh.halfedge_handle( e_it, 1 );

    if (h0.is_valid() && h1.is_valid())

      if (_mesh.face_handle(h0).is_valid() && _mesh.face_handle(h1).is_valid()){
        //get vertices of corresponding faces
        const typename MeshT::VertexHandle & a = _mesh.to_vertex_handle(h0);
        const typename MeshT::VertexHandle & b = _mesh.to_vertex_handle(h1);
        const typename MeshT::VertexHandle & c = _mesh.to_vertex_handle(_mesh.next_halfedge_handle(h0));
        const typename MeshT::VertexHandle & d = _mesh.to_vertex_handle(_mesh.next_halfedge_handle(h1));

        const int deviation_pre =  abs((int)(_mesh.valence(a) - targetValence(_mesh, a)))
                                  +abs((int)(_mesh.valence(b) - targetValence(_mesh, b)))
                                  +abs((int)(_mesh.valence(c) - targetValence(_mesh, c)))
                                  +abs((int)(_mesh.valence(d) - targetValence(_mesh, d)));
        _mesh.flip(e_it);

        const int deviation_post = abs((int)(_mesh.valence(a) - targetValence(_mesh, a)))
                                  +abs((int)(_mesh.valence(b) - targetValence(_mesh, b)))
                                  +abs((int)(_mesh.valence(c) - targetValence(_mesh, c)))
                                  +abs((int)(_mesh.valence(d) - targetValence(_mesh, d)));

        if (deviation_pre <= deviation_post)
          _mesh.flip(e_it);
      }
  }
}

///returns 4 for boundary vertices and 6 otherwise
template< class MeshT > 
inline
int IsotropicRemesher< MeshT >::targetValence( MeshT& _mesh, const typename MeshT::VertexHandle& _vh ){

  if (isBoundary(_mesh,_vh))
    return 4;
  else
    return 6;
}

template< class MeshT > 
inline
bool IsotropicRemesher< MeshT >::isBoundary( MeshT& _mesh, const typename MeshT::VertexHandle& _vh ){

  typename MeshT::ConstVertexOHalfedgeIter voh_it;

  for (voh_it = _mesh.voh_iter( _vh ); voh_it; ++voh_it )
    if ( _mesh.is_boundary( _mesh.edge_handle( voh_it.handle() ) ) )
      return true;

  return false;
}

template< class MeshT > 
inline
bool IsotropicRemesher< MeshT >::isFeature( MeshT& _mesh, const typename MeshT::VertexHandle& _vh ){

  typename MeshT::ConstVertexOHalfedgeIter voh_it;

  for (voh_it = _mesh.voh_iter( _vh ); voh_it; ++voh_it )
    if ( _mesh.status( _mesh.edge_handle( voh_it.handle() ) ).feature() )
      return true;

  return false;
}

template< class MeshT >
void IsotropicRemesher< MeshT >::tangentialRelaxation( MeshT& _mesh )
{
  _mesh.update_normals();

  //add checked property
  OpenMesh::VPropHandleT< typename MeshT::Point > q;
  if ( !_mesh.get_property_handle(q, "q Property") )
    _mesh.add_property(q,"q Property" );

  typename MeshT::ConstVertexIter v_it;
  typename MeshT::ConstVertexIter v_end = _mesh.vertices_end();

  //first compute barycenters
  for (v_it = _mesh.vertices_sbegin(); v_it != v_end; ++v_it){

    typename MeshT::Point tmp(0.0, 0.0, 0.0);
    uint N = 0;

    typename MeshT::VVIter vv_it;
    for (vv_it = _mesh.vv_iter(v_it); vv_it; ++vv_it){
      tmp += _mesh.point(vv_it);
      N++;
    }

    if (N > 0)
      tmp /= (double) N;

    _mesh.property(q, v_it) = tmp;
  }

  //move to new position
  for (v_it = _mesh.vertices_sbegin(); v_it != v_end; ++v_it){
    if ( !isBoundary(_mesh, v_it) && !isFeature(_mesh, v_it) )
      _mesh.set_point(v_it,  _mesh.property(q, v_it) + (_mesh.normal(v_it)| (_mesh.point(v_it) - _mesh.property(q, v_it) ) ) * _mesh.normal(v_it));
  }

  _mesh.remove_property(q);
}

template <class MeshT>
template <class SpatialSearchT>
typename MeshT::Point
IsotropicRemesher< MeshT >::findNearestPoint(const MeshT&                   _mesh,
                                          const typename MeshT::Point&   _point,
                                          typename MeshT::FaceHandle&    _fh,
                                          SpatialSearchT*                _ssearch,
                                          double*                        _dbest)
{

  typename MeshT::Point  p_best = _mesh.point(_mesh.vertex_handle(0));
  typename MeshT::Scalar d_best = (_point-p_best).sqrnorm();

  typename MeshT::FaceHandle fh_best;

  if( _ssearch == 0 )
  {
    // exhaustive search
    typename MeshT::ConstFaceIter cf_it  = _mesh.faces_begin();
    typename MeshT::ConstFaceIter cf_end = _mesh.faces_end();

    for(; cf_it != cf_end; ++cf_it)
    {
      typename MeshT::ConstFaceVertexIter cfv_it = _mesh.cfv_iter(cf_it);

      const typename MeshT::Point& pt0 = _mesh.point(   cfv_it);
      const typename MeshT::Point& pt1 = _mesh.point( ++cfv_it);
      const typename MeshT::Point& pt2 = _mesh.point( ++cfv_it);

      typename MeshT::Point ptn;

      typename MeshT::Scalar d = ACG::Geometry::distPointTriangleSquared( _point,
                     pt0,
                     pt1,
                     pt2,
                     ptn );

      if( d < d_best)
      {
        d_best = d;
        p_best = ptn;

        fh_best = cf_it.handle();
      }
    }

    // return facehandle
    _fh = fh_best;

    // return distance
    if(_dbest)
      *_dbest = sqrt(d_best);


    return p_best;
  }
  else
  {
    typename MeshT::FaceHandle     fh = _ssearch->nearest(_point).handle;
    typename MeshT::CFVIter        fv_it = _mesh.cfv_iter(fh);

    const typename MeshT::Point&   pt0 = _mesh.point(  fv_it);
    const typename MeshT::Point&   pt1 = _mesh.point(++fv_it);
    const typename MeshT::Point&   pt2 = _mesh.point(++fv_it);

    // project
    d_best = ACG::Geometry::distPointTriangleSquared(_point, pt0, pt1, pt2, p_best);

    // return facehandle
    _fh = fh;

    // return distance
    if(_dbest)
      *_dbest = sqrt(d_best);

    return p_best;
  }
}


template< class MeshT >
template< class SpatialSearchT >
void IsotropicRemesher< MeshT >::projectToSurface( MeshT& _mesh, MeshT& _original, SpatialSearchT* _ssearch )
{

  typename MeshT::VertexIter v_it;
  typename MeshT::VertexIter v_end = _mesh.vertices_end();

  for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it){

    if (isBoundary(_mesh, v_it)) continue;
    if ( isFeature(_mesh, v_it)) continue;

    typename MeshT::Point p = _mesh.point(v_it);
    typename MeshT::FaceHandle fhNear;
    double distance;

    typename MeshT::Point pNear = findNearestPoint(_original, p, fhNear, _ssearch, &distance);

    _mesh.set_point(v_it, pNear);
  }
}


/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#define SELECTIONFUNCTIONST_C

#include "SelectionPlugin.hh"

#include <QStack>

#include <MeshTools/MeshNavigationT.hh>
#include <MeshTools/MeshSelectionT.hh>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <math.h>

//***********************************************************************************

/** \brief paint selection with a sphere
 *
 * @param _mesh a mesh
 * @param _target_idx handle of the face that was hit by the mouse picking
 * @param _hitpoint point where the mouse picking hit
 */
template < class MeshT >
void SelectionPlugin::paintSphereSelection( MeshT* _mesh ,
                                            int _target_idx ,
                                            typename MeshT::Point _hitpoint ) {

  float sqr_radius = sphere_radius_*sphere_radius_;
  typename MeshT::FaceHandle hitface = _mesh->face_handle(_target_idx);

  if ( !hitface.is_valid() )
    return;

  // initialize everything

  if (selectionType_ & VERTEX){
    //reset tagged status
    typename MeshT::VertexIter v_it, v_end( _mesh->vertices_end() );
      for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
        _mesh->status(v_it).set_tagged(false);

  }

  if (selectionType_ & EDGE){
    //reset tagged status
    typename MeshT::EdgeIter e_it, e_end( _mesh->edges_end() );
      for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
        _mesh->status(e_it).set_tagged(false);

  }


  //reset tagged status
  typename MeshT::FaceIter f_it, f_end( _mesh->faces_end() );
  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
    _mesh->status(f_it).set_tagged(false);

  _mesh->status(hitface).set_tagged(true);

  //add an additional property
  OpenMesh::FPropHandleT< bool > checkedProp;
  if ( !_mesh->get_property_handle(checkedProp,"checkedProp") )
    _mesh->add_property(checkedProp, "checkedProp");

  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
    _mesh->property(checkedProp, f_it) = false;


  std::vector<typename MeshT::FaceHandle>   face_handles;
  face_handles.reserve(50);
  face_handles.push_back(hitface);


  // find primitives to be selected

  while (!face_handles.empty()) {
    typename MeshT::FaceHandle fh = face_handles.back(); face_handles.pop_back();

    for (typename MeshT::FaceFaceIter ff_it(*_mesh,fh) ; ff_it ; ++ff_it ) {

        // Check if already tagged
        if ( _mesh->status(ff_it).tagged() || _mesh->property(checkedProp, ff_it) )
          continue;

        // Check which points of the new face lie inside the sphere
        uint fVertices = 0;
        bool lastVertexInside = false;

        std::vector<typename MeshT::VertexHandle>   vertex_handles;
        std::vector<typename MeshT::EdgeHandle>     edge_handles;

        for (typename MeshT::FaceHalfedgeIter fh_it(*_mesh,ff_it); fh_it; ++fh_it){

          typename MeshT::VertexHandle vh = _mesh->from_vertex_handle( fh_it.handle() );

          if ( (_mesh->point(vh) - _hitpoint).sqrnorm() <= sqr_radius ){
            vertex_handles.push_back( vh );
            
          if ( ( _mesh->point(_mesh->to_vertex_handle( fh_it.handle() ) ) -  _hitpoint).sqrnorm() <= sqr_radius )
            edge_handles.push_back( _mesh->edge_handle( fh_it.handle() ) );
              

            lastVertexInside = true;
          }else
            lastVertexInside = false;

          fVertices++;
        }

        //check what has to be tagged
        bool tagged = false;

        if (selectionType_ & VERTEX){
          for (uint i=0; i < vertex_handles.size(); i++)
            _mesh->status( vertex_handles[i] ).set_tagged(true);

          if (vertex_handles.size() > 0) tagged = true;
        }
        if (selectionType_ & EDGE){
          for (uint i=0; i < edge_handles.size(); i++)
            _mesh->status( edge_handles[i] ).set_tagged(true);

          if (edge_handles.size() > 0) tagged = true;
        }
        if (selectionType_ & FACE){
          if (vertex_handles.size() == fVertices){

            _mesh->status(ff_it).set_tagged(true);
            tagged = true;
          }
        }

        //if something was tagged also check the 1-ring
        if (tagged){
          _mesh->property(checkedProp, ff_it) = true;
          face_handles.push_back(ff_it.handle());
        }
    }
  }

  // select all tagged primitives
  bool sel = ! deselection_;

  if (selectionType_ & VERTEX){
    typename MeshT::VertexIter v_it, v_end( _mesh->vertices_end() );
      for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
        if (_mesh->status(v_it).tagged())
          _mesh->status(v_it).set_selected(sel);
  }
  if (selectionType_ & EDGE){
    typename MeshT::EdgeIter e_it, e_end( _mesh->edges_end() );

      for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
        if (_mesh->status(e_it).tagged())
          _mesh->status(e_it).set_selected(sel);
  }
  if (selectionType_ & FACE){
    typename MeshT::FaceIter f_it, f_end( _mesh->faces_end() );

    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      if (_mesh->status(f_it).tagged())
          _mesh->status(f_it).set_selected(sel);
  }

  _mesh->remove_property(checkedProp);
}

//***********************************************************************************

#ifdef ENABLE_POLYLINE_SUPPORT

/** \brief Peform surface Lasso Selection on a mesh
 *
 * the actual selection routine for a given mesh and a polyline
 *
 * Note: the given faceHandle determines wether inside or outside of the polyline is selected
 *
 * @param _mesh a mesh
 * @param _line a polyline
 * @param _fh handle of the face that was hit by the mouse picking
 * @param _search spatial search tree
 */
template< class MeshT , class SpatialSearchT>
void
SelectionPlugin::surfaceLassoSelection( MeshT*                     _mesh,
                                        PolyLine*                  _line,
                                        typename MeshT::FaceHandle _fh,
                                        SpatialSearchT*            _search)
{

  //add cut property
  OpenMesh::HPropHandleT< bool > cutHE;
  if ( !_mesh->get_property_handle(cutHE, "PolyLine Cut Property") )
    _mesh->add_property(cutHE,"PolyLine Cut Property" );

  //init cut property to false
  typename MeshT::HalfedgeIter he_it, he_end = _mesh->halfedges_end();
  for( he_it = _mesh->halfedges_begin(); he_it != he_end; ++he_it )
    _mesh->property( cutHE, he_it ) = false;

  //add face property
  OpenMesh::FPropHandleT< bool > faceProp;
  if ( !_mesh->get_property_handle(faceProp, "Face Property") )
    _mesh->add_property(faceProp,"Face Property" );

  //init face property
  typename MeshT::FaceIter f_it, f_end = _mesh->faces_end();
  for( f_it = _mesh->faces_begin(); f_it != f_end; ++f_it )
    _mesh->property( faceProp, f_it ) = false;

  //add visited property
  OpenMesh::VPropHandleT< bool > visited;
  _mesh->add_property(visited,"Visited Property" );

  //init visited property
  typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
  for( v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it )
    _mesh->property( visited, v_it ) = false;

  //resample line
  _line->resample_on_mesh_edges(*_mesh,_search);

  //set HE's cut property to true if the polyLine cuts the edge
  for ( uint i = 0; i < _line->n_vertices(); ++i) {
    typename MeshT::EdgeHandle eh = _mesh->edge_handle(_line->vertex_ehandle(i));
    if (eh.is_valid()){
      typename MeshT::HalfedgeHandle h0 = _mesh->halfedge_handle(eh,0);
      typename MeshT::HalfedgeHandle h1 = _mesh->halfedge_handle(eh,1);

      if ( h0.is_valid() ){
        _mesh->property( cutHE, h0 ) = true;

        typename MeshT::FaceHandle fh = _mesh->face_handle(h0);
        if ( fh.is_valid() )
          _mesh->property( faceProp, fh ) = true;
      }

      if ( h1.is_valid() ){
        _mesh->property( cutHE, h1 ) = true;

        typename MeshT::FaceHandle fh = _mesh->face_handle(h1);
        if ( fh.is_valid() )
          _mesh->property( faceProp, fh ) = true;
      }
    }
  }

  QStack< typename MeshT::VertexHandle > stack;
  stack.push( _mesh->fv_iter(_fh).handle() );

  while ( !stack.isEmpty() ){

    typename MeshT::VertexHandle vh = stack.pop();
    if (_mesh->property( visited, vh)) continue;

    //get all neigboring vertices
    for ( typename MeshT::VertexOHalfedgeIter voh_it(*_mesh,vh); voh_it; ++voh_it) {
      if (! _mesh->property( cutHE, voh_it) ){
        stack.push( _mesh->to_vertex_handle(voh_it) );

        if (selectionType_ & EDGE)
          _mesh->status( _mesh->edge_handle(voh_it.handle()) ).set_selected( !deselection_ );

        if (selectionType_ & FACE){

          typename MeshT::FaceHandle fh0 = _mesh->face_handle( voh_it.handle() );
          if ( fh0.is_valid() && !_mesh->property( faceProp, fh0 ) )
            _mesh->status( fh0 ).set_selected( !deselection_ );

          typename MeshT::HalfedgeHandle opp = _mesh->opposite_halfedge_handle( voh_it.handle() );

          if ( opp.is_valid() ){
            typename MeshT::FaceHandle fh1 = _mesh->face_handle(opp);
            if ( fh1.is_valid() && !_mesh->property( faceProp, fh1 ) )
              _mesh->status( fh1 ).set_selected( !deselection_ );
          }
        }
      }
    }
    //and select vertex
    if (selectionType_ & VERTEX)
      _mesh->status(vh).set_selected( !deselection_ );

    _mesh->property( visited, vh ) = true;
  }

  //remove properties
  _mesh->remove_property(cutHE);
  _mesh->remove_property(faceProp);
  _mesh->remove_property(visited);
}
#endif

//***********************************************************************************

/** \brief select primitives of the closest boundary
 *
 * @param _mesh a mesh
 * @param _vh handle of the vertex that was picked
 * @param _selectionType selection type (VERTEX|FACE|EDGE)
 */
template< class MeshT >
void SelectionPlugin::closestBoundarySelection( MeshT* _mesh, int _vh, unsigned char _selectionType) {
  typename MeshT::VertexHandle vh = _mesh->vertex_handle(_vh);

  if (vh.is_valid()){
    //get boundary vertex
    typename MeshT::VertexHandle vhBound = MeshNavigation::findClosestBoundary(_mesh , vh);
    if (vhBound.is_valid()){

      //walk around the boundary and select primitves
      OpenMesh::VPropHandleT< bool > visited;
      if ( !_mesh->get_property_handle(visited,"Visited Vertices") )
        _mesh->add_property(visited, "Visited Vertices");

      typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
      for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it)
        _mesh->property(visited, v_it) = false;

      std::stack< typename MeshT::VertexHandle > stack;
      stack.push( vhBound );

      while (!stack.empty()){

        typename MeshT::VertexHandle vh = stack.top();
        stack.pop();

        if (_mesh->property(visited,vh))
          continue;

        //find outgoing boundary-edges
        for (typename MeshT::VertexOHalfedgeIter voh_it(*_mesh,vh); voh_it; ++voh_it){
          if ( _mesh->is_boundary( _mesh->edge_handle( voh_it.handle() ) ) ){
            stack.push( _mesh->to_vertex_handle(voh_it) );

            if (_selectionType & EDGE)
              _mesh->status( _mesh->edge_handle( voh_it.handle() ) ).set_selected( !deselection_ );
          }

          if (_selectionType & FACE){
            typename MeshT::FaceHandle f1 = _mesh->face_handle( voh_it.handle() );
            typename MeshT::FaceHandle f2 = _mesh->face_handle( _mesh->opposite_halfedge_handle(voh_it.handle()) );
            if ( f1.is_valid() ) _mesh->status( f1 ).set_selected( !deselection_ );
            if ( f2.is_valid() ) _mesh->status( f2 ).set_selected( !deselection_ );
          }
        }

        _mesh->property(visited,vh) = true;

        if (_selectionType & VERTEX)
          _mesh->status( vh ).set_selected( !deselection_ );
      }
      _mesh->remove_property(visited);


    }else
      emit log(LOGERR, tr("Unable to find boundary."));
  }else
    emit log(LOGERR, tr("Invalid vertex handle."));
}

//***********************************************************************************

/** \brief Toggle the selection state of mesh primitives
 *
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 * @param _hit_point point that was picked
 */
template< class MeshT >
void SelectionPlugin::toggleMeshSelection(MeshT* _mesh, uint _fh, ACG::Vec3d& _hit_point)
{

  typename MeshT::FaceHandle fh = _mesh->face_handle(_fh);

  if ( !fh.is_valid() )
    return;

  //Vertex Selection
  if (selectionType_ & VERTEX){

    typename MeshT::FaceVertexIter fv_it(*_mesh, fh);
    typename MeshT::VertexHandle closest = fv_it.handle();
    typename MeshT::Scalar shortest_distance = (_mesh->point(closest) - _hit_point).sqrnorm();

    do {
      if ( (_mesh->point(fv_it.handle() ) - _hit_point).sqrnorm() < shortest_distance ) {
        shortest_distance = (_mesh->point(fv_it.handle() ) - _hit_point).sqrnorm();
        closest = fv_it.handle();
      }

      ++fv_it;

    } while( fv_it );

    _mesh->status(closest).set_selected( !  _mesh->status(closest).selected() );

    if (_mesh->status(closest).selected())
      emit scriptInfo( "selectVertices( ObjectId , [" + QString::number(closest.idx()) + "] )" );
    else
      emit scriptInfo( "unselectVertices( ObjectId , [" + QString::number(closest.idx()) + "] )" );
  }

  //Edge Selection
  if (selectionType_ & EDGE){

    typename MeshT::FaceEdgeIter fe_it(*_mesh, fh);

    typename MeshT::EdgeHandle closest(-1);
    typename MeshT::Scalar     closest_dist(-1);

    typename MeshT::Point pp = (typename MeshT::Point) _hit_point;

    for( ; fe_it; ++fe_it)
    {
      typename MeshT::HalfedgeHandle heh0 = _mesh->halfedge_handle( fe_it.handle(), 0);
      typename MeshT::HalfedgeHandle heh1 = _mesh->halfedge_handle( fe_it.handle(), 1);

      typename MeshT::Point lp0 = _mesh->point(_mesh->to_vertex_handle(heh0));
      typename MeshT::Point lp1 = _mesh->point(_mesh->to_vertex_handle(heh1));

      double dist_new = ACG::Geometry::distPointLineSquared( pp, lp0, lp1);

      if( dist_new < closest_dist || closest_dist == -1)
      {
        // save closest Edge
        closest_dist = dist_new;
        closest = fe_it.handle();
      }
    }

    _mesh->status(closest).set_selected( !  _mesh->status(closest).selected() );

    if (_mesh->status(closest).selected())
      emit scriptInfo( "selectEdges( ObjectId , [" + QString::number(closest.idx()) + "] )" );
    else
      emit scriptInfo( "unselectEdges( ObjectId , [" + QString::number(closest.idx()) + "] )" );
  }

  //Face Selection
  if (selectionType_ & FACE){
     _mesh->status(fh).set_selected( !_mesh->status(fh).selected() );

     if (_mesh->status(fh).selected())
       emit scriptInfo( "selectFaces( ObjectId , [" + QString::number(fh.idx()) + "] )" );
     else
       emit scriptInfo( "unselectFaces( ObjectId , [" + QString::number(fh.idx()) + "] )" );
  }

}

//***********************************************************************************

/** \brief Select primitives of the connected component
 *
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 */
template< class MeshT >
void SelectionPlugin::componentSelection(MeshT* _mesh, uint _fh) {

  // reset tagged status
  typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      _mesh->status(f_it).set_tagged(false);

  std::vector<typename MeshT::FaceHandle> face_handles;


  typename MeshT::FaceHandle hitFace = typename MeshT::FaceHandle(_fh);
  face_handles.reserve(50);
  face_handles.push_back( hitFace );
  _mesh->status(hitFace).set_tagged(true);

  while (!face_handles.empty()) {
    typename MeshT::FaceHandle fh = face_handles.back();
    face_handles.pop_back();

    for (typename MeshT::FaceFaceIter ff_it(*_mesh,fh) ; ff_it ; ++ff_it ) {

      // Check if already tagged
      if ( _mesh->status(ff_it).tagged() )
          continue;

      _mesh->status(ff_it).set_tagged(true);
      face_handles.push_back(ff_it.handle());
    }
  }

  // now select all tagged primitives
  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      if (_mesh->status(f_it).tagged()){

        if (selectionType_ & VERTEX)
          for ( typename MeshT::FaceVertexIter fv_it(*_mesh,f_it) ; fv_it; ++fv_it )
            _mesh->status(fv_it).set_selected( !deselection_ );

        if (selectionType_ & EDGE)
          for ( typename MeshT::FaceEdgeIter fe_it(*_mesh,f_it) ; fe_it; ++fe_it )
            _mesh->status(fe_it).set_selected( !deselection_ );

        if (selectionType_ & FACE)
          _mesh->status(f_it).set_selected( !deselection_ );
      }
}

//***********************************************************************************

/** \brief Select all primitves of a planar region surrounding the faceHandle
 *
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 */
template< class MeshT >
void SelectionPlugin::floodFillSelection(MeshT* _mesh, uint _fh) {

  // reset tagged status
  typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      _mesh->status(f_it).set_tagged(false);

  std::vector<typename MeshT::FaceHandle> face_handles;


  typename MeshT::FaceHandle hitFace = typename MeshT::FaceHandle(_fh);
  face_handles.reserve(50);
  face_handles.push_back( hitFace );
  _mesh->status(hitFace).set_tagged(true);

  typename MeshT::Point n1 = _mesh->normal( hitFace );

  double maxAngle = OpenMesh::deg_to_rad( maxFloodFillAngle_ );

  while (!face_handles.empty()) {
    typename MeshT::FaceHandle fh = face_handles.back();
    face_handles.pop_back();

    for (typename MeshT::FaceFaceIter ff_it(*_mesh,fh) ; ff_it ; ++ff_it ) {

      // Check if already tagged
      if ( _mesh->status(ff_it).tagged() )
          continue;

      typename MeshT::Point n2 = _mesh->normal( ff_it );

      double angle = acos( n1 | n2 );

      if ( angle <= maxAngle ){
        _mesh->status(ff_it).set_tagged(true);
        face_handles.push_back(ff_it.handle());
      }
    }
  }

  // now select all tagged primitives
  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      if (_mesh->status(f_it).tagged()){

        if (selectionType_ & VERTEX)
          for ( typename MeshT::FaceVertexIter fv_it(*_mesh,f_it) ; fv_it; ++fv_it )
            _mesh->status(fv_it).set_selected( !deselection_ );

        if (selectionType_ & EDGE)
          for ( typename MeshT::FaceEdgeIter fe_it(*_mesh,f_it) ; fe_it; ++fe_it )
            _mesh->status(fe_it).set_selected( !deselection_ );

        if (selectionType_ & FACE)
          _mesh->status(f_it).set_selected( !deselection_ );
      }
}

//***********************************************************************************

/** \brief Select all primitves that are projected to the given region
 *
 * @param _mesh a mesh
 * @param _state current gl state
 * @param _region region
 * @return was something selected
 */
template< class MeshT >
bool SelectionPlugin::volumeSelection(MeshT* _mesh, ACG::GLState& _state, QRegion *_region)
{
  bool rv = false;
  ACG::Vec3d proj;

  //reset tagged status
  typename MeshT::VertexIter v_it, v_end( _mesh->vertices_end() );
  for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
    _mesh->status(v_it).set_tagged(false);

  //tag all vertives that are projected into region
  for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
  {
    proj = _state.project (_mesh->point (v_it));
    if (_region->contains (QPoint (proj[0], _state.context_height () - proj[1])))
    {
      _mesh->status(v_it).set_tagged(true);
      rv = true;
      if (selectionType_ & VERTEX)
      {
         _mesh->status(v_it).set_selected( !deselection_ );
      }
    }
  }


  if (selectionType_ & EDGE) {
    typename MeshT::EdgeIter e_it, e_end(_mesh->edges_end());
    for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
    {
      if (_mesh->status(_mesh->to_vertex_handle(_mesh->halfedge_handle(e_it, 0))).tagged () ||
          _mesh->status(_mesh->to_vertex_handle(_mesh->halfedge_handle(e_it, 1))).tagged ())
        _mesh->status(e_it).set_selected ( !deselection_ );
    }
  }
  if (selectionType_ & FACE) {
    typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
    {
      bool select = false;
      for (typename MeshT::FaceVertexIter fv_it(*_mesh,f_it); fv_it; ++fv_it)
        if (_mesh->status (fv_it).tagged())
          select = true;

      if (select)
        _mesh->status(f_it).set_selected ( !deselection_ );
    }
  }

  return rv;
}


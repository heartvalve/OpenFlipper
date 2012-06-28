//=============================================================================
//
//  CLASS CatmullClarkT - IMPLEMENTATION
//
//=============================================================================

#define ACG_CATMULLCLARKT_C

//== INCLUDES =================================================================

#include "CatmullClarkT.hh"
#include <OpenMesh/Tools/Utils/MeshCheckerT.hh>

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================

template<class MeshT>
bool
CatmullClarkT<MeshT>::prepare( )
{
  mesh_.add_property( vp_pos_ );
  mesh_.add_property( ep_pos_ );
  mesh_.add_property( fp_pos_ );
  mesh_.add_property( creaseWeights_ );

  // initialize all weights to 0 (= smooth edge)
  for( EdgeIter e_it = mesh_.edges_begin(); e_it != mesh_.edges_end(); ++e_it)
    creaseweight( e_it.handle()) = 0.0;

  return true;
}

//-----------------------------------------------------------------------------

template<class MeshT>
bool
CatmullClarkT<MeshT>::cleanup( )
{
  mesh_.remove_property( vp_pos_ );
  mesh_.remove_property( ep_pos_ );
  mesh_.remove_property( fp_pos_ );
  mesh_.remove_property( creaseWeights_ );
  return true;
}

//-----------------------------------------------------------------------------

template<class MeshT>
bool
CatmullClarkT<MeshT>::subdivide( size_t _n)
{
  // Do _n subdivisions
  for ( size_t i = 0; i < _n; ++i)
  {

    // Compute face centroid
    FaceIter f_itr   = mesh_.faces_begin();
    FaceIter f_end   = mesh_.faces_end();
    for ( ; f_itr != f_end; ++f_itr)
    {
      Point centroid;
      mesh_.calc_face_centroid( f_itr.handle(), centroid);
      mesh_.property( fp_pos_, f_itr.handle() ) = centroid;
    }

    // Compute position for new (edge-) vertices and store them in the edge property
    EdgeIter e_itr   = mesh_.edges_begin();
    EdgeIter e_end   = mesh_.edges_end();
    for ( ; e_itr != e_end; ++e_itr)
      compute_midpoint( e_itr.handle() );

    // compute new positions for old vertices
    VertexIter v_itr = mesh_.vertices_begin();
    VertexIter v_end = mesh_.vertices_end();
    for ( ; v_itr != v_end; ++v_itr)
      update_vertex( v_itr.handle() );

    // Commit changes in geometry
    v_itr  = mesh_.vertices_begin();
    for ( ; v_itr != v_end; ++v_itr)
      mesh_.set_point(v_itr, mesh_.property( vp_pos_, v_itr ) );

    // Split each edge at midpoint stored in edge property ep_pos_;
    // Attention! Creating new edges, hence make sure the loop ends correctly.
    e_itr = mesh_.edges_begin();
    for ( ; e_itr != e_end; ++e_itr)
      split_edge( e_itr.handle() );

    // Commit changes in topology and reconsitute consistency
    // Attention! Creating new faces, hence make sure the loop ends correctly.
    f_itr   = mesh_.faces_begin();
    for ( ; f_itr != f_end; ++f_itr)
      split_face( f_itr.handle());


#if defined(_DEBUG) || defined(DEBUG)
    // Now we have an consistent mesh!
    assert( OpenMesh::Utils::MeshCheckerT<MeshT>(mesh_).check() );
#endif
  }

  mesh_.update_normals();

  return true;
}

//-----------------------------------------------------------------------------

template<class MeshT>
void
CatmullClarkT<MeshT>::split_face( const FaceHandle& _fh)
{
  /*
     Split an n-gon into n quads by connecting
     each vertex of fh to vh.

     - _fh will remain valid (it will become one of the quads)
     - the halfedge handles of the new quads will
     point to the old halfeges
   */

  // Since edges already refined (valence*2)
  size_t valence = mesh_.valence(_fh)/2;

  // new mesh vertex from face centroid
  VertexHandle vh = mesh_.add_vertex(mesh_.property( fp_pos_, _fh ));

  HalfedgeHandle hend = mesh_.halfedge_handle(_fh);
  HalfedgeHandle hh = mesh_.next_halfedge_handle(hend);

  HalfedgeHandle hold = mesh_.new_edge(mesh_.to_vertex_handle(hend), vh);

  mesh_.set_next_halfedge_handle(hend, hold);
  mesh_.set_face_handle(hold, _fh);

  hold = mesh_.opposite_halfedge_handle(hold);

  for(size_t i = 1; i < valence; i++)
  {
    HalfedgeHandle hnext = mesh_.next_halfedge_handle(hh);

    FaceHandle fnew = mesh_.new_face();

    mesh_.set_halfedge_handle(fnew, hh);

    HalfedgeHandle hnew = mesh_.new_edge(mesh_.to_vertex_handle(hnext), vh);

    mesh_.set_face_handle(hnew,  fnew);
    mesh_.set_face_handle(hold,  fnew);
    mesh_.set_face_handle(hh,    fnew);
    mesh_.set_face_handle(hnext, fnew);

    mesh_.set_next_halfedge_handle(hnew, hold);
    mesh_.set_next_halfedge_handle(hold, hh);
    mesh_.set_next_halfedge_handle(hh, hnext);
    hh = mesh_.next_halfedge_handle(hnext);
    mesh_.set_next_halfedge_handle(hnext, hnew);

    hold = mesh_.opposite_halfedge_handle(hnew);
  }

  mesh_.set_next_halfedge_handle(hold, hh);
  mesh_.set_next_halfedge_handle(hh, hend);
  hh = mesh_.next_halfedge_handle(hend);
  mesh_.set_next_halfedge_handle(hend, hh);
  mesh_.set_next_halfedge_handle(hh, hold);

  mesh_.set_face_handle(hold, _fh);

  mesh_.set_halfedge_handle(vh, hold);
}

//-----------------------------------------------------------------------------

template<class MeshT>
void
CatmullClarkT<MeshT>::split_edge( const EdgeHandle& _eh)
{
  HalfedgeHandle heh     = mesh_.halfedge_handle(_eh, 0);
  HalfedgeHandle opp_heh = mesh_.halfedge_handle(_eh, 1);

  HalfedgeHandle new_heh, opp_new_heh, t_heh;
  VertexHandle   vh;
  VertexHandle   vh1( mesh_.to_vertex_handle(heh));
  Point          zero(0,0,0);

  // new vertex
  vh                = mesh_.new_vertex( zero );
  mesh_.set_point( vh, mesh_.property( ep_pos_, _eh ) );

  // Re-link mesh entities
  if (mesh_.is_boundary(_eh))
  {
    for (t_heh = heh;
        mesh_.next_halfedge_handle(t_heh) != opp_heh;
        t_heh = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(t_heh)))
    {}
  }
  else
  {
    for (t_heh = mesh_.next_halfedge_handle(opp_heh);
        mesh_.next_halfedge_handle(t_heh) != opp_heh;
        t_heh = mesh_.next_halfedge_handle(t_heh) )
    {}
  }

  new_heh     = mesh_.new_edge(vh, vh1);
  opp_new_heh = mesh_.opposite_halfedge_handle(new_heh);
  mesh_.set_vertex_handle( heh, vh );

  mesh_.set_next_halfedge_handle(t_heh, opp_new_heh);
  mesh_.set_next_halfedge_handle(new_heh, mesh_.next_halfedge_handle(heh));
  mesh_.set_next_halfedge_handle(heh, new_heh);
  mesh_.set_next_halfedge_handle(opp_new_heh, opp_heh);

  if (mesh_.face_handle(opp_heh).is_valid())
  {
    mesh_.set_face_handle(opp_new_heh, mesh_.face_handle(opp_heh));
    mesh_.set_halfedge_handle(mesh_.face_handle(opp_new_heh), opp_new_heh);
  }

  if( mesh_.face_handle(heh).is_valid())
  {
    mesh_.set_face_handle( new_heh, mesh_.face_handle(heh) );
    mesh_.set_halfedge_handle( mesh_.face_handle(heh), heh );
  }

  mesh_.set_halfedge_handle( vh, new_heh);
  mesh_.set_halfedge_handle( vh1, opp_new_heh );

  // Never forget this, when playing with the topology
  mesh_.adjust_outgoing_halfedge( vh );
  mesh_.adjust_outgoing_halfedge( vh1 );
}

//-----------------------------------------------------------------------------

template<class MeshT>
void
CatmullClarkT<MeshT>::compute_midpoint( const EdgeHandle& _eh)
{
  HalfedgeHandle heh, opp_heh;

  heh      = mesh_.halfedge_handle( _eh, 0);
  opp_heh  = mesh_.halfedge_handle( _eh, 1);

  Point pos( mesh_.point( mesh_.to_vertex_handle( heh)));

  pos +=  mesh_.point( mesh_.to_vertex_handle( opp_heh));

  // boundary edge: just average vertex positions
  // this yields the [1/2 1/2] mask
  if (mesh_.is_boundary(_eh) )
  {
    pos *= 0.5;
  }
  else if (mesh_.status(_eh).selected() )
  {
    pos *= 0.5; // change this
  }

  else // inner edge: add neighbouring Vertices to sum
    // this yields the [1/16 1/16; 3/8 3/8; 1/16 1/16] mask
  {
    pos += mesh_.property(fp_pos_, mesh_.face_handle(heh));
    pos += mesh_.property(fp_pos_, mesh_.face_handle(opp_heh));
    pos *= 0.25;
  }
  mesh_.property( ep_pos_, _eh ) = pos;
}

//-----------------------------------------------------------------------------

template<class MeshT>
void
CatmullClarkT<MeshT>::update_vertex( const VertexHandle& _vh)
{
  Point            pos(0.0,0.0,0.0);

  // TODO boundary, Extraordinary Vertex and  Creased Surfaces
  // see "A Factored Approach to Subdivision Surfaces"
  // http://faculty.cs.tamu.edu/schaefer/research/tutorial.pdf
  // and http://www.cs.utah.edu/~lacewell/subdeval
  if ( mesh_.is_boundary( _vh))
  {
    Normal   Vec;
    pos = mesh_.point(_vh);
    VertexEdgeIter   ve_itr;
    for ( ve_itr = mesh_.ve_iter( _vh); ve_itr; ++ve_itr)
      if ( mesh_.is_boundary( ve_itr.handle()))
        pos += mesh_.property( ep_pos_, ve_itr.handle());
    pos /= 3.0;
  }
  else // inner vertex
  {
    /* For each (nonboundary) vertex V, introduce a new vertex whose
       position is F/n + 2E/n + (n-3)V/n where F is the average of
       the new face vertices of all faces adjacent to the old vertex
       V, E is the average of the midpoints of all edges incident
       on the old vertex V, and n is the number of edges incident on
       the vertex.
       */
    /*
    Normal           Vec;
    VertexEdgeIter   ve_itr;
    double           valence(0.0);

    // R = Calculate Valence and sum of edge midpoints
    for ( ve_itr = mesh_.ve_iter( _vh); ve_itr; ++ve_itr)
    {
      valence+=1.0;
      pos += mesh_.property(ep_pos_, ve_itr.handle());
    }
    pos /= valence*valence;
    */

    double           valence(0.0);
    VOHIter voh_it = mesh_.voh_iter( _vh );
    for( ; voh_it; ++voh_it )
    {
      pos += mesh_.point( mesh_.to_vertex_handle( voh_it ) );
      valence+=1.0;
    }
    pos /= valence*valence;

    VertexFaceIter vf_itr;
    Point          Q(0, 0, 0);
    double         neigboring_faces(0.0);

    for ( vf_itr = mesh_.vf_iter( _vh); vf_itr; ++vf_itr) //, neigboring_faces += 1.0 )
    {
      Q += mesh_.property(fp_pos_, vf_itr.handle());
    }

    Q /= valence*valence;//neigboring_faces;

    pos += (valence-2.0)/valence*mesh_.point(_vh) + Q;
    //      pos = vector_cast<Vec>(mesh_.point(_vh));
  }

  mesh_.property( vp_pos_, _vh ) = pos;
}

//-----------------------------------------------------------------------------

//=============================================================================
} // namespace ACG
//=============================================================================

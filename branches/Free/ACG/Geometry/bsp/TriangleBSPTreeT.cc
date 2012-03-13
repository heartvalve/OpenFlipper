//=============================================================================
//
//  CLASS TriangleBSPTreeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_TRIANGLEBSPTREET_C

//== INCLUDES =================================================================

#include "TriangleBSPTreeT.hh"

//== NAMESPACES ===============================================================

namespace ACG
{

//== IMPLEMENTATION ==========================================================

template< class MeshT >
TriangleBSPTreeT<MeshT>::
TriangleBSPTreeT( const MeshT &_mesh, unsigned int _max_handles, unsigned int _max_depth ) : mesh_( _mesh )
{
   // create Triangle BSP
   triangle_bsp_ = new OpenMeshTriangleBSPT<MeshT>( mesh_ );

   // build Triangle BSP
   triangle_bsp_->reserve( mesh_.n_faces() );

   typename MeshT::ConstFaceIter f_it  = mesh_.faces_begin();
   typename MeshT::ConstFaceIter f_end = mesh_.faces_end();

   for ( ; f_it!=f_end; ++f_it )
      triangle_bsp_->push_back( f_it.handle() );

   triangle_bsp_->build( _max_handles, _max_depth );
}

//-----------------------------------------------------------------------------

template< class MeshT >
typename MeshT::Point
TriangleBSPTreeT<MeshT>::
closest_point( const typename MeshT::Point &_p, typename MeshT::FaceHandle &_fh )
{
   typename MeshT::FaceHandle     fh = triangle_bsp_->nearest( _p ).handle;
   typename MeshT::CFVIter        fv_it = mesh_.cfv_iter( fh );

   const typename MeshT::Point&   pt0 = mesh_.point( fv_it );
   const typename MeshT::Point&   pt1 = mesh_.point( ++fv_it );
   const typename MeshT::Point&   pt2 = mesh_.point( ++fv_it );

   // project
   typename MeshT::Point p_best;
   Geometry::distPointTriangleSquared( _p, pt0, pt1, pt2, p_best );

   // return facehandle
   _fh = fh;

   return p_best;
}

//-----------------------------------------------------------------------------

template< class MeshT >
typename MeshT::Point
TriangleBSPTreeT<MeshT>::
closest_point( const typename MeshT::Point &_p )
{
   typename MeshT::FaceHandle fh;
   return closest_point( _p, fh );
}

//-----------------------------------------------------------------------------

template< class MeshT >
typename MeshT::FaceHandle
TriangleBSPTreeT<MeshT>::
closest_face( const typename MeshT::Point &_p )
{
   return triangle_bsp_->nearest( _p ).handle;
}

//=============================================================================
} // namespace ACG
//=============================================================================

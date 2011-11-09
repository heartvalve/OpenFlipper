//=============================================================================
//
//  CLASS ModRoundnessT - IMPLEMENTATION
//
//=============================================================================

#define MB_MODROUNDNESST_C


//== INCLUDES =================================================================

#include "ModRoundnessT.hh"
#include <ACG/Geometry/Algorithms.hh>


//== NAMESPACES ===============================================================

namespace OpenMesh  {
namespace Decimater {


//== IMPLEMENTATION ==========================================================


template <class DecimaterT>
void
ModRoundnessT<DecimaterT>::
initialize()
{
  typename Mesh::FaceIter  f_it, f_end(mesh_.faces_end());
  typename Mesh::FVIter    fv_it;

  for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
  {
    typename Mesh::Point& p0 = mesh_.point(fv_it=mesh_.fv_iter(f_it));
    typename Mesh::Point& p1 = mesh_.point(++fv_it);
    typename Mesh::Point& p2 = mesh_.point(++fv_it);

    mesh_.property(roundness_, f_it) = 
      1.0/ACG::Geometry::aspectRatio(p0, p1, p2);
  }
}


//-----------------------------------------------------------------------------


template <class DecimaterT>
void
ModRoundnessT<DecimaterT>::
preprocess_collapse(const CollapseInfo& _ci)
{
  typename Mesh::FaceHandle  fh;
  typename Mesh::FVIter      fv_it;

  for (typename Mesh::VFIter vf_it=mesh_.vf_iter(_ci.v0); vf_it; ++vf_it)
  {
    fh = vf_it.handle();
    if (fh != _ci.fl && fh != _ci.fr)
    {
      typename Mesh::Point& p0 = mesh_.point(fv_it=mesh_.fv_iter(fh));
      typename Mesh::Point& p1 = mesh_.point(++fv_it);
      typename Mesh::Point& p2 = mesh_.point(++fv_it);

      mesh_.property(roundness_, fh) = 
	1.0/ACG::Geometry::aspectRatio(p0, p1, p2);
    }
  }
}


//-----------------------------------------------------------------------------


template <class DecimaterT>
float
ModRoundnessT<DecimaterT>::
collapse_priority(const CollapseInfo& _ci)
{
  typename Mesh::VertexHandle  v2, v3;
  typename Mesh::FaceHandle    fh;
  const typename Mesh::Point   *p1(&_ci.p1), *p2, *p3;
  typename Mesh::Scalar        r0, r1, r0_min(1.0), r1_min(1.0);
  typename Mesh::CVVIter       vv_it(mesh_, _ci.v0);

  v3 = vv_it.handle();
  p3 = &mesh_.point(v3); 

  while (vv_it) 
  {
    v2 = v3;
    p2 = p3;

    v3 = (++vv_it).handle(); 
    p3 = &mesh_.point(v3);

    fh = mesh_.face_handle(vv_it.current_halfedge_handle());

    // if not boundary
    if (fh.is_valid())
    {
      // roundness before
      if ((r0 = mesh_.property(roundness_,fh)) < r0_min)
	r0_min = r0;

      // roundness after
      if (!(v2 == _ci.v1 || v3 == _ci.v1))
	if ((r1 = 1.0/ACG::Geometry::aspectRatio(*p1, *p2, *p3)) < r1_min)
	  r1_min = r1;
    }
  }


  if (Base::is_binary())
  {
    return ((r1_min > r0_min) || (r1_min > min_roundness_)) ? 0.0 : -1.0;
  }
  else
  {
    if (r1_min > r0_min)
      return 1.0 - r1_min;
    else 
      return (r1_min > min_roundness_) ? 2.0 - r1_min : -1.0;
  }
}


//=============================================================================
}
}
//=============================================================================

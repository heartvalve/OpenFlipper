//=============================================================================
//
//  CLASS CatmullClarkT
//
//=============================================================================


#ifndef ACG_CATMULLCLARKT_HH
#define ACG_CATMULLCLARKT_HH


//== INCLUDES =================================================================

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG    { // NAMESPACE ACG


//== CLASS DEFINITION =========================================================




/** \class CatmullClarkT CatmullClarkT.hh <ACG/.../CatmullClarkT.hh>
  Plugin Author: Henrik Zimmer (zimmer@informatik.rwth-aachen.de)
  Based on code from Leon Kos, CAD lab, Mech.Eng., University of Ljubljana, Slovenia
  (http://www.lecad.fs.uni-lj.si/~leon) found in OpenMesh mailing list

    A more elaborate description follows.
*/

template <class MeshT>
class CatmullClarkT
{
public:

  typedef typename MeshT::FaceHandle             FaceHandle;
  typedef typename MeshT::VertexHandle           VertexHandle;
  typedef typename MeshT::EdgeHandle             EdgeHandle;
  typedef typename MeshT::HalfedgeHandle         HalfedgeHandle;

  typedef typename MeshT::Point                  Point;
  typedef typename MeshT::Normal                 Normal;
  typedef typename MeshT::FaceIter               FaceIter;
  typedef typename MeshT::EdgeIter               EdgeIter;
  typedef typename MeshT::VertexIter             VertexIter;

  typedef typename MeshT::VertexEdgeIter         VertexEdgeIter;
  typedef typename MeshT::VertexFaceIter         VertexFaceIter;

  typedef typename MeshT::VOHIter                VOHIter;

  /// Constructor
  CatmullClarkT( MeshT& _mesh ) : mesh_(_mesh) { prepare(); }

  /// Destructor
  ~CatmullClarkT() { cleanup(); }

  // get/set functions for properties to make life easier!
	const double& creaseweight( const EdgeHandle& _eh) const { return mesh_.property(creaseWeights_, _eh); }
	      double& creaseweight( const EdgeHandle& _eh)       { return mesh_.property(creaseWeights_, _eh); }


public:

  const char *name() const { return "Uniform CatmullClark"; }

  bool prepare( );
  bool cleanup( );
  bool subdivide( size_t _n);

private:
  // Topology helpers
  void split_edge( const EdgeHandle& _eh);

  void split_face( const FaceHandle& _fh);

  void compute_midpoint( const EdgeHandle& _eh);

  void update_vertex(const  VertexHandle& _vh);

private:
  OpenMesh::VPropHandleT< Point > vp_pos_; // next vertex pos
  OpenMesh::EPropHandleT< Point > ep_pos_; // new edge pts
  OpenMesh::FPropHandleT< Point > fp_pos_; // new face pts
  OpenMesh::EPropHandleT<double> creaseWeights_;// crease weights
  MeshT& mesh_;
};


//=============================================================================
} // NAMESPACE ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_CATMULLCLARKT_C)
#define ACG_CATMULLCLARK_TEMPLATES
#include "CatmullClarkT.cc"
#endif
//=============================================================================
#endif // ACG_CATMULLCLARKT_HH defined
//=============================================================================


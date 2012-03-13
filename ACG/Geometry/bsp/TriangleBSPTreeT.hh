//=============================================================================
//
//  CLASS TriangleBSPTreeT
//
// Author:  Dominik Sibbing <sibbing@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Date: 2008$
//
//=============================================================================


#ifndef ACG_TRIANGLEBSPTREET_HH
#define ACG_TRIANGLEBSPTREET_HH


//== INCLUDES =================================================================

#include "../TriangleTreeBaseT.hh"
#include "TriangleBSPT.hh"

#include <ACG/Geometry/Algorithms.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG
{

//== CLASS DEFINITION =========================================================




/** \class TriangleBSPTreeT TriangleBSPTreeT.hh <ACG/.../TriangleBSPTreeT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class MeshT>
class TriangleBSPTreeT : public TriangleTreeBaseT<MeshT>
{
public:

   /// Constructor
   TriangleBSPTreeT( const MeshT &_mesh, unsigned int _max_handles = 10, unsigned int _max_depth = 100 );

   /// Destructor
   virtual ~TriangleBSPTreeT() {delete triangle_bsp_;}

   /// closest point and face handle
   typename MeshT::Point      closest_point( const typename MeshT::Point &_p, typename MeshT::FaceHandle &_fh );
   
   /// closest point
   typename MeshT::Point      closest_point( const typename MeshT::Point &_p );

   /// closest face
   typename MeshT::FaceHandle closest_face( const typename MeshT::Point &_p );


private:

   // triangle bsp
   OpenMeshTriangleBSPT<MeshT> *triangle_bsp_;
   
   const MeshT &mesh_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_TRIANGLEBSPTREET_C)
#define ACG_TRIANGLEBSPTREET_TEMPLATES
#include "TriangleBSPTreeT.cc"
#endif
//=============================================================================
#endif // ACG_TRIANGLEBSPTREET_HH defined
//=============================================================================


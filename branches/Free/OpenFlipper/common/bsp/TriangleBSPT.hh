//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS TriangleBSPT
//
//=============================================================================

#ifndef MB_TRIANGLEBSP_HH
#define MB_TRIANGLEBSP_HH


//== INCLUDES =================================================================

#include "TriangleBSPCoreT.hh"
#include "BSPImplT.hh"
//#include "Distance.hh"
#include <ACG/Geometry/Algorithms.hh>


//== CLASS DEFINITION =========================================================


template <class BSPTraits>
class TriangleBSPT : public BSPImplT< TriangleBSPCoreT<BSPTraits> >
{
public:
  typedef BSPImplT< TriangleBSPCoreT<BSPTraits> > Base;
  TriangleBSPT(const BSPTraits& _traits) : Base(_traits) {}
};


//== CLASS DEFINITION =========================================================


template <class Mesh>
class OpenMeshTriangleBSPTraits
{
public:

  typedef typename Mesh::Scalar      Scalar;
  typedef typename Mesh::Point       Point;
  typedef typename Mesh::FaceHandle  Handle;

  OpenMeshTriangleBSPTraits(const Mesh& _mesh) : mesh_(_mesh) {}

  void points(Handle _h, Point& _p0, Point& _p1, Point& _p2) const 
  { 
    typename Mesh::CFVIter fv_it(mesh_.cfv_iter(_h));
    _p0 = mesh_.point(fv_it); ++fv_it;
    _p1 = mesh_.point(fv_it); ++fv_it;
    _p2 = mesh_.point(fv_it);
  }

  Scalar sqrdist(Handle _h, const Point& _p) const 
  {
    Point p0, p1, p2, q;
    points(_h, p0, p1, p2);
    return ACG::Geometry::distPointTriangleSquared(_p, p0, p1, p2, q);
  }

private:

  const Mesh& mesh_;
};


//== CLASS DEFINITION =========================================================


template <class Mesh>
class OpenMeshTriangleBSPT 
  : public TriangleBSPT<OpenMeshTriangleBSPTraits<Mesh> >
{
public:
  typedef OpenMeshTriangleBSPTraits<Mesh>  Traits;
  typedef TriangleBSPT<Traits>             Base;
  OpenMeshTriangleBSPT(const Mesh& _mesh) : Base(Traits(_mesh)) {}
};


//=============================================================================
#endif // MB_TRIANGLEBSP_HH defined
//=============================================================================

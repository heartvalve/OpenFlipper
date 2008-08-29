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
//  CLASS BSPImplT
//
//=============================================================================

#ifndef BSPIMPLT_HH
#define BSPIMPLT_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/Geometry/VectorT.hh>


//== CLASS DEFINITION =========================================================


template <class BSPCore>
class BSPImplT : public BSPCore
{
public: //---------------------------------------------------------------------

  typedef typename BSPCore::Traits      Traits;
  typedef typename BSPCore::Handle      Handle;
  typedef typename BSPCore::Point       Point;
  typedef typename BSPCore::Scalar      Scalar;
  typedef typename BSPCore::Node        Node;
  typedef typename BSPCore::Handles     Handles;
  typedef typename BSPCore::HandleIter  HandleIter;
  

public: //---------------------------------------------------------------------

  BSPImplT(const Traits& _traits) : BSPCore(_traits) {}
  ~BSPImplT() {}


  /// Store nearest neighbor information
  struct NearestNeighbor
  {
    NearestNeighbor() {}
    NearestNeighbor(Handle _h, Scalar _d) : handle(_h), dist(_d) {}
    Handle  handle;
    Scalar  dist;
  };


  /// Return handle of the nearest neighbor
  NearestNeighbor nearest(const Point& _p) const;

  

private: //---------------------------------------------------------------------


  /// Store nearest neighbor information
  struct NearestNeighborData
  {
    Point   ref;
    Scalar  dist;
    Handle  nearest;
  };


  // Recursive part of nearest()
  void _nearest(Node* _node, NearestNeighborData& _data) const;
};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(BSPIMPLT_C)
#  define BSPIMPLT_TEMPLATES
#  include "BSPImplT.cc"
#endif
//=============================================================================
#endif // BSPIMPLT_HH defined
//=============================================================================

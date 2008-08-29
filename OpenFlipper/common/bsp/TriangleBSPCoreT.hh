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
//  CLASS TriangleBSPCoreT
//
//=============================================================================

#ifndef TRIANGLEBSPCORET_HH
#define TRIANGLEBSPCORET_HH


//== INCLUDES =================================================================


#include <ACG/Geometry/Types/PlaneT.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <vector>


//== CLASS DEFINITION =========================================================


template <class BSPTraits>
class TriangleBSPCoreT
{
public: //---------------------------------------------------------------------

  typedef BSPTraits                      Traits;
  typedef typename BSPTraits::Point      Point;
  typedef typename BSPTraits::Handle     Handle;
  typedef typename Point::value_type     Scalar;
  typedef ACG::Geometry::PlaneT<Scalar>  Plane;
  typedef std::vector<Handle>            Handles;
  typedef typename Handles::iterator     HandleIter;



public: //---------------------------------------------------------------------


  /** Constructor: need traits that define the types and 
      give us the points by traits_.point(PointHandle) */
  TriangleBSPCoreT(const BSPTraits& _traits) : traits_(_traits), root_(0) {}

  /// Destructor
  ~TriangleBSPCoreT() { delete root_; }


  /// Reserve memory for _n entries
  void reserve(unsigned int _n) { handles_.reserve(_n); }
  /// Add a handle to the BSP
  void push_back(Handle _h)     { handles_.push_back(_h); }
  /// Finally build the tree
  void build(unsigned int _max_handles, unsigned int _max_depth);



protected: //-------------------------------------------------------------------


  // Node of the tree: contains parent, children and splitting plane
  struct Node
  {
    Node(const Handles& _handles, Node* _parent);
    ~Node();

    HandleIter begin() { return handles_.begin(); }
    HandleIter end()   { return handles_.end();   }

    Handles     handles_;
    Node        *parent_, *left_child_, *right_child_;
    Plane       plane_;
  };

  

private: //---------------------------------------------------------------------


  // Recursive part of build()
  void _build(Node*        _node,
	      unsigned int _max_handles, 
	      unsigned int _depth);




protected: //-------------------------------------------------------------------


  BSPTraits  traits_;
  Handles    handles_;
  Node*      root_;
};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(TRIANGLEBSPCORET_C)
#  define TRIANGLEBSPCORET_TEMPLATES
#  include "TriangleBSPCoreT.cc"
#endif
//=============================================================================
#endif // TRIANGLEBSPCORET_HH defined
//=============================================================================

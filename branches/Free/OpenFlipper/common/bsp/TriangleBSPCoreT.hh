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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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

#include "TriangleBSPT.hh"


//== CLASS DEFINITION =========================================================


template <class BSPTraits>
class TriangleBSPCoreT
{
public: //---------------------------------------------------------------------

  typedef BSPTraits                      Traits;
  typedef typename BSPTraits::Point      Point;
  typedef typename BSPTraits::Handle     Handle;
  typedef typename BSPTraits::Node	 Node;
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


private: //---------------------------------------------------------------------


  // Recursive part of build()
  void _build(Node*        _node,
	      unsigned int _max_handles, 
	      unsigned int _depth);




protected: //-------------------------------------------------------------------


  BSPTraits  traits_;
  Handles    handles_;
  Node*      root_;
  int	nodes;
  
};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(TRIANGLEBSPCORET_C)
#  define TRIANGLEBSPCORET_TEMPLATES
#  include "TriangleBSPCoreT.cc"
#endif
//=============================================================================
#endif // TRIANGLEBSPCORET_HH defined
//=============================================================================

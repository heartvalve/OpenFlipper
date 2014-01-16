/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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

//=============================================================================
//
//  CLASS SmartTaggerVT
//
//
//   original Author:  David Bommes <bommes@cs.rwth-aachen.de>
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================


#ifndef ACG_SMARTTAGGERT_HH
#define ACG_SMARTTAGGERT_HH


//== INCLUDES =================================================================

// OpenMesh
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/System/omstream.hh>


//== DEFINES ==================================================================

#define STV_DEBUG_CHECKS

//== NAMESPACES ===============================================================

namespace ACG {

//== FORWARD DECLARATIONS =====================================================

// Smarttagging for vertices
template< class Mesh> class SmartTaggerVT;
// Smarttagging for edges
template< class Mesh> class SmartTaggerET;
// Smarttagging for faces
template< class Mesh> class SmartTaggerFT;
// Smarttagging for halfedges
template< class Mesh> class SmartTaggerHT;


//== CLASS DEFINITION =========================================================


//template <class Mesh, class EPHandle, class EHandle>
template <class Mesh, class EHandle, class EPHandle>
class SmartTaggerT
{
public:
   
  /// Constructor
  SmartTaggerT(Mesh& _mesh, unsigned int _tag_range = 1);
 
  /// Destructor
  ~SmartTaggerT();

  // untag all elements
  inline void untag_all();

  // untag all elements and set new tag_range
  inline void untag_all( const unsigned int _new_tag_range);
  
  // set tag to a value in [0..tag_range]
  inline void         set_tag  ( const EHandle _eh, unsigned int _tag = 1);
  // get tag value in range [0..tag_range]
  inline unsigned int get_tag  ( const EHandle _eh) const;
  // overloaded member for boolean tags
  inline bool         is_tagged( const EHandle _eh) const;

  // set new tag range and untag_all
  inline void         set_tag_range( const unsigned int _tag_range);
  
protected:

  inline void all_tags_to_zero();

protected:

  // Reference to Mesh
  Mesh& mesh_;

  // property which holds the current tags
  EPHandle ep_tag_;

  // current tags range is [current_base_+1...current_base_+tag_range_]
  unsigned int current_base_;

  // number of different tagvalues available
  unsigned int tag_range_;
};


//== SPECIALIZATION ===========================================================

  // define standard Tagger  
  template< class Mesh>
  class SmartTaggerVT 
    : public SmartTaggerT< Mesh, typename Mesh::VertexHandle, OpenMesh::VPropHandleT<unsigned int> > 
  {
  public:
    typedef SmartTaggerT< Mesh, typename Mesh::VertexHandle, OpenMesh::VPropHandleT<unsigned int> > BaseType;
    SmartTaggerVT(Mesh& _mesh, unsigned int _tag_range = 1) : BaseType(_mesh, _tag_range) {}
  };

  template< class Mesh>
  class SmartTaggerET 
    : public SmartTaggerT< Mesh, typename Mesh::EdgeHandle, OpenMesh::EPropHandleT<unsigned int> > 
  {
  public:
    typedef SmartTaggerT< Mesh, typename Mesh::EdgeHandle, OpenMesh::EPropHandleT<unsigned int> > BaseType;
    SmartTaggerET(Mesh& _mesh, unsigned int _tag_range = 1) : BaseType(_mesh, _tag_range) {}
  };

  template< class Mesh>
  class SmartTaggerFT 
    : public SmartTaggerT< Mesh, typename Mesh::FaceHandle, OpenMesh::FPropHandleT<unsigned int> > 
  {
  public:
    typedef SmartTaggerT< Mesh, typename Mesh::FaceHandle, OpenMesh::FPropHandleT<unsigned int> > BaseType;
    SmartTaggerFT(Mesh& _mesh, unsigned int _tag_range = 1): BaseType(_mesh, _tag_range) {}
  };

  template< class Mesh>
  class SmartTaggerHT 
    : public SmartTaggerT< Mesh, typename Mesh::HalfedgeHandle, OpenMesh::HPropHandleT<unsigned int> >
  {
  public:
    typedef SmartTaggerT< Mesh, typename Mesh::HalfedgeHandle, OpenMesh::HPropHandleT<unsigned int> > BaseType;
    SmartTaggerHT(Mesh& _mesh, unsigned int _tag_range = 1): BaseType(_mesh, _tag_range){}
  };


//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_SMARTTAGGERT_C)
#define ACG_SMARTTAGGERT_TEMPLATES
#include "SmartTaggerT.cc"
#endif
//=============================================================================
#endif // ACG_SMARTTAGGERT_HH defined
//=============================================================================


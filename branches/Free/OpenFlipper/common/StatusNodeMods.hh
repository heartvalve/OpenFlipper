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
//  Mods for the Status Nodes
//
//=============================================================================

/** Structure used in the Status Nodes to show if an area is marked as a modeling area
 */
template< class MeshT >
struct AreaNodeMod
{
  /// tell status node if the vertex is marked as modeling area
  static inline bool is_area(const MeshT& _m, typename MeshT::VertexHandle _vh)
  { return _m.status(_vh).is_bit_set(AREA); }

  /// tell status node if the face is marked as modeling area
  static inline bool is_area(const MeshT& _m, typename MeshT::FaceHandle _fh)
  { return _m.status(_fh).is_bit_set(AREA); }

  /// tell status node if the vertex is marked as modeling area
  static bool is_vertex_selected(const MeshT& _mesh, typename MeshT::VertexHandle _vh)
  {
    if (_mesh.is_isolated(_vh))
      return is_area(_mesh, _vh);
    else
      return false;
  }


  /// default to false
  static bool is_edge_selected(const MeshT& /*_mesh */, typename MeshT::EdgeHandle /* _eh */ )
  {
    return false;
  }


  /// tell status node if the face is marked as modeling area
  static bool is_face_selected(const MeshT& _mesh, typename MeshT::FaceHandle _fh)
  {
    return is_area(_mesh, _fh);
  }
};

/** Structure used in the Status Nodes to show if an area is marked as a handle region
 */
template< class MeshT >
    struct HandleNodeMod
{

  /// tell status node if the vertex is marked as handle area 
  static inline bool is_handle(const MeshT& _m, typename MeshT::VertexHandle _vh)
  { return _m.status(_vh).is_bit_set(HANDLEAREA); }

  /// tell status node if the face is marked as handle area
  static inline bool is_handle(const MeshT& _m, typename MeshT::FaceHandle _fh)
  { return _m.status(_fh).is_bit_set(HANDLEAREA); }


  /// tell status node if the vertex is marked as handle area
  static bool is_vertex_selected(const MeshT& _mesh, typename MeshT::VertexHandle _vh)
  {
    if (!is_handle(_mesh, _vh))
      return false;

    for (typename MeshT::CVFIter vf_it(_mesh.cvf_iter(_vh)); vf_it; ++vf_it)
      if (is_handle(_mesh, vf_it))
        return false;

    return true;
  }

  /// tell status node if the edge is marked as handle area
  static bool is_edge_selected(const MeshT& _mesh, typename MeshT::EdgeHandle _eh)
  {
    typename MeshT::HalfedgeHandle hh;
    typename MeshT::FaceHandle     fh;

    hh = _mesh.halfedge_handle(_eh, 0);
    if (!is_handle(_mesh, _mesh.to_vertex_handle(hh))) return false;

    fh = _mesh.face_handle(hh);
    if (fh.is_valid() && is_handle(_mesh, fh)) return false;

    hh = _mesh.halfedge_handle(_eh, 1);
    if (!is_handle(_mesh, _mesh.to_vertex_handle(hh))) return false;

    fh = _mesh.face_handle(hh);
    if (fh.is_valid() && is_handle(_mesh, fh)) return false;

    return true;
  }


  /// tell status node if the face is marked as handle area
  static bool is_face_selected(const MeshT& _mesh, typename MeshT::FaceHandle _fh)
  {
    return is_handle(_mesh, _fh);
  }
};

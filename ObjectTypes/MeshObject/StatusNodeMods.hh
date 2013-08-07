/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef STATUSNODEMODS_HH
#define STATUSNODEMODS_HH

//=============================================================================
//
//  Mesh Attribute bits for the Status Nodes
//
//=============================================================================

/** \brief Additional Node status Bits for Mesh
*
* These status bits define special Attributes on the mesh. The are used to define the handle area
* and modeling areas. 
*
*/
enum StatusBits {
  /// Modifyable area
  AREA      = OpenMesh::Attributes::UNUSED << 1,
  /// Handle Area
  HANDLEAREA = OpenMesh::Attributes::UNUSED << 2
};

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
  {
    for (typename MeshT::ConstFaceVertexIter cfv_it = _m.cfv_iter(_fh); cfv_it.is_valid(); ++cfv_it)
      if (_m.status(*cfv_it).is_bit_set(AREA))
        return true;

    return false;
  }

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


  /// default to false
  static bool is_halfedge_selected(const MeshT& /*_mesh */, typename MeshT::HalfedgeHandle /* _eh */ )
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
  {
      bool isArea = true;
      for(typename MeshT::ConstFaceVertexIter cfv_it = _m.cfv_iter(_fh); cfv_it.is_valid(); ++cfv_it) {
          if(!_m.status(*cfv_it).is_bit_set(HANDLEAREA)) {
              isArea = false;
              break;
          }
      }
      return isArea;
  }


  /// tell status node if the vertex is marked as handle area
  static bool is_vertex_selected(const MeshT& _mesh, typename MeshT::VertexHandle _vh)
  {
    if (!is_handle(_mesh, _vh))
      return false;

    for (typename MeshT::CVFIter vf_it(_mesh.cvf_iter(_vh)); vf_it.is_valid(); ++vf_it)
      if (is_handle(_mesh, *vf_it))
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


  /// default to false
  static bool is_halfedge_selected(const MeshT& /*_mesh */, typename MeshT::HalfedgeHandle /* _eh */ )
  {
    return false;
  }


  /// tell status node if the face is marked as handle area
  static bool is_face_selected(const MeshT& _mesh, typename MeshT::FaceHandle _fh)
  {
    return is_handle(_mesh, _fh);
  }
};

/** Structure used in the Status Nodes to show if an primitive is marked as a feature
 */
template< class MeshT >
    struct FeatureNodeMod
{

  /// tell status node if the vertex is marked as handle area
  static bool is_vertex_selected(const MeshT& _mesh, typename MeshT::VertexHandle _vh)
  {
    return _mesh.status(_vh).feature();
  }

  /// tell status node if the edge is marked as handle area
  static bool is_edge_selected(const MeshT& _mesh, typename MeshT::EdgeHandle _eh)
  {
    return _mesh.status(_eh).feature();
  }


  /// default to false
  static bool is_halfedge_selected(const MeshT& _mesh, typename MeshT::HalfedgeHandle _heh)
  {
    return _mesh.status(_heh).feature();
  }

  /// tell status node if the face is marked as feature
  static bool is_face_selected(const MeshT& _mesh, typename MeshT::FaceHandle _fh)
  {
    return _mesh.status(_fh).feature();
  }

};

#endif


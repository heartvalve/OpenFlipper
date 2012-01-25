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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2011-01-26 10:23:50 +0100 (Wed, 26 Jan 2011) $                    *
*                                                                            *
\*===========================================================================*/


/**
 * \file OpenFlipperPolyhedralMesh.hh
 * This class extends the polyhedral mesh base class provided
 * by OpenVolumeMesh by color property extensions
*/

#ifndef OPENFLIPPER_POLYHEDRAL_MESH_INCLUDE_HH
#define OPENFLIPPER_POLYHEDRAL_MESH_INCLUDE_HH

//== INCLUDES =================================================================

#include <string>
#include <ACG/Math/VectorT.hh>
#include <OpenVolumeMesh/PolyhedralMesh/PolyhedralMesh.hh>

//== NAMESPACE ================================================================

namespace OpenVolumeMesh {

//== CLASS DEF ================================================================

template < typename VecT >
class ColorPolyhedralMesh : public PolyhedralMesh<VecT> {
public:
    ColorPolyhedralMesh() :
        str_vc_prop_("Vertex Colors"),
        str_ec_prop_("Edge Colors"),
        str_hec_prop_("HalfEdge Colors"),
        str_fc_prop_("Face Colors"),
        str_hfc_prop_("HalfFace Colors"),
        str_cc_prop_("Cell Colors"),
        vcolors_available_(false),
        ecolors_available_(false),
        hecolors_available_(false),
        fcolors_available_(false),
        hfcolors_available_(false),
        ccolors_available_(false) {

        // Request statuses for all entities
        PolyhedralMesh<VecT>::request_status();
    }

    virtual ~ColorPolyhedralMesh() {

        release_vertex_colors();
        release_edge_colors();
        release_halfedge_colors();
        release_face_colors();
        release_halfface_colors();
        release_cell_colors();
    }

    //==================
    // Vertices
    //==================
    void request_vertex_colors() {
        PolyhedralMesh<VecT>::add_property(vc_prop_, str_vc_prop_);
        vcolors_available_ = true;
    }
    void release_vertex_colors() {
        if(vcolors_available_) {
            PolyhedralMesh<VecT>::remove_property(vc_prop_);
        }
    }
    bool has_vertex_colors() const {
        return vcolors_available_;
    }
    void vertex_color(const VertexHandle& _vh, const ACG::Vec4f& _c) {
        if(vcolors_available_) {
            PolyhedralMesh<VecT>::property(vc_prop_, _vh) = _c;
        }
    }
    const ACG::Vec4f& vertex_color(const VertexHandle& _vh) const {
        if(vcolors_available_) {
            return PolyhedralMesh<VecT>::property(vc_prop_, _vh);
        }
    }

    //==================
    // Edges
    //==================
    void request_edge_colors() {
        PolyhedralMesh<VecT>::add_property(ec_prop_, str_ec_prop_);
        ecolors_available_ = true;
    }
    void release_edge_colors() {
        if(ecolors_available_) {
            PolyhedralMesh<VecT>::remove_property(ec_prop_);
        }
    }
    bool has_edge_colors() const {
        return ecolors_available_;
    }
    void edge_color(const EdgeHandle& _eh, const ACG::Vec4f& _c) {
        if(ecolors_available_) {
            PolyhedralMesh<VecT>::property(ec_prop_, _eh) = _c;
        }
    }
    const ACG::Vec4f& edge_color(const EdgeHandle& _eh) const {
        if(ecolors_available_) {
            return PolyhedralMesh<VecT>::property(ec_prop_, _eh);
        }
    }

    //==================
    // Half-Edges
    //==================
    void request_halfedge_colors() {
        PolyhedralMesh<VecT>::add_property(hec_prop_, str_hec_prop_);
        hecolors_available_ = true;
    }
    void release_halfedge_colors() {
        if(hecolors_available_) {
            PolyhedralMesh<VecT>::remove_property(hec_prop_);
        }
    }
    bool has_halfedge_colors() const {
        return hecolors_available_;
    }
    void halfedge_color(const HalfEdgeHandle& _heh, const ACG::Vec4f& _c) {
        if(hecolors_available_) {
            PolyhedralMesh<VecT>::property(hec_prop_, _heh) = _c;
        }
    }
    const ACG::Vec4f& halfedge_color(const HalfEdgeHandle& _heh) const {
        if(hecolors_available_) {
            return PolyhedralMesh<VecT>::property(hec_prop_, _heh);
        }
    }

    //==================
    // Faces
    //==================
    void request_face_colors() {
        PolyhedralMesh<VecT>::add_property(fc_prop_, str_fc_prop_);
        fcolors_available_ = true;
    }
    void release_face_colors() {
        if(fcolors_available_) {
            PolyhedralMesh<VecT>::remove_property(fc_prop_);
        }
    }
    bool has_face_colors() const {
        return fcolors_available_;
    }
    void face_color(const FaceHandle& _fh, const ACG::Vec4f& _c) {
        if(fcolors_available_) {
            PolyhedralMesh<VecT>::property(fc_prop_, _fh) = _c;
        }
    }
    const ACG::Vec4f& face_color(const FaceHandle& _fh) const {
        if(fcolors_available_) {
            return PolyhedralMesh<VecT>::property(fc_prop_, _fh);
        }
    }

    //==================
    // Half-Faces
    //==================
    void request_halfface_colors() {
        PolyhedralMesh<VecT>::add_property(hfc_prop_, str_hfc_prop_);
        hfcolors_available_ = true;
    }
    void release_halfface_colors() {
        if(hfcolors_available_) {
            PolyhedralMesh<VecT>::remove_property(hfc_prop_);
        }
    }
    bool has_halfface_colors() const {
        return hfcolors_available_;
    }
    void halfface_color(const HalfFaceHandle& _hfh, const ACG::Vec4f& _c) {
        if(hfcolors_available_) {
            PolyhedralMesh<VecT>::property(hfc_prop_, _hfh) = _c;
        }
    }
    const ACG::Vec4f& halfface_color(const HalfFaceHandle& _hfh) const {
        if(hfcolors_available_) {
            return PolyhedralMesh<VecT>::property(hfc_prop_, _hfh);
        }
    }

    //==================
    // Cells
    //==================
    void request_cell_colors() {
        PolyhedralMesh<VecT>::add_property(cc_prop_, str_cc_prop_);
        ccolors_available_ = true;
    }
    void release_cell_colors() {
        if(ccolors_available_) {
            PolyhedralMesh<VecT>::remove_property(cc_prop_);
        }
    }
    bool has_cell_colors() const {
        return ccolors_available_;
    }
    void cell_color(const CellHandle& _ch, const ACG::Vec4f& _c) {
        if(ccolors_available_) {
            PolyhedralMesh<VecT>::property(cc_prop_, _ch) = _c;
        }
    }
    const ACG::Vec4f& cell_color(const CellHandle& _ch) const {
        if(ccolors_available_) {
            return PolyhedralMesh<VecT>::property(cc_prop_, _ch);
        }
    }

private:

    // Constants
    const std::string str_vc_prop_;
    const std::string str_ec_prop_;
    const std::string str_hec_prop_;
    const std::string str_fc_prop_;
    const std::string str_hfc_prop_;
    const std::string str_cc_prop_;

    // Property handles
    VPropHandleT<ACG::Vec4f> vc_prop_;
    EPropHandleT<ACG::Vec4f> ec_prop_;
    HEPropHandleT<ACG::Vec4f> hec_prop_;
    FPropHandleT<ACG::Vec4f> fc_prop_;
    HFPropHandleT<ACG::Vec4f> hfc_prop_;
    CPropHandleT<ACG::Vec4f> cc_prop_;

    // Availability flags
    bool vcolors_available_;
    bool ecolors_available_;
    bool hecolors_available_;
    bool fcolors_available_;
    bool hfcolors_available_;
    bool ccolors_available_;
};

} // Namespace OpenVolumeMesh

// Typedef for default mesh type
typedef OpenVolumeMesh::ColorPolyhedralMesh<ACG::Vec3d> PolyhedralMesh;

//=============================================================================
#endif // OPENFLIPPER_POLYHEDRAL_MESH_INCLUDE_HH defined
//=============================================================================


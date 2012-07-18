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
 *   $LastChangedBy$                                                 *
 *   $Date$                    *
 *                                                                            *
 \*===========================================================================*/

#ifndef VOLUMEMESHNODE_HH
#define VOLUMEMESHNODE_HH

//== INCLUDES =================================================================

#include <ACG/GL/gl.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <OpenVolumeMesh/Core/OpenVolumeMeshHandle.hh>
#include <OpenVolumeMesh/Core/BaseEntities.hh>
#include <OpenVolumeMesh/Attribs/StatusAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================

/** \class VolumeMeshNodeT VolumeMeshNodeT.hh
 */

template<class VolumeMeshT>
class VolumeMeshNodeT: public BaseNode {
public:

    // typedefs for easy access
    typedef VolumeMeshT VolumeMesh;
    typedef OpenVolumeMesh::VertexHandle VertexHandle;
    typedef OpenVolumeMesh::EdgeHandle EdgeHandle;
    typedef OpenVolumeMesh::HalfEdgeHandle HalfEdgeHandle;
    typedef OpenVolumeMesh::FaceHandle FaceHandle;
    typedef OpenVolumeMesh::HalfFaceHandle HalfFaceHandle;
    typedef OpenVolumeMesh::CellHandle CellHandle;

    typedef typename VolumeMeshT::PointT Vertex;
    typedef OpenVolumeMesh::OpenVolumeMeshEdge Edge;
    typedef OpenVolumeMesh::OpenVolumeMeshFace Face;
    typedef OpenVolumeMesh::OpenVolumeMeshCell Cell;

    class Plane {
    public:
        Plane(const Vec3d& _p, const Vec3d& _n, const Vec3d& _x, const Vec3d& _y) :
            position(_p), normal(_n), xdirection(_x), ydirection(_y) {
        }

        // position
        Vec3d position;
        // normal
        Vec3d normal;
        // xdirection scaled by 1.0/width -> [0,1] is inrange
        Vec3d xdirection;
        // ydirection scaled by 1.0/height -> [0,1] is inrange
        Vec3d ydirection;
    };

    /// Constructor
    VolumeMeshNodeT(const VolumeMesh& _mesh,
                    OpenVolumeMesh::StatusAttrib& _statusAttrib,
                    OpenVolumeMesh::ColorAttrib<Vec4f>& _colorAttrib,
                    OpenVolumeMesh::NormalAttrib<VolumeMesh>& _normalAttrib,
                    const MaterialNode* _matNode, BaseNode* _parent = 0,
                    std::string _name = "<VolumeMeshNode>");

    /// Destructor
    ~VolumeMeshNodeT();

    /// static name of this class
    ACG_CLASSNAME(VolumeMeshNodeT)

    /// return available draw modes
    DrawModes::DrawMode availableDrawModes() const;

    /// update bounding box
    void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

    /// draw lines and normals
    void draw(GLState& /*_state*/, const DrawModes::DrawMode& _drawMode);

    /// picking
    void pick(GLState& _state, PickTarget _target);

    // clear all cutplanes
    void clear_cut_planes() {
        cut_planes_.clear();
    }

    // add a new cutplane
    void add_cut_plane(const Plane& _p);
    void add_cut_plane(const Vec3d& _p, const Vec3d& _n, const Vec3d& _xsize, const Vec3d& _ysize);

    // set shrinkage of cells
    void set_scaling(double _scale) {
        scale_ = _scale;
        geom_changed_ = true;
    }

    // Get scaling
    double scaling() const { return scale_; }

    // Choose whether to visualize the boundary only
    void set_boundary_only(bool _boundary) { boundary_only_ = _boundary; }
    bool boundary_only() const { return boundary_only_; }

    // Show irregular entities
    void set_show_irregs(bool _show) { show_irregs_ = _show; }
    bool show_irregs() const { return show_irregs_; }

    // Show outer valence 2 irregularities
    void set_show_outer_valence_two(bool _show) { show_outer_val2_irregs_ = _show; }
    bool show_outer_valence_two() const { return show_outer_val2_irregs_; }

    // Set translucency flag
    void set_translucent(bool _translucent) { translucent_ = _translucent; }
    bool translucent() const { return translucent_; }

    // Set translucency factor
    void set_translucency_factor(float _factor) { translucency_factor_ = _factor; }
    float translucency_factor() const { return translucency_factor_; }

    // Set whether to update the topology in next render pass
    void set_topology_changed(bool _topology_changed) { topology_changed_ = _topology_changed; }

    // Set whether to update the geometry in next render pass
    void set_geometry_changed(bool _geom_changed) { geom_changed_ = _geom_changed; }

    // Set whether to update the color in next render pass
    void set_color_changed(bool _color_changed) { color_changed_ = _color_changed; }

    // Set whether to update the selectopm in next render pass
    void set_selection_changed(bool _selection_changed) { selection_changed_ = _selection_changed; }

    // Set whether to update the valence visualization in next render pass
    void set_valence_changed(bool _valence_changed) { valence_changed_ = _valence_changed; }

private:

    // initialize display list
    void init();

    // Update geometry display list
    void update_geometry(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Update selection display list
    void update_selection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Update valence display list
    void update_valence(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw vertices
    void draw_vertices(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw edges
    void draw_edges(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw faces
    void draw_faces(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw vertex selection
    void draw_vertex_selection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw edge selection
    void draw_edge_selection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw face selection
    void draw_face_selection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw cell selection
    void draw_cell_selection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Draw irregular edges
    void draw_irreg_edges(GLState& _state, const DrawModes::DrawMode& _drawMode);

    // Get color for valence
    ACG::Vec4f get_valence_color_code(unsigned int _valence, bool _inner) const;

    // test whether _p is inside w.r.t. all cutplanes
    bool is_inside(const Vec3d& _p);

    /// pick vertices
    void pick_vertices(GLState& _state);

    /// pick edges
    void pick_edges(GLState& _state, unsigned int _offset);

    /// pick faces
    void pick_faces(GLState& _state, unsigned int _offset);

    /// pick cells
    void pick_cells(GLState& _state, unsigned int _offset);

    /// Copy constructor (not used)
    VolumeMeshNodeT(const VolumeMeshNodeT& _rhs);

    /// Assignment operator (not used)
    VolumeMeshNodeT& operator=(const VolumeMeshNodeT& _rhs);

private:

    const VolumeMesh& mesh_;

    std::vector<Plane> cut_planes_;

    double scale_;

    bool topology_changed_;
    bool geom_changed_;
    bool color_changed_;
    bool selection_changed_;
    bool valence_changed_;

    GLuint geometryList_;
    GLuint selectionList_;
    GLuint valenceList_;

    DrawModes::DrawMode lastDrawMode_;

    // Flag that indicates whether to render the boundary faces only
    bool boundary_only_;

    // Flag that indicates whether to render cells translucently
    bool translucent_;

    // Translucency factor in [0, 1]
    GLfloat translucency_factor_;

    // Flag that indicates whether to render irregular edges
    bool show_irregs_;

    // Indicate whether to display outer valence 2 edges
    bool show_outer_val2_irregs_;

    // Hold color of selection
    ACG::Vec4f selection_color_;

    OpenVolumeMesh::StatusAttrib& statusAttrib_;
    OpenVolumeMesh::ColorAttrib<Vec4f>& colorAttrib_;
    OpenVolumeMesh::NormalAttrib<VolumeMesh>& normalAttrib_;

    const MaterialNode* materialNode_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(VOLUMEMESHNODET_CC)
#include "VolumeMeshNodeT.cc"
#endif
//=============================================================================
#endif // VOLUMEMESHNODE_HH
//=============================================================================


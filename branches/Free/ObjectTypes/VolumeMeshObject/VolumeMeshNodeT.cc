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

#define VOLUMEMESHNODET_CC

//== INCLUDES =================================================================

#include "VolumeMeshNode.hh"
#include <ACG/GL/gl.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

template<class VolumeMeshT>
VolumeMeshNodeT<VolumeMeshT>::VolumeMeshNodeT(const VolumeMesh& _mesh,
                                              OpenVolumeMesh::StatusAttrib& _statusAttrib,
                                              OpenVolumeMesh::ColorAttrib<Vec4f>& _colorAttrib,
                                              OpenVolumeMesh::NormalAttrib<VolumeMesh>& _normalAttrib,
                                              const MaterialNode* _matNode, BaseNode* _parent,
                                              std::string _name) :
BaseNode(_parent, _name),
mesh_(_mesh),
scale_(1.0),
topology_changed_(true),
geom_changed_(true),
color_changed_(true),
selection_changed_(true),
valence_changed_(true),
lastDrawMode_(DrawModes::NONE),
boundary_only_(false),
translucent_(false),
translucency_factor_(0.1),
show_irregs_(false),
show_outer_val2_irregs_(false),
selection_color_(ACG::Vec4f(1.0f, 0.0f, 0.0f, 1.0f)),
statusAttrib_(_statusAttrib),
colorAttrib_(_colorAttrib),
normalAttrib_(_normalAttrib),
materialNode_(_matNode) {

    // Initialize rendering display list
    init();
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
VolumeMeshNodeT<VolumeMeshT>::~VolumeMeshNodeT() {

    glDeleteLists(geometryList_, 1);
    glDeleteLists(selectionList_, 1);
    glDeleteLists(valenceList_, 1);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {

    int n_vertices(mesh_.n_vertices());
    for (int i = 0; i < n_vertices; ++i) {
        Vec3d p(mesh_.vertex(VertexHandle(i)));
        _bbMin.minimize(p);
        _bbMax.maximize(p);
    }
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
DrawModes::DrawMode VolumeMeshNodeT<VolumeMeshT>::availableDrawModes() const {
    return (DrawModes::WIREFRAME | DrawModes::POINTS | DrawModes::POINTS_COLORED | DrawModes::SOLID_FLAT_SHADED);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::update_geometry(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    glNewList(geometryList_, GL_COMPILE);

    // Draw points
    draw_vertices(_state, _drawMode);

    // Draw edges
    draw_edges(_state, _drawMode);

    // Draw faces
    draw_faces(_state, _drawMode);

    glEndList();

    geom_changed_ = false;
}


//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_vertices(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    if (_drawMode & DrawModes::POINTS) {

        // Set point size
        glPointSize(materialNode_->point_size());

        // draw all points
        glBegin(GL_POINTS);

        int n_vertices(mesh_.n_vertices());
        for (int i = 0; i < n_vertices; ++i) {
            Vec3d p = mesh_.vertex(VertexHandle(i));

            if(is_inside(p)) {

                // Skip selected
                if(statusAttrib_[VertexHandle(i)].selected()) continue;

                glVertex3d(p[0], p[1], p[2]);
            }
        }
        glEnd();
    } else if (_drawMode & DrawModes::POINTS_COLORED) {

        // Set point size
        glPointSize(materialNode_->point_size());

        // draw all points
        glBegin(GL_POINTS);

        int n_vertices(mesh_.n_vertices());
        for (int i = 0; i < n_vertices; ++i) {
            Vec3d p = mesh_.vertex(VertexHandle(i));

            if(is_inside(p)) {

                // Skip selected
                if(statusAttrib_[VertexHandle(i)].selected()) continue;

                const Vec4f& c = colorAttrib_[VertexHandle(i)];

                glColor4f(c[0], c[1], c[2], c[3]);

                glVertex3d(p[0], p[1], p[2]);
            }
        }
        glEnd();
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_edges(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // draw line segments
    if (_drawMode & DrawModes::WIREFRAME) {

        int n_edges(mesh_.n_edges());

        for (int i = 0; i < n_edges; ++i) {

            Edge e(mesh_.edge(EdgeHandle(i)));
            Vec3d p0 = mesh_.vertex(e.from_vertex());
            Vec3d p1 = mesh_.vertex(e.to_vertex());

            if (is_inside(p0) && is_inside(p1)) {

                // Skip selected
                if(statusAttrib_[EdgeHandle(i)].selected())
                    continue;

                glBegin(GL_LINES);
                glVertex3d(p0[0], p0[1], p0[2]);
                glVertex3d(p1[0], p1[1], p1[2]);
                glEnd();
            }
        }
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_faces(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // faces
    if (_drawMode & DrawModes::SOLID_FLAT_SHADED) {

        if(boundary_only_) {

            // Only draw boundary

            // draw all boundary faces
            for(OpenVolumeMesh::BoundaryFaceIter bf_it = mesh_.bf_iter(); bf_it.valid(); ++bf_it) {

                // Skip selected
                if(statusAttrib_[*bf_it].selected()) continue;

                unsigned char c = 0;
                if(mesh_.is_boundary(mesh_.halfface_handle(*bf_it, 1))) c = 1;
                std::vector<HalfEdgeHandle> hes = mesh_.halfface(mesh_.halfface_handle(*bf_it, c)).halfedges();

                bool all_inside = true;
                for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                        he_it != hes.end(); ++he_it) {

                    if(!is_inside(mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()))) {
                        all_inside = false;
                        break;
                    }
                }

                if(all_inside) {

                    glBegin(GL_POLYGON);

                    ACG::Vec3d n = normalAttrib_[*bf_it];

                    // Set normal
                    glNormal3d(n[0], n[1], n[2]);

                    for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                            he_it != hes.end(); ++he_it) {

                        ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());

                        // Set vertex of face
                        glVertex3d(p[0], p[1], p[2]);
                    }

                    glEnd();
                }
            }

        } else {

            // Draw all cells
            for(OpenVolumeMesh::CellIter c_it = mesh_.c_iter(); c_it.valid(); ++c_it) {

                // Skip selected
                if(statusAttrib_[*c_it].selected()) continue;

                // Compute cell's cog
                ACG::Vec3d cog(0.0, 0.0, 0.0);
                unsigned int val = mesh_.n_vertices_in_cell(*c_it);
                for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {
                    cog += mesh_.vertex(*cv_it) / (double)val;
                }

                // Now draw all faces
                std::vector<HalfFaceHandle> hfs = mesh_.cell(*c_it).halffaces();

                bool all_inside = true;
                for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                        hf_it != hfs.end(); ++hf_it) {

                    std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
                    for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                            he_it != hes.end(); ++he_it) {

                        ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                        p = (scale_ * p) + ((1.0 - scale_) * cog);

                        if(!is_inside(p)) {
                            all_inside = false;
                            break;
                        }
                    }
                    if(!all_inside) break;
                }

                if(all_inside) {

                    for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                            hf_it != hfs.end(); ++hf_it) {

                        // Skip selected
                        if(statusAttrib_[mesh_.face_handle(*hf_it)].selected()) continue;

                        std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();

                        glBegin(GL_POLYGON);

                        ACG::Vec3d n = normalAttrib_[mesh_.face_handle(*hf_it)];

                        // Invert normal if it points inside the cell
                        ACG::Vec3d t_v = mesh_.vertex(mesh_.halfedge(*hes.begin()).from_vertex());
                        if((n | (cog - t_v)) > 0) {
                            n *= -1;
                        }

                        // Set normal
                        glNormal3d(n[0], n[1], n[2]);

                        for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                                he_it != hes.end(); ++he_it) {

                            ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                            p = (scale_ * p) + ((1.0 - scale_) * cog);
                            glVertex3d(p[0], p[1], p[2]);
                        }

                        glEnd();
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::update_selection(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    glNewList(selectionList_, GL_COMPILE);

    draw_vertex_selection(_state, _drawMode);
    draw_edge_selection(_state, _drawMode);
    draw_face_selection(_state, _drawMode);
    draw_cell_selection(_state, _drawMode);

    glEndList();

    selection_changed_ = false;
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_vertex_selection(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Always draw vertex selection no matter which draw mode

    // Set point size
    float p_backup = _state.point_size();
    glPointSize(materialNode_->point_size() * 3.0f);

    // draw all points
    glBegin(GL_POINTS);

    int n_vertices(mesh_.n_vertices());
    for (int i = 0; i < n_vertices; ++i) {
        Vec3d p = mesh_.vertex(VertexHandle(i));

        if(is_inside(p)) {

            // Skip unselected
            if(!statusAttrib_[VertexHandle(i)].selected()) continue;

            glVertex3d(p[0], p[1], p[2]);
        }
    }
    glEnd();

    glPointSize(p_backup);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_edge_selection(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Don't draw lines if in point mode
    if(!(_drawMode & DrawModes::WIREFRAME) &&
            !(_drawMode & DrawModes::SOLID_FLAT_SHADED)) return;

    // Set line width
    float l_backup = _state.line_width();
    glLineWidth(l_backup * 3.0f);

    int n_edges(mesh_.n_edges());

    for (int i = 0; i < n_edges; ++i) {

        Edge e(mesh_.edge(EdgeHandle(i)));
        Vec3d p0 = mesh_.vertex(e.from_vertex());
        Vec3d p1 = mesh_.vertex(e.to_vertex());

        if (is_inside(p0) && is_inside(p1)) {

            // Skip unselected
            if(!statusAttrib_[EdgeHandle(i)].selected())
                continue;

            glBegin(GL_LINES);
            glVertex3d(p0[0], p0[1], p0[2]);
            glVertex3d(p1[0], p1[1], p1[2]);
            glEnd();
        }
    }

    glLineWidth(l_backup);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_face_selection(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Don't draw faces if in point or wireframe mode
    if(!(_drawMode & DrawModes::SOLID_FLAT_SHADED)) return;

    if(boundary_only_) {

        // Only draw boundary

        // draw all boundary faces
        for(OpenVolumeMesh::BoundaryFaceIter bf_it = mesh_.bf_iter(); bf_it.valid(); ++bf_it) {

            // Skip unselected
            if(!statusAttrib_[(*bf_it)].selected()) continue;

            unsigned char c = 0;
            if(mesh_.is_boundary(mesh_.halfface_handle(*bf_it, 1))) c = 1;
            std::vector<HalfEdgeHandle> hes = mesh_.halfface(mesh_.halfface_handle(*bf_it, c)).halfedges();

            bool all_inside = true;
            for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                    he_it != hes.end(); ++he_it) {

                if(!is_inside(mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()))) {
                    all_inside = false;
                    break;
                }
            }

            if(all_inside) {

                glBegin(GL_POLYGON);

                ACG::Vec3d n = normalAttrib_[(*bf_it)];

                // Set normal
                glNormal3d(n[0], n[1], n[2]);

                for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                        he_it != hes.end(); ++he_it) {

                    ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());

                    // Set vertex of face
                    glVertex3d(p[0], p[1], p[2]);
                }

                glEnd();
            }
        }

    } else {

        // Draw all cells
        for(OpenVolumeMesh::CellIter c_it = mesh_.c_iter(); c_it.valid(); ++c_it) {

            // Compute cell's cog
            ACG::Vec3d cog(0.0, 0.0, 0.0);
            unsigned int val = mesh_.n_vertices_in_cell(*c_it);
            for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {
                cog += mesh_.vertex(*cv_it) / (double)val;
            }

            // Now draw all faces
            std::vector<HalfFaceHandle> hfs = mesh_.cell(*c_it).halffaces();

            bool all_inside = true;
            for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                    hf_it != hfs.end(); ++hf_it) {

                std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
                for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                        he_it != hes.end(); ++he_it) {

                    ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                    p = (scale_ * p) + ((1.0 - scale_) * cog);

                    if(!is_inside(p)) {
                        all_inside = false;
                        break;
                    }
                }
                if(!all_inside) break;
            }

            if(all_inside) {

                for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                        hf_it != hfs.end(); ++hf_it) {

                    // Skip unselected
                    if(!statusAttrib_[mesh_.face_handle(*hf_it)].selected()) continue;

                    std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();

                    glBegin(GL_POLYGON);

                    ACG::Vec3d n = normalAttrib_[mesh_.face_handle(*hf_it)];

                    // Invert normal if it points inside the cell
                    ACG::Vec3d t_v = mesh_.vertex(mesh_.halfedge(*hes.begin()).from_vertex());
                    if((n | (cog - t_v)) > 0) {
                        n *= -1;
                    }

                    // Set normal
                    glNormal3d(n[0], n[1], n[2]);

                    for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                            he_it != hes.end(); ++he_it) {

                        ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                        p = (scale_ * p) + ((1.0 - scale_) * cog);
                        glVertex3d(p[0], p[1], p[2]);
                    }

                    glEnd();
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_cell_selection(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Don't draw cells if in point or wireframe mode
    if(!(_drawMode & DrawModes::SOLID_FLAT_SHADED)) return;

    // Draw all cells
    for(OpenVolumeMesh::CellIter c_it = mesh_.c_iter(); c_it.valid(); ++c_it) {

        // Skip unselected
        if(!statusAttrib_[*c_it].selected()) continue;

        // Compute cell's cog
        ACG::Vec3d cog(0.0, 0.0, 0.0);
        unsigned int val = mesh_.n_vertices_in_cell(*c_it);
        for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {
            cog += mesh_.vertex(*cv_it) / (double)val;
        }

        // Now draw all faces
        std::vector<HalfFaceHandle> hfs = mesh_.cell(*c_it).halffaces();

        bool all_inside = true;
        for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                hf_it != hfs.end(); ++hf_it) {

            std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
            for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                    he_it != hes.end(); ++he_it) {

                ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                p = (scale_ * p) + ((1.0 - scale_) * cog);

                if(!is_inside(p)) {
                    all_inside = false;
                    break;
                }
            }
            if(!all_inside) break;
        }

        if(all_inside) {

            for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                    hf_it != hfs.end(); ++hf_it) {

                std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();

                glBegin(GL_POLYGON);

                ACG::Vec3d n = normalAttrib_[mesh_.face_handle(*hf_it)];

                // Invert normal if it points inside the cell
                ACG::Vec3d t_v = mesh_.vertex(mesh_.halfedge(*hes.begin()).from_vertex());
                if((n | (cog - t_v)) > 0) {
                    n *= -1;
                }

                // Set normal
                glNormal3d(n[0], n[1], n[2]);

                for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                        he_it != hes.end(); ++he_it) {

                    ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                    p = (scale_ * p) + ((1.0 - scale_) * cog);
                    glVertex3d(p[0], p[1], p[2]);
                }

                glEnd();
            }
        }
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::update_valence(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    glNewList(valenceList_, GL_COMPILE);

    draw_irreg_edges(_state, _drawMode);

    glEndList();

    valence_changed_ = false;
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw_irreg_edges(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Don't draw lines if in point mode
    if(!(_drawMode & DrawModes::WIREFRAME) &&
            !(_drawMode & DrawModes::SOLID_FLAT_SHADED)) return;

    // Set line width
    float l_backup = _state.line_width();
    glLineWidth(l_backup * 3.0f);

    ACG::Vec4f bc = _state.base_color();

    int n_edges(mesh_.n_edges());

    for (int i = 0; i < n_edges; ++i) {

        Edge e(mesh_.edge(EdgeHandle(i)));
        Vec3d p0 = mesh_.vertex(e.from_vertex());
        Vec3d p1 = mesh_.vertex(e.to_vertex());

        if (is_inside(p0) && is_inside(p1)) {

            bool boundary = mesh_.is_boundary(EdgeHandle(i));
            unsigned int valence = mesh_.valence(EdgeHandle(i));

            // Skip regular edges
            if(((boundary && valence == 2) && !show_outer_val2_irregs_) ||
                    (boundary && valence == 3) ||
                    (!boundary && valence == 4)) continue;

            ACG::Vec4f c = get_valence_color_code(valence, !boundary);
            glColor4f(c[0], c[1], c[2], c[3]);

            glBegin(GL_LINES);
            glVertex3d(p0[0], p0[1], p0[2]);
            glVertex3d(p1[0], p1[1], p1[2]);
            glEnd();
        }
    }

    glColor4f(bc[0], bc[1], bc[2], bc[3]);

    glLineWidth(l_backup);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
ACG::Vec4f VolumeMeshNodeT<VolumeMeshT>::get_valence_color_code(unsigned int _valence, bool _inner) const {

    if(_inner && _valence == 3) {
        return ACG::Vec4f(0.0f, 1.0f, 1.0f, 1.0f);
    } else if(_inner && _valence == 5) {
        return ACG::Vec4f(1.0f, 0.0f, 1.0f, 1.0f);
    } else if(!_inner && _valence > 3) {
        return ACG::Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
    } else if(!_inner && _valence == 2) {
        return ACG::Vec4f(1.0f, 1.0f, 0.0f, 1.0f);
    } else if(_inner && _valence > 5) {
        return ACG::Vec4f(0.5f, 1.0f, 0.5f, 1.0f);
    }
    return ACG::Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    // Update the selection if something has changed
    if(geom_changed_ || BaseNode::isDirty() || lastDrawMode_ != _drawMode) {
        update_selection(_state, _drawMode);
        update_geometry(_state, _drawMode);
    } else {
        if(selection_changed_) {
            update_selection(_state, _drawMode);
        }
    }

    // Update the geometry if something has changed
    if(valence_changed_ || BaseNode::isDirty() || lastDrawMode_ != _drawMode)
        update_valence(_state, _drawMode);

    GLboolean lighting = false;
    glGetBooleanv(GL_LIGHTING, &lighting);
    GLboolean color_material = false;
    glGetBooleanv(GL_COLOR_MATERIAL, &color_material);
    GLboolean blend = false;
    glGetBooleanv(GL_BLEND, &blend);
    GLboolean depth = false;
    glGetBooleanv(GL_DEPTH_TEST, &depth);

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::disable(GL_COLOR_MATERIAL);

    ACG::Vec4f ac = _state.ambient_color();
    ACG::Vec4f dc = _state.diffuse_color();
    ACG::Vec4f sc = _state.specular_color();
    _state.set_ambient_color(selection_color_ * 0.5);
    _state.set_diffuse_color(selection_color_ * 0.7);
    _state.set_specular_color(selection_color_);

    glCallList(selectionList_);

    _state.set_ambient_color(ac);
    _state.set_diffuse_color(dc);
    _state.set_specular_color(sc);

    ACG::GLState::disable(GL_LIGHTING);

    if(show_irregs_)
        glCallList(valenceList_);

    ACG::GLState::enable(GL_LIGHTING);

    ACG::Vec4f bc = _state.base_color();
    // Reset to base color
    glColor4f(bc[0], bc[1], bc[2], translucency_factor_);

    if(translucent_) {
        ACG::GLState::enable(GL_COLOR_MATERIAL);
        ACG::GLState::blendFunc(GL_SRC_ALPHA, GL_ONE); // GL_DST_COLOR
        ACG::GLState::enable(GL_BLEND);
        ACG::GLState::disable(GL_DEPTH_TEST);
    } else {
        ACG::GLState::disable(GL_COLOR_MATERIAL);
        ACG::GLState::disable(GL_BLEND);
        ACG::GLState::enable(GL_DEPTH_TEST);
        ACG::GLState::depthFunc(GL_LESS);
    }

    if(_drawMode & DrawModes::POINTS_COLORED) {
        ACG::GLState::disable(GL_LIGHTING);
        ACG::GLState::disable(GL_BLEND);
    }

    glCallList(geometryList_);

    if(lighting) {
        ACG::GLState::enable(GL_LIGHTING);
    } else {
        ACG::GLState::disable(GL_LIGHTING);
    }
    if(color_material) {
        ACG::GLState::enable(GL_COLOR_MATERIAL);
    } else {
        ACG::GLState::disable(GL_COLOR_MATERIAL);
    }
    if(blend) {
        ACG::GLState::enable(GL_BLEND);
    } else {
        ACG::GLState::disable(GL_BLEND);
    }
    if(translucent_) {
        if(depth)
            ACG::GLState::enable(GL_DEPTH_TEST);
        else
            ACG::GLState::disable(GL_DEPTH_TEST);
        ACG::GLState::depthFunc(GL_LESS);
    }

    glColor4f(bc[0], bc[1], bc[2], bc[3]);

    // Keep draw mode
    lastDrawMode_ = _drawMode;
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick(GLState& _state, PickTarget _target) {

    switch (_target) {
    case PICK_VERTEX: {
        _state.pick_set_maximum(mesh_.n_vertices());
        pick_vertices(_state);
        break;
    }

    case PICK_EDGE: {
        _state.pick_set_maximum(mesh_.n_edges());
        pick_edges(_state, 0);
        break;
    }

    case PICK_FACE: {
        _state.pick_set_maximum(mesh_.n_faces());
        pick_faces(_state, 0);
        break;
    }

    case PICK_CELL: {
        _state.pick_set_maximum(mesh_.n_cells());
        pick_cells(_state, 0);
        break;
    }

    case PICK_ANYTHING: {
        int nv = mesh_.n_vertices();
        int ne = mesh_.n_edges();
        int nf = mesh_.n_faces();
        int nc = mesh_.n_cells();

        _state.pick_set_maximum(nv + ne + nf + nc);
        pick_vertices(_state);
        pick_edges(_state, nv);
        pick_faces(_state, nv + ne);
        pick_cells(_state, nv + ne + nf);
        break;
    }

    default:
        break;
    }

}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick_vertices(GLState& _state) {

    // set pointsize for picking
    glPointSize(8.0);

    // draw all points
    glBegin(GL_POINTS);

    for (OpenVolumeMesh::CellIter c_it = mesh_.c_iter(); c_it.valid(); ++c_it) {

        // Compute cell's cog
        ACG::Vec3d cog(0.0, 0.0, 0.0);
        unsigned int val = mesh_.n_vertices_in_cell(*c_it);
        for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {
            cog += mesh_.vertex(*cv_it) / (double)val;
        }

        std::vector<HalfFaceHandle> hfs = mesh_.cell(*c_it).halffaces();
        bool all_inside(true);
        for (typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                hf_it != hfs.end(); ++hf_it) {

            std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
            for (typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin(); he_it != hes.end(); ++he_it) {
                if (!is_inside(Vec3d(mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()) * scale_ + (1.0 - scale_) * cog))) {

                    all_inside = false;
                    break;
                }
            }
            if(!all_inside) break;
        }

        if (all_inside)
            for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {

                Vec3d scaled_pos = mesh_.vertex(*cv_it) * scale_ + (1.0 - scale_) * cog;
                _state.pick_set_name(cv_it->idx());
                glVertex3d(scaled_pos[0], scaled_pos[1], scaled_pos[2]);
            }
    }
    glEnd();

}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick_edges(GLState& _state, unsigned int _offset) {

    //  mesh_.generate_trivial_edges();
    glLineWidth(3.0);

    glBegin(GL_LINES);

    for (OpenVolumeMesh::EdgeIter e_it = mesh_.e_iter(); e_it.valid(); ++e_it) {

        _state.pick_set_name(e_it->idx() + _offset);

        Vec3d p0(mesh_.vertex(mesh_.edge(*e_it).from_vertex()));
        Vec3d p1(mesh_.vertex(mesh_.edge(*e_it).to_vertex()));

        if (is_inside(p0) && is_inside(p1)) {
            glVertex3d(p0[0], p0[1], p0[2]);
            glVertex3d(p1[0], p1[1], p1[2]);
        }
    }
    glEnd();
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick_faces(GLState& _state, unsigned int _offset) {

    for (OpenVolumeMesh::FaceIter f_it = mesh_.f_iter(); f_it.valid(); ++f_it) {

        bool all_inside = true;
        std::vector<HalfEdgeHandle> hes = mesh_.face(*f_it).halfedges();
        for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                he_it != hes.end(); ++he_it) {
            if(!is_inside(mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()))) {
                all_inside = false;
                break;
            }
        }

        if(all_inside) {

            glBegin(GL_POLYGON);
            ACG::Vec3d n = normalAttrib_[*f_it];
            glNormal3d(n[0], n[1], n[2]);

            _state.pick_set_name(f_it->idx() + _offset);

            for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin();
                he_it != hes.end(); ++he_it) {

                ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex());
                glVertex3d(p[0], p[1], p[2]);
            }
            glEnd();
        }
    }
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick_cells(GLState& _state, unsigned int _offset) {

    for (OpenVolumeMesh::CellIter c_it = mesh_.c_iter(); c_it.valid(); ++c_it) {

        // Compute cell's cog
        ACG::Vec3d cog(0.0, 0.0, 0.0);
        unsigned int val = mesh_.n_vertices_in_cell(*c_it);
        for(OpenVolumeMesh::CellVertexIter cv_it = mesh_.cv_iter(*c_it); cv_it.valid(); ++cv_it) {
            cog += mesh_.vertex(*cv_it) / (double)val;
        }

        std::vector<HalfFaceHandle> hfs = mesh_.cell(*c_it).halffaces();
        bool all_inside(true);
        for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                hf_it != hfs.end(); ++hf_it) {

            std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
            for (typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin(); he_it != hes.end(); ++he_it) {
                if (!is_inside(Vec3d(mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()) * scale_ + (1.0 - scale_) * cog))) {

                    all_inside = false;
                    break;
                }
            }
            if(!all_inside) break;
        }

        if(all_inside) {

            _state.pick_set_name(c_it->idx() + _offset);

            for(typename std::vector<HalfFaceHandle>::const_iterator hf_it = hfs.begin();
                hf_it != hfs.end(); ++hf_it) {

                glBegin(GL_POLYGON);
                ACG::Vec3d n = normalAttrib_[mesh_.face_handle(*hf_it)];
                glNormal3d(n[0], n[1], n[2]);

                std::vector<HalfEdgeHandle> hes = mesh_.halfface(*hf_it).halfedges();
                for(typename std::vector<HalfEdgeHandle>::const_iterator he_it = hes.begin(); he_it != hes.end(); ++he_it) {

                    ACG::Vec3d p = mesh_.vertex(mesh_.halfedge(*he_it).to_vertex()) * scale_ + cog * (1.0 - scale_);
                    glVertex3d(p[0], p[1], p[2]);
                }
                glEnd();
            }
        }
    }
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::add_cut_plane(const Vec3d& _p, const Vec3d& _n, const Vec3d& _xsize,
        const Vec3d& _ysize) {

    add_cut_plane(Plane(_p, _n, _xsize, _ysize));
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::add_cut_plane(const Plane& _p) {

    cut_planes_.push_back(_p);
    geom_changed_ = true;
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::init() {

    geometryList_ = glGenLists(1);
    selectionList_ = glGenLists(1);
    valenceList_ = glGenLists(1);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
bool VolumeMeshNodeT<VolumeMeshT>::is_inside(const Vec3d& _p) {

    for(typename std::vector<Plane>::iterator it = cut_planes_.begin(); it != cut_planes_.end(); ++it) {
            // get local position
            Vec3d pl = _p - it->position;
            // evaluate dot products
            double pn = (pl | it->normal);
            double px = (pl | it->xdirection);
            double py = (pl | it->ydirection);

            if (pn < 0.0 && px > -0.5 && px < 0.5 && py > -0.5 && py < 0.5)
                return false;
        }

        return true;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

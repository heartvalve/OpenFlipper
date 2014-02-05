/*===========================================================================*\
*                                                                            *
 *                              OpenFlipper                                   *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
#include <OpenVolumeMesh/Attribs/TexCoordAttrib.hh>

#include "VolumeMeshBufferManager.hh"
#include "VolumeMeshDrawModesContainer.hh"


#include <ACG/GL/RenderObject.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/IRenderer.hh>

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

    typedef typename VolumeMeshBufferManager<VolumeMeshT>::Plane Plane;


    /// Constructor
    VolumeMeshNodeT(const VolumeMesh& _mesh,
                    OpenVolumeMesh::StatusAttrib& _statusAttrib,
                    OpenVolumeMesh::ColorAttrib<Vec4f>& _colorAttrib,
                    OpenVolumeMesh::NormalAttrib<VolumeMesh>& _normalAttrib,
                    OpenVolumeMesh::TexCoordAttrib<Vec2f> &_texcoordAttrib,
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
    void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

    /** \brief Draws the object deferred
    *
    */
    void getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);

    /// picking
    void pick(GLState& _state, PickTarget _target);

    // clear all cutplanes
    void clear_cut_planes();

    // add a new cutplane
    void add_cut_plane(const Plane& _p);
    void add_cut_plane(const Vec3d& _p, const Vec3d& _n, const Vec3d& _xsize, const Vec3d& _ysize);

    // set shrinkage of cells
    void set_scaling(double _scale);

    // Get scaling
    double scaling() const { return scale_; }

    // Choose whether to visualize the boundary only
    void set_boundary_only(bool _boundary);
    bool boundary_only() const { return boundary_only_; }

    // Set translucency factor
    void set_translucency_factor(float _factor) { translucency_factor_ = _factor; }
    float translucency_factor() const { return translucency_factor_; }

    // Set whether to update the topology in next render pass
    void set_topology_changed(bool _topology_changed);

    // Set whether to update the geometry in next render pass
    void set_geometry_changed(bool _geom_changed);

    // Set whether to update the color in next render pass
    void set_color_changed(bool _color_changed);

    // Set whether to update the texture in next render pass
    void set_texture_changed(bool _texture_changed);

    // Set whether to update the selectopm in next render pass
    void set_selection_changed(bool _selection_changed);

private:

    /// renders the cells of the mesh
    void drawCells(GLState& _state, const DrawModes::DrawMode& _drawMode);
    /// renders the faces or halffaces of the mesh
    void drawFaces(GLState& _state, const DrawModes::DrawMode& _drawMode);
    /// renders the edges or halfedges of the mesh
    void drawEdges(GLState& _state, const DrawModes::DrawMode& _drawMode);
    /// renders the vertices of the mesh
    void drawVertices(GLState& _state, const DrawModes::DrawMode& _drawMode);
    /// draw selected primitves
    void drawSelection(GLState& _state, const DrawModes::DrawMode& _drawMode);

    /// adds render objects for drawing cells to the given renderer
    void getCellRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);
    /// adds render objects for drawing faces or halffaces to the given renderer
    void getFaceRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);
    /// adds render objects for drawing edges or halfedges to the given renderer
    void getEdgeRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);
    /// adds render objects for drawing vertices to the given renderer
    void getVertexRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);

    /// adds render objects for drawing selected primitves to the given renderer
    void getSelectionRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);

    /// invalidates all buffers
    void invalidateAllBuffers();
    /// invalidates all buffers used for normal rendering
    void invalidateAllDrawBuffers();
    /// invalidates all buffers used for picking
    void invalidateAllPickingBuffers();
    /// invalidates all buffers used for rendering selected primitives
    void invalidateAllSelectionBuffers();

    /// pick vertices
    void pickVertices(GLState& _state);
    /// pick edges
    void pickEdges(GLState& _state, unsigned int _offset);
    /// pick faces
    void pickFaces(GLState& _state, unsigned int _offset);
    /// pick cells
    void pickCells(GLState& _state, unsigned int _offset);

    /// updates face normals
    void update_face_normals();
    /// updates vertex normals
    void update_vertex_normals();

    /// Copy constructor (not used)
    VolumeMeshNodeT(const VolumeMeshNodeT& _rhs);

    /// Assignment operator (not used)
    VolumeMeshNodeT& operator=(const VolumeMeshNodeT& _rhs);

private:

    const VolumeMesh& mesh_;

    double scale_;

    // Flag that indicates whether to render the boundary faces only
    bool boundary_only_;

    // Translucency factor in [0, 1]
    GLfloat translucency_factor_;

    // Hold color of selection
    ACG::Vec4f selection_color_;

    OpenVolumeMesh::StatusAttrib& statusAttrib_;
    OpenVolumeMesh::ColorAttrib<Vec4f>& colorAttrib_;
    OpenVolumeMesh::NormalAttrib<VolumeMesh>& normalAttrib_;
    OpenVolumeMesh::TexCoordAttrib<Vec2f>& texcoordAttrib_;

    const MaterialNode* materialNode_;

    VolumeMeshBufferManager<VolumeMeshT> cellsBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> facesBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> edgesBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> verticesBufferManager_;

    VolumeMeshBufferManager<VolumeMeshT> cellSelectionBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> faceSelectionBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> edgeSelectionBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> vertexSelectionBufferManager_;

    VolumeMeshBufferManager<VolumeMeshT> cellPickBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> facePickBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> edgePickBufferManager_;
    VolumeMeshBufferManager<VolumeMeshT> vertexPickBufferManager_;

    VolumeMeshDrawModesContainer drawModes_;

    DrawModes::DrawMode lastDrawMode_;
    DrawModes::DrawMode lastCellDrawMode_;
    DrawModes::DrawMode lastFaceDrawMode_;
    DrawModes::DrawMode lastEdgeDrawMode_;
    DrawModes::DrawMode lastVertexDrawMode_;

    PickTarget lastPickTarget_;

    bool face_normals_calculated_;
    bool vertex_normals_calculated_;

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


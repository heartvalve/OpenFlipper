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

#define VOLUMEMESHNODET_CC

//== INCLUDES =================================================================

#include "VolumeMeshNode.hh"

#include <ACG/GL/gl.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>

#include <ACG/GL/RenderObject.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/IRenderer.hh>

#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>

typedef OpenVolumeMesh::HexahedralMeshTopologyKernel HexahedralMeshTopologyKernel;


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


template<class VolumeMeshT>
VolumeMeshNodeT<VolumeMeshT>::VolumeMeshNodeT(const VolumeMesh& _mesh,
                                              OpenVolumeMesh::StatusAttrib& _statusAttrib,
                                              OpenVolumeMesh::ColorAttrib<Vec4f>& _colorAttrib,
                                              OpenVolumeMesh::NormalAttrib<VolumeMesh>& _normalAttrib,
                                              OpenVolumeMesh::TexCoordAttrib<Vec2f>& _texcoordAttrib,
                                              const MaterialNode* _matNode, BaseNode* _parent,
                                              std::string _name) :
  BaseNode(_parent, _name),
  mesh_(_mesh),
  scale_(1.0),
  boundary_only_(false),
  translucency_factor_(0.1f),
  selection_color_(ACG::Vec4f(1.0f, 0.0f, 0.0f, 1.0f)),
  statusAttrib_(_statusAttrib),
  colorAttrib_(_colorAttrib),
  normalAttrib_(_normalAttrib),
  texcoordAttrib_(_texcoordAttrib),
  materialNode_(_matNode),

  cellsBufferManager_   (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  facesBufferManager_   (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  edgesBufferManager_   (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  verticesBufferManager_(_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),

  cellSelectionBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  faceSelectionBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  edgeSelectionBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  vertexSelectionBufferManager_(_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),

  cellPickBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  facePickBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  edgePickBufferManager_  (_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),
  vertexPickBufferManager_(_mesh, _statusAttrib, _colorAttrib, _normalAttrib, _texcoordAttrib),

  drawModes_(),
  lastDrawMode_      (DrawModes::NONE),
  lastCellDrawMode_  (DrawModes::NONE),
  lastFaceDrawMode_  (DrawModes::NONE),
  lastEdgeDrawMode_  (DrawModes::NONE),
  lastVertexDrawMode_(DrawModes::NONE),
  lastPickTarget_(PICK_ANYTHING),
  face_normals_calculated_(false),
  vertex_normals_calculated_(false)
{
    vertexSelectionBufferManager_.setSelectionOnly(true);
    edgeSelectionBufferManager_.setSelectionOnly(true);
    faceSelectionBufferManager_.setSelectionOnly(true);
    cellSelectionBufferManager_.setSelectionOnly(true);
}


//----------------------------------------------------------------------------

template<class VolumeMeshT>
VolumeMeshNodeT<VolumeMeshT>::~VolumeMeshNodeT() {

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
    DrawModes::DrawMode result;

    if (mesh_.n_cells() > 0)
    {
        result |= drawModes_.cellsTransparent;
        result |= drawModes_.cellsFlatShaded;
        result |= drawModes_.cellsSmoothShaded;
        result |= drawModes_.cellsPhongShaded;
        if (colorAttrib_.vertex_colors_available())
            result |= drawModes_.cellsColoredPerVertex;
        if (colorAttrib_.halfface_colors_available())
            result |= drawModes_.cellsColoredPerHalfface;
        if (colorAttrib_.face_colors_available())
            result |= drawModes_.cellsColoredPerFace;
        if (colorAttrib_.cell_colors_available())
            result |= drawModes_.cellsColoredPerCell;
    }

    if (mesh_.n_faces() > 0)
    {
        result |= drawModes_.facesFlatShaded;
        result |= drawModes_.facesSmoothShaded;
        result |= drawModes_.facesPhongShaded;
        if (colorAttrib_.vertex_colors_available())
            result |= drawModes_.facesColoredPerVertex;
        if (colorAttrib_.face_colors_available())
            result |= drawModes_.facesColoredPerFace;
        if (texcoordAttrib_.vertex_texcoords_available())
            result |= drawModes_.facesTextured;
        if (texcoordAttrib_.vertex_texcoords_available())
            result |= drawModes_.facesTexturedShaded;
    }

    if (mesh_.n_halffaces() > 0)
    {
        result |= drawModes_.halffacesFlatShaded;
        result |= drawModes_.halffacesSmoothShaded;
        result |= drawModes_.halffacesPhongShaded;
        if (colorAttrib_.vertex_colors_available())
            result |= drawModes_.halffacesColoredPerVertex;
        if (colorAttrib_.halfface_colors_available())
            result |= drawModes_.halffacesColoredPerHalfface;
    }

    if (mesh_.n_edges() > 0)
    {
        result |= drawModes_.edgesWireframe;

        if (mesh_.n_cells () > 0)
            result |= drawModes_.edgesOnCells;
        if (mesh_.n_faces() > 0)
            result |= drawModes_.edgesHiddenLine;
        if (colorAttrib_.edge_colors_available())
            result |= drawModes_.edgesColoredPerEdge;

        if (dynamic_cast<const HexahedralMeshTopologyKernel*>(&mesh_) != NULL)
        {
            result |= drawModes_.irregularInnerEdges;
            result |= drawModes_.irregularOuterEdges;
        }
    }

    if (mesh_.n_halfedges() > 0)
    {
        result |= drawModes_.halfedgesWireframe;

        if (mesh_.n_faces() > 0)
            result |= drawModes_.halfedgesHiddenLine;
        if (colorAttrib_.halfedge_colors_available())
            result |= drawModes_.halfedgesColoredPerHalfedge;
    }

    if (mesh_.n_vertices() > 0)
    {
        result |= drawModes_.verticesFlatShaded;

        if (colorAttrib_.vertex_colors_available())
            result |= drawModes_.verticesColored;
    }

    return  result;

}


//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::drawCells(GLState& _state, const DrawModes::DrawMode& _drawMode)
{

    ACG::GLState::depthRange(0.01, 1.0);

    cellsBufferManager_.setOptionsFromDrawMode(_drawMode);
    GLState::bindBuffer(GL_ARRAY_BUFFER, cellsBufferManager_.getBuffer());
    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, cellsBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::enableClientState(GL_NORMAL_ARRAY);
    GLState::normalPointer(GL_FLOAT, cellsBufferManager_.getStride(), reinterpret_cast<GLvoid*>(cellsBufferManager_.getNormalOffset()));

    if (_drawMode & (drawModes_.cellsColoredPerCell | drawModes_.cellsColoredPerFace | drawModes_.cellsColoredPerHalfface | drawModes_.cellsColoredPerVertex))
    {
        //for a drawmode with colors we have to activate and bind colors
        GLState::enableClientState(GL_COLOR_ARRAY);
        GLState::colorPointer(4, GL_UNSIGNED_BYTE, cellsBufferManager_.getStride(), reinterpret_cast<GLvoid*>(cellsBufferManager_.getColorOffset()));

        GLState::shadeModel(GL_SMOOTH);
        GLState::disable(GL_LIGHTING);
        GLState::enable(GL_DEPTH_TEST);

    }
    else if (_drawMode & drawModes_.cellsSmoothShaded)
    {
        GLState::shadeModel(GL_SMOOTH);
        GLState::enable(GL_LIGHTING);
        GLState::enable(GL_DEPTH_TEST);
    }
    else if (_drawMode & drawModes_.cellsFlatShaded)
    {
        GLState::shadeModel(GL_FLAT);
        GLState::enable(GL_LIGHTING);
        GLState::enable(GL_DEPTH_TEST);
    }
    else if (_drawMode & drawModes_.cellsTransparent)
    {
        GLState::shadeModel(GL_FLAT);
        GLState::disable(GL_LIGHTING);
        GLState::disable(GL_DEPTH_TEST);
        GLState::disableClientState(GL_COLOR_ARRAY);
        Vec4f bc = _state.specular_color();
        _state.set_color(Vec4f(bc[0],bc[1],bc[2],translucency_factor_));
        GLState::enable(GL_BLEND);
        GLState::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glDrawArrays(GL_TRIANGLES, 0, cellsBufferManager_.getNumOfVertices());

    if (_drawMode & drawModes_.cellsTransparent)
    {
        _state.set_color(_state.base_color());
        GLState::disable(GL_BLEND);
    }

    GLState::disableClientState(GL_COLOR_ARRAY);
    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

    ACG::GLState::depthRange(0.0, 1.0);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::drawFaces(GLState& _state, const DrawModes::DrawMode& _drawMode)
{

    ACG::GLState::depthRange(0.01, 1.0);

    //faces are drawn from both sides, halffaces only when facing the camera
    //however, if we only draw the boundary, we draw halffaces from both sides so we can see them
    //when looking inside the object
    if ((_drawMode & drawModes_.faceBasedDrawModes) || boundary_only_)
    {
        GLState::disable(GL_CULL_FACE);
    }
    else
    {
        GLState::enable(GL_CULL_FACE);
        GLState::cullFace(GL_BACK);
    }

    GLState::enable(GL_DEPTH_TEST);

    facesBufferManager_.setOptionsFromDrawMode(_drawMode);
    GLState::bindBuffer(GL_ARRAY_BUFFER, facesBufferManager_.getBuffer());
    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    if (_drawMode & (drawModes_.hiddenLineBackgroundFaces))
    {
        GLState::disable(GL_LIGHTING);
        GLState::shadeModel(GL_FLAT);
        GLState::disableClientState(GL_COLOR_ARRAY);
        GLState::disableClientState(GL_NORMAL_ARRAY);
        _state.set_color(_state.clear_color());
    }
    else if (_drawMode & (drawModes_.facesColoredPerFace | drawModes_.facesColoredPerVertex
                     | drawModes_.halffacesColoredPerHalfface | drawModes_.halffacesColoredPerVertex ))
    {
        GLState::enableClientState(GL_COLOR_ARRAY);
        GLState::colorPointer(4, GL_UNSIGNED_BYTE, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facesBufferManager_.getColorOffset()));

        GLState::disable(GL_LIGHTING);
        GLState::shadeModel(GL_SMOOTH);
    }
    else if (_drawMode & (drawModes_.facesTexturedShaded) )
    {
        glClientActiveTexture(GL_TEXTURE0);
        ACG::GLState::texcoordPointer(2, GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facesBufferManager_.getTexCoordOffset()));
        ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);

        GLState::enable(GL_LIGHTING);
        GLState::disableClientState(GL_COLOR_ARRAY);

        GLState::enableClientState(GL_NORMAL_ARRAY);
        GLState::normalPointer(GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facesBufferManager_.getNormalOffset()));

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GLState::shadeModel(GL_SMOOTH);
    }
    else if (_drawMode & (drawModes_.facesTextured) )
    {
        glClientActiveTexture(GL_TEXTURE0);
        ACG::GLState::texcoordPointer(2, GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facesBufferManager_.getTexCoordOffset()));
        ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);

        GLState::disableClientState(GL_COLOR_ARRAY);
        GLState::disableClientState(GL_NORMAL_ARRAY);

        GLState::disable(GL_LIGHTING);
        GLState::shadeModel(GL_SMOOTH);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
    else
    {
        GLState::enable(GL_LIGHTING);
        GLState::disableClientState(GL_COLOR_ARRAY);

        GLState::enableClientState(GL_NORMAL_ARRAY);
        GLState::normalPointer(GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facesBufferManager_.getNormalOffset()));

        GLState::shadeModel(GL_SMOOTH);
    }

    glDrawArrays(GL_TRIANGLES, 0, facesBufferManager_.getNumOfVertices());

    GLState::disableClientState(GL_COLOR_ARRAY);

    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

    if (_drawMode & (drawModes_.edgesHiddenLine | drawModes_.halfedgesHiddenLine))
        _state.set_color(_state.base_color());

    ACG::GLState::depthRange(0.0, 1.0);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::drawEdges(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
    ACG::GLState::depthRange(0.0, 1.0);

    edgesBufferManager_.setDefaultColor(_state.specular_color());
    edgesBufferManager_.setOptionsFromDrawMode(_drawMode);

    GLState::bindBuffer(GL_ARRAY_BUFFER, edgesBufferManager_.getBuffer());
    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, edgesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    if (_drawMode & ( drawModes_.edgesColoredPerEdge | drawModes_.halfedgesColoredPerHalfedge |
                      drawModes_.irregularInnerEdges | drawModes_.irregularOuterEdges ))
    {
        GLState::enableClientState(GL_COLOR_ARRAY);
        GLState::colorPointer(4, GL_UNSIGNED_BYTE, edgesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(edgesBufferManager_.getColorOffset()));

        GLState::disable(GL_LIGHTING);
        GLState::shadeModel(GL_SMOOTH);

        glLineWidth(_state.line_width() * 2.0f);
    }
    else
    {
        _state.set_color( _state.specular_color() );

        ACG::GLState::disable(GL_LIGHTING);
        ACG::GLState::shadeModel(GL_FLAT);

        GLState::disableClientState(GL_NORMAL_ARRAY);
    }


    glDrawArrays(GL_LINES, 0, edgesBufferManager_.getNumOfVertices());
    glLineWidth(_state.line_width());

    GLState::disableClientState(GL_COLOR_ARRAY);

    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::drawVertices(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
    verticesBufferManager_.setOptionsFromDrawMode(_drawMode);
    GLState::bindBuffer(GL_ARRAY_BUFFER, verticesBufferManager_.getBuffer());
    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, verticesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::depthRange(0.0,1.0);

    if (_drawMode & ( drawModes_.verticesColored ))
    {
        GLState::enableClientState(GL_COLOR_ARRAY);
        GLState::colorPointer(4, GL_UNSIGNED_BYTE, verticesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(verticesBufferManager_.getColorOffset()));

        GLState::disable(GL_LIGHTING);
        GLState::shadeModel(GL_SMOOTH);
    }
    else
    {
        _state.set_color( _state.specular_color() );

        ACG::GLState::disable(GL_LIGHTING);
        ACG::GLState::shadeModel(GL_FLAT);

        GLState::disableClientState(GL_NORMAL_ARRAY);
    }

    glDrawArrays(GL_POINTS, 0, verticesBufferManager_.getNumOfVertices());

    GLState::disableClientState(GL_COLOR_ARRAY);

    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::drawSelection(GLState& _state, const DrawModes::DrawMode& _drawMode)
{

    //save current shader
    GLint currentProgramm;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramm);
    //disable shader for drawing of the selecttion
    ACG::GLState::useProgram(0);

    GLState::enable(GL_DEPTH_TEST);
    GLState::depthFunc(GL_LEQUAL);
    _state.set_color( selection_color_ );

    GLState::disable(GL_LIGHTING);
    GLState::shadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    GLState::disableClientState(GL_NORMAL_ARRAY);
    GLState::disableClientState(GL_COLOR_ARRAY);

    GLState::disable(GL_TEXTURE_2D);

    GLState::enableClientState(GL_VERTEX_ARRAY);

    if ((_drawMode & drawModes_.cellBasedDrawModes) && !(_drawMode & drawModes_.vertexBasedDrawModes))
        vertexSelectionBufferManager_.enableVertexOnCellPrimitives();
    else
        vertexSelectionBufferManager_.enableVertexPrimitives();

    if ((_drawMode & (drawModes_.cellBasedDrawModes | drawModes_.edgesOnCells)) && !(_drawMode & (drawModes_.edgeBasedDrawModes & ~drawModes_.edgesOnCells)))
        edgeSelectionBufferManager_.enableEdgeOnCellPrimitives();
    else
        edgeSelectionBufferManager_.enableEdgePrimitives();

    if ((_drawMode & drawModes_.cellBasedDrawModes) && !(_drawMode & (drawModes_.faceBasedDrawModes | drawModes_.halffaceBasedDrawModes)))
        faceSelectionBufferManager_.enableFaceOnCellPrimitives();
    else
        faceSelectionBufferManager_.enableFacePrimitives();

    cellSelectionBufferManager_.enableCellPrimitives();

    glLineWidth(1.5*_state.line_width());

    GLState::depthRange(0.0,1.0);

    GLState::bindBuffer(GL_ARRAY_BUFFER, vertexSelectionBufferManager_.getBuffer());
    GLState::vertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_POINTS, 0, vertexSelectionBufferManager_.getNumOfVertices());

    GLState::bindBuffer(GL_ARRAY_BUFFER, edgeSelectionBufferManager_.getBuffer());
    GLState::vertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINES, 0, edgeSelectionBufferManager_.getNumOfVertices());

    GLState::depthRange(0.01,1.0);

    GLState::bindBuffer(GL_ARRAY_BUFFER, faceSelectionBufferManager_.getBuffer());
    GLState::vertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, faceSelectionBufferManager_.getNumOfVertices());

    GLState::bindBuffer(GL_ARRAY_BUFFER, cellSelectionBufferManager_.getBuffer());
    GLState::vertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, cellSelectionBufferManager_.getNumOfVertices());

    GLState::depthRange(0.0,1.0);

    _state.set_color(_state.base_color());

    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);


    glLineWidth(_state.line_width());

    ACG::GLState::useProgram(currentProgramm);

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::invalidateAllBuffers()
{
    invalidateAllDrawBuffers();
    invalidateAllPickingBuffers();
    invalidateAllSelectionBuffers();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::invalidateAllDrawBuffers()
{
    cellsBufferManager_.invalidate();
    facesBufferManager_.invalidate();
    edgesBufferManager_.invalidate();
    verticesBufferManager_.invalidate();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::invalidateAllPickingBuffers()
{
    cellPickBufferManager_.invalidate();
    facePickBufferManager_.invalidate();
    edgePickBufferManager_.invalidate();
    vertexPickBufferManager_.invalidate();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::invalidateAllSelectionBuffers()
{
    cellSelectionBufferManager_.invalidate();
    faceSelectionBufferManager_.invalidate();
    edgeSelectionBufferManager_.invalidate();
    vertexSelectionBufferManager_.invalidate();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::draw(GLState& _state, const DrawModes::DrawMode& _drawMode) {

    //save state
    GLboolean lighting = false;
    glGetBooleanv(GL_LIGHTING, &lighting);
    GLboolean color_material = false;
    glGetBooleanv(GL_COLOR_MATERIAL, &color_material);
    GLboolean blend = false;
    glGetBooleanv(GL_BLEND, &blend);
    GLboolean depth = false;
    glGetBooleanv(GL_DEPTH_TEST, &depth);
    GLenum prev_depth = _state.depthFunc();
    GLboolean cullFace = false;
    glGetBooleanv(GL_CULL_FACE, &cullFace);
    GLint cullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
    GLint texmode;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texmode);
    GLboolean texturing;
    glGetBooleanv(GL_TEXTURE_2D,  &texturing);
    const Vec4f oldColor = _state.color();

    // ignore two sided lighting setting and turn it off
    GLboolean lightModel = _state.twosided_lighting();
    _state.set_twosided_lighting(false);

    bool clientStateEnabledVertexArray   = GLState::isClientStateEnabled(GL_VERTEX_ARRAY);
    bool clientStateEnabledColorArray    = GLState::isClientStateEnabled(GL_COLOR_ARRAY);
    bool clientStateEnabledNormalArray   = GLState::isClientStateEnabled(GL_NORMAL_ARRAY);
    bool clientStateEnabledTexCoordArray = GLState::isClientStateEnabled(GL_TEXTURE_COORD_ARRAY);

    DrawModes::DrawMode cellDrawMode   = drawModes_.getFirstCellDrawMode(_drawMode);
    DrawModes::DrawMode faceDrawMode   = drawModes_.getFirstFaceDrawMode(_drawMode);
    DrawModes::DrawMode edgeDrawMode   = drawModes_.getFirstEdgeDrawMode(_drawMode);
    DrawModes::DrawMode vertexDrawMode = drawModes_.getFirstVertexDrawMode(_drawMode);

    if (!face_normals_calculated_)
    {
        if ( (cellDrawMode & drawModes_.cellsFlatShaded) ||
             (faceDrawMode & (drawModes_.facesFlatShaded | drawModes_.halffacesFlatShaded  | drawModes_.facesTexturedShaded)) )
            update_face_normals();
    }
    if (!vertex_normals_calculated_)
    {
        if ( (cellDrawMode & (drawModes_.cellsSmoothShaded | drawModes_.cellsPhongShaded)) ||
             (faceDrawMode & (drawModes_.facesSmoothShaded | drawModes_.halffacesSmoothShaded | drawModes_.facesPhongShaded | drawModes_.halffacesPhongShaded)) )
            update_vertex_normals();
    }

    //the VolumeMeshBufferManager can handle non atomic drawmodes if it consists of one
    // edge based draw mode (except edges on cells) and irregular edges
    edgeDrawMode |= _drawMode & (drawModes_.irregularInnerEdges | drawModes_.irregularOuterEdges);


    if (cellDrawMode)
        drawCells(_state, cellDrawMode);
    else
        cellsBufferManager_.free();
    if (faceDrawMode)
        drawFaces(_state, faceDrawMode);
    else
        facesBufferManager_.free();
    if (edgeDrawMode)
        drawEdges(_state, edgeDrawMode);
    else
        edgesBufferManager_.free();
    if (vertexDrawMode)
        drawVertices(_state, vertexDrawMode);
    else
        verticesBufferManager_.free();

    drawSelection(_state, cellDrawMode | faceDrawMode | edgeDrawMode | vertexDrawMode);


    lastDrawMode_ = cellDrawMode | faceDrawMode | edgeDrawMode | vertexDrawMode;
    lastCellDrawMode_   = cellDrawMode;
    lastFaceDrawMode_   = faceDrawMode;
    lastEdgeDrawMode_   = edgeDrawMode;
    lastVertexDrawMode_ = vertexDrawMode;

    //restore state
    if(lighting)
        ACG::GLState::enable(GL_LIGHTING);
    else
        ACG::GLState::disable(GL_LIGHTING);
    if(color_material)
        ACG::GLState::enable(GL_COLOR_MATERIAL);
    else
        ACG::GLState::disable(GL_COLOR_MATERIAL);
    if(blend)
        ACG::GLState::enable(GL_BLEND);
    else
        ACG::GLState::disable(GL_BLEND);
    if(depth)
        ACG::GLState::enable(GL_DEPTH_TEST);
    else
        ACG::GLState::disable(GL_DEPTH_TEST);
    _state.depthFunc(prev_depth);
    if(cullFace)
        ACG::GLState::enable(GL_CULL_FACE);
    else
        ACG::GLState::disable(GL_CULL_FACE);
    ACG::GLState::cullFace(cullFaceMode);
    if(texturing)
        ACG::GLState::enable(GL_TEXTURE_2D);
    else
        ACG::GLState::disable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texmode);
    _state.set_color(oldColor);

    _state.set_twosided_lighting(lightModel);

    if (clientStateEnabledVertexArray)
        GLState::enableClientState(GL_VERTEX_ARRAY);
    else
        GLState::disableClientState(GL_VERTEX_ARRAY);

    if (clientStateEnabledColorArray)
        GLState::enableClientState(GL_COLOR_ARRAY);
    else
        GLState::disableClientState(GL_COLOR_ARRAY);

    if (clientStateEnabledNormalArray)
        GLState::enableClientState(GL_NORMAL_ARRAY);
    else
        GLState::disableClientState(GL_NORMAL_ARRAY);

    if (clientStateEnabledTexCoordArray)
        GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);
    else
        GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);

}



template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getCellRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
    RenderObject ro;
    ro.initFromState(&_state);

    ro.debugName = "VolumeMeshNodeCells";

    //Todo: use DrawModeProperties

    // reset renderobject
    ro.depthTest = true;
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);

    ro.shaderDesc.clearTextures();

    ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;

    ro.depthRange = Vec2f(0.01f, 1.0f);

    if (_drawMode & (drawModes_.cellsColoredPerCell | drawModes_.cellsColoredPerFace | drawModes_.cellsColoredPerHalfface | drawModes_.cellsColoredPerVertex))
    {
        ro.shaderDesc.vertexColors = true;
        ro.shaderDesc.numLights = -1;
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;

    }
    else if (_drawMode & drawModes_.cellsSmoothShaded)
    {
        ro.shaderDesc.numLights = 0;
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;
    }
    else if (_drawMode & drawModes_.cellsFlatShaded)
    {
        ro.shaderDesc.numLights = 0;
        ro.shaderDesc.shadeMode = SG_SHADE_FLAT;
    }
    else if (_drawMode & drawModes_.cellsTransparent)
    {
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;
        ro.shaderDesc.numLights = -1;
        ro.depthTest = false;

        ro.diffuse = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);
        ro.ambient = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);
        ro.specular = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);
        ro.emissive = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);

        ro.alpha = translucency_factor_;
        ro.blending = true;
        ro.blendDest = GL_ONE_MINUS_SRC_ALPHA;
        ro.blendSrc = GL_SRC_ALPHA;
    }

    cellsBufferManager_.setOptionsFromDrawMode(_drawMode);
    ro.vertexBuffer = cellsBufferManager_.getBuffer();
    ro.vertexDecl = cellsBufferManager_.getVertexDeclaration();

    ro.glDrawArrays(GL_TRIANGLES, 0, cellsBufferManager_.getNumOfVertices());
    _renderer->addRenderObject(&ro);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getFaceRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
    RenderObject ro;
    ro.initFromState(&_state);

    ro.debugName = "VolumeMeshNodeFaces";

    //Todo: use DrawModeProperties

    // reset renderobject
    ro.depthTest = true;
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);

    ro.shaderDesc.clearTextures();

    ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;

    ro.depthRange = Vec2f(0.01f, 1.0f);

    facesBufferManager_.setOptionsFromDrawMode(_drawMode);


    if ((_drawMode & drawModes_.faceBasedDrawModes) || boundary_only_)
        ro.culling = false;
    else
        ro.culling = true;

    if (_drawMode & (drawModes_.hiddenLineBackgroundFaces))
    {
        ro.shaderDesc.numLights = -1;
        ro.shaderDesc.shadeMode = SG_SHADE_FLAT;

        ro.shaderDesc.vertexColors = false;

        ro.diffuse = ACG::Vec3f(_state.clear_color()[0],_state.clear_color()[1],_state.clear_color()[2]);
        ro.ambient = ACG::Vec3f(_state.clear_color()[0],_state.clear_color()[1],_state.clear_color()[2]);
        ro.specular = ACG::Vec3f(_state.clear_color()[0],_state.clear_color()[1],_state.clear_color()[2]);
        ro.emissive = ACG::Vec3f(_state.clear_color()[0],_state.clear_color()[1],_state.clear_color()[2]);
    }
    else if (_drawMode & (drawModes_.facesColoredPerFace | drawModes_.facesColoredPerVertex
                     | drawModes_.halffacesColoredPerHalfface | drawModes_.halffacesColoredPerVertex ))
    {
        ro.shaderDesc.vertexColors = true;
        ro.shaderDesc.numLights = -1;
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;
    }
    else
    {
        ro.shaderDesc.numLights = 0;
        ro.shaderDesc.vertexColors = false;
    }

    ro.vertexBuffer = facesBufferManager_.getBuffer();
    ro.vertexDecl = facesBufferManager_.getVertexDeclaration();

    ro.glDrawArrays(GL_TRIANGLES, 0, facesBufferManager_.getNumOfVertices());
    _renderer->addRenderObject(&ro);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getEdgeRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
    RenderObject ro;
    ro.initFromState(&_state);

    ro.debugName = "VolumeMeshNodeEdges";

    //Todo: use DrawModeProperties

    // reset renderobject
    ro.depthRange = Vec2f(0.0f, 1.0f);
    ro.depthTest = true;
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);

    ro.shaderDesc.clearTextures();

    // no lighting, as there are no normals anyway
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    // thick lines
    if (_mat && _mat->lineWidth() > 1.0f)
      ro.setupLineRendering(_mat->lineWidth(), ACG::Vec2f(_state.viewport_width(), _state.viewport_height()));

    edgesBufferManager_.setDefaultColor(_state.specular_color());
    edgesBufferManager_.setOptionsFromDrawMode(_drawMode);

    if (_drawMode & ( drawModes_.edgesColoredPerEdge | drawModes_.halfedgesColoredPerHalfedge |
                      drawModes_.irregularInnerEdges | drawModes_.irregularOuterEdges ))
    {
        ro.shaderDesc.vertexColors = true;
    }
    else
    {
        ro.emissive = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);
    }

    ro.vertexBuffer = edgesBufferManager_.getBuffer();
    ro.vertexDecl = edgesBufferManager_.getVertexDeclaration();

    ro.glDrawArrays(GL_LINES, 0, edgesBufferManager_.getNumOfVertices());
    _renderer->addRenderObject(&ro);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getVertexRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
    RenderObject ro;
    ro.initFromState(&_state);

    ro.debugName = "VolumeMeshNodeVertices";


    //Todo: use DrawModeProperties

    // reset renderobject
    ro.depthRange = Vec2f(0.0f, 1.0f);
    ro.depthTest = true;
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);

    ro.shaderDesc.clearTextures();

    // no lighting, as there are no normals anyway
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    // draw with point size
    if (_mat && _mat->pointSize() > 1.0f)
      ro.setupPointRendering(_mat->pointSize(), ACG::Vec2f(_state.viewport_width(), _state.viewport_height()));

    verticesBufferManager_.setDefaultColor(_state.specular_color());
    verticesBufferManager_.setOptionsFromDrawMode(_drawMode);


    if (_drawMode & ( drawModes_.verticesColored ))
      ro.shaderDesc.vertexColors = true;
    else
      ro.emissive = ACG::Vec3f(_state.specular_color()[0],_state.specular_color()[1],_state.specular_color()[2]);

    ro.vertexBuffer = verticesBufferManager_.getBuffer();
    ro.vertexDecl = verticesBufferManager_.getVertexDeclaration();

    ro.glDrawArrays(GL_POINTS, 0, verticesBufferManager_.getNumOfVertices());
    _renderer->addRenderObject(&ro);
}



template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getSelectionRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
    RenderObject ro;
    ro.initFromState(&_state);

    ro.debugName = "VolumeMeshNodeSelections";

    ro.setMaterial(_mat);

    ro.depthTest = true;
    ro.depthFunc = GL_LEQUAL;

    ro.shaderDesc.clearTextures();
    ro.shaderDesc.vertexColors = false;
    ro.shaderDesc.numLights = -1;
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    ro.diffuse = ACG::Vec3f(selection_color_[0],selection_color_[1],selection_color_[2]);
    ro.ambient = ACG::Vec3f(selection_color_[0],selection_color_[1],selection_color_[2]);
    ro.specular = ACG::Vec3f(selection_color_[0],selection_color_[1],selection_color_[2]);
    ro.emissive = ACG::Vec3f(selection_color_[0],selection_color_[1],selection_color_[2]);

    if ((_drawMode & drawModes_.cellBasedDrawModes) && !(_drawMode & drawModes_.vertexBasedDrawModes))
        vertexSelectionBufferManager_.enableVertexOnCellPrimitives();
    else
        vertexSelectionBufferManager_.enableVertexPrimitives();

    if ((_drawMode & (drawModes_.cellBasedDrawModes | drawModes_.edgesOnCells)) && !(_drawMode & (drawModes_.edgeBasedDrawModes & ~drawModes_.edgesOnCells)))
        edgeSelectionBufferManager_.enableEdgeOnCellPrimitives();
    else
        edgeSelectionBufferManager_.enableEdgePrimitives();

    if ((_drawMode & drawModes_.cellBasedDrawModes) && !(_drawMode & (drawModes_.faceBasedDrawModes | drawModes_.halffaceBasedDrawModes)))
        faceSelectionBufferManager_.enableFaceOnCellPrimitives();
    else
        faceSelectionBufferManager_.enableFacePrimitives();

    cellSelectionBufferManager_.enableCellPrimitives();

    ro.depthRange = Vec2f(0.0f,1.0f);

    ro.vertexBuffer = vertexSelectionBufferManager_.getBuffer();
    ro.vertexDecl = vertexSelectionBufferManager_.getVertexDeclaration();
    ro.glDrawArrays(GL_POINTS, 0, vertexSelectionBufferManager_.getNumOfVertices());
    if (vertexSelectionBufferManager_.getNumOfVertices())
    {
      ro.debugName = "VolumeMeshNode.VertexSelections";
      ro.setupPointRendering(_state.point_size(), ACG::Vec2f(_state.viewport_width(), _state.viewport_height()));
      _renderer->addRenderObject(&ro);

      // reset shader templates
      ro.shaderDesc.vertexTemplateFile = "";
      ro.shaderDesc.geometryTemplateFile = "";
      ro.shaderDesc.fragmentTemplateFile = "";
    }

    ro.vertexBuffer = edgeSelectionBufferManager_.getBuffer();
    ro.vertexDecl = edgeSelectionBufferManager_.getVertexDeclaration();
    ro.glDrawArrays(GL_LINES, 0, edgeSelectionBufferManager_.getNumOfVertices());
    if (edgeSelectionBufferManager_.getNumOfVertices())
    {
      ro.debugName = "VolumeMeshNode.EdgeSelections";

      ro.setupLineRendering(_state.line_width(), ACG::Vec2f(_state.viewport_width(), _state.viewport_height()));
      _renderer->addRenderObject(&ro);

      // reset shader templates
      ro.shaderDesc.vertexTemplateFile = "";
      ro.shaderDesc.geometryTemplateFile = "";
      ro.shaderDesc.fragmentTemplateFile = "";
    }

    ro.depthRange = Vec2f(0.01f, 1.0f);

    ro.vertexBuffer = faceSelectionBufferManager_.getBuffer();
    ro.vertexDecl = faceSelectionBufferManager_.getVertexDeclaration();
    ro.glDrawArrays(GL_TRIANGLES, 0, faceSelectionBufferManager_.getNumOfVertices());
    if (faceSelectionBufferManager_.getNumOfVertices())
    {
      ro.debugName = "VolumeMeshNode.FaceSelections";

      _renderer->addRenderObject(&ro);
    }

    ro.vertexBuffer = cellSelectionBufferManager_.getBuffer();
    ro.vertexDecl = cellSelectionBufferManager_.getVertexDeclaration();
    ro.glDrawArrays(GL_TRIANGLES, 0, cellSelectionBufferManager_.getNumOfVertices());
    if (cellSelectionBufferManager_.getNumOfVertices())
    {
      ro.debugName = "VolumeMeshNode.CellSelections";
      _renderer->addRenderObject(&ro);
    }


}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{

    DrawModes::DrawMode cellDrawMode   = drawModes_.getFirstCellDrawMode(_drawMode);
    DrawModes::DrawMode faceDrawMode   = drawModes_.getFirstFaceDrawMode(_drawMode);
    DrawModes::DrawMode edgeDrawMode   = drawModes_.getFirstEdgeDrawMode(_drawMode);
    DrawModes::DrawMode vertexDrawMode = drawModes_.getFirstVertexDrawMode(_drawMode);

    if (!face_normals_calculated_)
    {
        if ( (cellDrawMode & drawModes_.cellsFlatShaded) ||
             (faceDrawMode & (drawModes_.facesFlatShaded | drawModes_.halffacesFlatShaded  | drawModes_.facesTexturedShaded)) )
            update_face_normals();
    }
    if (!vertex_normals_calculated_)
    {
        if ( (cellDrawMode & (drawModes_.cellsSmoothShaded | drawModes_.cellsPhongShaded)) ||
             (faceDrawMode & (drawModes_.facesSmoothShaded | drawModes_.halffacesSmoothShaded | drawModes_.facesPhongShaded | drawModes_.halffacesPhongShaded)) )
            update_vertex_normals();
    }

    //the VolumeMeshBufferManager can handle non atomic drawmodes if it consists of one
    // edge based draw mode (except edges on cells) and irregular edges
    edgeDrawMode |= _drawMode & (drawModes_.irregularInnerEdges | drawModes_.irregularOuterEdges);

    if (cellDrawMode)
        getCellRenderObjects(_renderer, _state, cellDrawMode, _mat);
    else
        cellsBufferManager_.free();
    if (faceDrawMode)
        getFaceRenderObjects(_renderer, _state, faceDrawMode, _mat);
    else
        facesBufferManager_.free();
    if (edgeDrawMode)
        getEdgeRenderObjects(_renderer, _state, edgeDrawMode, _mat);
    else
        edgesBufferManager_.free();
    if (vertexDrawMode)
        getVertexRenderObjects(_renderer, _state, vertexDrawMode, _mat);
    else
        verticesBufferManager_.free();

    getSelectionRenderObjects(_renderer, _state, cellDrawMode | faceDrawMode | edgeDrawMode | vertexDrawMode, _mat);

}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pick(GLState& _state, PickTarget _target) {

    // save state
    bool clientStateEnabledVertexArray   = GLState::isClientStateEnabled(GL_VERTEX_ARRAY);
    bool clientStateEnabledColorArray    = GLState::isClientStateEnabled(GL_COLOR_ARRAY);
    bool clientStateEnabledNormalArray   = GLState::isClientStateEnabled(GL_NORMAL_ARRAY);
    bool clientStateEnabledTexCoordArray = GLState::isClientStateEnabled(GL_TEXTURE_COORD_ARRAY);

    GLState::depthRange(0.01, 1.0);
    if (lastCellDrawMode_)
    {
        //draw cells so the user cannot pick invisible stuff

        GLState::bindBuffer(GL_ARRAY_BUFFER, cellsBufferManager_.getBuffer());
        GLState::enableClientState(GL_VERTEX_ARRAY);
        GLState::vertexPointer(3, GL_FLOAT, cellsBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

        GLState::disableClientState(GL_NORMAL_ARRAY);
        GLState::disableClientState(GL_COLOR_ARRAY);

        Vec4f bc = _state.specular_color();
        _state.set_color(Vec4f(0.0,0.0,0.0,0.0));

        glDrawArrays(GL_TRIANGLES, 0, cellsBufferManager_.getNumOfVertices());

        _state.set_color(bc);
        GLState::bindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (lastFaceDrawMode_)
    {
        //draw faces so the user cannot pick invisible stuff

        GLState::bindBuffer(GL_ARRAY_BUFFER, facesBufferManager_.getBuffer());
        GLState::enableClientState(GL_VERTEX_ARRAY);
        GLState::vertexPointer(3, GL_FLOAT, facesBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

        GLState::disableClientState(GL_NORMAL_ARRAY);
        GLState::disableClientState(GL_COLOR_ARRAY);

        Vec4f bc = _state.specular_color();
        _state.set_color(Vec4f(0.0,0.0,0.0,0.0));

        glDrawArrays(GL_TRIANGLES, 0, facesBufferManager_.getNumOfVertices());

        _state.set_color(bc);
        GLState::bindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLenum oldDepthFunc = _state.depthFunc();

    GLState::depthRange(0.0, 1.0);

    _state.set_depthFunc(GL_LEQUAL);

    switch (_target) {
    case PICK_VERTEX: {
        if (lastPickTarget_ != PICK_VERTEX)
            vertexPickBufferManager_.invalidateColors();
        _state.pick_set_maximum(mesh_.n_vertices());
        pickVertices(_state);
        break;
    }

    case PICK_EDGE: {
        _state.pick_set_maximum(mesh_.n_edges());
        pickEdges(_state, 0);
        break;
    }

    case PICK_FACE: {
        _state.pick_set_maximum(mesh_.n_faces());
        pickFaces(_state, 0);
        break;
    }

    case PICK_CELL: {
        _state.pick_set_maximum(mesh_.n_cells());
        pickCells(_state, 0);
        break;
    }

    case PICK_ANYTHING: {
        if (lastPickTarget_ != PICK_ANYTHING)
            vertexPickBufferManager_.invalidateColors();

        int nv = mesh_.n_vertices();
        int ne = mesh_.n_edges();
        int nf = mesh_.n_faces();
        int nc = mesh_.n_cells();

        _state.pick_set_maximum(nv + ne + nf + nc);
        pickVertices(_state);
        pickEdges(_state, nv);
        pickFaces(_state, nv + ne);
        pickCells(_state, nv + ne + nf);
        break;
    }

    default:
        break;
    }

    _state.set_depthFunc(oldDepthFunc);

    lastPickTarget_ = _target;

    // restore state
    if (clientStateEnabledVertexArray)
        GLState::enableClientState(GL_VERTEX_ARRAY);
    else
        GLState::disableClientState(GL_VERTEX_ARRAY);

    if (clientStateEnabledColorArray)
        GLState::enableClientState(GL_COLOR_ARRAY);
    else
        GLState::disableClientState(GL_COLOR_ARRAY);

    if (clientStateEnabledNormalArray)
        GLState::enableClientState(GL_NORMAL_ARRAY);
    else
        GLState::disableClientState(GL_NORMAL_ARRAY);

    if (clientStateEnabledTexCoordArray)
        GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);
    else
        GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::clear_cut_planes()
{
     cellsBufferManager_.clearCutPlanes();
     facesBufferManager_.clearCutPlanes();
     edgesBufferManager_.clearCutPlanes();
     verticesBufferManager_.clearCutPlanes();

     cellSelectionBufferManager_.clearCutPlanes();
     faceSelectionBufferManager_.clearCutPlanes();
     edgeSelectionBufferManager_.clearCutPlanes();
     vertexSelectionBufferManager_.clearCutPlanes();

     cellPickBufferManager_.clearCutPlanes();
     facePickBufferManager_.clearCutPlanes();
     edgePickBufferManager_.clearCutPlanes();
     vertexPickBufferManager_.clearCutPlanes();
}

//----------------------------------------------------------------------------



template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pickVertices(GLState& _state) {

    if (lastDrawMode_ & drawModes_.vertexBasedDrawModes)
        vertexPickBufferManager_.enableVertexPrimitives();
    else if (lastDrawMode_ & drawModes_.cellBasedDrawModes)
        vertexPickBufferManager_.enableVertexOnCellPrimitives();
    else
        vertexPickBufferManager_.enableVertexPrimitives();

    vertexPickBufferManager_.disableNormals();
    vertexPickBufferManager_.enablePickColors();



    GLState::bindBuffer(GL_ARRAY_BUFFER, vertexPickBufferManager_.getPickBuffer(_state, 0));

    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, vertexPickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::enableClientState(GL_COLOR_ARRAY);
    GLState::colorPointer(4, GL_UNSIGNED_BYTE, vertexPickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(vertexPickBufferManager_.getColorOffset()));

    GLState::shadeModel(GL_SMOOTH);
    GLState::disable(GL_LIGHTING);

    float oldPointSize = _state.point_size();
    _state.set_point_size(1.5*_state.point_size());

    glDrawArrays(GL_POINTS, 0, vertexPickBufferManager_.getNumOfVertices());

    _state.set_point_size(oldPointSize);

    GLState::disableClientState(GL_COLOR_ARRAY);
    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pickEdges(GLState& _state, unsigned int _offset) {

    if ((lastDrawMode_ & (drawModes_.cellBasedDrawModes | drawModes_.edgesOnCells)) && !(lastDrawMode_ & (drawModes_.edgeBasedDrawModes & ~drawModes_.edgesOnCells)))
        edgePickBufferManager_.enableEdgeOnCellPrimitives();
    else
        edgePickBufferManager_.enableEdgePrimitives();

    edgePickBufferManager_.enablePickColors();
    edgePickBufferManager_.disableNormals();

    GLState::bindBuffer(GL_ARRAY_BUFFER, edgePickBufferManager_.getPickBuffer(_state, _offset));

    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, edgePickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::enableClientState(GL_COLOR_ARRAY);
    GLState::colorPointer(4, GL_UNSIGNED_BYTE, edgePickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(edgePickBufferManager_.getColorOffset()));

    GLState::shadeModel(GL_SMOOTH);
    GLState::disable(GL_LIGHTING);

    float oldLineWidth = _state.line_width();
    _state.set_line_width(4.0*_state.line_width());

    glDrawArrays(GL_LINES, 0, edgePickBufferManager_.getNumOfVertices());

    _state.set_line_width(oldLineWidth);

    GLState::disableClientState(GL_COLOR_ARRAY);
    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);

}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pickFaces(GLState& _state, unsigned int _offset) {

    if (lastDrawMode_ & (drawModes_.faceBasedDrawModes | drawModes_.halffaceBasedDrawModes))
        facePickBufferManager_.enableFacePrimitives();
    else if (lastDrawMode_ & drawModes_.cellBasedDrawModes)
        facePickBufferManager_.enableFaceOnCellPrimitives();
    else
        facePickBufferManager_.enableFacePrimitives();

    facePickBufferManager_.disableNormals();
    facePickBufferManager_.enablePickColors();

    GLState::bindBuffer(GL_ARRAY_BUFFER, facePickBufferManager_.getPickBuffer(_state, _offset));

    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, facePickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::enableClientState(GL_COLOR_ARRAY);
    GLState::colorPointer(4, GL_UNSIGNED_BYTE, facePickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(facePickBufferManager_.getColorOffset()));

    GLState::shadeModel(GL_SMOOTH);
    GLState::disable(GL_LIGHTING);

    glDrawArrays(GL_TRIANGLES, 0, facePickBufferManager_.getNumOfVertices());

    GLState::disableClientState(GL_COLOR_ARRAY);
    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::pickCells(GLState& _state, unsigned int _offset) {

    cellPickBufferManager_.enablePickColors();
    cellPickBufferManager_.disableNormals();
    cellPickBufferManager_.enableCellPrimitives();

    GLState::bindBuffer(GL_ARRAY_BUFFER, cellPickBufferManager_.getPickBuffer(_state, _offset));

    GLState::enableClientState(GL_VERTEX_ARRAY);
    GLState::vertexPointer(3, GL_FLOAT, cellPickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(0));

    GLState::enableClientState(GL_COLOR_ARRAY);
    GLState::colorPointer(4, GL_UNSIGNED_BYTE, cellPickBufferManager_.getStride(), reinterpret_cast<GLvoid*>(cellPickBufferManager_.getColorOffset()));

    GLState::shadeModel(GL_SMOOTH);
    GLState::disable(GL_LIGHTING);

    glDrawArrays(GL_TRIANGLES, 0, cellPickBufferManager_.getNumOfVertices());

    GLState::disableClientState(GL_COLOR_ARRAY);
    GLState::bindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::update_face_normals()
{
    normalAttrib_.update_face_normals();
    face_normals_calculated_ = true;
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::update_vertex_normals()
{
    normalAttrib_.update_vertex_normals();
    vertex_normals_calculated_ = true;
    //update_vertex_normals will also compute face normals
    face_normals_calculated_ = true;
}

//----------------------------------------------------------------------------

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::add_cut_plane(const Vec3d& _p, const Vec3d& _n, const Vec3d& _xsize,
        const Vec3d& _ysize) {

    add_cut_plane(VolumeMeshBufferManager<VolumeMeshT>::Plane(_p, _n, _xsize, _ysize));
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_scaling(double _scale)
{
    scale_ = _scale;
    cellsBufferManager_.setScale(scale_);
    edgesBufferManager_.setScale(scale_);
    cellSelectionBufferManager_.setScale(scale_);
    faceSelectionBufferManager_.setScale(scale_);
    edgeSelectionBufferManager_.setScale(scale_);
    vertexSelectionBufferManager_.setScale(scale_);
    cellPickBufferManager_.setScale(scale_);
    facePickBufferManager_.setScale(scale_);
    edgePickBufferManager_.setScale(scale_);
    vertexPickBufferManager_.setScale(scale_);
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_boundary_only(bool _boundary)
{
    boundary_only_ = _boundary;

    cellsBufferManager_.setBoundaryOnly(_boundary);
    facesBufferManager_.setBoundaryOnly(_boundary);
    edgesBufferManager_.setBoundaryOnly(_boundary);
    verticesBufferManager_.setBoundaryOnly(_boundary);

    cellSelectionBufferManager_.setBoundaryOnly(_boundary);
    faceSelectionBufferManager_.setBoundaryOnly(_boundary);
    edgeSelectionBufferManager_.setBoundaryOnly(_boundary);
    vertexSelectionBufferManager_.setBoundaryOnly(_boundary);

    cellPickBufferManager_.setBoundaryOnly(_boundary);
    facePickBufferManager_.setBoundaryOnly(_boundary);
    edgePickBufferManager_.setBoundaryOnly(_boundary);
    vertexPickBufferManager_.setScale(_boundary);

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_topology_changed(bool _topology_changed)
{
    if (_topology_changed)
        invalidateAllBuffers();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_geometry_changed(bool _geom_changed)
{
    if (_geom_changed)
        invalidateAllBuffers();
}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_color_changed(bool _color_changed)
{
    if (_color_changed)
    {
        cellsBufferManager_.invalidateColors();
        facesBufferManager_.invalidateColors();
        edgesBufferManager_.invalidateColors();
        verticesBufferManager_.invalidateColors();
    }

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_texture_changed(bool _texture_changed)
{
    if (_texture_changed)
    {
        cellsBufferManager_.invalidateTexCoords();
        facesBufferManager_.invalidateTexCoords();
        edgesBufferManager_.invalidateTexCoords();
        verticesBufferManager_.invalidateTexCoords();
    }

}

template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::set_selection_changed(bool _selection_changed)
{
    if (_selection_changed)
        invalidateAllSelectionBuffers();
}

//----------------------------------------------------------------------------


template<class VolumeMeshT>
void VolumeMeshNodeT<VolumeMeshT>::add_cut_plane(const Plane& _p) {
    cellsBufferManager_.addCutPlane(_p);
    facesBufferManager_.addCutPlane(_p);
    edgesBufferManager_.addCutPlane(_p);
    verticesBufferManager_.addCutPlane(_p);

    cellSelectionBufferManager_.addCutPlane(_p);
    faceSelectionBufferManager_.addCutPlane(_p);
    edgeSelectionBufferManager_.addCutPlane(_p);
    vertexSelectionBufferManager_.addCutPlane(_p);

    cellPickBufferManager_.addCutPlane(_p);
    facePickBufferManager_.addCutPlane(_p);
    edgePickBufferManager_.addCutPlane(_p);
    vertexPickBufferManager_.addCutPlane(_p);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

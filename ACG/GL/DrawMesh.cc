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

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include <GL/glew.h>
#include "DrawMesh.hh"

namespace ACG {

DrawMeshBase::DrawMeshBase() :
        vbo_(0), ibo_(0),
        numTris_(0), numVerts_(0),
        meshComp_(0),
        lineIBO_(0),
        indexType_(0),
        pickVertexIBO_(0) {

    vertexDecl_ = new VertexDeclaration;
    vertexDeclEdgeCol_ = new VertexDeclaration;
    vertexDeclHalfedgeCol_ = new VertexDeclaration;
    vertexDeclHalfedgePos_ = new VertexDeclaration;
}

DrawMeshBase::~DrawMeshBase() {
    if (vbo_) glDeleteBuffersARB(1, &vbo_);
    if (ibo_) glDeleteBuffersARB(1, &ibo_);
    if (lineIBO_) glDeleteBuffersARB(1, &lineIBO_);

    delete vertexDecl_;
    delete vertexDeclEdgeCol_;
    delete vertexDeclHalfedgeCol_;
    delete vertexDeclHalfedgePos_;

    if (pickVertexIBO_) glDeleteBuffersARB(1, &pickVertexIBO_);
}

void DrawMeshBase::deleteIbo() {
    if (ibo_)
        glDeleteBuffers(1, &ibo_);
    ibo_ = 0;
}

void DrawMeshBase::bindVbo() {
    if (!vbo_)
      glGenBuffersARB(1, &vbo_);

    ACG::GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
}

void DrawMeshBase::bindIbo() {
    if (!ibo_)
      glGenBuffersARB(1, &ibo_);

    ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo_);
}

void DrawMeshBase::bindLineIbo() {
    if (!lineIBO_)
      glGenBuffersARB(1, &lineIBO_);

    ACG::GLState::bindBufferARB(GL_ELEMENT_ARRAY_BUFFER, lineIBO_);
}

void DrawMeshBase::bindPickVertexIbo() {
    if (!pickVertexIBO_)
        glGenBuffersARB(1, &pickVertexIBO_);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pickVertexIBO_);
}

void DrawMeshBase::createIndexBuffer() {
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
            numTris_ * 3 * sizeof(unsigned int),
            meshComp_->getIndexBuffer(), GL_STATIC_DRAW_ARB);
}

void DrawMeshBase::fillLineBuffer(size_t n_edges, void *data) {
    // 2 or 4 byte indices:
    if (indexType_ == GL_UNSIGNED_SHORT)
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                   n_edges * 2 * sizeof(unsigned short),
                   data, GL_STATIC_DRAW_ARB);
    else
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                   n_edges * 2 * sizeof(unsigned int),
                   data, GL_STATIC_DRAW_ARB);
}

void DrawMeshBase::fillVertexBuffer() {
    if (!vertices_.empty())
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVerts_ * vertexDecl_->getVertexStride(), &vertices_[0], GL_STATIC_DRAW_ARB);
}

void DrawMeshBase::fillInvVertexMap(size_t n_vertices, void *data) {
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(int) * n_vertices, data, GL_STATIC_DRAW);
}

} /* namespace ACG */

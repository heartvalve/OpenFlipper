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
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include <ACG/GL/acg_glew.hh>
#include "MeshNode2T.hh"

#include <stdexcept>

namespace ACG {
namespace SceneGraph {

MeshNodeBase::MeshNodeBase(BaseNode* _parent, std::string _name) :
    BaseNode(_parent, _name),
    drawMesh_(0),
    polyEdgeBuf_(0),
    polyEdgeBufSize_(0),
    polyEdgeBufTex_(0) {
}

void MeshNodeBase::supplyDrawMesh(DrawMeshBase *drawMesh) {
    /*
     * We take the luxury of checking these conditions even in release
     * mode as it this method is rarely called.
     */
    if (drawMesh_)
        throw std::runtime_error("MeshNodeBase::supplyDrawMesh called "
                "more than once.");
    if (!drawMesh)
        throw std::runtime_error("MeshNodeBase::supplyDrawMesh called "
                "with NULL parameter.");

    drawMesh_ = drawMesh;
}

void MeshNodeBase::updatePolyEdgeBuf()
{

#ifdef GL_ARB_texture_buffer_object
  // drawMesh_ must have been supplied.
  assert(drawMesh_);

  MeshCompiler * const mc = drawMesh_->getMeshCompiler();
  if (mc && !mc->isTriangleMesh())
  {
    // create/update the poly-edge buffer
    if (!polyEdgeBuf_)
      glGenBuffers(1, &polyEdgeBuf_);

    const int nTris = mc->getNumTriangles();

    const int newBufSize = (nTris/2+1);

    if (polyEdgeBufSize_ != newBufSize)
    {
      glBindBuffer(GL_TEXTURE_BUFFER, polyEdgeBuf_);

      // The poly-edge buffer is a texture buffer that stores one byte for each triangle, which encodes triangle edge properties.
      // An inner edge is an edge that was added during the triangulation of a n-poly,
      // whereas outer edges are edges that already exist in the input mesh object.
      // This information is used in the wireframe/hiddenline geometry shader to identify edges, which should not be rendered.
      // Buffer storage:
      // each triangle uses 3 bits to mark edges as visible or hidden
      //  outer edge -> bit = 1 (visible)
      //  inner edge -> bit = 0 (hidden)
      // each byte can store edges for two triangles and the remaining 2 bits are left unused

      polyEdgeBufSize_ = newBufSize;
      unsigned char* polyEdgeBufData = new unsigned char[newBufSize];

      // set zero
      memset(polyEdgeBufData, 0, newBufSize);

      // build buffer
      for (int i = 0; i < nTris; ++i)
      {
        int byteidx = i>>1;
        int bitidx = (i&1) * 3;

        for (int k = 0; k < 3; ++k)
          if (mc->isFaceEdge(i, k))
            polyEdgeBufData[byteidx] += 1 << (k + bitidx);
      }

      glBufferData(GL_TEXTURE_BUFFER, polyEdgeBufSize_, polyEdgeBufData, GL_STATIC_DRAW);


      delete [] polyEdgeBufData;
      polyEdgeBufData = 0;

      // create texture object for the texture buffer

      if (!polyEdgeBufTex_)
      {
        glGenTextures(1, &polyEdgeBufTex_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, polyEdgeBufTex_);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, polyEdgeBuf_);

        glBindTexture(GL_TEXTURE_2D, 0);
      }

      ACG::glCheckErrors();
    }
  }
#endif
}


} /* namespace SceneGraph */
} /* namespace ACG */

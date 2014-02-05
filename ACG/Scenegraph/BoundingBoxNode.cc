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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS BoundingBoxNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "BoundingBoxNode.hh"
#include "SceneGraph.hh"
#include "../GL/gl.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

DrawModes::DrawMode
BoundingBoxNode::
availableDrawModes() const
{
  return DrawModes::WIREFRAME;
}

//----------------------------------------------------------------------------
BoundingBoxNode::~BoundingBoxNode() {

}

//----------------------------------------------------------------------------

void
BoundingBoxNode::
draw(GLState& /* _state */ , const DrawModes::DrawMode& _drawMode)
{

  ACG::SceneGraph::BoundingBoxAction act;
  ACG::SceneGraph::traverse(this, act);

  ACG::Vec3d bbmin = (ACG::Vec3d) act.bbMin();
  ACG::Vec3d bbmax = (ACG::Vec3d) act.bbMax();

  if (_drawMode & DrawModes::WIREFRAME)
  {
    glPushAttrib (GL_ENABLE_BIT);

    ACG::GLState::disable(GL_LIGHTING);

    glBegin(GL_LINES);

    glVertex3f(bbmin[0],bbmin[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmin[0],bbmin[1],bbmin[2]);

    glVertex3f(bbmin[0],bbmin[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmax[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmax[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmax[2]);
    glVertex3f(bbmin[0],bbmin[1],bbmax[2]);

    glVertex3f(bbmin[0],bbmin[1],bbmin[2]);
    glVertex3f(bbmin[0],bbmin[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmin[1],bbmax[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmax[0],bbmax[1],bbmax[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmin[2]);
    glVertex3f(bbmin[0],bbmax[1],bbmax[2]);

    glEnd();

    glPopAttrib ();
  }
}



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

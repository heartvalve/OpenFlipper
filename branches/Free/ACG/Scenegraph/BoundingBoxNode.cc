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
//   $Revision: 4826 $
//   $Author: dekkers $
//   $Date: 2009-02-12 20:50:56 +0100 (Do, 12. Feb 2009) $
//
//=============================================================================




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

unsigned int
BoundingBoxNode::
availableDrawModes() const
{
  return DrawModes::WIREFRAME;
}


//----------------------------------------------------------------------------

void
BoundingBoxNode::
draw(GLState& /* _state */ , unsigned int _drawMode)
{

  ACG::SceneGraph::BoundingBoxAction act;
  ACG::SceneGraph::traverse(this, act);

  ACG::Vec3d bbmin = (ACG::Vec3d) act.bbMin();
  ACG::Vec3d bbmax = (ACG::Vec3d) act.bbMax();

  if (_drawMode & DrawModes::WIREFRAME)
  {
    glPushAttrib (GL_ENABLE_BIT);

    glDisable(GL_LIGHTING);

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

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




//=============================================================================
//
//  CLASS CoordFrameNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "CoordFrameNode.hh"
#include "SceneGraph.hh"
#include "../GL/gl.hh"
#include "../GL/gltext.hh"
#include "../GL/stipple_alpha.hh"

#include <cstdio>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


CoordFrameNode::
CoordFrameNode(BaseNode* _parent, const std::string& _name)
  : MaterialNode(_parent, _name)
{
  bb_min_ = bb_max_ = Vec3f(0,0,0);
}


//-----------------------------------------------------------------------------


DrawModes::DrawMode
CoordFrameNode::availableDrawModes() const
{
  return ( DrawModes::WIREFRAME           |
	   DrawModes::SOLID_FLAT_SHADED   );
}


//-----------------------------------------------------------------------------


void 
CoordFrameNode::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  _bbMin.minimize(bb_min_);
  _bbMax.maximize(bb_max_);
}
  

//-----------------------------------------------------------------------------


void 
CoordFrameNode::draw(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawMode */ )
{
  // draw bounding box

  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::shadeModel(GL_FLAT);

  glBegin(GL_LINE_LOOP);
  glVertex3f(bb_min_[0], bb_min_[1], bb_min_[2]);
  glVertex3f(bb_max_[0], bb_min_[1], bb_min_[2]);
  glVertex3f(bb_max_[0], bb_max_[1], bb_min_[2]);
  glVertex3f(bb_min_[0], bb_max_[1], bb_min_[2]);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(bb_min_[0], bb_min_[1], bb_max_[2]);
  glVertex3f(bb_max_[0], bb_min_[1], bb_max_[2]);
  glVertex3f(bb_max_[0], bb_max_[1], bb_max_[2]);
  glVertex3f(bb_min_[0], bb_max_[1], bb_max_[2]);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(bb_min_[0], bb_min_[1], bb_min_[2]);
  glVertex3f(bb_min_[0], bb_min_[1], bb_max_[2]);
  glVertex3f(bb_max_[0], bb_min_[1], bb_min_[2]);
  glVertex3f(bb_max_[0], bb_min_[1], bb_max_[2]);
  glVertex3f(bb_max_[0], bb_max_[1], bb_min_[2]);
  glVertex3f(bb_max_[0], bb_max_[1], bb_max_[2]);
  glVertex3f(bb_min_[0], bb_max_[1], bb_min_[2]);
  glVertex3f(bb_min_[0], bb_max_[1], bb_max_[2]);
  glEnd();



  // draw planes: transparently filled

  std::vector<float>::const_iterator p_it, p_end;
  Vec3f v0, v1, v2, v3;
  char s[20], axis;


  stipple_alpha(0.25);

  p_it  = x_planes_.begin(); 
  p_end = x_planes_.end();
  axis  = 'x';


  for (bool finished(false); !finished; )
  {
    // break check
    if (p_it == p_end)
    {
      switch (axis)
      {
	case 'x':
	  p_it  = y_planes_.begin(); 
	  p_end = y_planes_.end();
	  axis  = 'y';
	  break;

	case 'y':
	  p_it  = z_planes_.begin(); 
	  p_end = z_planes_.end();
	  axis  = 'z';
	  break;

	default:
	  finished = true;
	  break;
      }
      continue;
    }


    switch (axis)
    {
      case 'x':
	v0 = Vec3f(*p_it, bb_min_[1], bb_min_[2]);
	v1 = Vec3f(*p_it, bb_max_[1], bb_min_[2]);
	v2 = Vec3f(*p_it, bb_max_[1], bb_max_[2]);
	v3 = Vec3f(*p_it, bb_min_[1], bb_max_[2]);
	break;

      case 'y':
	v0 = Vec3f(bb_min_[0], *p_it, bb_min_[2]);
	v1 = Vec3f(bb_max_[0], *p_it, bb_min_[2]);
	v2 = Vec3f(bb_max_[0], *p_it, bb_max_[2]);
	v3 = Vec3f(bb_min_[0], *p_it, bb_max_[2]);
	break;

      case 'z':
	v0 = Vec3f(bb_min_[0], bb_min_[1], *p_it);
	v1 = Vec3f(bb_max_[0], bb_min_[1], *p_it);
	v2 = Vec3f(bb_max_[0], bb_max_[1], *p_it);
	v3 = Vec3f(bb_min_[0], bb_max_[1], *p_it);
	break;
    };


    // quads
    glBegin(GL_QUADS);
    glVertex(v0);  glVertex(v1);  glVertex(v2);  glVertex(v3);
    glEnd();


    // outlines
    glBegin(GL_LINE_LOOP);
    glVertex(v0);  glVertex(v1);  glVertex(v2);  glVertex(v3);
    glEnd();


    // text
    sprintf(s, "%c=%f", axis, *p_it);
    glText(v0, s);  glText(v1, s);  glText(v2, s);  glText(v3, s);


    ++p_it;
  }


  stipple_alpha(1.0);
}


//-----------------------------------------------------------------------------


void 
CoordFrameNode::update_bounding_box()
{
  BoundingBoxAction bb_action;
  traverse(this, bb_action);

  bb_min_ = bb_action.bbMin();
  bb_max_ = bb_action.bbMax();
}


//-----------------------------------------------------------------------------


void 
CoordFrameNode::set_bounding_box(const Vec3f& _bb_min, const Vec3f& _bb_max)
{
  bb_min_ = _bb_min;
  bb_max_ = _bb_max;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

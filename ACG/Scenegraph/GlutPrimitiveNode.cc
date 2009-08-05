/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS GlutPrimitiveNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "GlutPrimitiveNode.hh"
#include "../GL/gl.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
GlutPrimitiveNode::
boundingBox(Vec3f& _bbMin, Vec3f& _bbMax)
{
  if (_bbMin[0] > -size_)  _bbMin[0] = -size_;
  if (_bbMin[1] > -size_)  _bbMin[1] = -size_;
  if (_bbMin[2] > -size_)  _bbMin[2] = -size_;
  if (_bbMax[0] <  size_)  _bbMax[0] =  size_;
  if (_bbMax[1] <  size_)  _bbMax[1] =  size_;
  if (_bbMax[2] <  size_)  _bbMax[2] =  size_;
}


//----------------------------------------------------------------------------

  
unsigned int 
GlutPrimitiveNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS              |
	   DrawModes::WIREFRAME           |
	   DrawModes::HIDDENLINE          |
	   DrawModes::SOLID_FLAT_SHADED   |
	   DrawModes::SOLID_SMOOTH_SHADED );
}


//----------------------------------------------------------------------------


void
GlutPrimitiveNode::
draw(GLState& _state, unsigned int _drawMode)
{
  glDepthFunc(depthFunc());
  glPushMatrix();
  glTranslatef(position_[0], position_[1], position_[2]);


  if (_drawMode & DrawModes::POINTS)
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    draw_obj();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::WIREFRAME)
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_obj();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::HIDDENLINE)
  {
    Vec4f base_color_backup = _state.base_color();

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(_state.clear_color());
    glDepthRange(0.01, 1.0);
    draw_obj();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor(base_color_backup);
    glDepthRange(0.0, 1.0);
    draw_obj();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::SOLID_FLAT_SHADED)
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_obj();
  }


  if (_drawMode & DrawModes::SOLID_SMOOTH_SHADED)
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    draw_obj();
  }


  glPopMatrix();
  glDepthFunc(GL_LESS);
}


//----------------------------------------------------------------------------


void
GlutPrimitiveNode::draw_obj() const
{
  switch (type_)
  {
    case CONE: 
      glutSolidCone(size_, innersize_, slices_, stacks_);
      break;
 
    case CUBE: 
      glutSolidCube(size_);
      break;
    
    case DODECAHEDRON: 
      glutSolidDodecahedron();
      break;
    
    case ICOSAHEDRON: 
      glutSolidIcosahedron();
      break;

    case OCTAHEDRON:
      glutSolidOctahedron();
      break;

    case  SPHERE: 
      glutSolidSphere(size_, slices_, stacks_);
      break;
       
    case TEAPOT: 
      glutSolidTeapot(size_);
      break;

    case TETRAHEDRON: 
      glutSolidTetrahedron();
      break;
 
    case TORUS: 
      glutSolidTorus(innersize_, size_, slices_, stacks_);
      break;
  };
}


//----------------------------------------------------------------------------


void
GlutPrimitiveNode::
pick(GLState& _state , PickTarget _target)
{
  // initialize picking stack
  if (!_state.pick_set_maximum (1))
  {
    std::cerr << "Strange pickSetMaximum failed for index 1 in GlutPrimitiveNode\n";
    return;
  }
  _state.pick_set_name(0);


  switch (_target)
  {
    case PICK_ANYTHING:
    case PICK_FACE: 
    { 
      glDisable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      glPushMatrix();
      glTranslatef(position_[0], position_[1], position_[2]);
      draw_obj();
      glPopMatrix();
      break; 
    }

    default:
      break;
  }      
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

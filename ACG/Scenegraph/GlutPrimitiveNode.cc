/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
set_position(const Vec3d& _p, int _idx)
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    primitives_[_idx].position = _p; 
}

//----------------------------------------------------------------------------

const Vec3d
GlutPrimitiveNode::
get_position(int _idx) const 
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    return primitives_[_idx].position; 
  
  return Vec3d(-1,-1,-1);
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
set_size(double _s, int _idx) 
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    primitives_[_idx].size = _s; 
}

//----------------------------------------------------------------------------

double
GlutPrimitiveNode::
get_size(int _idx) const
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    return primitives_[_idx].size; 
  return -1;
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (int i = 0; i < (int)primitives_.size(); ++i)
  {
    Vec3d sizeVec(primitives_[i].size, primitives_[i].size, primitives_[i].size);
    _bbMax.maximize(primitives_[i].position + sizeVec);
    _bbMin.minimize(primitives_[i].position - sizeVec);
  }
}

//----------------------------------------------------------------------------
  
DrawModes::DrawMode
GlutPrimitiveNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS              |
	   DrawModes::WIREFRAME           |
	   DrawModes::HIDDENLINE          |
	   DrawModes::SOLID_FLAT_SHADED   |
	   DrawModes::SOLID_SMOOTH_SHADED |
	   DrawModes::SOLID_FACES_COLORED );
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
draw(GLState& _state, DrawModes::DrawMode _drawMode)
{  
  for (int i = 0; i < (int)primitives_.size(); ++i)
  {
  
    glPushMatrix();
    glTranslatef(primitives_[i].position[0], primitives_[i].position[1], primitives_[i].position[2]);


    if (_drawMode & DrawModes::POINTS)
    {
      glDisable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      draw_obj(i);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    if (_drawMode & DrawModes::WIREFRAME)
    {
      glDisable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      draw_obj(i);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (_drawMode & DrawModes::SOLID_FACES_COLORED)
    {
      glDisable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }

      draw_obj(i);
      
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
      draw_obj(i);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glColor(base_color_backup);
      glDepthRange(0.0, 1.0);
      draw_obj(i);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    if (_drawMode & DrawModes::SOLID_FLAT_SHADED)
    {
      glEnable( GL_COLOR_MATERIAL );
      glEnable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }
      
      draw_obj(i);
    }


    if (_drawMode & DrawModes::SOLID_SMOOTH_SHADED)
    {
      glEnable( GL_COLOR_MATERIAL );
      glEnable(GL_LIGHTING);
      glShadeModel(GL_SMOOTH);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }

      draw_obj(i);
    }

    glPopMatrix();
  } // end of primitives iter
}

//----------------------------------------------------------------------------

void 
GlutPrimitiveNode::
add_primitive(GlutPrimitiveType _type, Vec3d _pos, Vec3d _axis, ACG::Vec4f _color)
{
  Primitive p(_type, _pos, _axis, _color);
  primitives_.push_back(p);
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::draw_obj(int _idx) const
{
  if (_idx < 0 || _idx >= (int)primitives_.size()) // range check
    return;

  Vec3d axis  = primitives_[_idx].axis;
  double size = axis.norm();

  if (size > 1e-10)
  {
    glPushMatrix();

    Vec3d direction = axis;
    Vec3d z_axis(0,0,1);
    Vec3d rot_normal;
    double rot_angle;

    direction.normalize();
    rot_angle  = acos((z_axis | direction)) * 180 / M_PI;
    rot_normal = ((z_axis % direction).normalize());
  

    if (fabs(rot_angle) > 0.0001 && fabs(180 - rot_angle) > 0.0001)
      glRotatef(rot_angle,rot_normal[0], rot_normal[1], rot_normal[2]);
    else
      glRotatef(rot_angle,1,0,0);

    
    switch (primitives_[_idx].type)
    {
      case CONE: 
        glutSolidCone(primitives_[_idx].size, primitives_[_idx].innersize, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;

      case CUBE: 
        glutSolidCube(primitives_[_idx].size);
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
        glutSolidSphere(primitives_[_idx].size, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;
        
      case TEAPOT: 
        glutSolidTeapot(primitives_[_idx].size);
        break;

      case TETRAHEDRON: 
        glutSolidTetrahedron();
        break;

      case TORUS: 
        glutSolidTorus(primitives_[_idx].innersize, primitives_[_idx].size, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;
    }

    glPopMatrix();
  }
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
pick(GLState& _state , PickTarget _target)
{
  // initialize picking stack
  if (!_state.pick_set_maximum (primitives_.size()))
  {
    std::cerr << "Strange pickSetMaximum failed for index " << primitives_.size() << " in GlutPrimitiveNode\n";
    return;
  }

  switch (_target)
  {
    case PICK_ANYTHING:
    case PICK_FACE: 
    { 
      for (int i = 0; i < (int)primitives_.size(); ++i)
      {
        _state.pick_set_name(i);
        glPushMatrix();
        glTranslatef(primitives_[i].position[0], primitives_[i].position[1], primitives_[i].position[2]);
        draw_obj(i);
        glPopMatrix();
      }
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

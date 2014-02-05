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
//  CLASS QuadNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "QuadNode.hh"
#include "DrawModes.hh"


//== NAMESPACES ============================================================== 


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


QuadNode::QuadNode( BaseNode*   _parent,
			    std::string _name  )
  : BaseNode(_parent, _name)
{
}
  

//----------------------------------------------------------------------------


QuadNode::~QuadNode() 
{
}


//----------------------------------------------------------------------------


void
QuadNode::boundingBox( Vec3d & _bbMin, Vec3d & _bbMax )
{
  Vec3f bbMin(FLT_MAX,FLT_MAX,FLT_MAX);
  Vec3f bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
  
  PointVector::const_iterator p_it  = point_.begin(), p_end = point_.end();
  
  for ( ; p_it != p_end; ++p_it )
  {
    bbMin.minimize( *p_it );
    bbMax.maximize( *p_it );
  }
  
  Vec3d bbMind = ACG::Vec3d(bbMin);
  Vec3d bbMaxd = ACG::Vec3d(bbMax);
  
  _bbMin.minimize(bbMind);
  _bbMax.maximize(bbMaxd);    
}


//----------------------------------------------------------------------------

  
DrawModes::DrawMode
QuadNode::
availableDrawModes() const
{
  DrawModes::DrawMode drawModes(DrawModes::NONE);

  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::SOLID_FLAT_SHADED;
  //  drawModes |= DrawModes::SOLID_FACES_COLORED;

  return drawModes;
}


//----------------------------------------------------------------------------


void
QuadNode::
draw(GLState& /* _state */ , const DrawModes::DrawMode& _drawMode)
{
  if (_drawMode & DrawModes::WIREFRAME ||
      _drawMode & DrawModes::HIDDENLINE )
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_wireframe();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::SOLID_FLAT_SHADED ||
      _drawMode & DrawModes::HIDDENLINE )
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }

  if (_drawMode & DrawModes::SOLID_FACES_COLORED)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }
}


//----------------------------------------------------------------------------


void
QuadNode::
draw_vertices()
{
  //  glDrawArrays(GL_POINTS, 0, mesh_.n_vertices());
}


//----------------------------------------------------------------------------


void
QuadNode::draw_faces()
{
  glBegin(GL_QUADS);

  unsigned int i = 0;
  unsigned int j = 0;

  for ( ; i < point_.size(); i += 4, j += 1 )
  {
    glNormal(  normal_[j] );
    
    glVertex( point_[i + 0] );
    glVertex( point_[i + 1] );
    glVertex( point_[i + 2] );
    glVertex( point_[i + 3] );
  }

  glEnd();
}


//----------------------------------------------------------------------------


void
QuadNode::draw_wireframe()
{
  glBegin(GL_QUADS);
  for ( unsigned int i = 0; i < point_.size(); ++i )
    glVertex( point_[i] );
  glEnd();
}


//----------------------------------------------------------------------------


void
QuadNode::pick( GLState & _state, PickTarget /* _target */  )
{
  _state.pick_set_maximum (1);
  _state.pick_set_name (0);
  draw_faces();
}


//----------------------------------------------------------------------------

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS GridNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "GridNode.hh"
#include "SceneGraph.hh"
#include "../GL/gl.hh"
#include <stdio.h>
#include <math.h>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


GridNode::
GridNode(BaseNode* _parent, const std::string& _name)
  : MaterialNode(_parent, _name),
    horizontalLines_(7),
    verticalLines_(7),
    width_(1500.0),
    height_(1500.0)
{
  baseLineColor_ = Vec3f(0.5, 0.5, 0.5);
  midLineColor_  = Vec3f(0.3, 0.3, 0.3);
  bb_min_ = Vec3f(-0.5*width_, 0.0, -0.5*height_);
  bb_max_ = Vec3f( 0.5*width_, 0.0,  0.5*height_);
}


//-----------------------------------------------------------------------------


unsigned int
GridNode::availableDrawModes() const
{
  return ( DrawModes::WIREFRAME           |
	        DrawModes::SOLID_FLAT_SHADED   );
}


//-----------------------------------------------------------------------------


void
GridNode::boundingBox(Vec3f& _bbMin, Vec3f& _bbMax)
{
  _bbMin.minimize(bb_min_);
  _bbMax.maximize(bb_max_);
}


//-----------------------------------------------------------------------------


void
GridNode::draw(GLState&  _state  , unsigned int /* _drawMode */ )
{

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  glDisable(GL_LIGHTING);

  glPushAttrib( GL_DEPTH_TEST );
  glEnable( GL_DEPTH_TEST );

  glLineWidth(0.1);
  _state.push_modelview_matrix();

  //first we calculate the hitpoint of the camera direction with the grid
  GLMatrixd modelview = _state.modelview();

  Vec3d viewDirection;
  viewDirection[0] = -modelview(0,2);
  viewDirection[1] = -modelview(1,2);
  viewDirection[2] = -modelview(2,2);

  Vec3f eye = (Vec3f) _state.inverse_modelview().transform_point(ACG::Vec3d(0.0, 0.0, 0.0));

  double lambda = - (eye[1] / viewDirection[1]);

  Vec3f hitPoint;
  hitPoint[0] = eye[0] + lambda * viewDirection[0];
  hitPoint[1] = 0;
  hitPoint[2] = eye[2] + lambda * viewDirection[2];

//   _state.translate(hitPoint[0], hitPoint[1], hitPoint[2]);
//   glutSolidSphere(10.0, 10, 10 );
//   _state.translate(-hitPoint[0], -hitPoint[1], -hitPoint[2]);


  //the distance between camera and hitpoint defines a factor that controls
  //how many lines are drawn
  double distance = _state.modelview().transform_point( hitPoint ).norm();

  int factor =  floor(2000.0 / distance) - 1;

  int vertical = verticalLines_;
  int horizontal = horizontalLines_;

  if (factor > 0){
    // 'factor' times split the grid (split every cell into 4 new cells)
    int rest = 0;

    for (int i=0; i < factor; i++)
      rest -= floor( pow(2, i));

    vertical = vertical * floor( pow(2,factor)) + rest;
    horizontal = horizontal * floor( pow(2,factor)) + rest;

    vertical = std::min(vertical, 65 );
    horizontal = std::min(vertical, 65 );
  }

  //now start drawing
  _state.translate(-0.5*width_, 0, -0.5*height_);

  glBegin(GL_LINES);

    //red line (x-axis)
    glColor3f( 0.7, 0.0, 0.0 );
    glVertex3f(   0.0, 0.0, height_*0.5);
    glVertex3f(width_, 0.0, height_*0.5);

    //blue line (z-axis)
    glColor3f( 0.0, 0.0, 0.7 );
    glVertex3f(width_*0.5, 0.0,     0.0);
    glVertex3f(width_*0.5, 0.0, height_);

    //remaining vertical lines
    for ( int i = 0 ; i <  vertical ; ++i ) {

      //first draw a baseLine
      glColor3fv( &baseLineColor_[0] );

      double big  = width_ / (vertical-1) * i;
      double next = width_ / (vertical-1) * (i+1);

      glVertex3f( big, 0.0, 0.0);
      glVertex3f( big, 0.0, height_);

      if ( i+1 < vertical)
        for (int j=1; j < 10; j++){

          //then draw 9 darker lines in between
          glColor3fv( &midLineColor_[0] );
  
          double small = big + (next - big) / 10.0 * j;
          glVertex3f( small, 0.0, 0.0);
          glVertex3f( small, 0.0, height_);
        }
    }

    //remaining horizontal lines
    for ( int i = 0 ; i <  horizontal; ++i ) {

      //first draw a baseline
      glColor3fv( &baseLineColor_[0] );

      double big  = width_ / (vertical-1) * i;
      double next = width_ / (vertical-1) * (i+1);

      glVertex3f(   0.0, 0.0, height_ / (horizontal-1) * i);
      glVertex3f(width_, 0.0, height_ / (horizontal-1) * i);

      if ( i+1 < vertical)
        for (int j=1; j < 10; j++){

          //then draw 9 darker lines in between
          glColor3fv( &midLineColor_[0] );
  
          double small = big + (next - big) / 10.0 * j;
          glVertex3f(   0.0, 0.0, small);
          glVertex3f(width_, 0.0, small);
        }   
    }
  glEnd();

  _state.pop_modelview_matrix();

  glLineWidth(1.0);

  glPopAttrib( ); // ->Depth test
  glPopAttrib( ); // ->Lighting
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

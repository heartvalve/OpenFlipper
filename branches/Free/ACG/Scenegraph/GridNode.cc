/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
    maxRefinement_(49),
    gridSize_(1500.0),
    autoResize_(true)
{
  baseLineColor_ = Vec3f(0.5, 0.5, 0.5);
  midLineColor_  = Vec3f(0.3, 0.3, 0.3);
}


//-----------------------------------------------------------------------------


DrawModes::DrawMode
GridNode::availableDrawModes() const
{
  return ( DrawModes::WIREFRAME           |
	        DrawModes::SOLID_FLAT_SHADED   );
}


//-----------------------------------------------------------------------------


void
GridNode::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  
  // If we do not autoresize, we set our bounding box here
  // otherwise we set the size of the grid according to the 
  // real scene geometry from the state
  if ( !autoResize_ ) {
    bb_min_ = Vec3f(-0.5*gridSize_, 0.0, -0.5*gridSize_);
    bb_max_ = Vec3f( 0.5*gridSize_, 0.0,  0.5*gridSize_);
    
    _bbMin.minimize(bb_min_);
    _bbMax.maximize(bb_max_);
  }
  
}


//----------------------------------------------------------------

void
GridNode::pick(GLState& _state, PickTarget _target)
{


}

//-----------------------------------------------------------------------------


void
GridNode::draw(GLState&  _state  , DrawModes::DrawMode /* _drawMode */ )
{

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  glDisable(GL_LIGHTING);

  glPushAttrib( GL_DEPTH_TEST );
  glEnable( GL_DEPTH_TEST );

  glLineWidth(0.1);
  _state.push_modelview_matrix();

  //first we calculate the hitpoint of the camera direction with the grid
  GLMatrixd modelview = _state.modelview();

  Vec3d viewDirection(0.0, 1.0, 0.0); // = _state.viewing_direction();
  Vec3f eye = (Vec3f) _state.eye();

  double lambda = - (eye[1] / viewDirection[1]);

  Vec3f hitPoint;
  hitPoint[0] = eye[0] + lambda * viewDirection[0];
  hitPoint[1] = 0;
  hitPoint[2] = eye[2] + lambda * viewDirection[2];

  //the distance between camera and hitpoint defines a factor that controls
  //how many lines are drawn
  double distance = _state.modelview().transform_point( hitPoint ).norm();

  int factor =  floor(2000.0 / distance) - 1;

  int vertical = verticalLines_;
  int horizontal = horizontalLines_;

  if (factor > 20){
    vertical = maxRefinement_;
    horizontal = maxRefinement_;

  } else if (factor > 0){
    // 'factor' times split the grid (split every cell into 4 new cells)
    int rest = 0;

    for (int i=0; i < factor; i++)
      rest -= floor( pow(double(2.0), i));

    vertical   = vertical   * floor( pow(double(2.0),factor)) + rest;
    horizontal = horizontal * floor( pow(double(2.0),factor)) + rest;

    vertical   = std::min(vertical, maxRefinement_ );
    horizontal = std::min(vertical, maxRefinement_ );
  }
  
  if ( autoResize_ ) {
  
    // Get the current scene size from the glstate
    ACG::Vec3d bb_min,bb_max;
    _state.get_bounding_box(bb_min,bb_max);
    
    // compute the minimal scene radius from the bounding box
    double radius = std::min(fabs(bb_max[0]-bb_min[0]) * 2,fabs(bb_max[2]-bb_min[2]) * 2);
    
    // set grid size to the given radius
    gridSize_ = radius;

    
    _state.set_line_width(3);
    glBegin(GL_LINES);
    glVertex(bb_min);
    glVertex(bb_max);
    glEnd();
    
//     // update the bounding box
//     bb_min_ = Vec3f(-0.5*gridSize_, 0.0, -0.5*gridSize_);
//     bb_max_ = Vec3f( 0.5*gridSize_, 0.0,  0.5*gridSize_);
    
    std::cerr << "Draw " << bb_min << " "  << bb_max << std::endl;
    

    
  }

  //now start drawing
  _state.translate(-0.5*gridSize_, 0, -0.5*gridSize_);

  glBegin(GL_LINES);

    //red line (x-axis)
    glColor3f( 0.7, 0.0, 0.0 );
    glVertex3f(   0.0, 0.0, gridSize_*0.5);
    glVertex3f(gridSize_, 0.0, gridSize_*0.5);

    //blue line (z-axis)
    glColor3f( 0.0, 0.0, 0.7 );
    glVertex3f(gridSize_*0.5, 0.0,     0.0);
    glVertex3f(gridSize_*0.5, 0.0, gridSize_);

    //remaining vertical lines
    for ( int i = 0 ; i <  vertical ; ++i ) {

      //first draw a baseLine
      glColor3fv( &baseLineColor_[0] );

      double big  = gridSize_ / (vertical-1) * i;
      double next = gridSize_ / (vertical-1) * (i+1);

      glVertex3f( big, 0.0, 0.0);
      glVertex3f( big, 0.0, gridSize_);

      if ( i+1 < vertical)
        for (int j=1; j < 10; j++){

          //then draw 9 darker lines in between
          glColor3fv( &midLineColor_[0] );
  
          double smallPos = big + (next - big) / 10.0 * j;
          glVertex3f( smallPos, 0.0, 0.0);
          glVertex3f( smallPos, 0.0, gridSize_);
        }
    }

    //remaining horizontal lines
    for ( int i = 0 ; i <  horizontal; ++i ) {

      //first draw a baseline
      glColor3fv( &baseLineColor_[0] );

      double big  = gridSize_ / (vertical-1) * i;
      double next = gridSize_ / (vertical-1) * (i+1);

      glVertex3f(   0.0, 0.0, gridSize_ / (horizontal-1) * i);
      glVertex3f(gridSize_, 0.0, gridSize_ / (horizontal-1) * i);

      if ( i+1 < vertical)
        for (int j=1; j < 10; j++){

          //then draw 9 darker lines in between
          glColor3fv( &midLineColor_[0] );
  
          double smallPos = big + (next - big) / 10.0 * j;
          glVertex3f(   0.0, 0.0, smallPos);
          glVertex3f(gridSize_, 0.0, smallPos);
        }   
    }
  glEnd();

  _state.pop_modelview_matrix();

  glLineWidth(1.0);

  glPopAttrib( ); // ->Depth test
  glPopAttrib( ); // ->Lighting
}

//-----------------------------------------------------------------------------

void
GridNode::gridSize(float _size){
  gridSize_ = _size;

  bb_min_ = Vec3f(-0.5*gridSize_, 0.0, -0.5*gridSize_);
  bb_max_ = Vec3f( 0.5*gridSize_, 0.0,  0.5*gridSize_);
}

//-----------------------------------------------------------------------------

float
GridNode::gridSize(){
  return gridSize_;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

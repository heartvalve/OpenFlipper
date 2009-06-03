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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS CoordsysNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "CoordsysNode.hh"
#include "../GL/gl.hh"
#include <iostream>
#include <math.h>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


void
CoordsysNode::
boundingBox(Vec3f& /*_bbMin*/, Vec3f& /*_bbMax*/)
{
	//_bbMin.minimize( Vect3f  )
}


//----------------------------------------------------------------------------


unsigned int
CoordsysNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------

void
CoordsysNode::
drawCoordsys( GLState&  _state) {

  double topRadius = 0.01;
  double arrowLength = 0.04;
  double bodyRadius = 0.004;
  double bodyLength = 0.06;
  int slices = 10;
  int stacks = 10;
  int loops = 10;
  double sphereRadius = 0.01;

  GLUquadricObj *quadric = gluNewQuadric();

  // Origin
  glColor3f(0.5, 0.5, 0.5);
  gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
  glColor3f(1.0, 0.0, 0.0);
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Y-Axis
  glColor3f(0.0, 1.0, 0.0);
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Z-Axis
  glColor3f(0.0, 0.0, 1.0);
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  gluDeleteQuadric(quadric);
}

//============================================================================

void
CoordsysNode::drawCoordsysPick( GLState&  _state) {

  double topRadius = 0.01;
  double arrowLength = 0.04;
  double bodyRadius = 0.004;
  double bodyLength = 0.06;
  int slices = 10;
  int stacks = 10;
  int loops = 10;
  double sphereRadius = 0.01;

  GLUquadricObj *quadric = gluNewQuadric();

  // Origin
  _state.pick_set_name (1);
  gluSphere( quadric, sphereRadius, slices, stacks );

  // X-Axis
  _state.pick_set_name (2);
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Y-Axis
  _state.pick_set_name (3);
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();

  // Z-Axis
  _state.pick_set_name (4);
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  gluCylinder( quadric, bodyRadius, bodyRadius, bodyLength, slices, stacks );
  gluDisk( quadric, 0, topRadius, slices, loops );
  _state.translate ( 0, 0, -arrowLength );
  gluCylinder( quadric, 0, topRadius, arrowLength, slices, stacks );
  _state.pop_modelview_matrix ();


  gluDeleteQuadric(quadric);

}


//============================================================================


void
CoordsysNode::
draw(GLState&  _state  , unsigned int /*_drawMode*/)
{
  // Push Modelview-Matrix
  _state.push_modelview_matrix();

  Vec4f lastBaseColor = _state.base_color();

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  glEnable(GL_LIGHTING);
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);

  // Init state - changes when mode_ != POSITION
  Vec3d pos3D(0.0,0.0,0.0);

  if ( mode_ == SCREENPOS ) {

    int left, bottom, width, height;
    _state.get_viewport(left, bottom, width, height);

    // Projection reset
    _state.push_projection_matrix();
    _state.reset_projection();
    _state.perspective(45.0, _state.aspect(), 0.8, 20.0);

    float posx = left + width - 30.0 ;
    float posy = bottom + height - 30.0 ;

    Vec3d screenposCenterPoint( posx , posy , 0.0);

    _state.push_modelview_matrix();
    _state.reset_modelview();

    pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
    _state.pop_modelview_matrix();

    // reset scene translation
    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);
    _state.translate (pos3D[0], pos3D[1], pos3D[2]-0.3, MULT_FROM_LEFT);

    glDepthRange (1.0, 1.0);
    glDepthFunc (GL_ALWAYS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    glDepthRange (0.0, 1.0);
    glDepthFunc (GL_LESS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // set depth buffer to 0 so tah nothing can paint over cordsys
    glDepthRange (0.0, 0.0);
    glDepthFunc (GL_ALWAYS);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    glDepthRange (0.0, 1.0);
    glDepthFunc (GL_LESS);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

    // Projection reload
    _state.pop_projection_matrix();


  } else if (mode_ == POSITION) { /* mode_ == POSITION */

    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);

    // clear depth buffer in coordsys region
    glDepthRange (1.0, 1.0);
    glDepthFunc (GL_ALWAYS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // draw coordsys in normal mode
    glDepthRange (0.0, 1.0);
    glDepthFunc (GL_LESS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // set depth buffer to 0 so tah nothing can paint over cordsys
    glDepthRange (0.0, 0.0);
    glDepthFunc (GL_ALWAYS);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // reset to default
    glDepthRange (0.0, 1.0);
    glDepthFunc (GL_LESS);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  }

  glPopAttrib();

  glColor4fv(lastBaseColor.data());

  // Reload old configuration
  _state.pop_modelview_matrix();
}


void
CoordsysNode::
setMode(const CoordsysMode _mode)
{
  mode_ = _mode;
}

void
CoordsysNode::
setPosition(const Vec3f& _pos)
{
  pos3f_ = _pos;
}

CoordsysNode::CoordsysMode
CoordsysNode::
getMode() const
{
  return mode_;
}

void
CoordsysNode::pick(GLState& _state, PickTarget _target)
{
  if (_target == PICK_ANYTHING) {

    GLdouble mat[16];

    // Push Modelview-Matrix
    _state.push_modelview_matrix();
    _state.pick_set_maximum (5);
    _state.pick_set_name (0);

    // Init state - changes when mode_ != POSITION
    Vec3d pos3D(0.0,0.0,0.0);

    if ( mode_ == SCREENPOS ) {

      int left, bottom, width, height;
      _state.get_viewport(left, bottom, width, height);

      // Projection reset
      _state.push_projection_matrix();
      _state.reset_projection();
      _state.perspective(45.0, _state.aspect(), 0.8, 20.0);

      float posx = left + width - 30.0 ;
      float posy = bottom + height - 30.0 ;

      Vec3d screenposCenterPoint( posx , posy , 0.0);

      _state.push_modelview_matrix();
      _state.reset_modelview();

      pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
      _state.pop_modelview_matrix();

      // Projection reload
      _state.pop_projection_matrix();

      // the only way to get the gl pick matrix again
      glMatrixMode(GL_PROJECTION);

      glPushMatrix ();
      glMultMatrixd( _state.inverse_projection().get_raw_data());

      glGetDoublev(GL_PROJECTION_MATRIX, mat);

      GLMatrixd pickMat (mat);

      // add our matrix
      gluPerspective(45.0, _state.aspect(), 0.8, 20.0);

      glMatrixMode(GL_MODELVIEW);

      // reset scene translation
      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

      _state.set_modelview (modelview);
      _state.translate (pos3D[0], pos3D[1], pos3D[2]-0.3, MULT_FROM_LEFT);

      // clear depth buffer behind coordsys node
      clearPickArea(_state, pickMat, true, 1.0);

      // Koordinatensystem zeichnen
      drawCoordsysPick(_state);

      // set depth buffer to 0.0 so that nothing can paint above
      clearPickArea(_state, pickMat, false, 0.0);

      glMatrixMode(GL_PROJECTION);
      glPopMatrix ();
      glMatrixMode(GL_MODELVIEW);

    } else if (mode_ == POSITION) { /* mode_ == POSITION */

      glMatrixMode(GL_PROJECTION);

      glPushMatrix ();
      glMultMatrixd( _state.inverse_projection().get_raw_data());

      glGetDoublev(GL_PROJECTION_MATRIX, mat);

      glPopMatrix ();

      GLMatrixd pickMat (mat);

      glMatrixMode(GL_MODELVIEW);

      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

      // clear depth buffer behind coordsys node
      clearPickArea(_state, pickMat, true, 1.0);

      // Koordinatensystem zeichnen
      drawCoordsysPick(_state);

      // set depth buffer to 0.0 so that nothing can paint above
      clearPickArea(_state, pickMat, false, 0.0);
    }
    // Reload old configuration
    _state.pop_modelview_matrix();

  }
}

//----------------------------------------------------------------------------

void CoordsysNode::clearPickArea(GLState&  _state, GLMatrixd _pickMatrix, bool _draw, GLfloat _depth)
{
  std::vector<Vec2f> points;
  Vec2f center;
  float radius;

  int left, bottom, width, height;
  _state.get_viewport(left, bottom, width, height);

  GLUquadricObj *quadric = gluNewQuadric();

  // respect sphere radius
  Vec3d proj = _state.project (Vec3d (-0.01, -0.01, -0.01));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.1, 0.0, 0.0));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.0, 0.1, 0.0));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.0, 0.0, 0.1));
  points.push_back (Vec2f (proj[0], proj[1]));


  // get bounding circle of projected 4 points of the coord node
  boundingCircle(points, center, radius);

  _state.push_projection_matrix();
  _state.reset_projection();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix ();
  glMultMatrixd(_pickMatrix.get_raw_data());
  glMatrixMode(GL_MODELVIEW);

  _state.ortho (left, left + width, bottom, bottom + height, 0.0, 1.0);

  _state.push_modelview_matrix();
  _state.reset_modelview();
  glDepthFunc (GL_ALWAYS);
  glDepthRange (_depth, _depth);
  _state.translate (center[0], center[1], -0.5);

  if (_draw)
    _state.pick_set_name (0);
  else
    glColorMask(false, false, false, false);

  // 10% more to ensure everything is in
  gluDisk( quadric, 0, radius * 1.1, 10, 10 );

  glDepthFunc (GL_LESS);
  _state.pop_modelview_matrix();
  _state.pop_projection_matrix();

  glDepthRange (0.0, 1.0);

  if (!_draw)
    glColorMask(true, true, true, true);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode(GL_MODELVIEW);

  gluDeleteQuadric(quadric);
}

//----------------------------------------------------------------------------

void CoordsysNode::boundingCircle(std::vector<Vec2f> &_in, Vec2f &_center, float &_radius)
{
  if (_in.size () == 0)
    return;
  if (_in.size () < 2)
  {
    _center = _in[0];
    _radius = 0.0f;
    return;
  }
  bool found = false;

  // try all circumcircles of all possible lines
  for (unsigned int i = 0; i < _in.size () - 1; i++)
    for (unsigned int j = i + 1; j < _in.size (); j++)
    {
      Vec2f cen = (_in[i] + _in[j]) * 0.5;
      float rad = (_in[i] - cen).length ();
      bool allin = true;

      for (unsigned int k = 0; k < _in.size (); k++)
        if (k != i && k != j && (_in[k] - cen).length () > rad)
        {
          allin = false;
          break;
        }

      if (!allin)
        continue;

      if (found)
      {
        if (rad < _radius)
        {
          _center = cen;
          _radius = rad;
        }
      }
      else
      {
        found = true;
        _center = cen;
        _radius = rad;
      }
    }

  if (found)
    return;

  // try all circumcircles of all possible triangles
  for (unsigned int i = 0; i < _in.size () - 2; i++)
    for (unsigned int j = i + 1; j < _in.size () - 1; j++)
      for (unsigned int k = j + 1; k < _in.size (); k++)
      {
        float v = ((_in[k][0]-_in[j][0])*((_in[i][0]*_in[i][0])+(_in[i][1]*_in[i][1]))) +
                  ((_in[i][0]-_in[k][0])*((_in[j][0]*_in[j][0])+(_in[j][1]*_in[j][1]))) +
                  ((_in[j][0]-_in[i][0])*((_in[k][0]*_in[k][0])+(_in[k][1]*_in[k][1])));
        float u = ((_in[j][1]-_in[k][1])*((_in[i][0]*_in[i][0])+(_in[i][1]*_in[i][1]))) +
                  ((_in[k][1]-_in[i][1])*((_in[j][0]*_in[j][0])+(_in[j][1]*_in[j][1]))) +
                  ((_in[i][1]-_in[j][1])*((_in[k][0]*_in[k][0])+(_in[k][1]*_in[k][1])));
        float d = (_in[i][0]*_in[j][1])+(_in[j][0]*_in[k][1])+(_in[k][0]*_in[i][1]) -
                  (_in[i][0]*_in[k][1])-(_in[j][0]*_in[i][1])-(_in[k][0]*_in[j][1]);
        Vec2f cen(0.5 * (u/d), 0.5 * (v/d));
        float rad = (_in[i] - cen).length ();
        bool allin = true;

        for (unsigned int l = 0; l < _in.size (); l++)
          if (l != i && l != j && l != k && (_in[l] - cen).length () > rad)
          {
            allin = false;
            break;
          }

        if (!allin)
          continue;

        if (found)
        {
          if (rad < _radius)
          {
            _center = cen;
            _radius = rad;
          }
        }
        else
        {
          found = true;
          _center = cen;
          _radius = rad;
        }
      }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

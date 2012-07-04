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
//  CLASS PolyLineNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_POLYLINENODET_C

//== INCLUDES =================================================================

#include "PolyLineNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
  {
    _bbMin.minimize(polyline_.point(i));
    _bbMax.maximize(polyline_.point(i));
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
DrawModes::DrawMode
PolyLineNodeT<PolyLine>::
availableDrawModes()
{
  return (DrawModes::WIREFRAME | DrawModes::POINTS);
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  // Block if we do not have any vertices
  if ( polyline_.n_vertices() == 0 ) 
    return;

  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_TEXTURE_2D);
  
  // Bind the vertex array
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::vertexPointer( &(polyline_.points())[0] );   
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::disableClientState(GL_COLOR_ARRAY);

  // draw points
  if (_drawMode & DrawModes::POINTS)
  {
   
    // draw selection
    if( polyline_.vertex_selections_available())
    {
      // save old values
      Vec4f base_color_old = _state.base_color();
      float point_size_old = _state.point_size();
      _state.set_base_color(Vec4f(1,0,0,1));
      _state.set_point_size(point_size_old+4);

      glBegin(GL_POINTS);
      for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
        if( polyline_.vertex_selection(i))
          glArrayElement(i);
      glEnd();

      _state.set_base_color(base_color_old);
      _state.set_point_size(point_size_old);
    }

    // Draw all vertices (don't care about selection
    glDrawArrays(GL_POINTS,0,polyline_.n_vertices());
    
  }


  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
    // draw selection
    if( polyline_.edge_selections_available())
    {
      // save old values
      Vec4f base_color_old = _state.base_color();
      float line_width_old = _state.line_width();
      _state.set_base_color(Vec4f(1,0,0,1));
      _state.set_line_width(2*line_width_old);

      glBegin(GL_LINES);
      // draw possibly closed PolyLine
      for (unsigned int i=0; i< polyline_.n_edges(); ++i)
      {
	if( polyline_.edge_selection(i))
	{
          glArrayElement( i    % polyline_.n_vertices()  );
          glArrayElement( (i+1) % polyline_.n_vertices() );
	}
      }
      glEnd();

      _state.set_base_color(base_color_old);
      _state.set_line_width(line_width_old);
    }

    // draw all line segments
    glBegin(GL_LINE_STRIP);
      // draw possibly closed PolyLine
      if( polyline_.n_vertices())
        glArrayElement(0);
      for (unsigned int i=0; i< polyline_.n_edges(); ++i)
        glArrayElement( (i+1) % polyline_.n_vertices() );
    glEnd();
  }
  
  // draw normals
  if (polyline_.vertex_normals_available())
  {
    double avg_len = polyline_.n_edges() > 0 ? (polyline_.length() / polyline_.n_edges() * 0.75) : 0;
    std::vector<Point> ps;
    for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
    {
      ps.push_back(polyline_.point(i));
      ps.push_back(polyline_.point(i)+polyline_.vertex_normal(i)*avg_len);
      if(polyline_.vertex_binormals_available())
	ps.push_back(polyline_.point(i)+polyline_.vertex_binormal(i)*avg_len);
    }
    ACG::GLState::vertexPointer( &ps[0] );   
  
    Vec4f base_color_old = _state.base_color();
    float line_width_old = _state.line_width();
    _state.set_base_color(Vec4f(0.8,0,0,1));
    _state.set_line_width(1);

    int stride = polyline_.vertex_binormals_available() ? 3 : 2;
    glBegin(GL_LINES);
    for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
    {
      glArrayElement( stride*i );
      glArrayElement( stride*i+1 );
    }
    glEnd();
    
    if (polyline_.vertex_binormals_available())
    {
      _state.set_base_color(Vec4f(0,0,0.8,1));
      glBegin(GL_LINES);
      for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
      {
	glArrayElement( stride*i );
	glArrayElement( stride*i+2 );
      }
      glEnd();
    }

    _state.set_base_color(base_color_old);
    _state.set_line_width(line_width_old);
  }
  
  //Disable the vertex array
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
}

//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick(GLState& _state, PickTarget _target)
{
  if (  polyline_.n_vertices() == 0 ) 
    return;
  
  // Bind the vertex array
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::vertexPointer( &(polyline_.points())[0] );   
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
  
  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;
  
  // (draw lines slightly in front of everything else)
  //disabled right now because of rendering artifacts.
  //This probably doesn't make sense anyways since the lines are drawn as cylinders and therefore have a width
  // glDepthRange(0.0,0.99)
  
  switch (_target)
  {
    case PICK_VERTEX:
    {
      _state.pick_set_maximum (polyline_.n_vertices());
      pick_vertices( _state);
      break;
    }

    case PICK_EDGE:
    {
      _state.pick_set_maximum (n_end);
      pick_edges(_state, 0);
      break;
    }

    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (polyline_.n_vertices() + n_end);
      pick_vertices( _state);
      pick_edges( _state, polyline_.n_vertices());
      break;
    }

    default:
      break;
  }
   
  //see above
  // glDepthRange(0.0,1.0)
  
  //Disable the vertex array
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_vertices( GLState& _state )
{
  float point_size_old = _state.point_size();
  glPointSize(18);

  _state.pick_set_name(0);


  glDepthRange(0.0, 0.999999);
  for (unsigned int i=0; i< polyline_.n_vertices(); ++i) {
    _state.pick_set_name (i);
    glBegin(GL_POINTS);
      glArrayElement( i );
    glEnd();
  }
  glDepthRange(0.0, 1.0);
  
  
  glPointSize(point_size_old);
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_edges( GLState& _state, unsigned int _offset)
{
  // Check if we have any edges to draw ( % 0 causes division by zero on windows)
  if ( polyline_.n_edges() == 0 )
    return;
  
  // save old values
  float line_width_old = _state.line_width();
  //  _state.set_line_width(2*line_width_old);
  _state.set_line_width(14);
  
  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;

  _state.pick_set_name (0);


  glDepthRange(0.0, 0.999999);
  for (unsigned int i=0; i<n_end; ++i) {
    _state.pick_set_name (i+_offset);
    glBegin(GL_LINES);
      glArrayElement( i     % polyline_.n_vertices() );
      glArrayElement( (i+1) % polyline_.n_vertices() );
    glEnd();
  }
  glDepthRange(0.0, 1.0);
  
  _state.set_line_width(line_width_old);
}



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

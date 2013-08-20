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
//  CLASS QtLasso - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


//ACG
#include "QtLasso.hh"
#include "QtColorTranslator.hh"
#include "../GL/gl.hh"

// stdc++
#include <list>
#include <vector>

#include <QMouseEvent>
#include <QEvent>
#include <QRgb>



//== NAMESPACES ===============================================================

namespace ACG {


//== IMPLEMENTATION ==========================================================



#define MASK(x,y) (mask_[(x)+(y)*mask_width_])


//-----------------------------------------------------------------------------


QtLasso::
QtLasso(GLState& _glstate) :
  glstate_(_glstate),
  mask_(0),
  mask_width_(0),
  mask_height_(0),
  is_active_(false)
{
}


//-----------------------------------------------------------------------------


QtLasso::
~QtLasso()
{
  free_mask();
}


//-----------------------------------------------------------------------------


void
QtLasso::
slotMouseEvent(QMouseEvent* _event)
{
  bool emit_signal = false;


  // setup 2D projection
  unsigned int width  = glstate_.viewport_width();
  unsigned int height = glstate_.viewport_height();

  ACG::GLState::drawBuffer( GL_FRONT );

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width-1, 0, height-1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  ACG::GLState::disable(GL_DEPTH_TEST);
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_DITHER);
  glLineWidth(2.0);
  glColor3ub(0, 255, 10);

  glFinish();



  // process mouse event
  switch(_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      Vec2i p(_event->pos().x(), height-_event->pos().y()-1);

      // initialize
      if (!is_active())
      {
        is_active_ = true;
        first_point_ = last_point_ = p;
      }

      // draw line
      glBegin(GL_LINES);
      glVertex(last_point_);
      glVertex(p);
      glEnd();
      last_point_ = rubberband_point_ = p;
      break;
    }


    case QEvent::MouseMove:
    {
      if (is_active())
      {
        Vec2i p(_event->pos().x(), height-_event->pos().y());

        // draw freehand
        if (_event->modifiers() & Qt::LeftButton)
        {
          glBegin(GL_LINES);
          glVertex(last_point_);
          glVertex(p);
          glEnd();
          last_point_ = rubberband_point_ = p;
        }

        // draw rubber band
        else
        {
          ACG::GLState::enable(GL_COLOR_LOGIC_OP);
          glLogicOp(GL_XOR);
          glBegin(GL_LINES);
          glVertex(last_point_);
          glVertex(rubberband_point_);
          glVertex(last_point_);
          glVertex(p);
          glEnd();
          ACG::GLState::disable(GL_COLOR_LOGIC_OP);
          rubberband_point_ = p;
        }
      }
      break;
    }


    case QEvent::MouseButtonDblClick:
    {
      if (is_active())
      {
        // close polygon
        glBegin(GL_LINES);
        glVertex(last_point_);
        glVertex(first_point_);
        glEnd();
        glFinish();


        // mark reference point (0,0) with border color
        glPointSize(1.0);
        glBegin(GL_POINTS);
        glVertex2i(0, 0);
        glEnd();
        glPointSize(glstate_.point_size());


        // create mask and precompute matrix
        create_mask();
        is_active_ = false;

        emit_signal = true;
      }
      break;
    }


    default: // avoid warning
      break;
  }


  // restore GL settings
  ACG::GLState::drawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);

  glLineWidth(glstate_.line_width());
  glColor4fv(glstate_.base_color().data());
  ACG::GLState::enable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION );
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glFinish();



  // emit signal
  if (emit_signal)
  {
    if (_event->modifiers() & Qt::ShiftModifier)
      emit(signalLassoSelection(AddToSelection));

    else if (_event->modifiers() & Qt::ControlModifier)
      emit(signalLassoSelection(DelFromSelection));

    else
      emit(signalLassoSelection(NewSelection));
  }
}


//-----------------------------------------------------------------------------


void
QtLasso::
create_mask()
{
  unsigned int  x, y, xx, yy, i;
  GLubyte       *fbuffer;
  QRgb          rgb, borderRgb;


  // GL context
  const unsigned int w = glstate_.viewport_width();
  const unsigned int h = glstate_.viewport_height();
  const unsigned int size = w*h;


  // alloc mask
  free_mask();
  mask_ = new unsigned char[size];
  mask_width_ = w;
  mask_height_ = h;


  // alloc framebuffer
  fbuffer = new GLubyte[3*size];
  assert( fbuffer );


  // read framebuffer
  glReadBuffer( GL_FRONT );
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, fbuffer);
  glReadBuffer( GL_BACK );


  // read lasso color
  borderRgb = qRgb( fbuffer[0], fbuffer[1], fbuffer[2] );
  fbuffer[0] = fbuffer[1] = fbuffer[2] = 0;


  // mark lasso pixels
  for (y = 0; y < h; ++y)
  {
    const unsigned int offset = y*w;
    for (x = 0; x < w; ++x)
    {
      i = 3*(offset + x);
      rgb = qRgb(fbuffer[i], fbuffer[i+1], fbuffer[i+2]);
      mask_[offset+x] = (rgb == borderRgb) ? 3 : 1;
    }
  }


  // seed fill
  std::vector<Vec2i> toDo;
  toDo.reserve(w*h);
  toDo.push_back(Vec2i(0,0));

  while (!toDo.empty())
  {
    Vec2i p = toDo.back();
    toDo.pop_back();

    x = p[0];
    y = p[1];
    unsigned char &s = MASK(x, y);

    if (s != 3)
    {
      s = 0;

      xx = x-1; yy = y;
      if ((xx<w) && (MASK(xx,yy)==1))
      {
        toDo.push_back(Vec2i(xx,yy));
        MASK(xx,yy) = 2;
      }

      xx = x+1; yy = y;
      if ((xx<w) && (MASK(xx,yy)==1))
      {
        toDo.push_back(Vec2i(xx,yy));
        MASK(xx,yy) = 2;
      }

      xx = x; yy = y-1;
      if ((yy<h) && (MASK(xx,yy)==1))
      {
        toDo.push_back(Vec2i(xx,yy));
        MASK(xx,yy) = 2;
      }

      xx = x; yy = y+1;
      if ((yy<h) && (MASK(xx,yy)==1))
      {
        toDo.push_back(Vec2i(xx,yy));
        MASK(xx,yy) = 2;
      }
    }
  }


  // free
  delete[] fbuffer;
}


//-----------------------------------------------------------------------------


void
QtLasso::
free_mask()
{
  if (mask_)
  {
    delete[] mask_;
    mask_ = 0;
    mask_width_ = mask_height_ = 0;
  }
}


//-----------------------------------------------------------------------------


bool
QtLasso::
is_vertex_selected(const Vec3d& _v)
{
  unsigned int  x, y, w, h;


  // size changed?
  w = glstate_.viewport_width();
  h = glstate_.viewport_height();
  if ((w != mask_width_) || (h != mask_height_))
  {
    std::cerr << "Lasso: viewport size has changed.\n";
    return false;
  }


  // project vertex to 2D integer coordinates
  Vec3d v = glstate_.project(_v);
  x = (unsigned int)(v[0] + 0.5);
  y = (unsigned int)(v[1] + 0.5);


  // near and far plane clipping
  if (v[2] < 0.0 || v[2] > 1.0)
    return false;


  // visible ?
  return ((v[2]>0.0) && (x<w) && (y<h) && (MASK(x,y)));
}


//=============================================================================
} // namespace ACG
//=============================================================================

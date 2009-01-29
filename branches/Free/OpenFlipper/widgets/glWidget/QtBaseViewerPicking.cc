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
//   $Revision: 3597 $
//   $Author: moebius $
//   $Date: 2008-10-28 17:11:26 +0100 (Di, 28. Okt 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include "QtGLGraphicsScene.hh"
#include "QtGLGraphicsView.hh"

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ==========================================================

static const unsigned int  SELECTION_BUFFER_SIZE = 10000;
static const unsigned int  NAME_STACK_SIZE       = 2;

//== IMPLEMENTATION ==========================================================


void glViewer::renderPicking(bool _renderPicking, ACG::SceneGraph::PickTarget _mode) {
  renderPicking_    = _renderPicking;
  pickRendererMode_ = _mode;
}

//-----------------------------------------------------------------------------


bool glViewer::pick( ACG::SceneGraph::PickTarget _pickTarget,
                         const QPoint&               _mousePos,
                         unsigned int&               _nodeIdx,
                         unsigned int&               _targetIdx,
                         ACG::Vec3d*                 _hitPointPtr )
{
  if (sceneGraphRoot_)
  {
    GLint         w = glWidth(),
                  h = glHeight(),
                  x = _mousePos.x(),
                  y = h - _mousePos.y();
    GLint         viewport[4] = {0,0,w,h};
    GLuint        selectionBuffer[ SELECTION_BUFFER_SIZE ],
                  nameBuffer[ NAME_STACK_SIZE ];

    const ACG::GLMatrixd&  modelview  = glstate_->modelview();
    const ACG::GLMatrixd&  projection = glstate_->projection();


    // prepare GL state
    makeCurrent();

    glSelectBuffer( SELECTION_BUFFER_SIZE, selectionBuffer );
    glRenderMode(GL_SELECT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix((GLdouble) x, (GLdouble) y, 3, 3, viewport);
    glMultMatrixd(projection.get_raw_data());
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(modelview.get_raw_data());
    glDisable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);
    glInitNames();
    glPushName((GLuint) 0);

    // do the picking
    ACG::SceneGraph::PickAction action(_pickTarget);
    ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_);
    int hits = glRenderMode(GL_RENDER);

    // restore GL state
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixd(projection.get_raw_data());
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd(modelview.get_raw_data());
    glEnable(GL_LIGHTING);


    // process hit record
    if ( hits > 0 )
    {
      GLuint *ptr = selectionBuffer,
      num_names,
      z,
      min_z=~(0u),
      max_z=0;

      for (int i=0; i<hits; ++i)
      {
        num_names = *ptr++;
        if ( num_names != NAME_STACK_SIZE )
        {
          std::cerr << "glViewer::pick() : namestack error\n\n";
          return false;
        }

        if ( (z = *ptr++) < min_z )
        {
          min_z = z;
          max_z = *ptr++;
          for (unsigned int j=0; j<num_names; ++j)
            nameBuffer[j] = *ptr++;
        }
        else ptr += 1+num_names;
      }

      _nodeIdx   = nameBuffer[0];
      _targetIdx = nameBuffer[1];

      if (_hitPointPtr)
      {
        GLuint zscale=~(0u);
        GLdouble min_zz = ((GLdouble)min_z) / ((GLdouble)zscale);
        GLdouble max_zz = ((GLdouble)max_z) / ((GLdouble)zscale);
        GLdouble zz     = 0.5F * (min_zz + max_zz);
        *_hitPointPtr = glstate_->unproject(ACG::Vec3d(x,y,zz));
      }

      return true;
    }
    else if (hits < 0)
      std::cerr << "glViewer::pick() : selection buffer overflow\n\n";
  }

  return false;
}


//-----------------------------------------------------------------------------


bool
glViewer::
fast_pick( const QPoint&  _mousePos,
           ACG::Vec3d&    _hitPoint )
{
  // get x,y,z values of pixel
  GLint     x(_mousePos.x()), y(glHeight() - _mousePos.y());
  GLfloat   z;


  makeCurrent();
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);


  if (z < 0.99999)
  {
    _hitPoint = glstate_->unproject( ACG::Vec3d(x, y, z) );
    return true;
  }
  else return false;
}

//-----------------------------------------------------------------------------


void glViewer::pickMode( int _id )
{
  if (_id < (int) pick_modes_.size() )
  {
    pick_mode_idx_  = _id;
    pick_mode_name_ = pick_modes_[pick_mode_idx_].name;

    // adjust mouse tracking
    if ( properties_.pickingMode() )
      trackMouse(pick_modes_[pick_mode_idx_].tracking);

    // adjust Cursor
    if ( properties_.pickingMode() )
      glView_->setCursor( pick_modes_[pick_mode_idx_].cursor);

    // emit signal
    emit(signalPickModeChanged(pick_mode_name_));
  }
}


//-----------------------------------------------------------------------------


void glViewer::addPickMode(const std::string& _name,
                               bool _tracking,
                               int  _pos,
                               bool _visible,
                               QCursor _cursor)
{
  if ((unsigned int)_pos < pick_modes_.size())
  {
    std::vector<PickMode>::iterator it = pick_modes_.begin();
    it += _pos+1;
    pick_modes_.insert(it, PickMode(_name, _tracking, _visible, _cursor));
  }
  else
    pick_modes_.push_back(PickMode(_name, _tracking, _visible, _cursor));

  updatePickMenu();
}

//-----------------------------------------------------------------------------

void glViewer::setPickModeCursor(const std::string& _name, QCursor _cursor)
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _name ){
      pick_modes_[i].cursor = _cursor;

      //switch cursor if pickMode is active
      if (pick_mode_name_ == _name && properties_.pickingMode() )
        glView_->setCursor(_cursor);
      break;
    }
}

//-----------------------------------------------------------------------------

void glViewer::setPickModeMouseTracking(const std::string& _name, bool _mouseTracking)
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _name ){
      pick_modes_[i].tracking = _mouseTracking;

      //switch cursor if pickMode is active
      if (pick_mode_name_ == _name && properties_.pickingMode() )
        trackMouse(_mouseTracking);
      break;
    }
}

//-----------------------------------------------------------------------------


void glViewer::clearPickModes()
{
  pick_modes_.clear();
  pick_mode_idx_  = -1;
  pick_mode_name_ = "";
  updatePickMenu();
}


//-----------------------------------------------------------------------------


const std::string& glViewer::pickMode() const
{
  return pick_mode_name_;
}


//-----------------------------------------------------------------------------


void glViewer::pickMode(const std::string& _name)
{
  for (unsigned int i=0; i<pick_modes_.size(); ++i)
  {
    if (pick_modes_[i].name == _name)
    {
      pickMode( i );
      updatePickMenu();
      return;
    }
  }
}



//=============================================================================

//=============================================================================

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


bool glViewer::pick( ACG::SceneGraph::PickTarget _pickTarget,
                     const QPoint&               _mousePos,
                     unsigned int&               _nodeIdx,
                     unsigned int&               _targetIdx,
                     ACG::Vec3d*                 _hitPointPtr )
{
  if (sceneGraphRoot_)
  {
    // unsigned int node, target;
    // QTime time;
    // time.start ();
    int rv = pickColor (_pickTarget, _mousePos, _nodeIdx, _targetIdx, _hitPointPtr);

    // printf ("ColorPicking took %d msec\n",time.restart ());
    // rv = -1;
    // node = _nodeIdx;
    // target = _targetIdx;
    if (rv < 0)
      rv = pickGL (_pickTarget, _mousePos, _nodeIdx, _targetIdx, _hitPointPtr);
    // printf ("GLPicking took %d msec\n",time.restart ());

    // if (rv > 0 && (node != _nodeIdx || target != _targetIdx))
    //   printf ("***** Picking difference Color %d/%d GL %d/%d\n",node, target, _nodeIdx, _targetIdx);
    if (rv > 0)
      return rv;
  }
  return false;
}


//-----------------------------------------------------------------------------

int glViewer::pickColor( ACG::SceneGraph::PickTarget _pickTarget,
                         const QPoint&               _mousePos,
                         unsigned int&               _nodeIdx,
                         unsigned int&               _targetIdx,
                         ACG::Vec3d*                 _hitPointPtr )
{

  GLint         w = glWidth(),
                h = glHeight(),
                l = scenePos().x(),
                b = scene()->height () - scenePos().y() - h,
                x = _mousePos.x(),
                y = scene()->height () - _mousePos.y();
  GLubyte       pixels[9][4];
  GLfloat       depths[9];
  int           hit = -1;
  unsigned char order[9] = { 4, 7, 1, 3, 5, 0, 2, 6, 8 };

  const ACG::GLMatrixd&  modelview  = properties_.glState().modelview();
  const ACG::GLMatrixd&  projection = properties_.glState().projection();

  ACG::Vec4f clear_color = properties_.glState().clear_color();
  properties_.glState().set_clear_color (ACG::Vec4f (0.0, 0.0, 0.0, 0.0));

  // prepare GL state
  makeCurrent();

  glViewport (l, b, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMultMatrixd(projection.get_raw_data());
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(modelview.get_raw_data());
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  properties_.glState().pick_init (true);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse(sceneGraphRoot_, action);

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  glEnable(GL_LIGHTING);

  properties_.glState().set_clear_color (clear_color);

  if (properties_.glState().pick_error ())
    return -1;

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glReadPixels (x - 1, y - 1, 3, 3, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glReadPixels (x - 1, y - 1, 3, 3, GL_DEPTH_COMPONENT, GL_FLOAT, depths);

  for (int i = 0; i < 9; i++)
  {
    if (hit < 0 && (pixels[order[i]][2] != 0 || pixels[order[i]][1] != 0 || pixels[order[i]][0] != 0 || pixels[order[i]][3] != 0))
    {
      hit = order[i];
      break;
    }
  }

  if (hit < 0)
    return 0;


  ACG::Vec4uc rgba;
  rgba[0] = pixels[hit][0];
  rgba[1] = pixels[hit][1];
  rgba[2] = pixels[hit][2];
  rgba[3] = pixels[hit][3];

  std::vector<unsigned int> rv = properties_.glState().pick_color_to_stack (rgba);

  // something wrong with the color stack ?
  if (rv.size () < 2)
    return -1;

  _nodeIdx   = rv[1];
  _targetIdx = rv[0];

  if (_hitPointPtr)
  {
    *_hitPointPtr = properties_.glState().unproject(ACG::Vec3d(x,y,depths[hit]));
  }

  return 1;
}

//-----------------------------------------------------------------------------

bool glViewer::pickGL( ACG::SceneGraph::PickTarget _pickTarget,
                       const QPoint&               _mousePos,
                       unsigned int&               _nodeIdx,
                       unsigned int&               _targetIdx,
                       ACG::Vec3d*                 _hitPointPtr )
{
  GLint         w = glWidth(),
                h = glHeight(),
                l = scenePos().x(),
                b = scene()->height () - scenePos().y() - h,
                x = _mousePos.x(),
                y = scene()->height () - _mousePos.y();
  GLint         viewport[4] = {l,b,w,h};
  GLuint        selectionBuffer[ SELECTION_BUFFER_SIZE ],
                nameBuffer[ NAME_STACK_SIZE ];

  const ACG::GLMatrixd&  modelview  = properties_.glState().modelview();
  const ACG::GLMatrixd&  projection = properties_.glState().projection();

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
  glDisable(GL_BLEND);
  glClear(GL_DEPTH_BUFFER_BIT);
  properties_.glState().pick_init (false);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse(sceneGraphRoot_, action);
  int hits = glRenderMode(GL_RENDER);

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);

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
      *_hitPointPtr = properties_.glState().unproject(ACG::Vec3d(x,y,zz));
    }

    return true;
  }
  else if (hits < 0)
    std::cerr << "glViewer::pick() : selection buffer overflow\n\n";

  return false;
}

//-----------------------------------------------------------------------------

bool glViewer::pick_region( ACG::SceneGraph::PickTarget                _pickTarget,
                            const QRegion&                             _region,
                            QList<QPair<unsigned int, unsigned int> >& _list)
{
  GLint    w = glWidth(),
           h = glHeight(),
           l = scenePos().x(),
           b = scene()->height () - scenePos().y() - h;
  GLubyte* buffer;
  QRect    rect = _region.boundingRect();


  const ACG::GLMatrixd&  modelview  = properties_.glState().modelview();
  const ACG::GLMatrixd&  projection = properties_.glState().projection();

  ACG::Vec4f clear_color = properties_.glState().clear_color();
  properties_.glState().set_clear_color (ACG::Vec4f (0.0, 0.0, 0.0, 0.0));

  // prepare GL state
  makeCurrent();

  glViewport (l, b, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMultMatrixd(projection.get_raw_data());
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(modelview.get_raw_data());
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  properties_.glState().pick_init (true);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse(sceneGraphRoot_, action);

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);

  properties_.glState().set_clear_color (clear_color);

  if (properties_.glState().pick_error ())
    return false;

  buffer = new GLubyte[4 * rect.width() * rect.height()];

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glReadPixels (rect.x(), scene()->height () - rect.bottom() , rect.width(),
                rect.height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  QSet<QPair<unsigned int, unsigned int> > found;

  for (int y = 0; y < rect.height (); y++)
    for (int x = 0; x < rect.width (); x++)
    {
      if (_region.contains (QPoint (rect.x() + x, rect.y() + y)))
      {
        int bPos = (((rect.height () - (y + 1)) * rect.width ()) + x) * 4;
        if (buffer[bPos + 2] != 0 || buffer[bPos + 1] != 0 || buffer[bPos] != 0 || buffer[bPos + 3] != 0)
        {
          ACG::Vec4uc rgba;
          rgba[0] = buffer[bPos];
          rgba[1] = buffer[bPos + 1];
          rgba[2] = buffer[bPos + 2];
          rgba[3] = buffer[bPos + 3];

          std::vector<unsigned int> rv = properties_.glState().pick_color_to_stack (rgba);
          if (rv.size () < 2)
            continue;

          found << QPair<unsigned int, unsigned int>(rv[1], rv[0]);
        }
      }
    }

  delete buffer;
  _list = found.toList();

  return true;
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
    _hitPoint = properties_.glState().unproject( ACG::Vec3d(x, y, z) );
    return true;
  }
  else return false;
}


//=============================================================================

//=============================================================================

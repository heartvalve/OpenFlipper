/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include "QtGLGraphicsScene.hh"
#include "QtGLGraphicsView.hh"

#if QT_VERSION < 0x050000
  #include <QGLFramebufferObject>
#else // QT_VERSION > 0x050000
  #undef QT_NO_OPENGL
  #include <QOpenGLFramebufferObject>
  #define QT_NO_OPENGL
#endif //QT_VERSION < 0x050000

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
    int rv = pickFromCache (_pickTarget, _mousePos, _nodeIdx, _targetIdx, _hitPointPtr);

    // cache will return -1 if a update is needed or caching is not supported
    if (rv < 0)
      rv = pickColor (_pickTarget, _mousePos, _nodeIdx, _targetIdx, _hitPointPtr);

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
                y = scene()->height () - _mousePos.y(),
                pW = 1,
                pH = 1;
  GLubyte       pixels[9][4];
  GLfloat       depths[9];
  int           hit = -1;

  // traversing order (center, top, bottom, ...)
  unsigned char order[9] = { 4, 7, 1, 3, 5, 0, 2, 6, 8 };

  if (pickCacheSupported_)
  {
    // delete pick cache if the size changed
    if (pickCache_ && pickCache_->size () != QSize (glWidth (), glHeight ()))
    {
      delete pickCache_;
      pickCache_ = NULL;
    }
    // create a new pick cache frambuffer object
    if (!pickCache_)
    {
      pickCache_ = new QFramebufferObject (glWidth (), glHeight (), QFramebufferObject::Depth);
      if (!pickCache_->isValid ())
      {
        pickCacheSupported_ = false;
        delete pickCache_;
        pickCache_ = NULL;
      }
    }
    if (pickCache_)
    {
      // the viewport for the framebuffer object
      l = 0;
      b = 0;
      x = _mousePos.x() - scenePos().x();
      y = glHeight() - (_mousePos.y() - scenePos().y());

      // we can only pick inside of our window
      if (x < 0 || y < 0 || x >= (int)glWidth() || y >= (int)glHeight())
        return 0;

      pickCache_->bind ();
    }
  }

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
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_BLEND);
  ACG::GLState::enable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  properties_.glState().pick_init (true);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action,properties_.glState() );

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  ACG::GLState::enable(GL_LIGHTING);

  properties_.glState().set_clear_color (clear_color);

  if (properties_.glState().pick_error ())
  {
    if (pickCache_ && pickCache_->isBound ())
      pickCache_->release ();
    return -1;
  }

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // we can only read inside our viewport
  if (x + 1 < w)
    pW++;

  if (y + 1 < h)
    pH++;

  if (x > 0)
  {
    x--;
    pW++;
  }
  if (y > 0)
  {
    y--;
    pH++;
  }

  if (pH != 3 || pW != 3)
  {
    // initialize unused values with 0
    for (int i = 0; i < 9; i++)
    {
      pixels[i][0] = 0;
      pixels[i][1] = 0;
      pixels[i][2] = 0;
      pixels[i][3] = 0;
      depths[i] = 0.0;
    }
  }

  // read from framebuffer
  glReadPixels (x, y, pW, pH, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glReadPixels (x, y, pW, pH, GL_DEPTH_COMPONENT, GL_FLOAT, depths);

  // unbind pick cache
  if (pickCache_ && pickCache_->isBound ())
  {
    pickCache_->release ();
    updatePickCache_ = false;
    pickCacheTarget_ = _pickTarget;
  }

  // get first found pixel
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

//   // Debug Code to visualize picking cache ( DO NOT REMOVE!!!! Jan )
//   QImage murks(glWidth (),glHeight (),QImage::Format_ARGB32);
//   murks = pickCache_->toImage();
//   for ( int i = 0 ; i < glWidth() ; ++i ) 
//     for ( int j = 0 ; j < glHeight() ; ++j )  {
//       QColor bla (murks.pixel(i,j));
//       bla.setAlpha(255);
//       murks.setPixel(i,j,bla.rgba());
//     }
//   murks.save("murks.png");

  if (_hitPointPtr)
  {
    *_hitPointPtr = properties_.glState().unproject (
      ACG::Vec3d(_mousePos.x(), scene()->height () - _mousePos.y(),depths[hit]));
  }

  return 1;
}

//-----------------------------------------------------------------------------

int glViewer::pickFromCache( ACG::SceneGraph::PickTarget _pickTarget,
                             const QPoint&               _mousePos,
                             unsigned int&               _nodeIdx,
                             unsigned int&               _targetIdx,
                             ACG::Vec3d*                 _hitPointPtr )
{
  // do we need an update?
  if (!pickCacheSupported_ || updatePickCache_ || !pickCache_ ||
      pickCacheTarget_ != _pickTarget)
    return -1;

  GLint         x = _mousePos.x() - scenePos().x(),
                y = glHeight() - (_mousePos.y() - scenePos().y()),
                pW = 1,
                pH = 1;
  GLubyte       pixels[9][4];
  GLfloat       depths[9];
  int           hit = -1;

  // traversing order (center, top, bottom, ...)
  unsigned char order[9] = { 4, 7, 1, 3, 5, 0, 2, 6, 8 };

  // can't pick outside
  if (x < 0 || y < 0 || x >= (int)glWidth() || y >= (int)glHeight())
    return 0;

  // bind cache framebuffer object
  pickCache_->bind ();

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // we can only read inside our viewport
  if (x + 1 < (int)glWidth ())
    pW++;

  if (y + 1 < (int)glHeight ())
    pH++;

  if (x > 0)
  {
    x--;
    pW++;
  }
  if (y > 0)
  {
    y--;
    pH++;
  }

  if (pH != 3 || pW != 3)
  {
    // initialize unused values with 0
    for (int i = 0; i < 9; i++)
    {
      pixels[i][0] = 0;
      pixels[i][1] = 0;
      pixels[i][2] = 0;
      pixels[i][3] = 0;
      depths[i] = 0.0;
    }
  }

  // read from framebuffer
  glReadPixels (x, y, pW, pH, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glReadPixels (x, y, pW, pH, GL_DEPTH_COMPONENT, GL_FLOAT, depths);

  // unbind
  pickCache_->release ();

  // get first found pixel
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
    *_hitPointPtr = properties_.glState().unproject(
      ACG::Vec3d(_mousePos.x(), scene()->height () - _mousePos.y(),depths[hit]));
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

  // Initialize name buffer (This value is never used afterwards)
  nameBuffer[0] = 0;
  nameBuffer[1] = 0;

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
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_BLEND);
  glClear(GL_DEPTH_BUFFER_BIT);
  properties_.glState().pick_init (false);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action, properties_.glState());
  int hits = glRenderMode(GL_RENDER);

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::enable(GL_BLEND);

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
                            QList<QPair<unsigned int, unsigned int> >& _list,
                            QVector<float>*                            _depths,
                            QVector<ACG::Vec3d>*                       _points)
{
  QRect    rect = _region.boundingRect();
  GLint    w = glWidth(),
           h = glHeight(),
           l = scenePos().x(),
           b = scene()->height () - scenePos().y() - h,
           x = rect.x(),
           y = scene()->height () - rect.bottom();

  GLubyte* buffer = 0;
  GLfloat* depths = 0;

  if (pickCacheSupported_)
  {
    // delete pick cache if the size changed
    if (pickCache_ && pickCache_->size () != QSize (glWidth (), glHeight ()))
    {
      delete pickCache_;
      pickCache_ = NULL;
    }
    // create a new pick cache frambuffer object
    if (!pickCache_)
    {
      pickCache_ = new QFramebufferObject (glWidth (), glHeight (), QFramebufferObject::Depth);
      if (!pickCache_->isValid ())
      {
        pickCacheSupported_ = false;
        delete pickCache_;
        pickCache_ = NULL;
      }
    }
    if (pickCache_)
    {
      // the viewport for the framebuffer object
      l = 0;
      b = 0;
      x = rect.x() - scenePos().x();
      y = glHeight() - (rect.bottom() - scenePos().y());

      // we can only pick inside of our window
      if (x < 0 || y < 0 || x >= (int)glWidth() || y >= (int)glHeight())
        return 0;

      pickCache_->bind ();
    }
  }

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
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_BLEND);
  ACG::GLState::enable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  properties_.glState().pick_init (true);

  // do the picking
  ACG::SceneGraph::PickAction action(properties_.glState(), _pickTarget, properties_.drawMode());
  ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action,properties_.glState());

  // restore GL state
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd(projection.get_raw_data());
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd(modelview.get_raw_data());
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::enable(GL_BLEND);

  properties_.glState().set_clear_color(clear_color);

  if (properties_.glState().pick_error ())
  {
    if (pickCache_ && pickCache_->isBound ())
      pickCache_->release ();
    return false;
  }

  buffer = new GLubyte[4 * rect.width() * rect.height()];



  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glReadPixels (x, y, rect.width(),
                rect.height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  if (_depths || _points ) {
    depths = new GLfloat[ rect.width() * rect.height() ];
    glReadPixels (x, y, rect.width(), rect.height(), GL_DEPTH_COMPONENT, GL_FLOAT, depths);

    /*  Debug code, writing out the depth image
    QImage depthmapimage(rect.width(), rect.height(), QImage::Format_Indexed8);

    // color map
    for ( int i = 0 ; i <= 255 ; i++ )
      depthmapimage.setColor( i, qRgb( i, i, i ) );

    for ( int i = 0 ; i < rect.width() ; i++ )
      for ( int j = 0 ; j < rect.height() ; j++ )
      {
        depthmapimage.setPixel(i,rect.height()-j-1, (unsigned int)(depths[j*rect.width()+i]*255));
      }

    depthmapimage.save("test.png");
    */
  }

  // Iterate over the bounding rectangle of the region
  for (int y = 0; y < rect.height (); y++)
    for (int x = 0; x < rect.width (); x++)
    {

      // Check if the current point is in the polygon of the region
      if (_region.contains (QPoint (rect.x() + x, rect.y() + y)))
      {

        // Calculate position inside the buffer
        const int bPos = (((rect.height () - (y + 1)) * rect.width ()) + x) * 4;

        // Get the picking color from the buffer at the current position
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

          QPair<unsigned int, unsigned int> curr(rv[1], rv[0]);
          
          // added a new (targetidx/nodeidx) pair
          if( !_list.contains(curr))
          {
            _list << curr;

            if ( _depths  || _points ) {

              const double curr_depth(depths[(rect.height()-(y+1))*rect.width() + x]);

              // If depths should be returned, we extract it here
              if (_depths)
                (*_depths) << curr_depth;

              // unproject depth to real (3D) depth value
              if ( _points )
                (*_points) << properties_.glState().unproject(ACG::Vec3d(x+rect.x(),h-(y+rect.y()),curr_depth));
            }
          }
        }
      }
    }

  delete[] buffer;

  if ( _depths  || _points )
    delete[] depths;

  // unbind pick cache
  if (pickCache_ && pickCache_->isBound ())
  {
    pickCache_->release ();
    updatePickCache_ = false;
    pickCacheTarget_ = _pickTarget;
  }

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

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
//   $Revision: 5979 $
//   $Author: moebius $
//   $Date: 2009-05-07 10:57:06 +0200 (Do, 07. Mai 2009) $
//
//=============================================================================



#ifndef SIMPLEGLGRAPHICSSCENE_HH
#define SIMPLEGLGRAPHICSSCENE_HH

//=============================================================================
//
//  CLASS SimpleGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <QGraphicsScene>
#include <QGraphicsItem>

//== FORWARDDECLARATIONS ======================================================
class glViewer;

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================



/** OpenGL drawing area and widget scene -- for \a internal use only.
    The scene basically redirects calls to a
    ACG::QtWidgets::glViewer, the corresponding virtual methods there
    are prefixed with \c gl.
    \sa ACG::QtWidgets::glViewer
*/

class DLLEXPORT SimpleGLGraphicsScene : public QGraphicsScene
{
Q_OBJECT

public:
  SimpleGLGraphicsScene();

  void setView (glViewer *_view);

protected:

  virtual void drawBackground(QPainter *_painter, const QRectF &_rect);

  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _e);

private:

  glViewer *view_;

  bool initialized_;
};

#endif

//=============================================================================
//=============================================================================

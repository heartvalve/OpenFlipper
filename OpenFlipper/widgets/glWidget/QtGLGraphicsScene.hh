/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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



#ifndef QTGLGRAPHICSSCENE_HH
#define QTGLGRAPHICSSCENE_HH

//=============================================================================
//
//  CLASS QtGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "QtBaseViewer.hh"
#include "CursorPainter.hh"

//== FORWARDDECLARATIONS ======================================================
class glViewer;
class QtMultiViewLayout;
class CursorPainter;

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================



/** OpenGL drawing area and widget scene -- for \a internal use only.
    The scene basically redirects calls to a
    ACG::QtWidgets::glViewer, the corresponding virtual methods there
    are prefixed with \c gl.
    \sa ACG::QtWidgets::glViewer
*/

class DLLEXPORT QtGLGraphicsScene : public QGraphicsScene
{
Q_OBJECT

public:
  QtGLGraphicsScene(std::vector< glViewer *> *_views, QtMultiViewLayout *_layout);

  void setCursorPainter (CursorPainter *_cursorPainter);
  
protected:

  virtual void drawBackground(QPainter *_painter, const QRectF &_rect);

  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _e);

  // Catch enter and leave events for 3d cursor
  virtual bool event (QEvent *_event);

private:

  glViewer* findView (const QPointF &_p, bool _setActive = false);

  std::vector< glViewer *> *views_;
  QtMultiViewLayout *layout_;

  CursorPainter *cursorPainter_;
};

#endif

//=============================================================================
//=============================================================================

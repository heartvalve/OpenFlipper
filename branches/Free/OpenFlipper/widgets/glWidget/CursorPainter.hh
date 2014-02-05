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

#ifndef CURSORPAINTER_HH
#define CURSORPAINTER_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QCursor>
#include <QVector>
#include <QRectF>

#include <ACG/GL/GLState.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

//== NAMESPACES ===============================================================

//== FORWARDDECLARATIONS ======================================================
class glViewer;

//== CLASS DEFINITION =========================================================


/** Class that paints the cursor using gl calls
*/

class DLLEXPORT CursorPainter : public QObject
{
 Q_OBJECT

  public:
    /// Constructor
    CursorPainter (QObject *_parent = 0);

    /// Destructor
    ~CursorPainter ();

    /// Sets the current used cursor
    void setCursor (const QCursor &_cursor);

    /// Needs to be called after the gl context has been set up to initialize internal values
    void initializeGL ();

    /// Add a glViewer that will use this CursorPainter
    void registerViewer (glViewer *_viewer);

    /// Cursor painting function. The _state has to be setup that 0,0,0 is at the cursor position.
    void paintCursor (ACG::GLState *_state);

    /// Sets the current cursor position
    void updateCursorPosition (QPointF _scenePos);

    /// Return the current cursor position
    QPointF cursorPosition ();

    /// Enabled/Disables gl cursor painting
    void setEnabled (bool _enabled);

    /// Enabled/Disables native cursors
    void setForceNative (bool _enabled);

    /// Returns true if cursor painting is enabled and compatible cursor is set
    bool enabled ();

    /// Inform the cursor painter about mouse enter / leave
    void setMouseIn (bool _in);

    /// Bounding box of the cursor
    QRectF cursorBoundingBox ();

  private:
    // Create a texture for the cursor
    void cursorToTexture ();
    void cursorToCursor ();

  private:
    QCursor cursor_;
    QCursor nativeCursor_;
    QPointF cursorPos_;

    bool initialized_;

    QVector<glViewer *> views_;

    bool enabled_;
    bool mouseIn_;
    bool forceNative_;

    // x and y offset between click position and texture (hotspot)
    int xOff_;
    int yOff_;
    GLuint texture_;
    bool hasCursor_;
};

//=============================================================================
#endif
//=============================================================================

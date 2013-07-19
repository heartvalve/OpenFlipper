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


#ifndef SIMPLEVIEWER_HH
#define SIMPLEVIEWER_HH

//=============================================================================
//
//  CLASS simpleViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/ViewerProperties.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <QGraphicsView>
#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL



//== NAMESPACES ===============================================================

//== FORWARDDECLARATIONS ======================================================
class SimpleGLGraphicsScene;
class glViewer;


//== CLASS DEFINITION =========================================================


/** Simple scene graph viewer. Plugins can use this class to display the scenegraph
  * in own widgets. SimpleViewer acts as a bridge between the normal Qt widget
  * system and the QtGraphicsWidget based glViewer (OpenFlipper/widgets/glWidget/QtBaseViewer.hh).
  * 
*/

class DLLEXPORT SimpleViewer : public QGraphicsView
{
 Q_OBJECT

  public:
    /// Constructor
    SimpleViewer(QWidget* _parent = NULL);

    /// Destructor
    ~SimpleViewer();

    /// Viewer properties
    Viewer::ViewerProperties& properties ();

    /// Viewer
    glViewer * viewer ();

  protected:
    // handle resizes
    virtual void resizeEvent(QResizeEvent *_event);

  private:
    // initalisation
    void initialize (const QGLFormat &_format, QGLWidget *_shareWidget = NULL);

  private slots:

    // action mode handling for ViewerProperties
    void setActionMode(const Viewer::ActionMode  _am);
    void getActionMode(Viewer::ActionMode& _am);

    // pick mode name handling for ViewerProperties
    void getPickMode(std::string& _name);

  private:
    QGLWidget *glWidget_;

    Viewer::ViewerProperties props_;

    SimpleGLGraphicsScene *scene_;
    glViewer *mainWidget_;

    Viewer::ActionMode actionMode_;
};

//=============================================================================

//=============================================================================
#endif

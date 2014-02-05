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

#ifndef BASEWIDGET_HH
#define BASEWIDGET_HH

//== INCLUDES =================================================================

#include <QMainWindow>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBox>
#include <QStack>

#include "scene/graphicsView.hh"
#include "scene/graphicsScene.hh"


//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Context;

//== CLASS DEFINITION =========================================================

/** Main widget for the visual scripting interface editor
*/
class BaseWidget : public QMainWindow
{
  Q_OBJECT

  public:

    /// Singleton constructor
    static BaseWidget *createBaseWidget (Context *_ctx, QWidget *_parent = NULL);
    static BaseWidget *getBaseWidget ();

    /// Destructor
    ~BaseWidget ();

  signals:
    void codeToScriptEditor (QString _code);

  public slots:

    /// executes current script
    void executeCode ();

    /// converts visual script to textual script
    void codeToScript ();

    /// save to file
    bool save (bool _newName = false);

    /// save to file
    bool saveAs ();

    /// load file
    void load ();

    /// new empty file
    void newFile ();

    /// used changed something
    void contentChanged ();

    /// show a new scene (function) in editor
    void pushScene (GraphicsScene *_scene);

    /// go back to last scene (function)
    void popScene ();

    /// add a new scene
    void addScene (GraphicsScene *_scene);

    /// remove a scene
    void removeScene (GraphicsScene *_scene);

  private:

    // constructor
    BaseWidget (Context *_ctx, QWidget *_parent);

    // initalize widgets
    void setupUi ();

    // update window title bar
    void updateTitle ();

    // asks the user to save his changes
    bool saveIfChanged ();

    // catch window close
    void closeEvent (QCloseEvent *_event);

  private:

    Context *ctx_;

    GraphicsScene *mainScene_;
    QStack<GraphicsScene *> scenes_;

    QSplitter *splitter_;
    QToolBox  *toolbox_;
    QStackedWidget *views_;

    QString   fileName_;
    bool changedContent_;

    static BaseWidget *base_;
};

//=============================================================================
}
//=============================================================================

#endif

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

#ifndef COLORPLUGIN_HH
#define COLORPLUGIN_HH

#include <QObject>
#include <QMenuBar>
#include "ColorPlugin.hh"

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"

class ColorPlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, ToolbarInterface, StatusbarInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(MenuInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(ToolbarInterface)
Q_INTERFACES(StatusbarInterface)

  signals:
    void updateView();

    void scriptInfo( QString _functionName );

    void addMenu(QMenu* _menu, MenuType _type);

    void addToolbar(QToolBar* _toolbar);
    void removeToolbar(QToolBar* _toolbar);

    void showStatusMessage(QString _message, int _timeout = 0);

  private slots:

    void pluginsInitialized();

  public :

    void init();

    QString name() { return (QString("Color Plugin")); };
    QString description( ) { return (QString("Sets the Default colors e.g. for slides, paper,... snapshots")); };

  //===========================================================================
  /** @name Scriptable functions to set colors
    * @{ */
  //===========================================================================

  public slots:
    /**
     * Applies Default colors to the whole scene
     */
    void setDefaultColor();

    /**
     * Applies colors for presentations to the scene
     */
    void setPowerpointColor();

    /**
     * Applies colors for papers
     */
    void setPaperColor();

    /**
     * Sets the background color of the scene
    */
    void setBackgroundColor( Vector _color );

  //===========================================================================
  /** @name Other scriptable functions
   * @{ */
  //===========================================================================

  public slots:

    /**
     * Version info of the color plugin
     */
    QString version() { return QString("1.0"); };

  /** @} */
};

#endif //COLORPLUGIN_HH

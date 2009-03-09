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

#include "ColorOptions.hh"

#include "OpenFlipper/INIFile/INIFile.hh"

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"
#include "OpenFlipper/BasePlugin/OptionsInterface.hh"
#include "OpenFlipper/BasePlugin/INIInterface.hh"

class ColorPlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, ToolbarInterface, StatusbarInterface, OptionsInterface, INIInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(MenuInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(ToolbarInterface)
Q_INTERFACES(StatusbarInterface)
Q_INTERFACES(OptionsInterface)
Q_INTERFACES(INIInterface)

  signals:
    // BaseInterface
    void updateView();

    // ScriptInterface
    void scriptInfo( QString _functionName );

    // MenuInterface
    void addMenubarAction(QAction* _action, MenuActionType _type );

    // ToolbarInterface
    void addToolbar(QToolBar* _toolbar);
    void removeToolbar(QToolBar* _toolbar);

    // StatusbarInterface
    void showStatusMessage(QString _message, int _timeout = 0);

  private slots:

    // BaseInterface
    void pluginsInitialized();

    // OptionsInterface
    void applyOptions();

    // INIInterface
    void loadIniFileOptions(INIFile& _ini);
    void saveIniFileOptions(INIFile& _ini);

  public :

    // OptionsInterface
    bool initializeOptionsWidget(QWidget*& _widget);

    // BaseInterface
    QString name() { return (QString("Color Plugin")); };
    QString description( ) { return (QString("Sets the Default colors e.g. for slides, paper,... snapshots")); };

    /// Constructor
    ColorPlugin();


  //===========================================================================
  /** @name Show Colors in Options and Store them in INI files
    * @{ */
  //===========================================================================

  private:
    /// the options widget to set colors
    ColorOptions* optionsWidget_;

    /// generate an icon from a given color
    QIcon generateIcon(QRgb _color);

    /// generate an icon from a given color
    QIcon generateIcon(OpenMesh::Vec4f _color);

    /// get the color from a given icon
    QRgb getIconColor(QIcon _icon);

    /// set the color of a color vector to the one in the given icon
    void setNewColor(QIcon _icon, OpenMesh::Vec4f& _color);

    /// convert betweeen uint and Vec4f
    void convertColor(uint _color, OpenMesh::Vec4f& _vector);

    /// convert betweeen uint and Vec4f
    uint convertColor(OpenMesh::Vec4f& _vector);

    //internal storage of colors
    OpenMesh::Vec4f backgroundColorDefault_, backgroundColorPaper_, backgroundColorPresentation_;
    OpenMesh::Vec4f baseColorDefault_, baseColorPaper_, baseColorPresentation_;
    OpenMesh::Vec4f materialColorDefault_, materialColorPaper_, materialColorPresentation_;

    double lineWidthDefault_, lineWidthPaper_, lineWidthPresentation_;

  private slots:
    /// display a color dialog
    void getColorDialog();

  /** @} */

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
    void setPresentationColor();

    /**
     * Applies colors for papers
     */
    void setPaperColor();

    /**
     * Sets the background color of the scene
    */
    void setBackgroundColor( Vector _color );

  /** @} */

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

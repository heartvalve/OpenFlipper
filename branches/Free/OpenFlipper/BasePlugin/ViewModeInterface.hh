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

#ifndef VIEWMODEINTERFACE_HH
#define VIEWMODEINTERFACE_HH


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMenuBar>
#include <QStringList>


/** \file ViewModeInterface.hh
*
* Specify own view modes setting which toolboxes/toolbars/context menus will be visible . \ref viewModeInterfacePage
*/


/** \brief Interface class for adding view modes to the ui.

\n
\ref viewModeInterfacePage "Detailed description"
\n

*/
class ViewModeInterface {

  private slots :

  public :

    /// Destructor
    virtual ~ViewModeInterface() {};

  signals :

    /** \brief Defines a ViewMode for the Toolboxes
    *
    * With this function you can define a set of toolboxes which should be visible 
    * for the specified view mode.
    * 
    * @param _mode        Name of the ViewMode
    * @param _usedWidgets List of used Widgets names
    */
    virtual void defineViewModeToolboxes(QString _mode, QStringList _usedWidgets){};
    
    /** \brief Defines a ViewMode for the Toolbars
    *
    * With this function you can define a set of toolbars which should be visible 
    * for the specified view mode.
    * 
    * @param _mode         Name of the ViewMode
    * @param _usedToolbars List of used toolbars
    */
    virtual void defineViewModeToolbars(QString _mode, QStringList _usedToolbars){};
    
    /** \brief Defines a ViewMode for the context menus
    *
    * With this function you can define a set of context menus which should be visible 
    * for the specified view mode.
    * 
    * @param _mode             Name of the ViewMode
    * @param _usedContextMenus List of used context menus
    */
    virtual void defineViewModeContextMenus(QString _mode, QStringList _usedContextMenus){};
    
    /** \brief Defines an Icon for a ViewMode
    *
    * With this function you can define an Icon associated with this view mode
    * The Icon will be taken from the standard Icon path or in the OpenFlipper 
    * home directory.\n
    * The Icon should have a size of 150x150 pixels.
    * 
    * @param _mode     Name of the ViewMode
    * @param _iconName Filename of the Icon used for this view mode
    */
    virtual void defineViewModeIcon(QString _mode, QString _iconName){};

    /** \brief Set a ViewMode
    *
    * With this function you can switch to a viewMode
    * This will show all corresponding toolboxes/toolbars
    * 
    * @param _mode      Name of the ViewMode
    * @param _expandAll Expand all toolboxes
    */
    virtual void setViewMode(QString _mode, bool _expandAll = false){};

};


/** \page viewModeInterfacePage View Mode Interface
\image html ViewModeInterface.png
\n

This Interface can be used to configure visible widgets and menus in the viewer. A given
view mode contains a list of toolbars, toolboxes and context menus which should
be visible if the mode is active. Therefore you can define a subset of the
full interface for special tasks.

\section ViewModeExample Example using custom view modes

%OpenFlipper comes along with a lot of plugins each offering a wide variety
of GUI elements such as toolboxes and toolbars. In order to constrain %OpenFlipper's
interface to a certain number of toolboxes and/or toolbar elements, the ViewModeInterface
offers means to filter these elements.

So, for example, if we only wanted the toolboxes of the Data Control and the
Selection plugin to be visible, we have to emit signal ViewModeInterface::defineViewModeToolboxes(QString, QStringList)
in order to hide out everything else:

\code
// In your plugin initialization, define the view modes:
void ExamplePlugin::pluginsInitialized() {

  // Generate the list of toolboxes to use:
  QStringList toolBoxes = QString("Data Control,Selection").split(",");

  // Define the view mode.
  emit defineViewModeToolboxes("My Custom View Mode", toolBoxes);

  //define the viewMode Toolbars
  QStringList toolBars;
  toolBars.push_back( "Selection" );

  emit defineViewModeToolbars("My Custom View Mode", toolBars);
}
\endcode

When the user selects "My Custom View Mode" out of the view modes menu, all
toolboxes except for Data Control and Selection will disappear.
This works analogously for toolbar elements via the signal ViewModeInterface::defineViewModeToolbars(QString, QStringList).

\section Defaults
The Main Toolbar and the Viewer Toolbar will always be included in the list of toolbars and will be the default when
you define a View Mode with only toolboxes and context menus.
If you do not define context menus for your View Mode, they will default to all.

\section Icons
Additionally, if we wanted the "My Custom View Mode" entry in the view modes menu
to appear with a fancy icon, we just have to call ViewModeInterface::defineViewModeIcon(QString, QString):

\code
  emit defineViewModeIcon("My Custom View Mode", "myIconFile.png");
\endcode

\section view_mode_interface_usage Usage
To use the ViewModeInterface:
<ul>
<li> include ViewModeInterface.hh in your plugins header file
<li> derive your plugin from the class ViewModeInterface
<li> add Q_INTERFACES(ViewModeInterface) to your plugin class
<li> And add the signals you want to use to your plugin class
</ul>

*/


Q_DECLARE_INTERFACE(ViewModeInterface,"OpenFlipper.ViewModeInterface/1.0")

#endif // VIEWMODEINTERFACE_HH

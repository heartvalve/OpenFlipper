/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//
// C++ Interface: ViewModeInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef VIEWMODEINTERFACE_HH
#define VIEWMODEINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <QStringList>

 /**
  * \brief Define View Modes using this interface
  *
  * This Interface can be used to configure visible widgets in the viewer. A given
  * view mode contains a list of toolbars, toolboxes and context menus which should
  * be visible if the mode is active. Therefore you can define a subset of the
  * full interface for special tasks.
  *
  * \section ViewModeExample Example using custom view modes
  *
  * %OpenFlipper comes along with a lot of plugins each offering a wide variety
  * of GUI elements such as toolboxes and toolbars. In order to constrain %OpenFlipper's
  * interface to a certain number of toolboxes and/or toolbar elements, the ViewModeInterface
  * offers means to filter these elements.
  *
  * So, for example, if we only wanted the toolboxes of the Data Control and the
  * Selection plugin to be visible, we have to emit signal defineViewModeToolboxes(QString, QStringList)
  * in order to hide out everything else:
  *
  * \code
  * void TestPlugin::pluginsInitialized() {
  *
  *     QStringList toolBoxes = QString("Data Control,Selection").split(",");
  *
  *     emit defineViewModeToolboxes("My Custom View Mode", toolBoxes);
  * }
  * \endcode
  *
  * When the user selects "My Custom View Mode" out of the view modes menu, all
  * toolboxes except for Data Control and Selection will disappear.
  * This works analogously  for toolbar elements via the signal defineViewModeToolbars(QString, QStringList).
  * Additionally, if we wanted the "My Custom View Mode" entry in the view modes menu
  * to appear with a fancy icon, we just have to call defineViewModeIcon(QString, QString):
  *
  * \code
  * emit defineViewModeIcon("My Custom View Mode", "myIconFile.png");
  * \endcode
  *
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
    * @param _mode name of the ViewMode
    * @param _usedWidgets list of used Widgets names
    */
    virtual void defineViewModeToolboxes(QString /*_mode*/, QStringList /*_usedWidgets*/){};
    
    /** \brief Defines a ViewMode for the Toolbars
    *
    * With this function you can define a set of toolbars which should be visible 
    * for the specified view mode.
    * 
    * @param _mode name of the ViewMode
    * @param _usedWidgets list of used toolbars 
    */
    virtual void defineViewModeToolbars(QString /*_mode*/, QStringList /*_usedToolbars*/){};    
    
    /** \brief Defines a ViewMode for the Toolbars
    *
    * With this function you can define a set of context menus which should be visible 
    * for the specified view mode.
    * 
    * @param _mode name of the ViewMode
    * @param _usedWidgets list of used context menus 
    */
    virtual void defineViewModeContextMenus(QString /*_mode*/, QStringList /*_usedContextMenus*/){};    
    
    /** \brief Defines an Icon for a ViewMode
    *
    * With this function you can define an Icon associated with this view mode
    * The Icon will be taken from the standard Icon path or in the OpenFlipper 
    * home directory.\n
    * The Icon should have a size of 150x150 pixels.
    * 
    * @param _mode name of the ViewMode
    * @param _iconName Name of the Icon used for this view mode
    */
    virtual void defineViewModeIcon(QString /*_mode*/, QString /*_iconName*/){};        

};

Q_DECLARE_INTERFACE(ViewModeInterface,"OpenFlipper.ViewModeInterface/1.0")

#endif // VIEWMODEINTERFACE_HH

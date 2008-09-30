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




//
// C++ Interface: BasePlugin
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef TOOLBOXINTERFACE_HH
#define TOOLBOXINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <QStringList>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>

 /**
 * This Interface should be used by plugins which will provide a toolbox widget.
 * Each Plugin can create own Widgets in the Toolbox area as long as they use the returned widget as their parent.
 * ToolboxInterface::initializeToolbox(QWidget*& _widget). You can create extra Signals and slots in your Plugin.
 * These can be connected between your toolbox and your plugin. Signals and slots across plugins are currently
 * not available but may be possible via an internal message system later.\n
 */
class ToolboxInterface {

   private slots :

   public :

      /// Destructor
      virtual ~ToolboxInterface() {};

      /**   \brief Initialize the Toolbar Widget
       *
       *  Initialize the toolbar (create a widget and return a pointer to it ) \n
       * \n
       *   Example : \n
       *  _widget = new QWidget();  \n
       *  \n
       * In the widget you can add anything you want.
       * @param _widget Pointer to the new Widget
       * @return Return true if a widget has been created
      */
      virtual bool initializeToolbox(QWidget*& /*_widget*/) = 0;

       /** \brief The Toolbox of this Widget is activated/deactivated
       *
       * Called, when the user selected this widgets toolbox
       * @param _active (when true, toolbox has been activated else deactivated)
       */
      virtual void toolboxActivated(bool /*_active*/ ) = 0;

    signals :

       /** \brief Defines a new ViewMode
       *
       * Slot defines a new ViewMode e.g. a mode where only useful widgets
       * are visible by default
       * @param _mode name of the ViewMode
       * @param _usedWidgets list of used Widgets names
       */
      virtual void defineViewMode(QString& /*_mode*/, QStringList& /*_usedWidgets*/){};


};

Q_DECLARE_INTERFACE(ToolboxInterface,"OpenFlipper.ToolboxInterface/1.0")

#endif // TOOLBOXINTERFACE_HH

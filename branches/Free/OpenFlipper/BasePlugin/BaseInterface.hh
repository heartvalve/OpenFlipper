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

#ifndef BASEINTERFACE_HH
#define BASEINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>

 /** \brief Interface class from which all plugins have to be created.
  *
  * You have to implement at least name and description for your plugin.
  * All other functions and signals are optional. If you want to implement or use
  * them just add them to your plugin header.
 */
class BaseInterface {
   signals :
      /** \brief Update current view in Main Application
       *
       *  Emit this Signal if the examiner widget in the Main Application should update the current view.
       *  This should be done for example if you changed a scenegraph node or a mesh and have
       *  to redraw it.
      */
      virtual void update_view() {};

      /** \brief The object list has been changed by this plugin
       *
       *  Emit this Signal, if the object list has changed (e.g. Source or Target changed).\n
       *  If you changed the element itself (geometry, topology,..) You should also emit this signal.\n
       *  Dont emit this Signal in BaseInterface::slotObjectUpdated() as this causes an endless Loop!!
       *  Give the id of the new object as parameter or -1 if you deleted an object.
       */
      virtual void updated_objects(int) {};

      /**  \brief The active object has been switched by this plugin
       *
       *   This Signal is used to tell the other plugins that the active object has been changed.\n
       *   You should only do this if you are writing a plugin that manages the objects(e.g. DatacontrolPlugin).\n
      */
      virtual void activeObjectChanged() {};

   private slots :

      /**  \brief An object has been updated by an other plugin
       *
       *   This slot is called by the Main aplication if the number or status of existing objects changed or if
       *   an existing object has been changed. This could mean, that objects are added or deleted
       *   or that for an existing object with the given id has been modified.
       *   If you store local information about one of these Objects, you should check if its still valid!\n
       *   Dont emit BaseInterface::updated_objects(int) in this slot as this causes an endless Loop!!
       *  @param _identifier Identifier of the updated/new object or -1 if one is deleted
      */
      virtual void slotObjectUpdated( int _identifier ) {};

      /**  \brief Called if the whole scene is cleared
       *
       */
      virtual void slotAllCleared( ) {};

       /**  \brief The active object has changed
       *
       *   This slot is called by the Main aplication if the currently active object has changed.\n
       *   This means that the selection of target objects has changed.
      */
      virtual void slotActiveObjectChanged() {};


       /**  \brief Initialize Plugin
       *
       *   This slot is called if the plugin is loaded and has to be initialized. All initialization stuff
       *   in this slot has to stay inside the plugin, no external singals are allowed here.
       *   Dont create any objects via pluginfunctions here. Use the pluginsInitialized() slot for external
       *   Initialization. After execution of this slot your plugin should be fully functional.
       *   Only gui elements may be uninitialized and should be created in pluginsInitialized().
      */
      virtual void initializePlugin() {};

      /**  \brief Initialize Plugin step 2
       *
       *   This slot is called if all plugins are loaded and the core is ready. Here you can create objects,
       *   set Textures and everything which will involve signals to the core.
      */
      virtual void pluginsInitialized() {};

      /** This function is called when the application exits or when your plugin is about to be unloaded.
       * Here you can clean up your plugin, delete local variables...
       */
      virtual void exit(){};

      /** Using this function you can inform the core that your plugin can run without creating a widget.
       * If your plugin does not implement this function, it will not be loaded in scripting mode.
       * You dont have to do anything in this function.
       */
      virtual void noguiSupported( ) {} ;

  public slots:
      /** \brief Return a version string for your plugin
       *
       * This function will be used to determin the current version of your plugin.
       * Should have the form x.x.x ( you do not need to give that many subversions )
       */
      virtual QString version() { return QString("-1"); };

   public :

      /// Destructor
      virtual ~BaseInterface() {};

      /** \brief Return a Name for the plugin
       *
       * This Function has to return the name of the plugin.
      */
      virtual QString name() = 0;

      /** \brief Return a description of what the plugin is doing
       *
       * This function has to return a basic description of the plugin
       */
      virtual QString description() = 0;

};

Q_DECLARE_INTERFACE(BaseInterface,"OpenFlipper.BaseInterface/0.3")

#endif // BASEINTERFACE_HH

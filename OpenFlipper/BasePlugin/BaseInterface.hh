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

#ifndef BASEINTERFACE_HH
#define BASEINTERFACE_HH

#include <QtGui>
#include <QMenuBar>
#include <OpenFlipper/common/Types.hh>


/** \file BaseInterface.hh
*
* OpenFlippers main plugin Interface \ref baseInterfacePage.
*/


/** \brief Interface class from which all plugins have to be created.
  *
  * You have to implement at least name and description for your plugin.
  * All other functions and signals are optional. If you want to implement or use
  * them just add them to your plugin header. A detailed description of this interface
  * can be found in the \ref baseInterfacePage .
  *
  * See \ref pluginProgramming for a tutorial on plugin programming.
  *
  * Also see \ref dataFlow diagrams for a detailed overview of
  * OpenFlipper's data flow and interface function calls.
 */
class BaseInterface {

   //===========================================================================
    /** @name Initialization
    * @{ */
   //===========================================================================

  private slots:
      /**  \brief Initialize Plugin
       *
       *   This slot is called if the plugin is loaded and has to be initialized. All initialization stuff
       *   in this slot has to stay inside the plugin, no external signals are allowed here (and will be ignored).
       *   Don't create any objects via PluginFunctions here. Use the pluginsInitialized() slot for external
       *   initialization. After execution of this slot your plugin should be fully functional.
       *   Only gui elements may be uninitialized and should be created in pluginsInitialized().
      */
      virtual void initializePlugin() {};

      /**  \brief Initialize Plugin step 2
       *
       *   This slot is called if all plugins are loaded and the core is ready. Afterwards you can already send
       *   signals to other plugins and the core (e.g. adding global textures). Do not create objects via addEmpty
       *   or load objects in this slot, as the rendering system is not ready yet.
      */
      virtual void pluginsInitialized() {};


  /** @} */

  //===========================================================================
  /** @name Object/View updates
  * @{
  * \anchor BaseInterfaceUpdateSlots
  * */

  //===========================================================================

  signals :
    /** \brief Update current view in Main Application
      *
      *  Emit this Signal if the viewer widget in the main application should update the current view.
      *  If you do an updatedObject the core will trigger an update itself and you don't have to care
      *  about it.
    */
    virtual void updateView() {};

    /** \brief An object has been changed or added by this plugin
      *
      *  Emit this Signal, if you updated any part of an object.\n
      *  If you changed the element itself (geometry, topology,..) you also have to emit this signal.\n
      *  Don't emit this Signal in BaseInterface::slotObjectUpdated() as this causes an endless Loop!!
      *  Give the id of the new object as parameter or -1 if you updated all objects or deleted an object.
      *  For performance reasons try to always give the id and not -1!
      *
      *  @param _objectId Id of the object or -1 if referring to all or deleted objects.
      */
    virtual void updatedObject(int _objectId) {};

    /** \brief An object has been changed or added by this plugin
      *
      *  Emit this Signal, if you updated any part of an object.\n
      *  If you changed the element itself (geometry, topology,..) you also have to emit this signal.\n
      *  Don't emit this Signal in BaseInterface::slotObjectUpdated() as this causes an endless Loop!!
      *  Give the id of the new object as parameter or -1 if you updated all objects or deleted an object.
      *  For performance reasons try to always give the id and not -1!
      *
      * @param _identifier id of the object or -1 if referring to all or deleted objects.
      * @param _type the type states which part of the object (topology, selection, ..) has to be updated
      */
    virtual void updatedObject(int _identifier, const UpdateType& _type) {};
    
    /** \brief A scenegraph node has been shown or hidden
      *
      *  Emit this Signal, if you changed the visibility of a scenegraph node directly (not via object->show/hide).
      *  This is required to reset the near and far plane for the viewers to provide
      *  an optimal view. Use the id of the object the node is attached to or -1 if it is not connected to an object.
      *
      */
    virtual void nodeVisibilityChanged( int _identifier ) {};

  private slots:

    /**  \brief An object has been updated by another plugin
      *
      *   This slot is called by the main application if the number or status of existing objects changed or if
      *   an existing object has been changed. This could mean, that objects are added or deleted
      *   or that an existing object with the given id has been modified.
      *   If you store local information about one of these Objects, you should check if its still valid!\n
      *  Don't emit BaseInterface::updatedObject(int) in this slot as this causes an endless Loop!!
      *   You don't need to call updateView as the core triggers a redraw itself.
      *  @param _identifier Identifier of the updated/new object or -1 if one is deleted.
    */
    virtual void slotObjectUpdated( int _identifier ) {};

    /**  \brief An object has been updated by another plugin
      *
      *   This slot is called by the main application if the number or status of existing objects changed or if
      *   an existing object has been changed. This could mean, that objects are added or deleted
      *   or that an existing object with the given id has been modified.
      *   If you store local information about one of these Objects, you should check if its still valid!\n
      *  Don't emit BaseInterface::updatedObject(int) in this slot as this causes an endless Loop!!
      *   You don't need to call updateView as the core triggers a redraw itself.
      *  @param _identifier Identifier of the updated/new object or -1 if one is deleted.
      *  @param _type the type states which part of the object (topology, selection, ..) had been updated
    */
    virtual void slotObjectUpdated( int _identifier, const UpdateType& _type ) {};

    /**  \brief Called if the whole scene is cleared
      *
      * This slot is called if the main application cleared the whole scene. No objects will remain in memory
      * and all destructors of the objects are called before this signal is emitted.
      *
      */
    virtual void slotAllCleared( ) {};

      /**  \brief The active object has changed
      *
      *   This slot is called by the main application if the current selection of an object has changed.\n
      *   This means that the selection of source / target objects has changed.
      *   Additionally you get the id of the object that has been changed or -1 if all objects
      *   have been modified.
    */
    virtual void slotObjectSelectionChanged( int _identifier ) {};

    /** \brief An object has been shown or hidden
      *
      *  This slot is called if an object is shown or hidden.
      *  The id of the object is given as a parameter.
      *  If multiple or all objects have changed, the id will be -1.
      *
      */
    virtual void slotVisibilityChanged( int _identifier ) {};

    /**  \brief Object properties have been changed
      *
      *  This slot is called if the object properties (e.g. name ) have changed
      *  The id of the object is given as a parameter.
      *  If multiple or all objects have changed, the id will be -1.
      *
    */
    virtual void slotObjectPropertiesChanged( int _identifier ) {};
    
    /** \brief View has changed
      *
      * This slot is called when the view in one of the viewers changed
      * ( Viewing direction/viewer position )
      * !! Be careful to not change the view in this slot !!
      * !! This will of course lead to an endless loop !!
    */
    virtual void slotViewChanged() {};

    /** \brief A viewer changed its draw mode
     *
     * @param _viewerId Id of the viewer that changed its draw mode
     */
    virtual void slotDrawModeChanged(int _viewerId) {};
  /** @} */

  //===========================================================================
  /** @name Plugin identification
  * @{ */
  //===========================================================================
  public :

    /** \brief Return a name for the plugin
      *
      * This Function has to return the name of the plugin.
    */
    virtual QString name() = 0;

    /** \brief Return a description of what the plugin is doing
      *
      * This function has to return a basic description of the plugin
      */
    virtual QString description() = 0;

  public slots:
      /** \brief Return a version string for your plugin
       *
       * This function will be used to determine the current version of your plugin.
       * Should have the form x.x.x ( you do not need to give that many subversions )
       */
      virtual QString version() { return QString("-1"); };

  signals:

      /**  \brief Set a description for a public slot
       *
       *   public slots of each plugin are automatically available for scripting. \n
       *   Use this Signal to add a description for your slot so that everyone knows what it is used for. \n
       *
       *   @param _slotName the name of the slot
       *   @param _slotDescription a description for the slot
       *   @param _parameters list of parameters
       *   @param _descriptions list of descriptions for the parameters (_descriptions[i] corresponds to _parameters[i])
      */
      virtual void setSlotDescription(QString     _slotName    ,   QString     _slotDescription,
                                      QStringList _parameters  , QStringList _descriptions) {};

   /** @} */

  //===========================================================================
   /** @name Renderer control
    * @{ */
  //===========================================================================
  signals:

     /** \brief Set a renderer for the given viewer
      *
      * @param _viewer Id of the viewer to set the renderer for
      * @param _rendererName Name of the renderer to be used
      */
     virtual void setRenderer(unsigned int _viewer, QString _rendererName) {};

     /** \brief Get the current renderer for the given viewer
      *
      * @param _viewer Id of the viewer to set the renderer for
      * @param _rendererName Name of the renderer that is currently active
      */
     virtual void getCurrentRenderer(unsigned int _viewer, QString& _rendererName) {};

  /** @} */

  private slots :

    /** This function is called when the application exits or when your plugin is about to be unloaded.
      * Here you can clean up your plugin, delete local variables...
      */
    virtual void exit(){};

    /** Using this function you can inform the core that your plugin can run without creating a widget.
      * If your plugin does not implement this function, it will not be loaded in scripting mode without gui.
      * You don't have to do anything in this function.
      */
    virtual void noguiSupported( ) {} ;

  public :

    /// Destructor
    virtual ~BaseInterface() {};

};


/** \page baseInterfacePage Base Interface
The BaseInterface has to be used by every plugin in OpenFlipper. As the minimum a plugin
has to implement the BaseInterface::name() and BaseInterface::description() functions from this interface.

To use the BaseInterface:
<ul>
<li> include BaseInterface.hh in your plugins header file
<li> derive your plugin from the class BaseInterface
<li> add Q_INTERFACES(BaseInterface) to your plugin class 
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them except BaseInterface::description() and BaseInterface::name() )
</ul>

\section baseInterfacePluginInitialization Plugin Initialization
BaseInterface provides two functions to initialize a plugin. The first function is BaseInterface::initializePlugin().
This function is called immediately after the plugin has been connected with OpenFlipper. When a plugin is
loaded, all signals and slots from the used interfaces are connected to the core. After this, the
BaseInterface::initializePlugin() slot is called. In this slot you can initialize your plugin.
The order how plugins are loaded is not fixed. So you should not rely or communicate with other plugins
in this slot. \n
After all plugins are loaded, the slot  BaseInterface::pluginsInitialized() is called for each plugin. All
other plugins are now available and you can setup your user interface components in this slot.
The following graphic shows the initialization of a plugin.
\image html startupProcess.jpg

\section baseInterfaceObjectUpdateNotification Object Update Notification
The objects in OpenFlippers scene are stored and managed in OpenFlippers core. If a plugin changes one of the
objects, it has to notify the core about that change. In turn OpenFlipper will then notify all other plugins about
this change. This functionality is provided by the signals and slots for \ref BaseInterfaceUpdateSlots "update handling" .

\image html updateObject.jpg

If you change data you have to emit one of BaseInterface::updatedObject(int) or BaseInterface::updatedObject(int,const UpdateType&).
\n
BaseInterface::updatedObject(int) forces an update of the whole object while BaseInterface::updatedObject(int,const UpdateType&)
can be restricted to a part of the object ( Geometry,Selection, ... ; see UpdateType ) and is therefore faster and should be preferred.

Both signals get the id of the object that has been updated or -1 if all should be updated( you should not use -1 if you know what object changed!).

If the signal is emitted, the core calls BaseInterface::slotObjectUpdated( int , const UpdateType& ) of every plugin. You can
implement this slot if you need to react on object changes.

After all plugins have been informed, the scene will be redrawn.

For more details about the UpdateType read the documentation about UpdateType and \ref DefaultUpdateTypes "predefined update types".
A description for adding custom update types at runtime is available \ref UpdateTypeFunctions "here".

It is also possible to insert custom nodes into the scenegraph. If you changed these nodes, you also have to inform the core.
This is achieved by the BaseInterface::nodeVisibilityChanged() function. As nodes they are usually attached to objects in the scene,
you should pass the id of the object to the function or -1 if its a global node (which should not be used!).

If the complete scene gets cleared, the slot BaseInterface::slotAllCleared() will be executed after all objects have been removed from the scene.
A more fine grained information about objects added or removed from the scene is available via the \ref loadSaveInterfacePage .

There are three additional slots that are called by the core, if the object selection(source/target BaseInterface::slotObjectSelectionChanged() ),
the object visibility(Show/Hide BaseInterface::slotVisibilityChanged()) or other properties changed (e.g. name BaseInterface::slotObjectPropertiesChanged() )

Remarks:
<ul>
<li>If the object id passed to the functions is -1 all objects should be treated as updated.
<li>Never emit the signal BaseInterface::updatedObject() inside the updated object slots as this causes endless loops!
</ul>

\section baseInterfaceSceneUpdateNotification Scene Update Notifications

OpenFlipper manages the scene updates in its core. If objects get updated the scene will be automatically redrawn. Nevertheless,
you can force an update of the scene by emitting the signal BaseInterface::updateView().

\image html updateView.jpg

If the view (viewer position /viewing direction) has been changed, the slot BaseInterface::slotViewChanged() will be called. Be carefull, not to
change the view in this function or you get an endless loop!

\section baseInterfaceManagementFunctions Management Functions
There are some basic functions for managing plugins. The BaseInterface::description() function can be used
to provide a description for the plugin which will be printed along with the plugin name. The BaseInterface::name()
function has to return the name of the plugin. This name will also be used inside the scripting system.
Additionally it is possible to provide a plugin version number with BaseInterface::version().

OpenFlippers scripting system can use all public slots defined in your plugin. If it should also provide a
small text description for your functions, you could set them with BaseInterface::setSlotDescription(). Even if
you do not implement scripting functions in your plugin, the core will then be able to provide this information
to the user.

Along with the scripting, OpenFlipper provides a batch mode, running without any user interface. If your plugin
will work without an user interface (e.g. only scripting functions), you can define a slot BaseInterface::noguiSupported().
This empty slot is used to tell the core, that the plugin can work in batch mode. Otherwise it will not be loaded
when running a batch file.

*/

Q_DECLARE_INTERFACE(BaseInterface,"OpenFlipper.BaseInterface/1.0")

#endif // BASEINTERFACE_HH

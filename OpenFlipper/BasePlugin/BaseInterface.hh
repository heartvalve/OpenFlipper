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
 #include <OpenFlipper/common/Types.hh>

 /** \brief Interface class from which all plugins have to be created.
  *
  * You have to implement at least name and description for your plugin.
  * All other functions and signals are optional. If you want to implement or use
  * them just add them to your plugin header.
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
       *   Don't create any objects via pluginfunctions here. Use the pluginsInitialized() slot for external
       *   initialization. After execution of this slot your plugin should be fully functional.
       *   Only gui elements may be uninitialized and should be created in pluginsInitialized().
      */
      virtual void initializePlugin() {};

      /**  \brief Initialize Plugin step 2
       *
       *   This slot is called if all plugins are loaded and the core is ready. Here you can create objects,
       *   set Textures and everything which will involve signals to the core.
      */
      virtual void pluginsInitialized() {};


  /** @} */

  //===========================================================================
  /** @name Object/View updates
  * @{ */
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
      *  Dont emit this Signal in BaseInterface::slotObjectUpdated() as this causes an endless Loop!!
      *  Give the id of the new object as parameter or -1 if you updated all objects or deleted an object.
      *
      *  The parameter has to be the id of the object or -1 if refering to all or deleted objects.
      */
    virtual void updatedObject(int ) {};

    /** \brief A scenegraph node has been shown or hidden
      *
      *  Emit this Signal, if you changed the visibility of a scenegraph node directly (not via object->show/hide).
      *  This is required to reset the near and far plane for the viewers to provide
      *  an optimal view. Use the id of the object the node is attached to or -1 if it is not connected to an object.
      *
      */
    virtual void nodeVisibilityChanged( int /*_identifier*/ ) {};
   
  private slots:

    /**  \brief An object has been updated by another plugin
      *
      *   This slot is called by the main aplication if the number or status of existing objects changed or if
      *   an existing object has been changed. This could mean, that objects are added or deleted
      *   or that an existing object with the given id has been modified.
      *   If you store local information about one of these Objects, you should check if its still valid!\n
      *   Dont emit BaseInterface::updatedObject(int) in this slot as this causes an endless Loop!!
      *   You dont need to call updateView as the core triggers a redraw itself.
      *  @param _identifier Identifier of the updated/new object or -1 if one is deleted.
    */
    virtual void slotObjectUpdated( int /*_identifier*/ ) {};

    /**  \brief Called if the whole scene is cleared
      *
      * This slot is called if the main application cleared the whole scene. No objects will remain in memory
      * and all destructors of the objects are called before this signal is emitted.
      *
      */
    virtual void slotAllCleared( ) {};

      /**  \brief The active object has changed
      *
      *   This slot is called by the main aplication if the currentselection of an object has changed.\n
      *   This means that the selection of source / target objects has changed.
      *   Addisionally you get the id of the object that has been changed or -1 if all objects
      *   have been modified.
    */
    virtual void slotObjectSelectionChanged( int /*_identifier*/ ) {};

    /** \brief An object has been shown or hidden
      *
      *  This slot is called if an objecct is shown or hidden.
      *  The id of the object is given as a parameter.
      *  If multiple or all objects have changed, the id will be -1.
      *
      */
    virtual void slotVisibilityChanged( int /*_identifier*/ ) {};

    /**  \brief Object properties have been changed
      *
      *  This slot is called if the object properties (e.g. name ) have changed
      *  The id of the object is given as a parameter.
      *  If multiple or all objects have changed, the id will be -1.
      *
    */
    virtual void slotObjectPropertiesChanged( int /*_identifier*/ ) {};
    
    /** \brief View has changed
      *
      * This slot is called when the view in one of the viewers changed
      * ( Viewing direction/viewer position )
      * !! Be carefull to not change the view in this slot !!
      * !! This will of course lead to an endless loop !!
    */
    virtual void slotViewChanged() {};

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
       * This function will be used to determin the current version of your plugin.
       * Should have the form x.x.x ( you do not need to give that many subversions )
       */
      virtual QString version() { return QString("-1"); };

  signals:

      /**  \brief Set a description for a public slot
       *
       *   public slots of each plugin are automaticly available for scripting. \n
       *   Use this Signal to add a description for your slot so that everyone knows what it is used for. \n
       *
       *   @param _slotName the name of the slot
       *   @param _slotDescription a description for the slot
       *   @param _parameters list of parameters
       *   @param _descriptions list of descriptions for the parameters (_descriptions[i] corresponds to _parameters[i])
      */
      virtual void setSlotDescription(QString     /*_slotName*/,    QString     /*_slotDescription*/,
                                      QStringList /*_parameters*/, QStringList /*_descriptions*/) {};

  /** @} */

  private slots :

    /** This function is called when the application exits or when your plugin is about to be unloaded.
      * Here you can clean up your plugin, delete local variables...
      */
    virtual void exit(){};

    /** Using this function you can inform the core that your plugin can run without creating a widget.
      * If your plugin does not implement this function, it will not be loaded in scripting mode without gui.
      * You dont have to do anything in this function.
      */
    virtual void noguiSupported( ) {} ;

  public :

    /// Destructor
    virtual ~BaseInterface() {};

};

Q_DECLARE_INTERFACE(BaseInterface,"OpenFlipper.BaseInterface/1.0")

#endif // BASEINTERFACE_HH

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


#ifndef MOUSEINTERFACE_HH
#define MOUSEINTERFACE_HH


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


/** \file MouseInterface.hh
*
* Interface for registering types in OpenFlipper. \ref mouseInterfacePage
*/


 /** \brief Interface class for receiving mouse events
 *
 * \n
 * \ref mouseInterfacePage "Detailed description"
 * \n
 *
 */
class MouseInterface {

  private slots :

    /**  \brief Wheel Event from main application
      *
      *  This slot is called if a wheel event occurred in the Viewer and is extended by
      *  the current Mode set in the Viewer. You can add additional Picking Modes to
      *  the Viewer by using the picking interface.
      *  This slot will only get called in pickingMode.
      *  @param _event Mouse event
      *  @param _mode Name of the current Picking Mode.
    */
    virtual void slotMouseWheelEvent(QWheelEvent * _event, const std::string & _mode) {};

    /**  \brief Mouse Event from main application ( Picking Mode )
      *
      *  This slot is called if a mouse event occurred in the Viewer
      *  This slot will only get called in pickingMode.
      *  Right button clicks will not be passed to the plugins as this is reserved for
      *  the context Menu.
      *
      *  @param _event Mouse event
    */
    virtual void slotMouseEvent( QMouseEvent* _event ) {};
    
    /**  \brief Mouse Event from main application ( Light Mode )
      *
      *  This slot is called if a mouse event occurred in the Viewer
      *  This slot will only get called in LightMode.
      *  Right button clicks will not be passed to the plugins as this is reserved for
      *  the context Menu.
      *
      *  @param _event Mouse event
    */
    virtual void slotMouseEventLight( QMouseEvent*  _event ) {};

  public :

    /// Destructor
    virtual ~MouseInterface() {};

};

/** \page mouseInterfacePage Mouse Interface
\n
\image html mouseInterface.png
\n


The mouse interface can be used to receive mouse events which occur in the glViewer.

There are 4 main viewer modes:
<ul>
<li> <b>Move Mode</b> events are never passed to one of the plugins and is only used for scene navigation
inside the viewers.</li>
<li> <b>Picking Mode</b> events where passed through the MouseInterface::slotMouseEvent() and MouseInterface::slotMouseWheelEvent() to the plugins. This is usually the mode, that plugins use to implement mouse interaction.</li>
<li> <b>Light Mode</b> events where passed through the MouseInterface::slotMouseEventLight(). Plugins modifying light sources use that mode to get mouse events. </li>
<li> <b>Info Mode</b> events are passed through the Information interface \ref informationInterfacePage. This mode is used to provide information about clicked objects only.</li>
</ul>

You can add handlers for these mouse events in your plugin. Remember that all plugins receive these signals.\n

The picking action is only a global mode which is divided into several other picking modes that can be managed
through the PickingInterface. If you react on mouse events, you should check if the current
picking mode is yours and of course define such a mode for your plugin.
\code
void MousePlugin::slotMouseEvent(QMouseEvent* _event) {

  // Check if your pick mode is currently active
  if ( PluginFunctions::pickMode() == "YourPickMode" && PluginFunctions::actionMode() == Viewer::PickingMode ) {

    // If double click has been performed
    if (_event->type() == QEvent::MouseButtonDblClick) {
      unsigned int node_idx, target_idx;
      OpenMesh::Vec3d hitPoint;

      // Get picked object's identifier by picking in scenegraph
      if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) ) {

        BaseObjectData* object;

        // Get picked object
        if ( PluginFunctions::getPickedObject(node_idx, object) ) {

          // Do something with the object

        }
      }
    }
  }


}
\endcode

See our tutorial \ref ex3 for an example of how to use mouse and keyboard events
within a plugin.

To use the MouseInterface:
<ul>
<li> include MouseInterface.hh in your plugins header file
<li> derive your plugin from the class MouseInterface
<li> add Q_INTERFACES(MouseInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>


*/

Q_DECLARE_INTERFACE(MouseInterface,"OpenFlipper.MouseInterface/1.0")

#endif // MOUSEINTERFACE_HH

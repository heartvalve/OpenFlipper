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


#ifndef KEYINTERFACE_HH 
#define KEYINTERFACE_HH 


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

 


/** \file KeyInterface.hh
*
* Interface for receiving keyboard events inside your plugin.\ref keyInterfacePage
*/

/** \brief Keyboard Event Interface
 *
 * \ref keyInterfacePage "Detailed description"
 * \n
 *
 * Using this interface you can react on key events.
 */
class KeyInterface {
   signals:
      /**  \brief Register a key-combination for your plugin
       * 
       * To obtain key events from the viewer for a certain key-combination, you have
       * to register that combination first
       *
       * @param _key the key that should be registered
       * @param _modifiers the keyboard modifiers
       * @param _description a short description about the functionality (Will be used in the Options Widget)
       * @param _multiUse can the key additionally be registered by another plugin (You could make a key exclusive by this)
       */
      virtual void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false) {};

   public slots :
      
      /**  \brief Key Event from Main App
       * 
       * This slot is called if a key event occurred in the Viewer
       * @param _event Key event
      */
      virtual void slotKeyEvent( QKeyEvent* _event ) {};
      
      /**  \brief Key Release Event from Main App
       * 
       * This slot is called if a key release event occurred in the Viewer
       * @param _event Key event
      */
      virtual void slotKeyReleaseEvent( QKeyEvent* _event ) {};
      
   public : 
       
      /// Destructor
      virtual ~KeyInterface() {};
      
};


/** \page keyInterfacePage Key Interface
\image html keyInterface.png
\n

Using this interface you can react on key events. You first have to register
a key (or key combination) with KeyInterface::registerKey().
Note that the log widget will contain a warning if you register
a key twice or it is occupied by another plugin.

For each registered key your KeyInterface::slotKeyEvent() will be triggered when
the key was pressed and the slot KeyInterface::slotKeyReleaseEvent() when the key
was released. Both slots get the QKeyEvent and will contain all the modifiers
that were active when the event occurred.

In OpenFlippers options tab you can view all registered keys and reassign them.
The events will be mapped, such that the plugins still receive the right combination.
Therefore the mapping will be completely invisible to them.

See our tutorial \ref ex3 for an example of how to use mouse and keyboard events
within a plugin.

To use the KeyInterface:
<ul>
<li> include KeyInterface.hh in your plugins header file
<li> derive your plugin from the class KeyInterface
<li> add Q_INTERFACES(KeyInterface) to your plugin class
<li> Implement the required functions.
</ul>

\code

// In your plugin initialization register the keys you want to get.
void ExamplePlugin::initializePlugin() {

  // Register keys
  emit registerKey(Qt::Key_W,     Qt::NoModifier, "Rotate object down");
  emit registerKey(Qt::Key_A,     Qt::NoModifier, "Rotate object left");
  emit registerKey(Qt::Key_D,     Qt::NoModifier, "Rotate object right");
  emit registerKey(Qt::Key_S,     Qt::NoModifier, "Rotate object up");

}

// Called when one of the registered Keys is pressed.
void MouseAndKeyPlugin::slotKeyEvent( QKeyEvent* _event ) {

  // Check if it's really the right key
  if ( _event->modifiers() == Qt::NoModifier ) {

    // Switch pressed keys
    switch (_event->key()) {
      case Qt::Key_W:
        // Move forward
        break;
      case Qt::Key_A:
        // Move left
        break;
      case Qt::Key_D:
        // Move right
        break;
      case Qt::Key_S:
        // Move backward
        break;
      default:
        break;
    }

  }
}
\endcode
*/

Q_DECLARE_INTERFACE(KeyInterface,"OpenFlipper.KeyInterface/1.1")
      
#endif // KEYINTERFACE_HH

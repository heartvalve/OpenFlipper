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
// C++ Interface: Basic Interface for keyboard events received from the examiner widget
//
// Description: 
//
//
// Author: Jan Moebius <moebius@informatik.rwth-aachen.de>, (C) 2007



#ifndef KEYINTERFACE_HH 
#define KEYINTERFACE_HH 

#include <QtGui>
 
 /** \brief Interface class for plugins which have to react on keyboard  events
  * 
  * Using this interface you can react on key events. You first have to register
  * a key (or key combination). For each registered key you will receive a keyEvent when
  * the key was pressed and a keyReleaseEvent when the key was released.
  *
  * See our tutorial \ref ex3 for an example of how to use mouse and keyboard events
  * within a plugin.
  *
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
       * @param _description a short description about the functionality
       * @param _multiUse can the key additionally be registered by another plugin
       */
      virtual void registerKey(int /*_key*/, Qt::KeyboardModifiers /*_modifiers*/, QString /*_description*/, bool /*_multiUse*/ = false) {};

   public slots :
      
      /**  \brief Key Event from Main App
       * 
       * This slot is called if a key event occured in the Viewer 
       * @param _event Keyevent
      */
      virtual void slotKeyEvent( QKeyEvent* /*_event*/ ) {};
      
      /**  \brief Key Release Event from Main App
       * 
       * This slot is called if a key release event occured in the Viewer 
       * @param _event Keyevent
      */
      virtual void slotKeyReleaseEvent( QKeyEvent* /*_event*/ ) {};
      
   public : 
       
      /// Destructor
      virtual ~KeyInterface() {};
      
};

Q_DECLARE_INTERFACE(KeyInterface,"OpenFlipper.KeyInterface/1.1")
      
#endif // KEYINTERFACE_HH

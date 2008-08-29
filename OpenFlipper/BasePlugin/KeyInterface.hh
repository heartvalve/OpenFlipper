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
  * Using this interface you can react on key events. The key events are emitted to all plugins
  * implementing this interface. So a key event may be handled by multiple plugins. 
  * Use keys with care!!
  * @todo : implement core handling of key events and warn on multiple definitions for the same key.
 */ 
class KeyInterface {
   private slots :
      
      /**  \brief Key Event from Main App
       * 
       * This slot is called if a key event occured in the Viewer 
       * @param _event Keyevent
      */
      virtual void slotKeyEvent( QKeyEvent* _event ) {};
      
      /**  \brief Key Release Event from Main App
       * 
       * This slot is called if a key release event occured in the Viewer 
       * @param _event Keyevent
      */
      virtual void slotKeyReleaseEvent( QKeyEvent* _event ) {};
      
   public : 
       
      /// Destructor
      virtual ~KeyInterface() {};
      
};

Q_DECLARE_INTERFACE(KeyInterface,"OpenFlipper.KeyInterface/1.0")
      
#endif // KEYINTERFACE_HH

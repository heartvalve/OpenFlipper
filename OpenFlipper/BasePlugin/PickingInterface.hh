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
// C++ Interface: PickingInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef PICKINGINTERFACE_HH
#define PICKINGINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <OpenFlipper/common/Types.hh>

 /**
  * \brief Allow access to picking functions.
  *
  * Interface Class which allows access to picking functions.
  *
  * Read tutorial \ref ex3 for an example of how to use mouse picking.
 */
class PickingInterface {
   signals :
      /** \brief Add a new picking mode to the examiner
       *
       * @param _mode Identifier of Picking mode or "Separator" to add a Separator
      */
      virtual void addPickMode( const std::string /*_mode*/) {};

      /** \brief Add a new picking mode to the examiner which will be invisible
       *
       * The added PickMode will not appear in the context menus Picking menu.
       * You have to provide a button or menuentry yourself if you want to switch to
       * the picking mode provided here.
       *
       * @param _mode Identifier of Picking mode or "Separator" to add a Separator
      */
      virtual void addHiddenPickMode( const std::string /*_mode*/ ) {};

      /** \brief Set the cursor of the given PickMode
       *
       * Set the cursor that should be used inside the pickMode
       *
       * @param _mode Identifier of Picking mode
       * @param _cursor the new cursor
      */
      virtual void setPickModeCursor( const std::string /*_mode*/ , QCursor /*_cursor*/ ) {};

      /** \brief Set mouse tracking for the given PickMode
       *
       * Enable mouseTracking for the given PickMode. When MouseTracking is enabled
       * you receive mouseMove events also when no button is pressed (via MouseInterface)
       *
       * @param _mode Identifier of Picking mode
       * @param _mouseTracking new state of mouseTracking
      */
      virtual void setPickModeMouseTracking( const std::string /*_mode*/ , bool /*_mouseTracking*/ ) {};

      /**  \brief The pickingMode has changed
       *
       * This slot is called if the user changes the current picking mode
       * @param _mode Identifier of Picking mode
      */
      virtual void slotPickModeChanged( const std::string& /*_mode*/) {};

   public :

      /// Destructor
      virtual ~PickingInterface() {};

};

Q_DECLARE_INTERFACE(PickingInterface,"OpenFlipper.PickingInterface/1.1")

#endif // PICKINGINTERFACE_HH

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


#ifndef OPTIONSINTERFACE_HH
#define OPTIONSINTERFACE_HH

#include <QtGui>

 /**
 * This Interface should be used by plugins which will provide a their own options.
 * For your options to show up in OpenFlippers Options window implement the initializeOptionsWidget() slot.
 * The widget you return with this slot will be integrated into the options window and when
 * the user hits the apply- or ok-button in the options window the slot applyOptions() is called and
 * thus enables you to store the changes in your options.
 */
class OptionsInterface {

   private slots :

   public :

      /// Destructor
      virtual ~OptionsInterface() {};

      /**   \brief Initialize the Options Widget
       *
       *  Initialize the options widget (create a widget and return a pointer to it ) \n
       * \n
       *   Example : \n
       *  _widget = new QWidget();  \n
       * \n
       * In the widget you can add anything you want.\n
       * \n 
       * use the slot applyOptions() to save the values of options currently entered
       * in your widget.
       * \n
       * @param _widget Pointer to the new Widget
       * @return Return true if a widget has been created
      */
      virtual bool initializeOptionsWidget(QWidget*& /*_widget*/) = 0;

       /** \brief Apply changes to options
       *
       * Called, when the user hits the apply/ok button in the options window
       * of Openflipper.\n
       * 
       * use this slot to save the values of options currently entered
       * in your options widget.
       */
      virtual void applyOptions() = 0;

};

Q_DECLARE_INTERFACE(OptionsInterface,"OpenFlipper.OptionsInterface/1.0")

#endif // OPTIONSINTERFACE_HH

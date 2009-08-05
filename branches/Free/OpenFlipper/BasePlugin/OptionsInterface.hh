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


#ifndef OPTIONSINTERFACE_HH
#define OPTIONSINTERFACE_HH

#include <QtGui>

 /**
 * This Interface should be used by plugins which will provide their own options.
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

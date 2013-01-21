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
*   $Revision: 11127 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-03-15 16:18:28 +0100 (Di, 15 Mär 2011) $                     *
*                                                                            *
\*===========================================================================*/


#pragma once

#include <QtGui>


/** \file AboutInfoInterface.hh
*
* Interface for adding a widget to OpenFlippers About dialog .\ref AboutInfoInterfacePage
*/

/** \brief About Info interface
 *
 * \ref AboutInfoInterfacePage "Detailed description"
 * \n
 *
 * This interface can be used to add a widget to OpenFlippers About dialog to display license Information or other data.
 */
class AboutInfoInterface {

public:
  /// Destructor
  virtual ~AboutInfoInterface() {};

signals :
  /**   \brief Adds a widget to OpenFlippers about dialog
   *
   *
   * @param _text    Text for the tab (Supports html tags)
   * @param _tabName Name of the tab to be added
   */
   virtual void addAboutInfo(QString _text, QString _tabName ) {};
};


/** \page AboutInfoInterfacePage About Info interface
\image html AboutInfoInterface.png
\n
The AboutInfoInterface can be used by plugins to add an additional tab to OpenFlippers about dialog.
This is especially useful, if you use an external library which requires you to publish the license information
in the application.

You can add multiple tabs by additional calls to this interface

To use the AboutInfoInterface:
<ul>
<li> include AboutInfoInterface.hh in your plugins header file
<li> derive your plugin from the class AboutInfoInterface
<li> add Q_INTERFACES(AboutInfoInterface) to your plugin class
<li> Implement the signal of this interface
</ul>

\code
void ExamplePlugin::pluginsInitialized(){
  // Simply emit the signal with the text
  emit addAboutInfo("Info Text","Example Plugin License");
}
\endcode
*/

Q_DECLARE_INTERFACE(AboutInfoInterface,"OpenFlipper.AboutInfoInterface/1.0")


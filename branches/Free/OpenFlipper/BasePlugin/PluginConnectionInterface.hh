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


#ifndef PLUGINCONNECTIONINTERFACE_HH
#define PLUGINCONNECTIONINTERFACE_HH

#include <QString>

/** \file PluginConnectionInterface.hh
*
* Interface for connecting signals and slots across plugins. \ref pluginConnectionInterfacePage
*/

/**
  * \brief Allow to connect slots between plugins
  *
  * \ref pluginConnectionInterfacePage "Detailed description"
  * \n
  *
  * Interface Class which allows to connect signals and slots across plugins
  *
  * \n
  *  <b> WARNING! Never connect SIGNALS and SLOTS which are already defined in the Interfaces!! WARNING! </b>
  * \n
  *
  * This might result in endless loops and breaks OpenFlippers control loops. You have to keep track
  * of your connections yourself. If you call slots which call updatedOject which in turn call your original
  * slot you get a loop and OpenFlipper is not responsible for your endless loops! Be careful!
 */
class PluginConnectionInterface {
  
   signals :
     /** \brief connect signals and slots of plugins across plugins
     *
     * Connect a signal of one plugin to another plugin by its scripting name.
     * _pluginName1 is the scripting name of the plugin containing the signal ( Use script editor to check that name)
     * _signal is the signal name. Use it like with regular connect ( <tt>SIGNAL(...)</tt> ) macro
     * _pluginName2 is the scripting name of the plugin containing a slot
     * _slot is the slot name. Use it like with regular connect ( <tt>SLOT(...)</tt> ) macro
     *
     */
     virtual void crossPluginConnect( QString /*_pluginName1*/, const char* /*_signal*/, QString /*_pluginName2*/, const char* /*_slot*/) {};


   public :

      /// Destructor
      virtual ~PluginConnectionInterface() {};

};

/** \page pluginConnectionInterfacePage Plugin Connection Interface
\n
\image html PluginConnectionInterface.png
\n

The PluginConnectionInterface allows to connect signals and slots across different OpenFlipper plugins.

Usually you should implement the BaseInterface::pluginsInitialized() function from BaseInterface. In this function you can setup
your connections. Don't try to setup your connections earlier as the plugins you try to connect may not exist yet.

\n
 <b> WARNING! Never connect SIGNALS and SLOTS which are already defined in other Interfaces!! WARNING! </b>
\n
This might result in endless loops and breaks OpenFlippers control loops. You have to keep track
of your connections yourself. If you call slots which call updatedOject which in turn call your original
slot you get a loop and OpenFlipper is not responsible for your endless loops! Be careful!

The following code shows a simple example to connect signals and slots. For the signal and slot definition
you have to use the common QT macros SIGNAL and SLOT.
\code
void ExamplePlugin::pluginsInitialized()
{
 // Use the QT macros to generate the signatures
 emit crossPluginConnect(pluginA,SIGNAL(signalA(QString)),pluginB,SLOT(slotB(QString)));
}
\endcode

To use the ScriptInterface:
<ul>
<li> include PluginConnectionInterface.hh in your plugins header file
<li> derive your plugin from the class PluginConnectionInterface
<li> add Q_INTERFACES(PluginConnectionInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(PluginConnectionInterface,"OpenFlipper.PluginConnectionInterface/1.0")

#endif // PLUGINCONNECTIONINTERFACE_HH

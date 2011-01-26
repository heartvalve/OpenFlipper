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




//
// C++ Interface: PluginConnectionInterface
//
// Description:
//
//
// Author: Jan Moebius <moebius@cs.rwth-aachen.de>, (C) 2010
//
#ifndef PLUGINCONNECTIONINTERFACE_HH
#define PLUGINCONNECTIONINTERFACE_HH

#include <QString>

 /**
  * \brief Allow to connect slots between plugins
  *
  * Interface Class which allows to connect signals and slots across plugins
  *
  * WARNING! Never connect SIGNALS and SLOTS which are already defined in the Interfaces!! WARNING!
  * This might result in endless loops and breaks openflippers control loops You have to keep track
  * of your connections yourself. If you call slots which call updatedOject which in turn call your oiginal 
  * slot, OpenFlipper is not responsible for your endless loops! Be carefull!
  * WARNING! Never connect SIGNALS and SLOTS which are already defined in the Interfaces!! WARNING!
 */
class PluginConnectionInterface {
  
   signals :
     /** \brief connect signals and slots of plugins across plugins
     *
     * Connect a signal of one plugin to another plugin by its scripting name.
     * _pluginName1 is the scripting name of the plugin containing the signal ( Use script editor to check that name)
     * _signal is the signal name. Use it like with regular connect ( SIGNAL(...) ) macro
     * _pluginName2 is the scripting name of the plugin containing a slot
     * _slot is the slot name. Use it like with regular connect ( SLOT(...) ) macro
     *
     */
     virtual void crossPluginConnect( QString /*_pluginName1*/, const char* /*_signal*/, QString /*_pluginName2*/, const char* /*_slot*/) {};


   public :

      /// Destructor
      virtual ~PluginConnectionInterface() {};

};

Q_DECLARE_INTERFACE(PluginConnectionInterface,"OpenFlipper.PluginConnectionInterface/1.0")

#endif // PLUGINCONNECTIONINTERFACE_HH

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


#ifndef RPCINTERFACE_HH
#define RPCINTERFACE_HH

#include <OpenFlipper/BasePlugin/RPCWrappers.hh>

/** \file RPCInterface.hh
*
* Interface for calling functions across plugins. \ref RPCInterfacePage
*/

/** \brief Interface to call functions across plugins.
 *
 * \ref RPCInterfacePage "Detailed description"
 * \n
 * \n
 *
 * Interface for all Plugins which do remote procedure calls ( and cross plugin calls).\n
 * The functions provided in the interface are only used to verify that certain plugins or
 * functions are available. The actual calls are in the RPCWrappers.hh file.
*/
class RPCInterface {

   public:

      /// Destructor
      virtual ~RPCInterface() {};

   signals :

     /** Check if the plugin exists in the current Environment \n
       *
       * @param _pluginName Name of the Plugin (has to be the clean version .. no Spaces etc)
       * @param _exists found or not
       */
    virtual void pluginExists( QString _pluginName , bool& _exists  ) {};


    /** Check if a plugin exists and provides the given function \n
       * @param _pluginName Name of the Plugin (has to be the clean version .. no Spaces etc)
       * @param _functionName requested function
       * @param _exists found or not
       */
    virtual void functionExists( QString _pluginName , QString _functionName , bool& _exists  ) {};

};

/** \page RPCInterfacePage RPC Interface
\image html RPCInterface.png
\n
The RPCInterface can be used by plugins to call functions from other OpenFlipper plugins. The calls are
internally transformed into scripting calls and passed to the other plugins.

You can directly connect signals and slots via the \ref pluginConnectionInterfacePage .

The interface itself contains two basic functions which can be used to check if another plugin exists (RPCInterface::pluginExists())
or if a specific function exists in another plugin (RPCInterface::functionExists()). Both functions  get the clean name
of the plugin to check. To get that name, just open the script editor in OpenFlipper which lists all plugins and their
available functions.

Example Code for plugins:
\code
  bool isPluginAvailable;
  emit pluginExists ("scripting", isPluginAvailable);
\endcode

Example Code for functions:
\code
  bool isFunctionAvailable;
  emit functionExists ("scripting", "showScriptInEditor(QString)", isFunctionAvailable);
\endcode

In addition to these basic functions there is a set of template functions in RPCWrappers.hh which can be used to directly call
a function from another plugin via the scripting system. This requires the parameters to be registered to the QT MetaObject system which
is already done for the most common types of variables.

For calling a function you can use RPC::callFunction() or RPC::callFunctionValue() if the called function returns a value.
These functions get the pluginName and the function name and call the requested function via scripting.
It is possible to supply additional parameters to these functions that will be passed to the called function.

Example code:
\code

  // call without getting return value
  RPC::callFunction ("pluginName", "FunctionName");

  // call without getting return value and suplying a parameter
  int parameter1
  RPC::callFunction ("pluginName", "FunctionName", parameter1);

  // call with getting return value which is defined as the template parameter
  QString parameter2
  bool value = RPC::callFunctionValue<bool> ("pluginName", "FunctionName", parameter1, parameter2);

\endcode


To use the RPCInterface:
<ul>
<li> include RPCInterface.hh in your plugins header file
<li> derive your plugin from the class RPCInterface
<li> add Q_INTERFACES(RPCInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(RPCInterface,"OpenFlipper.RPCInterface/1.0")

#endif // RPCINTERFACE_HH

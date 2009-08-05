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
// C++ Interface: RPCInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef RPCINTERFACE_HH
#define RPCINTERFACE_HH

#include <OpenFlipper/BasePlugin/RPCWrappers.hh>

/** Interface for all Plugins which do remote procedure calls ( and cross plugin calls).\n
 * The functions provided in the interface are only used to verify that certain plugins or
 * functions are available. The actual calls are in the RPCWrappers.hh file.
*/
class RPCInterface {

   public:

      /// Destructor
      virtual ~RPCInterface() {};

   signals :

     /** Check if the plugin exists in the current Environment \n
       * @param _pluginName Name of the Plugin (has to be the clean version .. no Spaces etc)
       * @param _exists found or not
       */
    virtual void pluginExists( QString /*_pluginName*/ , bool& /*_exists*/  ) {};


    /** Check if a plugin exists and provides the given function \n
       * @param _pluginName Name of the Plugin (has to be the clean version .. no Spaces etc)
       * @param _functionName requested function
       * @param _exists found or not
       */
    virtual void functionExists( QString /*_pluginName*/ , QString /*_functionName*/ , bool& /*_exists*/  ) {};

};

Q_DECLARE_INTERFACE(RPCInterface,"OpenFlipper.RPCInterface/1.0")

#endif // RPCINTERFACE_HH

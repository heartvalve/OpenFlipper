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
// C++ Interface: RPCInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef RPCINTERFACE_HH
#define RPCINTERFACE_HH

/** Interface for all Plugins which do remote procedure calls ( and cross plugin calls)
*/
class RPCInterface {

  signals :

     /** Check if the plugin exists in the current Environment \n
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

    /** Execute the function of the given plugin \n
       *
       * @param _pluginName Name of the Plugin (has to be the clean version .. no Spaces etc)
       * @param _functionName requested function
       * @param _success Function call succeeded
    */
    virtual void call( QString _pluginName , QString _functionName , bool& _success  ) {};

    /** Executes the given expression.
     * No checks are performed if the remote function is available.
     *
     * @param _expression command to execute
     * @param _success successfull execution?
     */
    virtual void call( QString _expression , bool& _success  ) {};

    /** Executes the given expression and returns the result
     *  No checks are performed if the remote function is available.
     *  if an error occurs the result contains an invalid QVariant
     *
     * ex. getValue("databaseplugin.tableExists(\"meshes\")", result) )
     *     result.toBool() == true
     *
     * @param _expression command to execute
     * @param _result The return value of the command
     */
    virtual void getValue( QString _expression , QVariant& _result  ) {};

};

Q_DECLARE_INTERFACE(RPCInterface,"OpenFlipper.RPCInterface/1.0")

#endif // RPCINTERFACE_HH



//   std::cerr << "RPC Test : " << std::endl;
//
//   bool ok = false;
//   emit pluginExists( "colorplugin" , ok );
//
//   if ( ok )
//     std::cerr << "Found plugin" << std::endl;
//
//   emit functionExists( "colorplugin" , "slotTest(int)" , ok  );
//
//   if ( ok )
//     std::cerr << "Found Function" << std::endl;
//
//   ok = false;
//
//   emit call( "colorplugin.slotTest( 15 )",ok);
//
//   if ( ok )
//     std::cerr << "Execution ok" << std::endl;






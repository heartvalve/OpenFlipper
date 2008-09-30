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
// C++ Interface: ScriptInterface
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef SCRIPTINTERFACE_HH 
#define SCRIPTINTERFACE_HH 

#include <QtScript/QScriptEngine>

/** Interface for all Plugins which provide scriptable Functions.
 * These Functions should follow some guidelines :
 * -# Todo
*/
class ScriptInterface {
  
  signals :
    
     /** \brief Emit this signal if a scriptable function is executed
       * 
       * e.g. selectAllVertices( ObjectId )
       * 
       * @param _functionName the called function
       * @param _exists found or not
       */
    virtual void scriptInfo( QString /*_functionName*/  ) {};
    
    /** Emit this signal to execute a script
       * @param _script The script to execute
       */
    virtual void executeScript( QString /*_script*/  ) {};
    
    /** Emit this signal to get the core scripting engine
      * @param _engine The scripting Engine
      */
    virtual void getScriptingEngine( QScriptEngine*& /*_engine*/  ) {};
    
    /** With this signal you can get a list of all available scripting functions
     * @param _functions All available functions
     */
    virtual void getAvailableFunctions( QStringList& /*_functions*/  ) {};
    
  private slots:
    
    /** Slot for a scripting plugin. Gets the Script and executes it.
     * 
     * @param _pluginName Name of the plugin that executed a scriptable function
     * @param _functionName Name of the executed function
     */
    virtual void slotScriptInfo( QString /*_pluginName*/ , QString /*_functionName*/  ) {};
    
    
    /** Slot for a scripting plugin. Gets the Script and executes it.
     * 
     * @param _script Script to execute
     */
    virtual void slotExecuteScript( QString /*_script*/ ) {};
    
    
    /** Call this slot to open the given file and execute the script in it
     */
    virtual void slotExecuteFileScript( QString /*_filename*/ ) {};
    
    
};

Q_DECLARE_INTERFACE(ScriptInterface,"OpenFlipper.ScriptInterface/1.0")
      
#endif // SCRIPTINTERFACE_HH

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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
#include <OpenFlipper/common/Types.hh>

//== IMPLEMENTATION ==========================================================



void Core::slotPluginExists( QString _pluginName , bool& _exists ) {

  for ( int i = 0 ; i < (int)plugins.size(); ++i ) {
    if ( plugins[i].rpcName == _pluginName ) {
      _exists = true;
      return;
    }
  }

  _exists = false;
}

void Core::slotFunctionExists( QString _pluginName , QString _functionName , bool& _exists  ) {

  //Find plugin
  int plugin = -1;
  for ( int i = 0 ; i < (int)plugins.size(); ++i ) {
    if ( plugins[i].rpcName == _pluginName ) {
      plugin = i;
      break;
    }
  }

  if ( plugin == -1 ) {
    _exists = false;
    return;
  }

  _exists = plugins[plugin].rpcFunctions.contains(_functionName);
}

void Core::slotCall( QString _pluginName , QString _functionName , bool& _success  ) {

  //Find plugin
  int plugin = -1;
  for ( int i = 0 ; i < (int)plugins.size(); ++i ) {
    if ( plugins[i].rpcName == _pluginName ) {
      plugin = i;
      break;
    }
  }

  if ( plugin == -1 ) {
    _success = false;
    emit log(LOGERR, "Unable to call function from Plugin : " + _pluginName + " ( Plugin not Found! )");
    return;
  }

  if ( !plugins[plugin].rpcFunctions.contains(_functionName) ) {
    _success = false;
    emit log(LOGERR, "Unable to call function from Plugin : " + _pluginName);
    emit log(LOGERR, "Function " + _functionName + " not found!");
    return;
  }

  scriptEngine_.evaluate(_pluginName + "." + _functionName );
  if ( scriptEngine_.hasUncaughtException() ) {
    _success = false;
    QScriptValue result = scriptEngine_.uncaughtException();
    QString exception = result.toString();
    emit log( LOGERR , "RPC failed with : " + exception );
    return;
  }

  _success = true;

}

void Core::slotCall( QString _expression , bool& _success  ) {

  scriptEngine_.evaluate( _expression );
  if ( scriptEngine_.hasUncaughtException() ) {
    _success = false;
    QScriptValue result = scriptEngine_.uncaughtException();
    QString exception = result.toString();
    emit log( LOGERR , "RPC failed with : " + exception );
    return;
  }

  _success = true;

}

void Core::slotGetValue(QString _expression, QVariant& _result ){
  //execute the expression
  bool ok;

  slotCall("var tmpValue=" + _expression + ";", ok);

  if (!ok){
    _result = QVariant();
    return;
  }

  //get the return value
  QScriptValue val = scriptEngine_.globalObject().property("tmpValue");

//   std::cerr << "Type:" << val.toVariant().userType() << std::endl; 
//   std::cerr << "Value:" << val.toVariant().toString().toStdString()<< std::endl;

  _result = val.toVariant();
}

//=============================================================================

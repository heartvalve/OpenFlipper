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
//   $Revision: 4737 $
//   $Author: kremer $
//   $Date: 2009-02-10 11:46:02 +0100 (Di, 10. Feb 2009) $
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

/**
 * \file RPCWrappers.hh
 * This file contains functions to call functions and procedures across plugins.
 * The Qt Scripting system is used to pass the calls between different plugins.
 */

#ifndef RPCWRAPPERS_HH
#define RPCWRAPPERS_HH

#include <QtScript>
#include <vector>

/** Namespace containing RPC helper functions used to call functions across plugins
 */

/**
 * Example code :
 * QString version = RPC::callFunctionValue< QString >("backup","version");
 * int count       = RPC::callFunctionValue< int >("info","vertexCount",2);
 *
 */
namespace RPC {

/** \brief get a pointer to the scripting engine
 *
 */
QScriptEngine* getScriptEngine();

/** \brief call a function provided by a plugin
 *
 * @param _plugin Plugin name ( Scripting name )
 * @param _functionName Name of the remote function
 */
QScriptValue callFunction( QString _plugin, QString _functionName );

/** \brief call a function provided by a plugin getting multiple parameters
 *
 * @param _plugin Plugin name ( Scripting name )
 * @param _functionName Name of the remote function
 * @param _parameters vector of scriptvalues containing the functions parameters in the right order
 */
QScriptValue callFunction( QString _plugin, QString _functionName , std::vector< QScriptValue > _parameters );


template <typename T0>
void callFunction( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename T0, typename T1>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename T0, typename T1 , typename T2>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  parameters.push_back( QScriptValue( engine,_t2 ) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename T0, typename T1 , typename T2,  typename T3>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  parameters.push_back( QScriptValue( engine,_t2 ) );
  parameters.push_back( QScriptValue( engine,_t3 ) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename ReturnValue >
ReturnValue callFunctionValue( QString _plugin, QString _functionName) {
  return qscriptvalue_cast< ReturnValue >( callFunction(_plugin,_functionName) );
}

template <typename ReturnValue , typename T0>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

template <typename ReturnValue , typename T0, typename T1>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

template <typename ReturnValue , typename T0, typename T1 , typename T2 >
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  parameters.push_back( QScriptValue( engine,_t2 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

template <typename ReturnValue , typename T0, typename T1 , typename T2, typename T3>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( QScriptValue( engine,_t0 ) );
  parameters.push_back( QScriptValue( engine,_t1 ) );
  parameters.push_back( QScriptValue( engine,_t2 ) );
  parameters.push_back( QScriptValue( engine,_t3 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}


/** \brief DONT USE! (Function to set the internal reference to the script Engine)
 *
 * Function to set the internal reference to the script engine from the core
 */
void setScriptEngine( QScriptEngine* _engine );

}

#endif // RPCWRAPPERS_HH

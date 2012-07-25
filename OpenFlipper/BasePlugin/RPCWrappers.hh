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

/**
 * \file RPCWrappers.hh
 * This file contains functions to call functions and procedures across plugins.
 * The QT Scripting system is used to pass the calls between different plugins.
 *
 * Usage is described in \ref RPCInterfacePage
 */

#ifndef RPCWRAPPERS_HH
#define RPCWRAPPERS_HH

#include <QtScript>
#include <vector>
#include <OpenFlipper/common/Types.hh>


/** Namespace containing RPC helper functions used to call functions across plugins
 *
 * Usage is described in \ref RPCInterfacePage
 */
namespace RPC {


//===========================================================================
/** @name Script Engine Controls
 *
 * Functions to get the scripting engine. Normally you don't need them. And
 * RPC::setScriptEngine should never be used!
 * @{ */
//===========================================================================

/** \brief get a pointer to OpenFlippers core scripting engine
 *
 */
DLLEXPORT
QScriptEngine* getScriptEngine();


/** \brief DONT USE! (Function to set the internal reference to the script Engine)
 *
 * Function to set the internal reference to the script engine from the core
 */
DLLEXPORT
void setScriptEngine( QScriptEngine* _engine );

/** @} */


//===========================================================================
/** @name Call functions across plugins (simple calls)
 *
 * These functions can be used to call functions in other plugins.
 * @{ */
//===========================================================================

/** \brief call a function provided by a plugin
 *
 * @param _plugin Plugin name ( Scripting name )
 * @param _functionName Name of the remote function
 */
DLLEXPORT
QScriptValue callFunction( QString _plugin, QString _functionName );

/** \brief Call a function provided by a plugin getting multiple parameters
 *
 * This function gets the parameters which are converted to a QScriptValue on your own.
 *
 * @param _plugin Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _parameters vector of scriptvalues containing the functions parameters in the right order
 */
DLLEXPORT
QScriptValue callFunction( QString _plugin, QString _functionName , std::vector< QScriptValue > _parameters );

/** @} */

//===========================================================================
/** @name Call functions across plugins
 *
 * These templates can be used to call functions in other plugins.
 * @{ */
//===========================================================================

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter passed to the function
 */
template <typename T0>
void callFunction( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
//   QVariant bla = qVariantFromValue(_t0);
//
//   IdList list = qVariantValue<IdList>( bla ) ;
//   std::cerr << "iDList size:" <<  list.size() << std::endl;;
//   std::cerr << list[0] << std::endl;
//   std::cerr << list[1] << std::endl;
//   std::cerr << "Type id is : " << QMetaType::type("IdList") << std::endl;
//   engine->globalObject().setProperty("ParameterData22",engine->toScriptValue(list));
/*
  QVariant blubb = engine->globalObject().property("ParameterData22").toVariant();
  IdList list1 = qVariantValue<IdList>( blubb ) ;
  std::cerr << "iDList1 size:" <<  list1.size() << std::endl;;
  std::cerr << list1[0] << std::endl;
  std::cerr << list1[1] << std::endl;
  std::cerr << "Type id is : " << QMetaType::type("IdList") << std::endl;*/


  parameters.push_back( engine->toScriptValue(_t0) );
  callFunction(_plugin,_functionName,parameters);
}

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 */
template <typename T0, typename T1>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  callFunction(_plugin,_functionName,parameters);
}

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 */
template <typename T0, typename T1 , typename T2>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue(_t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  callFunction(_plugin,_functionName,parameters);
}

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 * @param _t3           Parameter 4 passed to the function
 */
template <typename T0, typename T1 , typename T2,  typename T3>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  parameters.push_back( engine->toScriptValue( _t3 ) );
  callFunction(_plugin,_functionName,parameters);
}

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 * @param _t3           Parameter 4 passed to the function
 * @param _t4           Parameter 5 passed to the function
 */
template <typename T0, typename T1 , typename T2,  typename T3, typename T4>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 , T4 _t4) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  parameters.push_back( engine->toScriptValue( _t3 ) );
  parameters.push_back( engine->toScriptValue( _t4 ) );
  callFunction(_plugin,_functionName,parameters);
}

/** \brief call a function in another plugin
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 * @param _t3           Parameter 4 passed to the function
 * @param _t4           Parameter 5 passed to the function
 */
template <typename T0, typename T1 , typename T2,  typename T3, typename T4, typename T5>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 , T4 _t4, T5 _t5) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  parameters.push_back( engine->toScriptValue( _t3 ) );
  parameters.push_back( engine->toScriptValue( _t4 ) );
  parameters.push_back( engine->toScriptValue( _t5 ) );
  callFunction(_plugin,_functionName,parameters);
}

/** @} */

//===========================================================================
/** @name Call functions across plugins which return a value
 *
 * These templates can be used to call functions that return a value.
 * You have to pass the type of return value as the first template parameter.
  * @{ */
//===========================================================================

/** \brief call a function in another plugin and get a return parameter
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @return value returned by the called function
 */
template <typename ReturnValue >
ReturnValue callFunctionValue( QString _plugin, QString _functionName) {
  return qscriptvalue_cast< ReturnValue >( callFunction(_plugin,_functionName) );
}

/** \brief call a function in another plugin and get a return parameter
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @return value returned by the called function
 */
template <typename ReturnValue , typename T0>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

/** \brief call a function in another plugin and get a return parameter
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @return value returned by the called function
 */
template <typename ReturnValue , typename T0, typename T1>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

/** \brief call a function in another plugin and get a return parameter
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 * @return value returned by the called function
 */
template <typename ReturnValue , typename T0, typename T1 , typename T2 >
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

/** \brief call a function in another plugin and get a return parameter
 *
 * @param _plugin       Plugin name ( Scripting name of the plugin )
 * @param _functionName Name of the remote function
 * @param _t0           Parameter 1 passed to the function
 * @param _t1           Parameter 2 passed to the function
 * @param _t2           Parameter 3 passed to the function
 * @param _t3           Parameter 4 passed to the function
 * @return value returned by the called function
 */
template <typename ReturnValue , typename T0, typename T1 , typename T2, typename T3>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 , T3 _t3 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  parameters.push_back( engine->toScriptValue( _t3 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

/** @} */


}

#endif // RPCWRAPPERS_HH

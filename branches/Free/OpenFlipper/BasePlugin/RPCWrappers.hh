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

/**
 * \file RPCWrappers.hh
 * This file contains functions to call functions and procedures across plugins.
 * The Qt Scripting system is used to pass the calls between different plugins.
 */

#ifndef RPCWRAPPERS_HH
#define RPCWRAPPERS_HH

#include <QtScript>
#include <vector>
#include <OpenFlipper/common/Types.hh>

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
DLLEXPORT
QScriptEngine* getScriptEngine();

/** \brief call a function provided by a plugin
 *
 * @param _plugin Plugin name ( Scripting name )
 * @param _functionName Name of the remote function
 */
DLLEXPORT
QScriptValue callFunction( QString _plugin, QString _functionName );

/** \brief call a function provided by a plugin getting multiple parameters
 *
 * @param _plugin Plugin name ( Scripting name )
 * @param _functionName Name of the remote function
 * @param _parameters vector of scriptvalues containing the functions parameters in the right order
 */
DLLEXPORT
QScriptValue callFunction( QString _plugin, QString _functionName , std::vector< QScriptValue > _parameters );


template <typename T0>
void callFunction( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
//   QVariant bla = qVariantFromValue(_t0);
//
//   idList list = qVariantValue<idList>( bla ) ;
//   std::cerr << "iDList size:" <<  list.size() << std::endl;;
//   std::cerr << list[0] << std::endl;
//   std::cerr << list[1] << std::endl;
//   std::cerr << "Type id is : " << QMetaType::type("idList") << std::endl;
//   engine->globalObject().setProperty("ParameterData22",engine->toScriptValue(list));
/*
  QVariant blubb = engine->globalObject().property("ParameterData22").toVariant();
  idList list1 = qVariantValue<idList>( blubb ) ;
  std::cerr << "iDList1 size:" <<  list1.size() << std::endl;;
  std::cerr << list1[0] << std::endl;
  std::cerr << list1[1] << std::endl;
  std::cerr << "Type id is : " << QMetaType::type("idList") << std::endl;*/


  parameters.push_back( engine->toScriptValue(_t0) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename T0, typename T1>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  callFunction(_plugin,_functionName,parameters);
}

template <typename T0, typename T1 , typename T2>
void callFunction( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue(_t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  callFunction(_plugin,_functionName,parameters);
}

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

template <typename ReturnValue >
ReturnValue callFunctionValue( QString _plugin, QString _functionName) {
  return qscriptvalue_cast< ReturnValue >( callFunction(_plugin,_functionName) );
}

template <typename ReturnValue , typename T0>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

template <typename ReturnValue , typename T0, typename T1>
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

template <typename ReturnValue , typename T0, typename T1 , typename T2 >
ReturnValue callFunctionValue( QString _plugin, QString _functionName, T0 _t0 , T1 _t1 , T2 _t2 ) {
  QScriptEngine* engine = getScriptEngine();
  std::vector< QScriptValue > parameters;
  parameters.push_back( engine->toScriptValue( _t0 ) );
  parameters.push_back( engine->toScriptValue( _t1 ) );
  parameters.push_back( engine->toScriptValue( _t2 ) );
  return qscriptvalue_cast<ReturnValue>( callFunction(_plugin,_functionName,parameters) );
}

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


/** \brief DONT USE! (Function to set the internal reference to the script Engine)
 *
 * Function to set the internal reference to the script engine from the core
 */
DLLEXPORT
void setScriptEngine( QScriptEngine* _engine );

}

#endif // RPCWRAPPERS_HH

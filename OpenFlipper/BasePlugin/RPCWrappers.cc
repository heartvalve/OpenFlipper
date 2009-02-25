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

#include <OpenFlipper/BasePlugin/RPCWrappers.hh>
#include <iostream>

namespace RPC {

static QScriptEngine* engine_;

QScriptValue callFunction( QString _plugin, QString _functionName , std::vector< QScriptValue > _parameters ) {

  QString command = _plugin+"."+_functionName+ "(";
  // Make the parameters available in the scripting environment
  for ( uint i = 0 ; i < _parameters.size(); ++i ) {
    engine_->globalObject().setProperty("ParameterData" + QString::number(i) , _parameters[i] );
    command += "ParameterData" + QString::number(i);
    if ( (i + 1) < _parameters.size() )
      command+=",";
  }
  command += ")";

  QScriptValue returnValue = engine_->evaluate(command);
  if ( returnValue.isError() ) {
    QString error = returnValue.toString();
    std::cerr << "Error : " << error.toStdString() << std::endl;
    return returnValue;
  }

  return returnValue;
}

QScriptValue callFunction( QString _plugin, QString _functionName ) {

  QString command = _plugin+"."+_functionName+ "()";

  QScriptValue returnValue = engine_->evaluate(command);
  if ( returnValue.isError() ) {
    QString error = returnValue.toString();
    std::cerr << "Error : " << error.toStdString() << std::endl;
    return returnValue;
  }

  return returnValue;
}


void setScriptEngine( QScriptEngine* _engine ) {
  engine_ = _engine;
}

QScriptEngine* getScriptEngine() {
  return engine_;
}

}


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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"

//== IMPLEMENTATION ========================================================== 



void Core::slotScriptInfo( QString _pluginName , QString _functionName  ) {
  emit scriptInfo( _pluginName , _functionName );
}

void Core::slotExecuteScript( QString _script ) {
  emit executeScript( _script );
}

void Core::slotGetScriptingEngine( QScriptEngine*& _engine  ) {
  _engine = &scriptEngine_; 
}

void Core::slotGetAllAvailableFunctions( QStringList& _functions  ) {
  _functions = scriptingFunctions_;
}

void Core::scriptLogFunction( QString _output) {
   emit scriptLog(_output); 
}

//=============================================================================
//== Script Special Functions ================================================= 
//=============================================================================

QScriptValue myPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
  QString result;
  for (int i = 0; i < context->argumentCount(); ++i) {
    if (i > 0)
        result.append(" ");
    result.append(context->argument(i).toString());
  }
  
  // Get the textedit for Output ( Set in Core.cc )
  QScriptValue calleeData = context->callee().property("textedit");
  Core *widget = qobject_cast<Core*>(calleeData.toQObject());
  
  widget->scriptLogFunction(result); 
  
  return engine->undefinedValue();
}


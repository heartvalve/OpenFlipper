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
//  Wrapper for Vector ( Vec3d ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "vec3dWrapper.hh"

//== IMPLEMENTATION ========================================================== 

//=== Vector ( = Vec3d) =======================================================

QScriptValue toScriptValueVector(QScriptEngine *engine, const Vector &s)
{
  QScriptValue obj = engine->newObject();
  obj.setProperty("x", QScriptValue(engine, s[0]));
  obj.setProperty("y", QScriptValue(engine, s[1]));
  obj.setProperty("z", QScriptValue(engine, s[2]));
//   obj.setProperty("toString", engine->newFunction(VectorToString) );
  return obj;
}

void fromScriptValueVector(const QScriptValue &obj, Vector &s)
{
  s[0] = obj.property("x").toNumber();
  s[1] = obj.property("y").toNumber();
  s[2] = obj.property("z").toNumber(); 
}

QScriptValue createVector(QScriptContext *context, QScriptEngine *engine)
{     
  Vector s;
  
  QScriptValue callee = context->callee();
  
  // If arguments are given, use them for initialization otherwise
  // initialize with 0
  if (context->argumentCount() == 3) {     
    s[0] = context->argument(0).toNumber();
    s[1] = context->argument(1).toNumber();
    s[2] = context->argument(2).toNumber(); 
  } else {
    s[0] = 0.0;
    s[1] = 0.0;
    s[2] = 0.0;  
  }
  
  return engine->toScriptValue(s);
}

QScriptValue VectorToString(QScriptContext *context, QScriptEngine *engine)
{
  QString result = "Vector : ( " + 
                   context->thisObject().property("x").toString() + "," +
                   context->thisObject().property("y").toString() + "," +
                   context->thisObject().property("z").toString() + " )";
  return QScriptValue(engine, result);
}

//=============================================================================


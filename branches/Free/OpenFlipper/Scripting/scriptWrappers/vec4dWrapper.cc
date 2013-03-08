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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26. Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/



//=============================================================================
//
//  Wrapper for Vector ( Vec4d ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "vec4dWrapper.hh"

//== IMPLEMENTATION ========================================================== 

//=== Vector4 ( = Vec4d) =======================================================

QScriptValue toScriptValueVector4(QScriptEngine *engine, const Vector4 &s)
{
  QScriptValue obj = engine->newObject();
  obj.setProperty("x", QScriptValue(engine, s[0]));
  obj.setProperty("y", QScriptValue(engine, s[1]));
  obj.setProperty("z", QScriptValue(engine, s[2]));
  obj.setProperty("w", QScriptValue(engine, s[3]));
  return obj;
}

void fromScriptValueVector4(const QScriptValue &obj, Vector4 &s)
{
    if (obj.isObject()) {
        s[0] = obj.property("x").toNumber();
        s[1] = obj.property("y").toNumber();
        s[2] = obj.property("z").toNumber();
        s[3] = obj.property("w").toNumber();
        return;
    }

    QString _from = obj.toString();
    if (_from.startsWith ("Vector4 ("))
        _from.remove (0, 8);
    else if (_from.startsWith ("Vector4 : ( "))
        _from.remove (0, 11);
    if (_from.endsWith (")"))
        _from.remove (_from.length () - 1, 1);

    QStringList sl = _from.split (',');

    float v[4];
    bool ok = true;

    if (sl.length () == 4) {
        for (int i = 0; i < 4 && ok; i++)
            v[i] = sl[i].toFloat (&ok);

        if (ok)
            for (int i = 0; i < 4; i++)
                s[i] = v[i];
        else {
            for (int i = 0; i < 4; i++)
                s[i] = 0;
            std::cerr << "String to Vec4D conversion failed!" << std::endl;
        }
    }
}

QScriptValue createVector4(QScriptContext *context, QScriptEngine *engine)
{     
  Vector4 s;
  
  QScriptValue callee = context->callee();
  
  // If arguments are given, use them for initialization otherwise
  // initialize with 0
  if (context->argumentCount() == 4) {
    s[0] = context->argument(0).toNumber();
    s[1] = context->argument(1).toNumber();
    s[2] = context->argument(2).toNumber();
    s[3] = context->argument(3).toNumber();
  } else {
    s[0] = 0.0;
    s[1] = 0.0;
    s[2] = 0.0;
    s[3] = 0.0;
  }
  
  return engine->toScriptValue(s);
}

QScriptValue Vector4ToString(QScriptContext *context, QScriptEngine *engine)
{
  QString result = "Vector4 : ( " +
                   context->thisObject().property("x").toString() + "," +
                   context->thisObject().property("y").toString() + "," +
                   context->thisObject().property("z").toString() + ","+
                   context->thisObject().property("w").toString() + " )";
  return QScriptValue(engine, result);
}

//=============================================================================


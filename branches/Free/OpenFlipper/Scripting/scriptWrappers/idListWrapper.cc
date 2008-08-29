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
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fr, 09 Mai 2008) $
//
//=============================================================================




//=============================================================================
//
//  Wrapper for idList ( std::vector< int > ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "idListWrapper.hh"

//== IMPLEMENTATION ==========================================================

QScriptValue toScriptValueIdList(QScriptEngine *engine, const idList &s)
{
  QScriptValue obj = engine->newObject();
  obj.setProperty("size", QScriptValue(engine, (int)s.size() ));
  for (uint i=0; i < s.size(); i++)
    obj.setProperty(QString::number(i), QScriptValue(engine, s[i]));

  return obj;
}

void fromScriptValueIdList(const QScriptValue &obj, idList &s)
{
  int size = obj.property("size").toNumber();

  for (int i=0; i < size; i++)
    s.push_back( obj.property( QString::number(i) ).toNumber() );
}

QScriptValue createIdList(QScriptContext *context, QScriptEngine *engine)
{
  idList s;

  QScriptValue callee = context->callee();

  // If arguments are given, use them for initialization otherwise
  // initialize with 0
  for (int i=0; i < context->argumentCount(); i++)
    s.push_back( context->argument(i).toNumber() );

  return engine->toScriptValue(s);
}

QScriptValue idListToString(QScriptContext *context, QScriptEngine *engine)
{
  int size = context->thisObject().property("size").toNumber();

  QString result;

  if (size == 0)
    result = "[]";
  else{
    result = "[" + context->thisObject().property("0").toString();

    for (int i = 1; i < size; i++)
      result += "," + context->thisObject().property( QString::number(i) ).toString();

    result += "]";
  }

  return QScriptValue(engine, result);
}

//=============================================================================


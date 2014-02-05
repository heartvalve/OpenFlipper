/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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



//=============================================================================
//
//  Wrapper for IdList ( std::vector< int > ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "idListWrapper.hh"

//== IMPLEMENTATION ==========================================================

QScriptValue toScriptValueIdList(QScriptEngine *engine, const IdList &s)
{
  QScriptValue obj = engine->newObject();
  obj.setProperty("size", QScriptValue(engine, (int)s.size() ));
  for (uint i=0; i < s.size(); i++)
    obj.setProperty(QString::number(i), QScriptValue(engine, s[i]));

  return obj;
}

void fromScriptValueIdList(const QScriptValue &obj, IdList &s)
{
  int size = obj.property("size").toNumber();

  for (int i=0; i < size; i++)
    s.push_back( obj.property( QString::number(i) ).toNumber() );
}

QScriptValue createIdList(QScriptContext *context, QScriptEngine *engine)
{
  IdList s;

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


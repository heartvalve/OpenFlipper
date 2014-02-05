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
//  Wrapper for Matrix4x4 ( ACG::Matrix4x4d ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "matrix4x4Wrapper.hh"

//== IMPLEMENTATION ========================================================== 

//=== Matrix4x4 ( = Vec3d) =======================================================

QScriptValue toScriptValueMatrix4x4(QScriptEngine *engine, const Matrix4x4 &s)
{
  QScriptValue obj = engine->newObject();
  for ( uint i = 0 ; i < 4 ; ++i )
    for ( uint j = 0 ; j < 4 ; ++j )
      obj.setProperty(QString::number(i) + QString::number(j),QScriptValue(engine, s(i,j)) );
  return obj;
}

void fromScriptValueMatrix4x4(const QScriptValue &obj, Matrix4x4 &s)
{
    if (obj.isObject()) {
        for ( uint i = 0 ; i < 4 ; ++i )
            for ( uint j = 0 ; j < 4 ; ++j )
                s(i,j) = obj.property(QString::number(i) + QString::number(j)).toNumber();
        return;
    }

    QString _from = obj.toString();
    if (_from.startsWith ("Matrix4x4 ("))
        _from.remove (0, 11);
    else if (_from.startsWith ("Matrix4x4 : ("))
        _from.remove (0, 14);
    if (_from.endsWith (")"))
        _from.remove (_from.length () - 1, 1);

    QStringList sl = _from.split (',');

    float v[16];
    bool ok = true;

    if (sl.length () == 16)
    {
        for (int i = 0; i < 16 && ok; i++)
            v[i] = sl[i].toFloat (&ok);

        if (ok)
            for (int i = 0; i < 16; i++)
                s(i/4,i%4) = v[i];
        else
        {
            for (int i = 0; i < 16; i++)
                s(i/4,i%4) = 0;
            std::cerr << "String to Matrix4x4 conversion failed!" << std::endl;
        }
    }
}

QScriptValue createMatrix4x4(QScriptContext *context, QScriptEngine *engine)
{     
  Matrix4x4 s;
  
  QScriptValue callee = context->callee();
  
  // If arguments are given, use them for initialization otherwise
  // initialize with 0
  if (context->argumentCount() == 16) {     
    for ( uint i = 0 ; i < 4 ; ++i )
      for ( uint j = 0 ; j < 4 ; ++j )
        s(i,j) = context->argument(i*4+j).toNumber();
  } else {
    for ( uint i = 0 ; i < 4 ; ++i )
      for ( uint j = 0 ; j < 4 ; ++j )
        s(i,j) = 0;
  }
  
  return engine->toScriptValue(s);
}

QScriptValue Matrix4x4ToString(QScriptContext *context, QScriptEngine *engine)
{
  QString result = "Matrix4x4 : ( ";
  
  for ( uint i = 0 ; i < 4 ; ++i )
    for ( uint j = 0 ; j < 4 ; ++j ) {
      if ( i == 3 && j == 3  )
        break;
      result = result + context->thisObject().property(QString::number(i) + QString::number(j)).toString() + " , ";
    }
    
  result = result + context->thisObject().property("33").toString() + " ) ";
  
  return QScriptValue(engine, result);
}

//=============================================================================


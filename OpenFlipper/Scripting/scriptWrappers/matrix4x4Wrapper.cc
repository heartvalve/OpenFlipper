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
      obj.setProperty(QString::number(j) + QString::number(i),QScriptValue(engine, s(j,i)) );
  return obj;
}

void fromScriptValueMatrix4x4(const QScriptValue &obj, Matrix4x4 &s)
{
  for ( uint i = 0 ; i < 4 ; ++i )
    for ( uint j = 0 ; j < 4 ; ++j )
      s(j,i) = obj.property(QString::number(j) + QString::number(i)).toNumber();
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
        s(j,i) = context->argument(i*4+j).toNumber();
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
      result = result + context->thisObject().property(QString::number(j) + QString::number(i)).toString() + " , ";
    }
    
  result = result + context->thisObject().property("33").toString() + " ) ";
  
  return QScriptValue(engine, result);
}

//=============================================================================


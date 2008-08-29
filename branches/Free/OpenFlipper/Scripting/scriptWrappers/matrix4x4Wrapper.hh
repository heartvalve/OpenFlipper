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




//
// C++ Interface: matrix4x4Wrapper.hh
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef WRAPPERMATRIX4X4_HH 
#define WRAPPERMATRIX4X4_HH 

#include "OpenFlipper/common/Types.hh"

#include <QtScript/QtScript>

//===========================================================================
/** @name Script Wrappers for Matrix4x4 class ( ACG::Matrix4x4d )
  * @{ */
//===========================================================================

/// Convert Matrix4x4 to scriptvalue
QScriptValue toScriptValueMatrix4x4(QScriptEngine *engine, const Matrix4x4 &s);
    
/// Convert scriptvalue to Matrix4x4
void fromScriptValueMatrix4x4(const QScriptValue &obj, Matrix4x4 &s);

/// Create an Matrix4x4 in Scripting environment
QScriptValue createMatrix4x4(QScriptContext *, QScriptEngine *engine);

QScriptValue Matrix4x4ToString(QScriptContext *context, QScriptEngine *engine);

/** @} */  
 
      
#endif // WRAPPERMATRIX4X4_HH

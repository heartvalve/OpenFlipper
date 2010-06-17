/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
// C++ Interface: vec3dWrapper.hh
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef WRAPPERVEC3D_HH 
#define WRAPPERVEC3D_HH 

#include "OpenFlipper/common/Types.hh"

#include <QtScript/QtScript>

//===========================================================================
/** @name Script Wrappers for Vector class ( ACG::Vec3d )
  * @{ */
//===========================================================================

/// Convert Vector to scriptvalue
QScriptValue toScriptValueVector(QScriptEngine *engine, const Vector &s);
    
/// Convert scriptvalue to Vector
void fromScriptValueVector(const QScriptValue &obj, Vector &s);

/// Create an Vector in Scripting environment
QScriptValue createVector(QScriptContext *, QScriptEngine *engine);

QScriptValue VectorToString(QScriptContext *context, QScriptEngine *engine);

/** @} */  
 
      
#endif // WRAPPERVEC3D_HH

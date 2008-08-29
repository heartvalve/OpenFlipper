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


#ifndef WRAPPERIDLIST_HH
#define WRAPPERIDLIST_HH

#include "OpenFlipper/common/Types.hh"

#include <QtScript/QtScript>

//===========================================================================
/** @name Script Wrappers for idList Type ( standard vector of ints )
  * @{ */
//===========================================================================

/// Convert idList to scriptvalue
QScriptValue toScriptValueIdList(QScriptEngine *engine, const idList &s);

/// Convert scriptvalue to idList
void fromScriptValueIdList(const QScriptValue &obj, idList &s);

/// Create an empty idList in Scripting environment
QScriptValue createIdList(QScriptContext *, QScriptEngine *engine);

QScriptValue idListToString(QScriptContext *context, QScriptEngine *engine);

/** @} */


#endif // WRAPPERIDLIST_HH

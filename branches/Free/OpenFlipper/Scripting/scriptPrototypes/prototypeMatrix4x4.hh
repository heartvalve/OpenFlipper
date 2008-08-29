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
// C++ Interface: prototypeMatrix4x4
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef PROTOTYPEVMATRIX4X4_HH 
#define PROTOTYPEVMATRIX4X4_HH 


#include <QtCore/QObject>
#include <QtScript/QtScript>
 
class prototypeMatrix4x4 : public QObject , public QScriptable
{
     Q_OBJECT
 
 public:
     prototypeMatrix4x4(QObject *parent = 0);

 public Q_SLOTS:
     QString toString() const;

};
 
      
#endif // PROTOTYPEVMATRIX4X4_HH

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


#define PROTOTYPEIDLIST_C

#include "prototypeIdList.hh"

prototypeIdList::prototypeIdList(QObject *parent ) :
    QObject(parent)
{

}

QString prototypeIdList::toString() const {

  int size = thisObject().property("size").toNumber();

  QString result;

  if (size == 0)
    result = "";
  else{
    result = "" + thisObject().property("0").toString();

    for (int i = 1; i < size; i++)
      result += "," + thisObject().property( QString::number(i) ).toString();

    result += "";
  }

  return result;
}



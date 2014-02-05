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
*   $Revision: 11209 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2011-03-24 18:02:29 +0100 (Thu, 24 Mar 2011) $                    *
*                                                                            *
\*===========================================================================*/

#ifndef INFORMATIONPLUGINS_HH_
#define INFORMATIONPLUGINS_HH_

#include <OpenFlipper/BasePlugin/InformationInterface.hh>
#include <OpenFlipper/common/DataTypes.hh>
#include <map>

// Get vector of supported types
DLLEXPORT
std::map<InformationInterface*,DataType>&
supportedInfoTypes();

// Test if information plugin is available for a specific
// data type
DLLEXPORT
InformationInterface*
getInfoPlugin(DataType _type);

#endif /* INFORMATIONPLUGINS_HH_ */

/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26 Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>

class DecimaterInfo : public PerObjectData
{

  public :
  
  //Konstruktor
  DecimaterInfo();

  //Destruktor
  ~DecimaterInfo();
  
  // Copy function
  PerObjectData* copyPerObjectData();
  
  void setDistanceConstraint( double _value );
  void setNormalDeviationConstraint( int _value );
  void setRoundnessConstraint( double _value );
  
  void removeDistanceConstraint();
  void removeNormalDeviationConstraint();
  void removeRoundnessConstraint();
  
  // Get/Set methods
  bool normalDeviation()    { return normalDeviation_; }
  bool distance()           { return distance_; }
  bool roundness()          { return roundness_; }
  
  int normalDeviationValue()    { return normalDeviation_value_; }
  double distanceValue()        { return distance_value_; }
  double roundnessValue()       { return roundness_value_; }

private :

  bool normalDeviation_;
  bool distance_;
  bool roundness_;
  
  int normalDeviation_value_;
  double distance_value_;
  double roundness_value_;
};


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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifndef WIN32
  #include <tr1/memory>
#endif

class DecimaterInfo : public PerObjectData
{

  public :

    typedef OpenMesh::Decimater::DecimaterT< TriMesh > DecimaterType;
    
    typedef OpenMesh::Decimater::ModNormalFlippingT< DecimaterType >::Handle    ModNormalFlippingH;
    typedef OpenMesh::Decimater::ModQuadricT< DecimaterType >::Handle           ModQuadricH;
    typedef OpenMesh::Decimater::ModRoundnessT< DecimaterType >::Handle         ModRoundnessH; 
  
  //Konstruktor
  DecimaterInfo();

  //Destruktor
  ~DecimaterInfo();
  
  // Copy function
  PerObjectData* copyPerObjectData();
  
  // Initialize decimater
  void initialize( ModQuadricH& _handle, TriMesh* _mesh );

  void setDistanceConstraint( ModQuadricH& _handle, double _value );
  void setNormalDeviationConstraint( ModNormalFlippingH& _handle, int _value );
  void setRoundnessConstraint( ModRoundnessH& _handle, double _value );
  
  void removeDistanceConstraint( ModQuadricH& _handle );
  void removeNormalDeviationConstraint( ModNormalFlippingH& _handle );
  void removeRoundnessConstraint( ModRoundnessH& _handle );
  
  // Get/Set methods
  bool normalDeviation()    { return normalDeviation_; }
  bool distance()           { return distance_; }
  bool roundness()          { return roundness_; }
  
  int normalDeviationValue()    { return normalDeviation_value_; }
  double distanceValue()        { return distance_value_; }
  double roundnessValue()       { return roundness_value_; }

  DecimaterType* decimater() { return decimater_.get(); };

private :

  std::tr1::shared_ptr< DecimaterType > decimater_;

  bool normalDeviation_;
  int normalDeviation_value_;
  
  bool distance_;
  double distance_value_;
  
  bool roundness_;
  double roundness_value_;

};


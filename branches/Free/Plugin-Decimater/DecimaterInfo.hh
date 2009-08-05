/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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

class DecimaterInfo : public PerObjectData
{

  public :

    typedef OpenMesh::Decimater::DecimaterT< TriMesh > DecimaterType;

  private :

    // Create Normal deviation ordering module
    OpenMesh::Decimater::ModNormalFlippingT< DecimaterType >::Handle hModNormalFlipping;
    OpenMesh::Decimater::ModQuadricT< DecimaterType >::Handle        hModQuadric;
    OpenMesh::Decimater::ModRoundnessT< DecimaterType >::Handle      hModRoundness;
    OpenMesh::Decimater::ModQuadricT< DecimaterType >::Handle        hModQuadric_priority;

  public :

  //Konstruktor
  DecimaterInfo(TriMesh* _mesh);

  //Destruktor
  ~DecimaterInfo();

  void removeConstraints();

  void setDistanceConstraint( double _value );
  void setNormalDeviationConstraint( int _value );
  void setRoundnessConstraint( double _value );

  DecimaterType* decimater() { return decimater_; };

private :

  OpenMesh::Decimater::DecimaterT< TriMesh >* decimater_;

  bool normalDeviation_;
  bool distance_;
  bool roundness_;

};


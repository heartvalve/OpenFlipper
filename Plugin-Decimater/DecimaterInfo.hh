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

#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>
#include "DecimaterToolbarWidget.hh"

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
  DecimaterInfo(DecimaterToolbarWidget* _tool , TriMesh* _mesh);

  //Destruktor
  ~DecimaterInfo();

  //remove/add priority and binary modules and initialize decimater
  void update();

  DecimaterType* decimater() { return decimater_; };

private :

  DecimaterToolbarWidget* tool_;

  OpenMesh::Decimater::DecimaterT< TriMesh >* decimater_;

  bool normalDeviation_;
  bool distance_;
  bool roundness_;


};


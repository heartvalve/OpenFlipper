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


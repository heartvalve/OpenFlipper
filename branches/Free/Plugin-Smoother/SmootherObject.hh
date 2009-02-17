#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>
#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class SmootherObject : public PerObjectData
{
  public:
    SmootherObject(TriMesh* _mesh);
    ~SmootherObject();
    OpenMesh::Smoother::JacobiLaplaceSmootherT< TriMesh >* smoother;
};


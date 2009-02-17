#include "SmootherObject.hh"


SmootherObject::SmootherObject(TriMesh* _mesh){
  smoother = new OpenMesh::Smoother::JacobiLaplaceSmootherT< TriMesh >( *_mesh );
}

SmootherObject::~SmootherObject(){
  delete smoother;
}


#include "PolyMeshBackup.hh"

//-----------------------------------------------------------------------------

PolyMeshBackup::PolyMeshBackup(PolyMeshObject* _meshObj, QString _name, UpdateType _type)
  : BaseBackup( _meshObj, _name, _type ),
    meshObj_(_meshObj)
{
//   std::cerr << "Create PolyMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  meshBackup_ = new PolyMesh( *(PluginFunctions::polyMesh(meshObj_)) );
}

//-----------------------------------------------------------------------------

PolyMeshBackup::~PolyMeshBackup(){
//   std::cerr << "Delete PolyMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  delete meshBackup_;
}

//-----------------------------------------------------------------------------

void PolyMeshBackup::apply(){

  //first apply the baseBackup
  BaseBackup::apply();

//   std::cerr << "Apply PolyMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  *(PluginFunctions::polyMesh(meshObj_)) = *meshBackup_;
}

//-----------------------------------------------------------------------------
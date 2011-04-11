#include "TriMeshBackup.hh"

//-----------------------------------------------------------------------------

TriMeshBackup::TriMeshBackup(TriMeshObject* _meshObj, QString _name, UpdateType _type)
  : BaseBackup( _meshObj, _name, _type ),
    meshObj_(_meshObj)
{
//   std::cerr << "Create TriMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  meshBackup_ = new TriMesh( *(PluginFunctions::triMesh(meshObj_)) );
}

//-----------------------------------------------------------------------------

TriMeshBackup::~TriMeshBackup(){
//   std::cerr << "Delete TriMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  delete meshBackup_;
}

//-----------------------------------------------------------------------------

void TriMeshBackup::apply(){

  //first apply the baseBackup
  BaseBackup::apply();

//   std::cerr << "Apply TriMeshBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  *(PluginFunctions::triMesh(meshObj_)) = *meshBackup_;
}

//-----------------------------------------------------------------------------
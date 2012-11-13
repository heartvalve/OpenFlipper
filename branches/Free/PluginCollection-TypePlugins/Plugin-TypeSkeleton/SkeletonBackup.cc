#include "SkeletonBackup.hh"

//-----------------------------------------------------------------------------

SkeletonBackup::SkeletonBackup(SkeletonObject* _skelObj, QString _name, UpdateType _type)
  : BaseBackup( _skelObj, _name, _type ),
    skelObj_(_skelObj)
{
//   std::cerr << "Create SkeletonBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  skelBackup_ = new Skeleton( *(PluginFunctions::skeleton(skelObj_)) );
}

//-----------------------------------------------------------------------------

SkeletonBackup::~SkeletonBackup(){
//   std::cerr << "Delete SkeletonBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  delete skelBackup_;
}

//-----------------------------------------------------------------------------

void SkeletonBackup::apply(){

  //first apply the baseBackup
  BaseBackup::apply();

//   std::cerr << "Apply SkeletonBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  *(PluginFunctions::skeleton(skelObj_)) = *skelBackup_;
}

//-----------------------------------------------------------------------------
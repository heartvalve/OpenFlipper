#include "BSplineCurveBackup.hh"

//-----------------------------------------------------------------------------

BSplineCurveBackup::BSplineCurveBackup(BSplineCurveObject* _curveObj, QString _name, UpdateType _type)
  : BaseBackup( _curveObj, _name, _type ),
    curveObj_(_curveObj)
{
//   std::cerr << "Create BSplineCurveBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  curveBackup_ = new BSplineCurve( *(PluginFunctions::splineCurve(curveObj_)) );
}

//-----------------------------------------------------------------------------

BSplineCurveBackup::~BSplineCurveBackup(){
//   std::cerr << "Delete BSplineCurveBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  delete curveBackup_;
}

//-----------------------------------------------------------------------------

void BSplineCurveBackup::apply(){

  //first apply the baseBackup
  BaseBackup::apply();

//   std::cerr << "Apply SkeletonBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
  *(PluginFunctions::splineCurve(curveObj_)) = *curveBackup_;
}

//-----------------------------------------------------------------------------
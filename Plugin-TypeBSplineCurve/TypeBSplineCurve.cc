//=============================================================================
//
//  CLASS Type BSpline Curve Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
//
//=============================================================================


#include "TypeBSplineCurve.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"


//-----------------------------------------------------------------------------

TypeBSplineCurvePlugin::
TypeBSplineCurvePlugin()
{
}

//-----------------------------------------------------------------------------

bool
TypeBSplineCurvePlugin::
registerType() 
{
  addDataType("BSplineCurve",tr("B-Spline Curve"));
  setTypeIcon("BSplineCurve", "BSplineCurveType.png");
  return true;
}

//-----------------------------------------------------------------------------

DataType
TypeBSplineCurvePlugin::
supportedType()
{
  DataType type = DATA_BSPLINE_CURVE;
  return type;
}

//-----------------------------------------------------------------------------

int
TypeBSplineCurvePlugin::
addEmpty()
{
  // new object data struct
  BSplineCurveObject * object = new BSplineCurveObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 1 )
    object->target(true);

  QString name = "BSplineCurve_" + QString::number( object->id() ) + ".bsc";

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded (object->id() );

  return object->id();
}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( typebsplinecurveplugin , TypeBSplineCurvePlugin );

//=============================================================================
//
//  CLASS Type BSpline Surface Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
//
//=============================================================================


#include "TypeBSplineSurface.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

//-----------------------------------------------------------------------------

TypeBSplineSurfacePlugin::
TypeBSplineSurfacePlugin()
{
}

//-----------------------------------------------------------------------------

bool
TypeBSplineSurfacePlugin::
registerType() 
{
  addDataType("BSplineSurface",tr("B-Spline Surface"));
  setTypeIcon("BSplineSurface", "BSplineSurfaceType.png");
  return true;
}

//-----------------------------------------------------------------------------

DataType
TypeBSplineSurfacePlugin::
supportedType()
{
  DataType type = DATA_BSPLINE_SURFACE;
  return type;
}

//-----------------------------------------------------------------------------

int
TypeBSplineSurfacePlugin::
addEmpty()
{
  // new object data struct
  BSplineSurfaceObject * object = new BSplineSurfaceObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 1 )
    object->target(true);

  QString name = "BSplineSurface_" + QString::number( object->id() ) + ".bss";

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

Q_EXPORT_PLUGIN2( typebsplinesurfaceplugin , TypeBSplineSurfacePlugin );

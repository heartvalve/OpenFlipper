/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  CLASS Type BSpline Surface Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2010-02-02 11:01:53 +0100 (Di, 02. Feb 2010) 
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

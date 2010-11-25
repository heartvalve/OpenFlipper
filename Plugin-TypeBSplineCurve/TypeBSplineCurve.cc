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
//  CLASS Type BSpline Curve Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2010-02-02 11:02:14 +0100 (Di, 02. Feb 2010) 
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

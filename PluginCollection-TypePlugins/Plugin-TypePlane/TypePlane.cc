/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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


#include "TypePlane.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

TypePlanePlugin::TypePlanePlugin() {
  
}

bool TypePlanePlugin::registerType() {
  addDataType("Plane",tr("Plane"));
  setTypeIcon( "Plane", "PlaneType.png");
  return true;
}

int TypePlanePlugin::addEmpty(){
    
  // new object data struct
  PlaneObject * object = new PlaneObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = QString(tr("New Plane %1.pla").arg( object->id() ));

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  // set the default colors
  const QColor color = OpenFlipper::Options::defaultColor();
  const ACG::Vec4f default_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  object->materialNode()->set_color(default_color);

  object->update();

  object->show();

  emit emptyObjectAdded (object->id() );

  return object->id();
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typeplaneplugin , TypePlanePlugin );
#endif


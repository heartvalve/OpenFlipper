/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#include "TypeQtWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

TypeQtWidgetPlugin::TypeQtWidgetPlugin():
widgets_()
{
}

bool TypeQtWidgetPlugin::registerType() {
  addDataType("QtWidget",tr("QtWidget"));
  setTypeIcon("QtWidget", "QtWidgetType.png");
  
  return true;
}

int TypeQtWidgetPlugin::addEmpty() {
    
  // new object data struct
  QtWidgetObject* object = new QtWidgetObject();
  widgets_[object->id()] = object;

  QString name = QString(tr("New Widget %1.lgt").arg( object->id() ));

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );
  object->update();
  object->enablePicking(true);
  object->show();
  
  emit emptyObjectAdded (object->id() );

  return object->id();
}

void TypeQtWidgetPlugin::slotMouseEvent(QMouseEvent *_event)
{
  passMouseEvents(_event);
}

void TypeQtWidgetPlugin::passMouseEvents(QMouseEvent *_event)
{
  for(std::map<int,QtWidgetObject*>::iterator iter = widgets_.begin(); iter != widgets_.end(); ++iter)
    iter->second->mouseEvent(_event);
}

void TypeQtWidgetPlugin::objectDeleted(int _id)
{
  widgets_.erase(_id);
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typeqtwidgetplugin , TypeQtWidgetPlugin );
#endif


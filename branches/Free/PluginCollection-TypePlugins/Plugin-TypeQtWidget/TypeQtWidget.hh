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




#ifndef TYPEQTWIDGETPLUGIN_HH
#define TYPEQTWIDGETPLUGIN_HH

#include <QObject>
#include <map>


#include <ObjectTypes/QtWidget/QtWidget.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>

class TypeQtWidgetPlugin : public QObject, BaseInterface, TypeInterface, MouseInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TypeInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(LoadSaveInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-TypeQtWidget")
#endif

  signals:

  // LoadSave Interface
  void emptyObjectAdded( int _id );

private slots:

  // BaseInterface
  void noguiSupported( ) {};

  // MouseInterface

public:

  ~TypeQtWidgetPlugin() {};
  TypeQtWidgetPlugin();

  QString name() { return (QString("TypeQtWidget")); };
  QString description( ) { return (QString(tr("Register QtWidget type"))); };

  bool registerType();

public slots:

  // Base Interface
  QString version() { return QString("1.0"); };

  // Type Interface
  int addEmpty();
  DataType supportedType() { return DATA_QT_WIDGET; };

  // Mouse Interface
  void  slotMouseEvent (QMouseEvent *_event);

  // LoadSave Interface
  void objectDeleted(int _id);

private:

  void passMouseEvents(QMouseEvent *_event);

  std::map<int,QtWidgetObject*> widgets_;

};

#endif //TYPEQTWIDGETPLUGIN_HH

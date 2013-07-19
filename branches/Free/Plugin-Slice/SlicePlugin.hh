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

#ifndef SLICEPLUGIN_HH
#define SLICEPLUGIN_HH

#include <QObject>
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <ACG/Scenegraph/ClippingNode.hh>
// #include <OpenFlipper/common/Types.hh>

#include "SliceToolbox.hh"

class SlicePlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Slice")
#endif

//BaseInterface
signals:
  void updateView();

//LoggingInterface:
  void log( Logtype _type, QString _message );
  void log( QString _message );
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

private slots:

  void initializePlugin(); // BaseInterface
  
public :
  SlicePlugin();
  ~SlicePlugin() {};

  QString name() { return (QString("Slice")); };
  QString description( ) { return (QString("Slice objects")); };

private :
  SliceToolBox* tool_;
  QIcon* toolIcon_;

  void getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax);

private slots:
  void resetParameters();
  void updateSlice();
  void updateSlice(int bla);

private:
  ACG::SceneGraph::ClippingNode* node_;

public slots:
  QString version() { return QString("1.0"); };
};

#endif //SLICEPLUGIN_HH

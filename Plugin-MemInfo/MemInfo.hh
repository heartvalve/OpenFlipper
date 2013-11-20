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

#pragma once

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>

class MemInfoPlugin: public QObject, BaseInterface, LoggingInterface, StatusbarInterface {
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(StatusbarInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-MemInfo")
#endif

  signals:

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // StatusbarInterface
  void addWidgetToStatusbar(QWidget* _widget);

public:

  ~MemInfoPlugin();

  // BaseInterface
  QString name() { return (QString("MemInfo"));  } ;
  QString description() {  return (QString("Plugin used to display current GPU/CPU Memory Status")); } ;

private slots:

  /// BaseInterface
  void initializePlugin();
  void pluginsInitialized();


  /// Update statusbar with NVIDIA memory infos
  void nvidiaMemoryInfoUpdate();

  /// Update statusbar with main memory infos
  void cpuMemoryInfoUpdate();

private:

  /// Status bar for GPU memory
  QProgressBar* gpuMemBar_;

  /// Status bar for Main memory
  QProgressBar* mainMemBar_;

  /// Timer that triggers an update of the bars
  QTimer*       updateTimer_;

public slots:

  QString version() { return QString("1.0"); };

};

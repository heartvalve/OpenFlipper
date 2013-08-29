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

#ifndef RULERPLUGIN_HH
#define RULERPLUGIN_HH

#include <QObject>
#include <QString>

#include <ACG/Utils/SmartPointer.hh>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/OptionsInterface.hh>

#include "Ruler.hh"
#include "RulerOptions.hh"

class RulerPlugin : public QObject, BaseInterface, MouseInterface, PickingInterface, ToolbarInterface, LoadSaveInterface, OptionsInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolbarInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(OptionsInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Ruler")
#endif

signals:
  void addToolbar(QToolBar *_toolbar);

  void addPickMode(const std::string &_mode);

  void updateView();

  void setPickModeMouseTracking  ( const std::string &_mode, bool  _mouseTracking);

public slots:

  void slotPickModeChanged(const std::string& _mode);

  void slotMouseEvent(QMouseEvent* _event);

  void slotViewChanged();
public:

  // OptionsInterface
  bool initializeOptionsWidget(QWidget*& _widget);

  RulerPlugin();
  ~RulerPlugin();

  QString name(){return QString("RulerPlugin");}
  QString description(){return QString("Measure the distance between two points.");}

private:

  void reset();

  void enableDragMode(const int _point);

  void disableDragMode();

  bool dragModeActive(){return lineDrag_ >= 0;}

  QAction *buttonAction_;

  const std::string pickModeName_;

  //saves the index of the dragged point, if no point was dragged, it is -1
  int lineDrag_;

  //checks if a double click was provided for resetting
  bool dblClickCheck_;

  bool viewupdated_;

  ptr::shared_ptr<Ruler> currentRuler_;

  RulerOptions* optionsWidget_;

  const QString textSizeSettingName_;
private slots:

  void initializePlugin();

  void pluginsInitialized();

  void slotChangePickMode();

  void slotAllCleared();

  void objectDeleted(int _id);

  // OptionsInterface
  void applyOptions();

public slots:
  QString version(){ return QString("1.0"); }
};


#endif //RULERPLUGIN_HH

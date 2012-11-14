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

//=============================================================================
//
//  CLASS PropertyVisPlugin
//
//=============================================================================


#ifndef PROPERTYVISPLUGIN_HH
#define PROPERTYVISPLUGIN_HH


//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>
#include <vector>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include "PropertyVisToolbar.hh"
#include "ObjectListItemModel.hh"

#include "PropertyModel.hh"
#include "PropertyModelFactory.hh"

#include <stdexcept>

//== CLASS DEFINITION =========================================================

class PropertyVisPlugin : public QObject, BaseInterface, ToolboxInterface, KeyInterface, ScriptInterface, MouseInterface, PickingInterface, LoggingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoggingInterface)

signals:
  void updateView();
  void updatedObject(int, const UpdateType&);

  void log(Logtype, QString);
  void log(QString);
  
  // ToolboxInterface
  void addToolbox( QString  _name  , QWidget* _widget ,QIcon* _icon);

  //PickingInterface
  void addHiddenPickMode( const std::string& _mode );
  

private slots:

  // BaseInterface
  void slotObjectUpdated( int _identifier, const UpdateType& _type );
  void slotAllCleared();
  
  // initialization functions
  void initializePlugin();
  void pluginsInitialized();

  // toolbox slots
  void slotMeshChanged(int _index = -1);
  void slotVisualize();
  
  // MouseInterface
  void slotMouseEvent( QMouseEvent* _event );
  
  //PickingInterface
  void slotPickModeChanged( const std::string& _mode);
  
  void updateGUI();

  /// Called when user selects a property.
  void propertySelectionChanged();

  /// Duplicates the selected properties.
  void slotDuplicateProperty();

  /// Removes the selected properties.
  void slotRemoveProperty();

  /// Receives log messages from PropertyModels and emits them
  void slotLog(Logtype _type, QString _message){ emit log(_type, _message); }
  void slotLog(QString _message){ emit log(_message); }

public :

  PropertyVisPlugin();
  ~PropertyVisPlugin() {}


  QString name() { return (QString("Property Visualization")); }
  QString description( ) { return (QString("Computes the PropertyVis of the the active Mesh")); }


private :
  // Widget for Toolbox
  PropertyVisToolbar* tool_;

private:

  /// Exchanges the PropertyModel after the user selected a different object.
  void setNewPropertyModel(int id);


private:
  ObjectListItemModel objectListItemModel_;

  PropertyModel* propertyModel_;
};

#if defined(INCLUDE_TEMPLATES) && !defined(PROPERTYVISPLUGIN_CC)
#define PROPERTYVISPLUGIN_TEMPLATES
#include "PropertyVisPluginT.cc"
#endif
//=============================================================================
#endif // PROPERTYVISPLUGIN_HH defined
//=============================================================================


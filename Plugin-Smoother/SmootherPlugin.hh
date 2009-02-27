//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================

//=============================================================================
//
// SmootherPlugin
//
//=============================================================================

#ifndef SMOOTHERPLUGIN_HH
#define SMOOTHERPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

#include "SmootherToolbarWidget.hh"


//== CLASS DEFINITION =========================================================


/** Plugin for Smoother Support
 */
class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)

signals:

  // BaseInterface
  void updateView();

  void updatedObject(int);

  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

public :

  /// default constructor
  SmootherPlugin() : tool_(0) {};

  /// default destructor
  ~SmootherPlugin() {};

  /// Initialize the toolbar (create a widget in the right side toolbox)
  bool initializeToolbox(QWidget*& _widget);

  /// Name of the Plugin
  QString name(){ return (QString("Smoother")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Smoothing ...")); };

private slots:

  /// Second initialization stage
  void pluginsInitialized();

private :

  /// Widget for Toolbox
  SmootherToolbarWidget* tool_;

private slots:

  /// Slot connected to the smooth button in the toolbox
  void slot_smooth();

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================
public slots:

   /// Smooth an object
   void smooth(int _objectId , int _iterations , QString _direction , QString _continuity, double _maxDistance = -1.0);

public slots:
   QString version() { return QString("1.0"); };

/** @} */

};

#endif //SMOOTHERPLUGIN_HH

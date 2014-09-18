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
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

#include "SmootherToolbarWidget.hh"


//== CLASS DEFINITION =========================================================


/** Plugin for Smoother Support
 */
class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface, ScriptInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ScriptInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Smoother")
#endif

signals:

  // BaseInterface
  void updateView();

  void updatedObject(int, const UpdateType&);

  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );
  
  // BackupInterface
  void createBackup( int _id , QString _name, UpdateType _type = UPDATE_ALL );

  // ScriptInterface
  void scriptInfo(QString _functionName);
  
public :

  /// default constructor
  SmootherPlugin();

  /// default destructor
  ~SmootherPlugin() {};

  /// Name of the Plugin
  QString name(){ return (QString("Smoother")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Smoothing ...")); };

private slots:

  void initializePlugin(); // BaseInterface
  
  /// Second initialization stage
  void pluginsInitialized();

  /// Tell system that this plugin runs without ui
  void noguiSupported( ) {} ;

private :

  /// Widget for Toolbox
  SmootherToolbarWidget* tool_;
  QIcon* toolIcon_;

private slots:

  /// Slot connected to the smooth button in the toolbox
  void slot_smooth();

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================
public slots:

   /** \brief smooth an object
    *
    * @param _objectId        Id of the object that should be smoothed
    * @param _iterations      Number of iterations
    * @param _direction       Tangential or normal?
    * @param _continuity      Continuity selection
    * @param _maxDistance     Maximal distance after smoothing
    * @param _respectFeatures Keep features
    *
    */
   void smooth(int _objectId , int _iterations , QString _direction , QString _continuity, double _maxDistance = -1.0, bool _respectFeatures = true);

   /** @} */

public slots:
   QString version() { return QString("1.0"); };



private:
   typedef OpenMesh::Smoother::JacobiLaplaceSmootherT< TriMesh > SmootherType;
};

#endif //SMOOTHERPLUGIN_HH

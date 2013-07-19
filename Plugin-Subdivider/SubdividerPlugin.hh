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


#ifndef SUBDIVIDERPLUGIN_HH
#define SUBDIVIDERPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "subdividerToolbar.hh"

class SubdividerPlugin : public QObject, BaseInterface , ToolboxInterface, LoggingInterface, LoadSaveInterface, ScriptInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(BackupInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Subdivider")
#endif

signals:
  void updateView();
  void updatedObject(int _id, const UpdateType& _type);

  void addPickMode( const std::string& _mode );

  void log(Logtype _type, QString _message);
  void log(QString _message);

  void addEmptyObject( DataType _type, int& _id );

  void defineViewMode(QString _mode, QStringList _usedWidgets);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );
  
  // BackupInterface
  void createBackup( int _id , QString _name, UpdateType _type = UPDATE_ALL );
  
  // ScriptInterface
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

private slots:

  void initializePlugin();
  
  void pluginsInitialized();
  
  /// Make plugin available in no-GUI mode
  void noguiSupported( ) {} ;
  
  void slotSubdivideUniform();

  void slotSimpleSubdivide();

public slots:

  /// Scripting slot for subdivision
  void subdivide(int _objectId, QString _algorithm , int _steps);
  
  /// Scripting slot for the simple subdivision algorithms
  void simpleSubdivide(int _objectId, QString _algorithm , int _steps, double _parameter);


public :
  SubdividerPlugin();
  ~SubdividerPlugin() {};

  QString name() { return (QString("Subdivider")); };
  QString description( ) { return (QString("Subdivide a mesh")); };

private :
  /// Widget for Toolbox
  subdividerToolbarWidget* tool_;
  QIcon* toolIcon_;

private:

  bool test_trimesh_object(int _identifier, BaseObjectData*& _object);


public slots:
   QString version() { return QString("1.0"); };
};

#endif //SUBDIVIDERPLUGIN_HH

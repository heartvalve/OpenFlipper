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

#ifndef HOLEFILLINGPLUGIN_HH
#define HOLEFILLINGPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "holefillerToolbar.hh"

class HoleFillerPlugin : public QObject, BaseInterface, MouseInterface, PickingInterface, ToolboxInterface, LoggingInterface, ScriptInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(BackupInterface)
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-HoleFilling")
#endif

  signals:
    void updateView();
    void updatedObject(int, UpdateType);

    void addPickMode( const std::string _mode );

    void log(Logtype _type, QString _message);
    void log(QString _message);

    void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

    void scriptInfo( QString _functionName  );
    
    void setSlotDescription(QString     _slotName    ,   QString     _slotDescription,
                                          QStringList _parameters  , QStringList _descriptions);

    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );


  private slots:
    void slotObjectUpdated(int _identifier);

    void slotMouseWheelEvent(QWheelEvent * /*_event*/, const std::string & /*_mode*/){};

    void slotMouseEvent( QMouseEvent* /*_event*/ ){};

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized() ;

    void detectButton( );

  public :

    ~HoleFillerPlugin() {};

    HoleFillerPlugin();

    QString name() { return (QString("Hole Filler")); };
    QString description( ) { return (QString("Fill Holes or connect meshes")); };

  private slots:
    void slotItemSelectionChanged();
    
    void slotCellDoubleClicked(int _row , int _col);
    void slotFillSelection();
  private :

    void update_menu();

    /// Widget for Toolbox
    HoleFillerToolbarWidget* tool_;

    /// map from the index in the table to (object-id, hole-id)
    std::vector< std::pair< int , int> > holeMapping_;

  //Scripting slots
  public slots:

    void fillAllHoles(int _objectID);

    void fillHole(int _objectID, int _edgeHandle);

  public slots:

    QString version() { return QString("1.3"); };
};

#endif //HOLEFILLINGPLUGIN_HH

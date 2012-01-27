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
*   $Revision: 13361 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-12 16:33:16 +0100 (Do, 12 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/




#ifndef MESHCOMPAREPLUGIN_HH
#define MESHCOMPAREPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <OpenFlipper/common/Types.hh>

class MeshComparePlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, ScriptInterface, BackupInterface, TextureInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(TextureInterface)

  signals:
    //BaseInterface
    void updateView();
    void updatedObject(int _id, const UpdateType& _type);
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // ToolboxInterface
    void addToolbox( QString  _name  , QWidget* _widget );
    
    // BackupInterface
    void createBackup( int _id , QString _name, UpdateType _type = UPDATE_ALL );

  public:

    MeshComparePlugin();
    ~MeshComparePlugin();

    // BaseInterface
    QString name() { return (QString("Mesh Compare")); };
    QString description( ) { return (QString("Compare two meshes")); };

  private slots:

      void initializePlugin();
      void pluginsInitialized();
    
     void compare();

     void compare(int _sourceId,int _targetId);

     // BaseInterface
     void slotObjectUpdated( int _identifier, const UpdateType& _type );

    // Scriptable functions
   public slots:
       
    QString version() { return QString("1.0"); };

};

#endif //MESHCOMPAREPLUGIN_HH

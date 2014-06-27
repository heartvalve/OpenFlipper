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

#ifndef ISOTROPICREMESHERPLUGIN_HH
#define ISOTROPICREMESHERPLUGIN_HH


#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/ProcessInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "IsotropicRemesherToolbox.hh"
#include "ProgressEmitter.hh"

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif


class IsotropicRemesherPlugin : public QObject, BaseInterface, BackupInterface , ToolboxInterface, LoggingInterface,
    RPCInterface, ProcessInterface, ScriptInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(RPCInterface)
Q_INTERFACES(ProcessInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(BackupInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-IsotropicRemesher")
#endif


signals:
  
  //BaseInterface
  void updateView();
  void updatedObject(int _id, const UpdateType& _type);

  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  //LoggingInterface:
  void log( Logtype _type, QString _message );
  void log( QString _message );

  // RPC Interface
  void pluginExists( QString _pluginName , bool& _exists  ) ;
  void functionExists( QString _pluginName , QString _functionName , bool& _exists  );
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );   
    
  // ProcessInterface 
  void startJob( QString _jobId, QString _description, int _min, int _max, bool _blocking = false);
  void setJobState(QString _jobId, int _value);
  void setJobName(QString _jobId, QString _name);
  void finishJob(QString _jobId);
  void setJobDescription(QString _jobId, QString _description);
  
  // BackupInterface
  void createBackup( int _id , QString _name, UpdateType /*_type*/ = UPDATE_ALL );
  
  // ScriptInterface
  void scriptInfo(QString _functionName);

private slots:

  // BaseInterface
  void initializePlugin();  
  void pluginsInitialized(); // BaseInterface

public :
  IsotropicRemesherPlugin();
  ~IsotropicRemesherPlugin() {};

  QString name() { return (QString("IsotropicRemesher")); };
  QString description( ) { return (QString("an isotropic remesher")); };

//GUI
private :
  IsotropicRemesherToolBox* tool_;
  QIcon* toolIcon_;
  double edgeLength_;

  OpenFlipperThread* thread_;

private slots:
  void slotRemesh();

  void slotRemeshButtonClicked();
  void slotSetMinLength();
  void slotSetMaxLength();
  void slotSetMeanLength();
  
  void threadFinished(QString _jobId);

//scripting functions
public slots:
  void isotropicRemesh(int _objectID, double _targetEdgeLength );


public slots:
  QString version() { return QString("1.0"); };
};

#endif //ISOTROPICREMESHERPLUGIN_HH

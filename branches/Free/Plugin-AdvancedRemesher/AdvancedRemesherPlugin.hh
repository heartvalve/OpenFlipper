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
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

#ifndef ADVANCEDREMESHERPLUGIN_HH
#define ADVANCEDREMESHERPLUGIN_HH

#include <QtGui>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/ProcessInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "AdvancedRemesherToolbox.hh"
#include "ProgressEmitter.hh"

class AdvancedRemesherPlugin : public QObject, BaseInterface, BackupInterface, ToolboxInterface, LoggingInterface, RPCInterface, ProcessInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(BackupInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(RPCInterface)
Q_INTERFACES(ProcessInterface)


signals:
  
    //BaseInterface
    void updateView();
    void updatedObject(int _id, const UpdateType _type);

    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

    //LoggingInterface:
    void log( Logtype _type, QString _message );
    void log( QString _message );

    // RPC Interface
    void pluginExists( QString _pluginName , bool& _exists  ) ;
    void functionExists( QString _pluginName , QString _functionName , bool& _exists  );

    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget );   

    // ProcessInterface 
    void startJob( QString _jobId, QString _description, int _min, int _max, bool _blocking = false);
    void setJobState(QString _jobId, int _value);
    void setJobName(QString _jobId, QString _name);
    void finishJob(QString _jobId);
    void setJobDescription(QString _jobId, QString _description);

    // BackupInterface
    void createBackup(int _id , QString _name, UpdateType _type = UPDATE_ALL);

private slots:

    // BaseInterface
    void initializePlugin();  
    void pluginsInitialized(); // BaseInterface

public :

    AdvancedRemesherPlugin();

    ~AdvancedRemesherPlugin();

    QString name() { return (QString("AdvancedRemesher")); };
    QString description( ) { return (QString("An advanced remesher plugin.")); };

//GUI
private :
    AdvancedRemesherToolBox* tool_;

    ProgressEmitter* progress_;

private slots:
    
    //---- Adaptive Remeshing ------------
    
    void adaptiveRemeshingButtonClicked();
    
    void adaptiveRemeshing();

    //---- Adaptive Remeshing ------------
    
    void uniformRemeshingButtonClicked();
    
    void uniformRemeshing();
    
    //------------------------------------
  
    void threadFinished(QString _jobId);

    //------------------------------------

    /// Compute mean edge length and set values
    void computeInitValues();


//scripting functions
public slots:
    
    void adaptiveRemeshing(int           _objectId,
                           double        _error,
                           double        _min_edge_length,
                           double        _max_edge_length,
                           unsigned int  _iters,
                           bool          _use_projection = true);
                           
    void uniformRemeshing(int           _objectId,
                          double        _edge_length,
                          unsigned int  _iters,
                          unsigned int  _area_iters,
                          bool          _use_projection = true);

public slots:
    QString version() { return QString("1.0"); };
};

#endif //ADVANCEDREMESHERPLUGIN_HH

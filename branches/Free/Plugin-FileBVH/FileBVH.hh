/*===========================================================================*\
*                                                                           *
*                              OpenFlipper                                  *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
*                           www.openflipper.org                             *
*                                                                           *
*---------------------------------------------------------------------------*
*  This file is part of OpenFlipper.                                        *
*                                                                           *
*  OpenFlipper is free software: you can redistribute it and/or modify      *
*  it under the terms of the GNU Lesser General Public License as           *
*  published by the Free Software Foundation, either version 3 of           *
*  the License, or (at your option) any later version with the              *
*  following exceptions:                                                    *
*                                                                           *
*  If other files instantiate templates or use macros                       *
*  or inline functions from this file, or you compile this file and         *
*  link it with other files to produce an executable, this file does        *
*  not by itself cause the resulting executable to be covered by the        *
*  GNU Lesser General Public License. This exception does not however       *
*  invalidate any other reasons why the executable file might be            *
*  covered by the GNU Lesser General Public License.                        *
*                                                                           *
*  OpenFlipper is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU LesserGeneral Public          *
*  License along with OpenFlipper. If not,                                  *
*  see <http://www.gnu.org/licenses/>.                                      *
*                                                                           *
\*===========================================================================*/

/*===========================================================================*\
*                                                                           *
*   $Revision: 7918 $                                                       *
*   $Author: wilden $                                                       *
*   $Date: 2009-12-15 10:05:44 +0100 (Tue, 15 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

#ifndef FILEBVHPLUGIN_HH
#define FILEBVHPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

#include <ObjectTypes/Skeleton/Skeleton.hh>

class FileBVHPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
    LoggingInterface, ScriptInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)

  signals:
    void openedFile( int _id );
    void addEmptyObject( DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id , QString _filename );
    void log(Logtype _type, QString _message);
    void log(QString _message);
    void updateView();

    void deleteObject( int _id );

  private slots:

    void fileOpened( int /*_id*/ ){};

    void noguiSupported() {};

    void initializePlugin();

  public :
    
     FileBVHPlugin();

     ~FileBVHPlugin() {};

     QString name() { return (QString("FileBVH")); };
     QString description( ) { return (QString(tr("Load/Save BioVision Format"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/);
     QWidget* loadOptionsWidget(QString /*_currentFilter*/);

  private:
    
    bool writeSkeleton( std::ostream& _out, Skeleton& _skeleton );

    ACG::Vec3d convertToAxisRotation(ACG::Matrix4x4d& _rotationMatrix);
    
    bool ignoreJointScaling_;

    QWidget*     loadOptions_;
    QCheckBox*   checkJointScaling_;
    QPushButton* loadDefaultButton_;
    
  public slots:

    /// Slot called when user wants to save the given Load options as default
    void slotLoadDefault();
    
    /// Loads Object
    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };
  
};

#endif //FILEBVHPLUGIN_HH

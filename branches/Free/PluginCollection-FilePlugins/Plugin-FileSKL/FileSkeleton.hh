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
*   $Revision: 10274 $                                                       *
*   $LastChangedBy: wilden $                                                 *
*   $Date: 2010-11-24 17:49:09 +0100 (Wed, 24 Nov 2010) $                    *
*                                                                            *
\*===========================================================================*/

#ifndef FILESKLPLUGIN_HH
#define FILESKLPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>

class FileSKLPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(INIInterface)

  signals:
    void openedFile( int _id );
    void addEmptyObject( DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id , QString _filename );
    void log(Logtype _type, QString _message);
    void log(QString _message);

  private slots:

    void fileOpened( int /*_id*/ ){};

    void loadIniFile( INIFile& _ini ,int _id );
    void saveIniFile( INIFile& _ini ,int _id);

    void noguiSupported( ) {} ;
    
    void initializePlugin();

  public :

     ~FileSKLPlugin() {};

     QString name() { return (QString("FileSKL")); };
     QString description( ) { return (QString("Load/Save Skeletons")); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/) { return 0; };
     QWidget* loadOptionsWidget(QString /*_currentFilter*/) { return 0; };

  public slots:

    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

  private :
	  template<typename Skeleton> bool LoadSkeleton(Skeleton *_pSkeleton, QString _filename);
	  template<typename Skeleton> bool SaveSkeleton(Skeleton *_pSkeleton, QString _filename);

};

#endif //FILESKLPLUGIN_HH

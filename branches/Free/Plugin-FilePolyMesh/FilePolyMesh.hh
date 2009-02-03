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




#ifndef FILEPOLYMESHPLUGIN_HH
#define FILEPOLYMESHPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>

class FilePolyMeshPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface
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

    void emptyObjectAdded( int _id );

  private slots:
    void fileOpened( int /*_id*/ ){};

    void loadIniFileLast( INIFile& _ini ,int _id );
    void saveIniFile( INIFile& _ini ,int _id);

    void noguiSupported( ) {} ;

  public :

     ~FilePolyMeshPlugin() {};

     QString name() { return (QString("FilePolyMesh")); };
     QString description( ) { return (QString("Load/Save Poly Meshes")); };

     void setObjectRoot( BaseObject* _root );

     bool supportAddEmpty(){return true;};

     int addEmpty( );

     QString typeName() { return (QString("Poly Mesh")); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/) { return 0; };
     QWidget* loadOptionsWidget(QString /*_currentFilter*/) { return 0; };

  private :
     BaseObject* objectRoot_;

  public slots:

    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    int addNewPolyMesh();

    QString version() { return QString("1.0"); };
};

#endif //FILEPOLYMESHPLUGIN_HH

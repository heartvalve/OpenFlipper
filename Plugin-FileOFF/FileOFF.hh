/*===========================================================================*\
*                                                                           *
*                              OpenFlipper                                  *
*      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
*   $Revision$                                                       *
*   $Author$                                                       *
*   $Date$                   *
*                                                                           *
\*===========================================================================*/

#ifndef FILEOFFPLUGIN_HH
#define FILEOFFPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

// Binary file support
#include <OpenMesh/Core/IO/SR_store.hh>
#include <limits>

#include "OFFImporter.hh"

class FileOFFPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
    LoggingInterface, ScriptInterface, INIInterface, StatusbarInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(INIInterface)
   Q_INTERFACES(StatusbarInterface)

  signals:
    void openedFile( int _id );
    void addEmptyObject( DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id , QString _filename );
    void log(Logtype _type, QString _message);
    void log(QString _message);
    void updateView();

    void emptyObjectAdded( int _id );
    void deleteObject( int _id );
    
    // StatusbarInterface
    void showStatusMessage(QString _message, int _timeout = 0);
    void setStatus( ApplicationStatus::applicationStatus _status);

  private slots:

    void fileOpened( int /*_id*/ ){};

    void noguiSupported( ) {} ;

    void initializePlugin();
    
    
    /// Slot called when user wants to save the given Load options as default
    void slotLoadDefault();
    
    /// Slot called when user wants to save the given Save options as default
    void slotSaveDefault();

  public :
    
     FileOFFPlugin();

     ~FileOFFPlugin() {};

     QString name() { return (QString("FileOFF")); };
     QString description( ) { return (QString(tr("Load/Save OFF-Files"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/);
     QWidget* loadOptionsWidget(QString /*_currentFilter*/);

  public slots:

    /// Loads Object and converts it to a triangle mesh if possible
    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.1"); };

  private:
    
    /// Before Parsing the actual file, read all features supported
    bool readFileOptions(QString _filename, OFFImporter& _importer);
      
    /// Read OFF file and parse it
    bool readOFFFile(QString _filename, OFFImporter& _importer);
      
    /// Parse ascii OFF file
    bool parseASCII(std::istream& _in, OFFImporter& _importer, DataType _type);
     
    /// Parse binary OFF file
    bool parseBinary(std::istream& _in, OFFImporter& _importer, DataType _type);
    
    /// Get color type
    int getColorType(std::string& _line, bool _texCoordsAvailable) const;
    
    /// Update user options depending on which options have been selected
    /// on the load dialog
    void updateUserOptions();
    
    // Binary reader and writer helpers
    void readValue(std::istream& _in, float& _value) const {
        float tmp;
        
        OpenMesh::IO::restore( _in , tmp, false ); //assuming LSB byte order
        _value = tmp;
    }
    
    void readValue(std::istream& _in, int& _value) const {
        int32_t tmp;
        
        OpenMesh::IO::restore( _in , tmp, false ); //assuming LSB byte order
        _value = tmp;
    }
    
    void readValue(std::istream& _in, unsigned int& _value) const {
        uint32_t tmp;
        
        OpenMesh::IO::restore( _in , tmp, false ); //assuming LSB byte order
        _value = tmp;
    }
    
    void writeValue(std::ostream& _out, int value) const {
        
        uint32_t tmp = value;
        OpenMesh::IO::store(_out, tmp, false);
    }
    
    void writeValue(std::ostream& _out, unsigned int value) const {
        
        uint32_t tmp = value;
        OpenMesh::IO::store(_out, tmp, false);
    }
    
    void writeValue(std::ostream& _out, float value) const {
        
        float tmp = value;
        OpenMesh::IO::store(_out, tmp, false);
    }
    
    void trimString( std::string& _string);
    
    /// Writer function
    template< class MeshT >
    bool writeMesh(std::ostream& _out, MeshT& _mesh );
    
    /// Write binary mesh data to file
    template< class MeshT >
    bool writeBinaryData(std::ostream& _out, MeshT& _mesh );
    
    /// Write ASCII mesh data to file
    template< class MeshT >
    bool writeASCIIData(std::ostream& _out, MeshT& _mesh );
    
    //Option Widgets
    QWidget* loadOptions_;
    QWidget* saveOptions_;
    
    QCheckBox*   saveBinary_;
    QCheckBox*   saveVertexColor_;
    QCheckBox*   saveFaceColor_;
    QCheckBox*   saveAlpha_;
    QCheckBox*   saveNormals_;
    QCheckBox*   saveTexCoords_;
    QPushButton* saveDefaultButton_;
    

    QComboBox*   triMeshHandling_;
    QCheckBox*   loadVertexColor_;
    QCheckBox*   loadFaceColor_;
    QCheckBox*   loadAlpha_;
    QCheckBox*   loadNormals_;
    QCheckBox*   loadTexCoords_;
    QPushButton* loadDefaultButton_;
    
    unsigned int userReadOptions_;
    unsigned int userWriteOptions_;
};

#if defined(INCLUDE_TEMPLATES) && !defined(FILEOFFPLUGIN_C)
#define FILEOFFPLUGIN_TEMPLATES
#include "FileOFFT.cc"
#endif

#endif //FILEOFFPLUGIN_HH

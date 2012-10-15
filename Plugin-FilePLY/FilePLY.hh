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

#ifndef FILEPLYPLUGIN_HH
#define FILEPLYPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class FilePLYPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
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

    void noguiSupported( ) {} ;

    void initializePlugin();  
    
    /// Slot called when user wants to save the given Load options as default
    void slotLoadDefault();
    
    /// Slot called when user wants to save the given Save options as default
    void slotSaveDefault();

  public :
    
    FilePLYPlugin();

    ~FilePLYPlugin() {};

    QString name() { return (QString("FilePLY")); };
    QString description( ) { return (QString(tr("Load/Save PLY-Files"))); };

    DataType supportedType();

    QString getSaveFilters();
    QString getLoadFilters();

    QWidget* saveOptionsWidget(QString /*_currentFilter*/);
    QWidget* loadOptionsWidget(QString /*_currentFilter*/);
     
  private:
     
    // Helper class that stores general file information
    typedef std::pair<std::string,std::string> PPair;
    struct PLYHeader {
        bool    binary;
        bool    bigEndian;
        bool    isTriangleMesh;
        
        int     numVertices;
        bool    hasVertexNormals;
        bool    hasVertexColors;
        bool    hasVertexColorAlpha;
        // If true, colors are separated in ambient, diffuse and specular
        bool    hasVertexTexCoords;
        std::vector<PPair> vProps;
        
        int     numFaces;
        bool    hasFaceNormals;
        bool    hasFaceColors;
        bool    hasFaceColorAlpha;
        std::string valenceType;
        std::string  indexType;
        // If true, colors are separated in ambient, diffuse and specular
        std::vector<PPair> fProps;
    };
    
    // Get data type size in bytes
    size_t getTypeSize(std::string _type);

  public slots:

    /// Loads Object and converts it to a triangle mesh if possible
    int loadObject(QString _filename);
    
    /// Loads object with forcing the given datatype
    int loadObject(QString _filename, DataType _type);

    /// Always loads mesh as polymesh
    int loadPolyMeshObject(QString _filename, const PLYHeader& _header);
    
    /// Loads a triangle mesh
    int loadTriMeshObject(QString _filename, const PLYHeader& _header);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };

  private:
    
    // Parse header of PLY file
    bool parseHeader(QString _filename, PLYHeader& _header);
    
    // Template functions
    
    template <class MeshT>
    bool readMeshFileAscii(QString _filename, MeshT* _mesh, const PLYHeader& _header);
    
    template <class MeshT>
    bool readMeshFileBinary(QString _filename, MeshT* _mesh, const PLYHeader& _header);
    
    template <class MeshT>
    bool writeMeshFileAscii(QString _filename, MeshT* _mesh);
    
    template <class MeshT>
    bool writeMeshFileBinary(QString _filename, MeshT* _mesh);
    
    template <class MeshT>
    void writeHeader(std::ofstream& _os, MeshT* _mesh, bool _binary);
    
    template <class MeshT>
    void backupTextureCoordinates(MeshT& _mesh);

    /// \brief Helper functions for writing/reading of binary data

    template <class T>
    void readValue(std::istream& _in, T& _value, bool _bigEndian) const {
        T tmp;    
        OpenMesh::IO::restore(_in , tmp, _bigEndian); //assuming LSB byte order
        _value = tmp;
    }
    
    template <class T>
    void writeValue(std::ostream& _out, T value, bool _bigEndian = false) const {
        T tmp = value;
        OpenMesh::IO::store(_out, tmp, _bigEndian);
    }
    
    
    //Option Widgets
    QWidget* loadOptions_;
    QWidget* saveOptions_;
    
    QCheckBox*   saveBinary_;
    QCheckBox*   saveVertexNormal_;
    QCheckBox*   saveVertexColor_;
    QCheckBox*   saveVertexTexCoord_;
    QCheckBox*   saveFaceNormal_;
    QCheckBox*   saveFaceColor_;
    QPushButton* saveDefaultButton_;
    
    QComboBox*   triMeshHandling_;
    QCheckBox*   loadVertexNormal_;
    QCheckBox*   loadVertexColor_;
    QCheckBox*   loadVertexTexCoord_;
    QCheckBox*   loadFaceNormal_;
    QCheckBox*   loadFaceColor_;
    QPushButton* loadDefaultButton_;
};

#if defined(INCLUDE_TEMPLATES) && !defined(FILEPLYPLUGIN_C)
#define FILEPLYPLUGIN_TEMPLATES
#include "FilePLYT.cc"
#endif

#endif //FILEPLYPLUGIN_HH

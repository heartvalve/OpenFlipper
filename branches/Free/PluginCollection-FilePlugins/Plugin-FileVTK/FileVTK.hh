/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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
*   $LastChangedBy$                                            *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef FILEVTKPLUGIN_HH
#define FILEVTKPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

enum Dataset {
  STRUCTURED_POINTS,
  STRUCTURED_GRID,
  RECTILINEAR_GRID,
  POLYDATA,
  UNSTRUCTURED_GRID
};

class FileVTKPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
    LoggingInterface, ScriptInterface, INIInterface, StatusbarInterface, RPCInterface, TextureInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(INIInterface)
   Q_INTERFACES(StatusbarInterface)
   Q_INTERFACES(RPCInterface)
   Q_INTERFACES(TextureInterface)

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

    //RPCInterface
    void pluginExists( QString _pluginName , bool& _exists  );

    //TextureInterface
    void setTextureMode(QString _textureName, QString _mode, int _id );
    void switchTexture( QString _textureName, int _id );
    void addMultiTexture( QString _textureGroup, QString _name, QString _filename, int _id, int& _textureId);
    void textureFilename( int /*_id*/, QString /*_textureName*/, QString& /*_textureFilename*/ );
    void textureIndex(QString _name,int _id, int& _index);
    void getCurrentTexture(int /*_id*/, QString& /*_name*/);
    void textureName (int, int, QString &);
    void getSubTextures (int, QString, QStringList &);
    void textureIndexPropertyName(int, QString&);

  private slots:

    void fileOpened( int /*_id*/ ){};

    void noguiSupported( ) {} ;

    void initializePlugin();

    /// Slot called when user wants to save the given Save options as default
    void slotSaveDefault();


  public :
      enum ObjectOptionsVtk
      {
        NONE                   = 0,
        BINARY                 = 1,
        FACENORMALS            = 1 << 1,
        VERTEXNORMALS          = 1 << 2,
        VERTEXTEXCOORDS        = 1 << 3,
      };

      FileVTKPlugin();

     ~FileVTKPlugin() {};

     QString name() { return (QString("FileVTK")); };
     QString description( ) { return (QString(tr("Load/Save ASCII VTK-Files"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* loadOptionsWidget(QString /*_currentFilter*/);
     QWidget* saveOptionsWidget(QString /*_currentFilter*/);

  public slots:

    /// Loads Object and converts it to a triangle mesh if possible
    int loadObject(QString _filename);

    /// load object and force type
    int loadObject(QString _filename, DataType _type);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };

  private:

    bool forceTriangleMesh_;
    bool forcePolyMesh_;

    QWidget* saveOptions_;

    QCheckBox*   saveBinary_;
    QCheckBox*   saveFaceNormals_;
    QCheckBox*   saveVertexNormals_;
    QCheckBox*   saveVertexTexCoords_;
    QLabel*      savePrecisionLabel_;
    QSpinBox*    savePrecision_;
    QPushButton* saveDefaultButton_;

    /// Reading binary file?
    bool binary_;
    unsigned int userWriteOptions_;

    /// Update user options depending on which options have been selected
    /// on the save dialog
    void updateUserOptions();

    template <typename MeshT>
    bool loadMesh(QTextStream& _stream,MeshT*& _mesh, Dataset _type);

    /** \brief Reads points from the stream and adds them to the mesh
    *
    * @param _spec Line containing the POINTS header
    * @param _in   Textstream (after reading the POINTS header)
    * @param _mesh Mesh to work on
    */
    template <typename MeshT>
    bool loadMeshPoints(QString _spec,QTextStream& _in,MeshT*& _mesh);

    /** \brief Reads lines from the stream and adds them to the mesh
    *
    *
    * @param _spec Line containing the LINES header
    * @param _in   Textstream (after reading the LINES header)
    * @param _mesh Mesh to work on
    */
    template <typename MeshT>
    bool loadMeshLines(QString _spec,QTextStream& _in,MeshT*& _mesh);

    /** \brief Reads polygons from the stream and adds them to the mesh
    *
    *
    * @param _spec Line containing the POLYGONS header
    * @param _in   Textstream (after reading the POLYGONS header)
    * @param _mesh Mesh to work on
    */
    template <typename MeshT>
    bool loadMeshPolygons(QString _spec,QTextStream& _in,MeshT*& _mesh);

    /** \brief Helper function for loadMeshPolygons() that takes care of adding non-manifold faces
    *
    *
    * @param _mesh Mesh to work on
    * @param _vhandles Non-manifold face to be added
    */
    template <typename MeshT>
    void add_non_manifold_face(MeshT*& _mesh, std::vector< OpenMesh::VertexHandle >& _vhandles);

    /** \brief Reads triangle strips from the stream and adds them to the mesh
    *
    *
    * @param _spec Line containing the TRIANGLE_STRIPS header
    * @param _in   Textstream (after reading the TRIANGLE_STRIPS header)
    * @param _mesh Mesh to work on
    */
    template <typename MeshT>
    bool loadMeshTriangleStrips(QString _spec,QTextStream& _in,MeshT*& _mesh);

    /** \brief Reads unstructured grid data from the stream and adds it to the mesh
    *
    *
    * @param _spec        Line containing the CELL header
    * @param _in          Textstream (after reading the CELL header)
    * @param _mesh        Mesh to work on
    */
    template <typename MeshT>
    bool loadMeshCells(QString _spec,QTextStream& _in,MeshT*& _mesh);

    /** \brief Reads Normals from the stream and adds them to the mesh
    *
    *
    * @param _spec        Line containing the NORMALS header
    * @param _in          Textstream (after reading the NORMALS header)
    * @param _mesh        Mesh to work on
    * @param _pointNormal point or face normals?
    * @param _count       Number of primitives to read
    */
    template <typename MeshT>
    bool loadMeshNormals(QString _spec,QTextStream& _in,MeshT*& _mesh,bool _pointNormal, quint32 _count);

    /** \brief Writes the header of the VTK file, then calls writeASCIIData (binary VTK is currently unsupported)
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    template< class MeshT >
    bool writeMesh(std::ostream& _out, MeshT& _mesh );

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    template< class MeshT >
    bool writeASCIIData(std::ostream& _out, MeshT& _mesh );
};


#endif //FILEVTKPLUGIN_HH

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

#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif

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
    class CellType {

    public:

      CellType() :
              type(0),
              index(-1)
      {
      }

      // The type of the cell
      unsigned int type;

      // The index of the primitiv in the mesh that was created from this cell
      unsigned int index;

      // list of indices in the cell
      std::vector<quint32> indices;
    };

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


    /** \brief Adds a normal to the cell.
    *
    * The cell may refer to any kind of primitves described by the vtk standard
    * (vertex, face, triangle strip, etc)
    *
    * @param _mesh        Mesh to work on
    * @param _cell        Description of the cell including its type and its index in the mesh
    * @param _normal      The normal
    */
    template <typename MeshT>
    void addCellNormal(MeshT*& _mesh, CellType _cell, OpenMesh::Vec3d _normal);

    /** \brief Adds a tetra cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addTetraCellToOpenMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a hexa cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addHexaCellToOpenMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a wedge cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addWedgeCellToOpenMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a pyramid cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addPyramidCellToOpenMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addFaceToOpenMesh(MeshT*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addFaceToOpenMesh(MeshT*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Updates face normals.
    *
    *
    * @param _mesh        Mesh whose face normals should be updated
    */
    template <typename MeshT>
    void updateFaceNormalsOfOpenMesh(MeshT*& _mesh);

    /** \brief Updates vertex normals.
    *
    *
    * @param _mesh        Mesh whose vertex normals should be updated
    */
    template <typename MeshT>
    void updateVertexNormalsOfOpenMesh(MeshT*& _mesh);

    /** \brief Removed temporary properties that might have been added during file reading.
    *
    *
    * @param _mesh        Mesh whose temporary properties should be removed
    */
    template <typename MeshT>
    void removeTemporaryPropertiesOfOpenMesh(MeshT*& _mesh);

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    template <typename MeshT>
    void addVertexNormalToOpenMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    template <typename MeshT>
    void addFaceNormalToOpenMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Sets normals of duplicated vertices that were created for non-manifold meshes.
    *
    *
    * @param _mesh        Mesh to work on
    */
    template <typename MeshT>
    void setNormalsOfDuplicatedVerticesOfOpenMesh(MeshT*& _mesh);

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    template< class MeshT >
    bool writeASCIIDataOfOpenMesh(std::ostream& _out, MeshT& _mesh );

    /** \brief Adds a tetra cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addTetraCell(TriMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a tetra cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addTetraCell(PolyMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a hexa cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addHexaCell(TriMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a hexa cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addHexaCell(PolyMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a wedge cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addWedgeCell(TriMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a wedge cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addWedgeCell(PolyMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a pyramid cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addPyramidCell(TriMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a pyramid cell to the mesh. (Does nothing, yet)
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addPyramidCell(PolyMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(TriMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(PolyMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(TriMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(PolyMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Updates face normals.
    *
    *
    * @param _mesh        Mesh whose face normals should be updated
    */
    void updateFaceNormals(TriMesh*& _mesh);

    /** \brief Updates face normals.
    *
    *
    * @param _mesh        Mesh whose face normals should be updated
    */
    void updateFaceNormals(PolyMesh*& _mesh);

    /** \brief Updates vertex normals.
    *
    *
    * @param _mesh        Mesh whose vertex normals should be updated
    */
    void updateVertexNormals(TriMesh*& _mesh);

    /** \brief Updates vertex normals.
    *
    *
    * @param _mesh        Mesh whose vertex normals should be updated
    */
    void updateVertexNormals(PolyMesh*& _mesh);

    /** \brief Removed temporary properties that might have been added during file reading.
    *
    *
    * @param _mesh        Mesh whose temporary properties should be removed
    */
    void removeTemporaryProperties(TriMesh*& _mesh);

    /** \brief Removed temporary properties that might have been added during file reading.
    *
    *
    * @param _mesh        Mesh whose temporary properties should be removed
    */
    void removeTemporaryProperties(PolyMesh*& _mesh);

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    void addVertexNormal(TriMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    void addVertexNormal(PolyMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    void addFaceNormal(TriMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    void addFaceNormal(PolyMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Sets normals of duplicated vertices that were created for non-manifold meshes.
    *
    *
    * @param _mesh        Mesh to work on
    */
    void setNormalsOfDuplicatedVertices(TriMesh*& _mesh);

    /** \brief Sets normals of duplicated vertices that were created for non-manifold meshes.
    *
    *
    * @param _mesh        Mesh to work on
    */
    void setNormalsOfDuplicatedVertices(PolyMesh*& _mesh);

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    bool writeASCIIData(std::ostream& _out, TriMesh& _mesh);

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    bool writeASCIIData(std::ostream& _out, PolyMesh& _mesh);

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT
    /** \brief Adds a tetra cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addTetraCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a hexa cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addHexaCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a wedge cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addWedgeCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a pyramid cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addPyramidCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices);

    /** \brief Adds a face to the volume mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addFaceToOpenVolumeMesh(MeshT*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    template <typename MeshT>
    int addFaceToOpenVolumeMesh(MeshT*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    template <typename MeshT>
    void addVertexNormalToOpenVolumeMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    template <typename MeshT>
    void addFaceNormalToOpenVolumeMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    template< class MeshT >
    bool writeASCIIDataOfOpenVolumeMesh(std::ostream& _out, MeshT& _mesh );
#endif //ENABLE_OPENVOLUMEMESH_SUPPORT

#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    /** \brief Adds a tetra cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addTetraCell(HexahedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a hexa cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addHexaCell(HexahedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a wedge cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addWedgeCell(HexahedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a pyramid cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addPyramidCell(HexahedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the volume mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(HexahedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the volume mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(HexahedralMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Updates face normals.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose face normals should be updated
    */
    void updateFaceNormals(HexahedralMesh*& _mesh){/* don't need to be updated */};

    /** \brief Updates vertex normals.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose vertex normals should be updated
    */
    void updateVertexNormals(HexahedralMesh*& _mesh){/* don't need to be updated */};

    /** \brief Removed temporary properties that might have been added during file reading.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose temporary properties should be removed
    */
    void removeTemporaryProperties(HexahedralMesh*& _mesh){/* don't need to be removed */};

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    void addVertexNormal(HexahedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    void addFaceNormal(HexahedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Sets normals of duplicated vertices that were created for non-manifold meshes.
    *
    * Does nothing because volume mesh does not need to add vertices for non-manifold meshes.
    * Only added for compatibility.
    *
    * @param _mesh        Mesh to work on
    */
    void setNormalsOfDuplicatedVertices(HexahedralMesh*& _mesh){/* we didn't duplicate any vertices */};

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    bool writeASCIIData(std::ostream& _out, HexahedralMesh& _mesh);
#endif //ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    /** \brief Adds a tetra cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addTetraCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a hexa cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addHexaCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a wedge cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addWedgeCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a pyramid cell to the volume mesh.
    *
    *
    * @param _mesh        Mesh the cell is added to
    * @param _indices     Indices of the vertices of the cell, ordered according to vtk standard
    *
    * @return             index of the first added primitive
    */
    int addPyramidCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the volume mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _indices     Indices of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(PolyhedralMesh*& _mesh, std::vector<quint32> indices);

    /** \brief Adds a face to the volume mesh.
    *
    *
    * @param _mesh        Mesh the face is added to
    * @param _index1      First index of the vertices of the face, counter clockwise order
    * @param _index2      Second index of the vertices of the face, counter clockwise order
    * @param _index3      Third index of the vertices of the face, counter clockwise order
    *
    * @return             index of the first added primitive
    */
    int addFace(PolyhedralMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3);

    /** \brief Updates face normals.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose face normals should be updated
    */
    void updateFaceNormals(PolyhedralMesh*& _mesh){/* don't need to be updated */};

    /** \brief Updates vertex normals.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose vertex normals should be updated
    */
    void updateVertexNormals(PolyhedralMesh*& _mesh){/* don't need to be updated */};

    /** \brief Removed temporary properties that might have been added during file reading.
    *
    * Does nothing. Only added for compatibility.
    *
    * @param _mesh        Mesh whose temporary properties should be removed
    */
    void removeTemporaryProperties(PolyhedralMesh*& _mesh){/* don't need to be removed */};

    /** \brief Adds a vertex normal.
    *
    *
    * @param _mesh        Mesh the vertex normal is added to
    * @param _index       Index of the vertex
    * @param _normal      Normal that is added
    */
    void addVertexNormal(PolyhedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Adds a face normal.
    *
    *
    * @param _mesh        Mesh the face normal is added to
    * @param _index       Index of the face
    * @param _normal      Normal that is added
    */
    void addFaceNormal(PolyhedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal);

    /** \brief Sets normals of duplicated vertices that were created for non-manifold meshes.
    *
    * Does nothing because volume mesh does not need to add vertices for non-manifold meshes.
    * Only added for compatibility.
    *
    * @param _mesh        Mesh to work on
    */
    void setNormalsOfDuplicatedVertices(PolyhedralMesh*& _mesh){/* we didn't duplicate any vertices */};

    /** \brief Writes the data of the VTK file in ASCII format
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    bool writeASCIIData(std::ostream& _out, PolyhedralMesh& _mesh);
#endif //ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT


    enum BestMeshType {
        BMT_None           = 0,
        BMT_TriMesh        = 1,
        BMT_PolyMesh       = 1 << 1,
        BMT_HexahedralMesh = 1 << 2,
        BMT_PolyhedralMesh = 1 << 3
    };

    /// Reads the file to check for present primitives and returns the object type that fits best
    BestMeshType findBestObjectType(QString _filename);

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
    *
    * @return idx of first added face
    */
    template <typename MeshT>
    int add_non_manifold_face(MeshT*& _mesh, std::vector< OpenMesh::VertexHandle >& _vhandles);

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
    bool loadMeshCells(QString _spec,QTextStream& _in,MeshT*& _mesh, std::vector<CellType>& _cells);

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
    bool loadMeshNormals(QString _spec,QTextStream& _in,MeshT*& _mesh, std::vector<CellType>& _cells, bool _pointNormal, quint32 _count);

    /** \brief Writes the header of the VTK file, then calls writeASCIIData (binary VTK is currently unsupported)
    *
    *
    * @param _out         Textstream to write the file
    * @param _mesh        Mesh to work on
    */
    template< class MeshT >
    bool writeMesh(std::ostream& _out, MeshT& _mesh );

};


#endif //FILEVTKPLUGIN_HH

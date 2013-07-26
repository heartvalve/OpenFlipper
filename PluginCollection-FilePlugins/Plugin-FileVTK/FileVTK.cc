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
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FileVTK.hh"

#include <iostream>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif

//-----------------------------------------------------------------------------------------------------

/// Constructor
FileVTKPlugin::FileVTKPlugin():
  forceTriangleMesh_(false),
  forcePolyMesh_(false),
  saveOptions_(0),
  saveBinary_(0),
  saveFaceNormals_(0),
  saveVertexNormals_(0),
  saveVertexTexCoords_(0),
  savePrecisionLabel_(0),
  savePrecision_(0),
  saveDefaultButton_(),
  binary_(false),
  userWriteOptions_(0)
{
}

//-----------------------------------------------------------------------------------------------------

void FileVTKPlugin::initializePlugin() {

    if(OpenFlipperSettings().value("FileVtk/Save/Binary",false).toBool())
        userWriteOptions_ |= BINARY; //currently unsupported
    if(OpenFlipperSettings().value("FileVtk/Save/FaceNormals",true).toBool())
        userWriteOptions_ |= FACENORMALS;
    if(OpenFlipperSettings().value("FileVtk/Save/VertexNormals",true).toBool())
        userWriteOptions_ |= VERTEXNORMALS;
    if(OpenFlipperSettings().value("FileVtk/Save/VertexTexCoords",true).toBool())
        userWriteOptions_ |= VERTEXTEXCOORDS;
}

//-----------------------------------------------------------------------------------------------------

QString FileVTKPlugin::getLoadFilters() {
    return QString( tr("Visualization Toolkit ASCII ( *.vtk )") );
}

//-----------------------------------------------------------------------------------------------------

QString FileVTKPlugin::getSaveFilters() {
   return QString( tr("Visualization Toolkit ASCII ( *.vtk )") );
}

//-----------------------------------------------------------------------------------------------------

DataType  FileVTKPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH | DATA_GROUP;

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    type |= DATA_POLYHEDRAL_MESH;
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    type |= DATA_HEXAHEDRAL_MESH;
#endif

    return type;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileVTKPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
  return 0;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileVTKPlugin::saveOptionsWidget(QString /*_currentFilter*/) {

    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);

        saveBinary_ = new QCheckBox("Save Binary");
        layout->addWidget(saveBinary_);
        saveBinary_->setCheckable(false);

        saveFaceNormals_ = new QCheckBox("Save Face Normals");
        layout->addWidget(saveFaceNormals_);

        saveVertexNormals_ = new QCheckBox("Save Vertex Normals");
        layout->addWidget(saveVertexNormals_);

        saveVertexTexCoords_ = new QCheckBox("Save Vertex TexCoords");
        layout->addWidget(saveVertexTexCoords_);

        savePrecisionLabel_ = new QLabel("Writer Precision");
        layout->addWidget(savePrecisionLabel_);

        savePrecision_ = new QSpinBox();
        savePrecision_->setMinimum(1);
        savePrecision_->setMaximum(12);
        savePrecision_->setValue(6);
        layout->addWidget(savePrecision_);

        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);

        saveOptions_->setLayout(layout);

        connect(saveBinary_, SIGNAL(clicked(bool)), savePrecision_, SLOT(setDisabled(bool)));
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));

        saveBinary_->setChecked( OpenFlipperSettings().value("FileVtk/Save/Binary",false).toBool() );
        saveFaceNormals_->setChecked( OpenFlipperSettings().value("FileVtk/Save/FaceNormals",true).toBool() );
        saveVertexNormals_->setChecked( OpenFlipperSettings().value("FileVtk/Save/VertexNormals",true).toBool() );
        saveVertexTexCoords_->setChecked( OpenFlipperSettings().value("FileVtk/Save/VertexTexCoords",true).toBool() );
    }

    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

void FileVTKPlugin::slotSaveDefault() {
  OpenFlipperSettings().setValue( "FileVtk/Save/Binary",          saveBinary_->isChecked()  );
  OpenFlipperSettings().setValue( "FileVtk/Save/FaceNormals",     saveFaceNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileVtk/Save/VertexNormals",   saveVertexNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileVtk/Save/VertexTexCoords", saveVertexTexCoords_->isChecked()  );
}

//-----------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-------------------------------------- cool helper function -----------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------


template <typename MeshT>
int FileVTKPlugin::addTetraCellToOpenMesh(MeshT /*_mesh*/, std::vector<quint32> /*_indices*/)
{
    emit log(LOGWARN,tr("Unsupported Cell Type TETRA") );
    return -1;
}

template <typename MeshT>
int FileVTKPlugin::addHexaCellToOpenMesh(MeshT /*_mesh*/, std::vector<quint32> /*_indices*/)
{
    emit log(LOGWARN,tr("Unsupported Cell Type HEXAHEDRON") );
    return -1;
}

template <typename MeshT>
int FileVTKPlugin::addWedgeCellToOpenMesh(MeshT /*_mesh*/, std::vector<quint32> /*_indices*/)
{
    emit log(LOGWARN,tr("Unsupported Cell Type WEDGE") );
    return -1;
}

template <typename MeshT>
int FileVTKPlugin::addPyramidCellToOpenMesh(MeshT /*_mesh*/, std::vector<quint32> /*_indices*/)
{
    emit log(LOGWARN,tr("Unsupported Cell Type PYRAMID") );
    return -1;
}

template <typename MeshT>
int FileVTKPlugin::addFaceToOpenMesh(MeshT*& _mesh, std::vector<quint32> _indices)
{
    std::vector<OpenMesh::VertexHandle> handles;
    for (std::vector<quint32>::const_iterator it = _indices.begin(); it != _indices.end(); ++it)
        handles.push_back(_mesh->vertex_handle(*it));

    OpenMesh::FaceHandle fh = _mesh->add_face(handles);

    // Try the other direction
    if ( ! fh.is_valid() ) {

        std::vector< OpenMesh::VertexHandle > inverseHandles;
        for ( int i = handles.size()-1 ; i >= 0 ; --i)
            inverseHandles.push_back(handles[i]);

        fh = _mesh->add_face(inverseHandles);

        if ( !fh.is_valid() ) {
            return add_non_manifold_face(_mesh, handles);
        }
    }
    return fh.idx();
}

template <typename MeshT>
int FileVTKPlugin::addFaceToOpenMesh(MeshT*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3)
{
    OpenMesh::VertexHandle v1 = _mesh->vertex_handle(_index1);
    OpenMesh::VertexHandle v2 = _mesh->vertex_handle(_index2);
    OpenMesh::VertexHandle v3 = _mesh->vertex_handle(_index3);
    return _mesh->add_face(v1,v2,v3).idx();
}

template <typename MeshT>
void FileVTKPlugin::updateFaceNormalsOfOpenMesh(MeshT*& _mesh)
{
    _mesh->update_face_normals();
}

void FileVTKPlugin::updateFaceNormals(TriMesh*& _mesh)        { updateFaceNormalsOfOpenMesh(_mesh);       }
void FileVTKPlugin::updateFaceNormals(PolyMesh*& _mesh)       { updateFaceNormalsOfOpenMesh(_mesh);       }


template <typename MeshT>
void FileVTKPlugin::updateVertexNormalsOfOpenMesh(MeshT*& _mesh)
{
    _mesh->update_vertex_normals();
}

template <typename MeshT>
void FileVTKPlugin::removeTemporaryPropertiesOfOpenMesh(MeshT*& _mesh)
{
    //if we added this property temporarily, we need to remove it now
    OpenMesh::VPropHandleT< OpenMesh::VertexHandle > originalVertexIdx;
    if (! _mesh->get_property_handle(originalVertexIdx,"FileVTKPlugin_originalVertexIdx")) {
      _mesh->remove_property( originalVertexIdx );
    }
}

template <typename MeshT>
void FileVTKPlugin::addVertexNormalToOpenMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal)
{
    OpenMesh::VertexHandle vh = _mesh->vertex_handle(_index);
    if ( vh.is_valid() )
      _mesh->set_normal( vh , _normal );
}


template <typename MeshT>
void FileVTKPlugin::addFaceNormalToOpenMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal)
{
    OpenMesh::FaceHandle fh = _mesh->face_handle(_index);
    if ( fh.is_valid() )
       _mesh->set_normal( fh, _normal );
}

template <typename MeshT>
void FileVTKPlugin::addCellNormal(MeshT*& _mesh, CellType _cell, OpenMesh::Vec3d _normal)
{
    if ((_cell.type <= 4) || (_cell.type >= 10))
    {
        // vertex, poly vertex, line, poly line, tetra, voxel, hexahedron, wedge, pyramid, and quadratic stuff
        emit log(LOGWARN,tr("Normals not supported for type %1").arg(_cell.type) );
    }
    else if (_cell.type == 5)
    {
        // Triangle
        addFaceNormal(_mesh, _cell.index, _normal);
    }
    else if (_cell.type == 6)
    {
        // Triangle strip
        int numberOfTriangles = _cell.indices.size() - 2;
        // we assume here that the triangles have contigous indices as they were freshly created while loading the file
        for (int i = 0; i < numberOfTriangles; i++)
            addFaceNormal(_mesh, _cell.index + i, _normal);
    }
    else if ((_cell.type >= 7) &&  (_cell.type <= 9))
    {
        // polygon, pixel, quad
        if (forceTriangleMesh_)
        {
            // _mesh is a TriMesh even if cell represents a polygon with more than 3 vertices
            // for more than 3 vertices the polygon will be split into triangles
            // we have to set the normal for all these triangles
            // we assume again that the triangles have contigous indices as they were freshly created while loading the file
            int numberOfTriangles = _cell.indices.size() - 2;
            for (int i = 0; i < numberOfTriangles; i++)
                addFaceNormal(_mesh, _cell.index + i, _normal);
        }
        else
        {
            // Here _mesh might also be a TriMesh. But this is only the case if all polygons are triangles
            // so we only need to set a single normal
            addFaceNormal(_mesh, _cell.index , _normal);
        }
    }
}

template <typename MeshT>
void FileVTKPlugin::setNormalsOfDuplicatedVerticesOfOpenMesh(MeshT*& _mesh)
{
    //iterate over all contained vertices and set the normals
    OpenMesh::VPropHandleT< OpenMesh::VertexHandle > originalVertexIdx;
    if (! _mesh->get_property_handle(originalVertexIdx,"FileVTKPlugin_originalVertexIdx")) {
      //TODO: check if this is correct;
      //if the property was not found, there are no copied vertices and we can leave
       return;
      //old and maybe unnecessary:  _mesh->add_property( originalVertexIdx, "FileVTKPlugin_originalVertexIdx" );
    }

    typename MeshT::VertexIter vit = _mesh->vertices_begin();
    typename MeshT::VertexIter vend = _mesh->vertices_end();

    for(; vit != vend; ++vit) {
      if ( _mesh->property(originalVertexIdx, vit).is_valid() ) {
          //copied vertex found
          _mesh->set_normal( vit, _mesh->normal(_mesh->property (originalVertexIdx, vit) ) );
      }
    }
}

template< class MeshT >
bool FileVTKPlugin::writeASCIIDataOfOpenMesh(std::ostream& _out, MeshT& _mesh ) {

    if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
        _out.precision(savePrecision_->value());

    _out << "DATASET POLYDATA\n";

    typename MeshT::Point p;
    typename MeshT::Normal n;
    typename OpenMesh::Vec4f c;
    typename MeshT::TexCoord2D t;

    typename MeshT::VertexIter vit = _mesh.vertices_begin();
    typename MeshT::VertexIter end_vit = _mesh.vertices_end();
    typename MeshT::FaceIter fit = _mesh.faces_begin();
    typename MeshT::FaceIter end_fit = _mesh.faces_end();
    typename MeshT::FaceVertexIter fvit;

    int total_face_vertices = 0;
    for (; fit != end_fit; ++fit) {
        total_face_vertices += _mesh.valence(fit.handle());
    }
    // Write vertex data
    _out << "POINTS " << _mesh.n_vertices() << " float\n";
    for (; vit != end_vit; ++vit) {
        // Write vertex p[0] p[1] p[2]
        p = _mesh.point(vit.handle());
        _out << p[0] << " " << p[1] << " " << p[2];
        _out << "\n";
    }

    // Write face connectivity
    _out << "POLYGONS "<< _mesh.n_faces() << " " << total_face_vertices << "\n";
    for (fit = _mesh.faces_begin(); fit != end_fit; ++fit) {
        // Write face valence
        _out << _mesh.valence(fit.handle());

        // Get face-vertex iterator
        fvit = _mesh.fv_iter(fit.handle());

        // Write vertex indices
        for (;fvit; ++fvit) {
            _out << " " << fvit.handle().idx();
        }
        _out << "\n";
    }

    //*************//
    //OPTIONAL DATA//
    //*************//

    /*** Write face attributes ***/
    _out << "CELL_DATA "<< _mesh.n_faces() << "\n";

    //Write face normals
    if (_mesh.has_face_normals() && (userWriteOptions_ & FileVTKPlugin::FACENORMALS)) {
        _out << "NORMALS faceNormals float\n";
        for (fit = _mesh.faces_begin(); fit != end_fit; ++fit) {
            n = _mesh.normal(fit.handle());
            _out << n[0] << " " << n[1] << " " << n[2];
            _out << "\n";
        }
    }

    /*** Write vertex attributes ***/
    _out << "POINT_DATA "<< _mesh.n_vertices() << "\n";

    //Write vertex normals
    if (_mesh.has_vertex_normals() && (userWriteOptions_ & FileVTKPlugin::VERTEXNORMALS)) {
        _out << "NORMALS vertexNormals float\n";
        for (vit = _mesh.vertices_begin(); vit != end_vit; ++vit) {
            n = _mesh.normal(vit.handle());
            _out << n[0] << " " << n[1] << " " << n[2];
            _out << "\n";
        }
    }

    // Write vertex texcoords (only 2D for now)
    if (_mesh.has_vertex_texcoords2D() && (userWriteOptions_ & FileVTKPlugin::VERTEXTEXCOORDS)) {
        _out << "TEXTURE_COORDINATES vertexTexcoords 2 float\n";
        for (vit = _mesh.vertices_begin(); vit != end_vit; ++vit) {
            t = _mesh.texcoord2D(vit.handle());
            _out << t[0] << " " << t[1];
            _out << "\n";
        }
    }

    return true;
}

int FileVTKPlugin::addTetraCell(TriMesh*& _mesh, std::vector<quint32> _indices)                 { return addTetraCellToOpenMesh(_mesh, _indices);             }
int FileVTKPlugin::addTetraCell(PolyMesh*& _mesh, std::vector<quint32> _indices)                { return addTetraCellToOpenMesh(_mesh, _indices);             }

int FileVTKPlugin::addHexaCell(TriMesh*& _mesh, std::vector<quint32> _indices)                  { return addHexaCellToOpenMesh(_mesh, _indices);              }
int FileVTKPlugin::addHexaCell(PolyMesh*& _mesh, std::vector<quint32> _indices)                 { return addHexaCellToOpenMesh(_mesh, _indices);              }

int FileVTKPlugin::addWedgeCell(TriMesh*& _mesh, std::vector<quint32> _indices)                 { return addWedgeCellToOpenMesh(_mesh, _indices);             }
int FileVTKPlugin::addWedgeCell(PolyMesh*& _mesh, std::vector<quint32> _indices)                { return addWedgeCellToOpenMesh(_mesh, _indices);             }

int FileVTKPlugin::addPyramidCell(TriMesh*& _mesh, std::vector<quint32> _indices)               { return addPyramidCellToOpenMesh(_mesh, _indices);           }
int FileVTKPlugin::addPyramidCell(PolyMesh*& _mesh, std::vector<quint32> _indices)              { return addPyramidCellToOpenMesh(_mesh, _indices);           }

int FileVTKPlugin::addFace(TriMesh*& _mesh, std::vector<quint32> _indices)                      { return addFaceToOpenMesh(_mesh, _indices);                  }
int FileVTKPlugin::addFace(PolyMesh*& _mesh, std::vector<quint32> _indices)                     { return addFaceToOpenMesh(_mesh, _indices);                  }

int FileVTKPlugin::addFace(TriMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3)  { return addFaceToOpenMesh(_mesh, _index1, _index2, _index3); }
int FileVTKPlugin::addFace(PolyMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3) { return addFaceToOpenMesh(_mesh, _index1, _index2, _index3); }

void FileVTKPlugin::updateVertexNormals(TriMesh*& _mesh)                                        { updateVertexNormalsOfOpenMesh(_mesh);                       }
void FileVTKPlugin::updateVertexNormals(PolyMesh*& _mesh)                                       { updateVertexNormalsOfOpenMesh(_mesh);                       }

void FileVTKPlugin::removeTemporaryProperties(TriMesh*& _mesh)                                  { removeTemporaryPropertiesOfOpenMesh(_mesh);                 }
void FileVTKPlugin::removeTemporaryProperties(PolyMesh*& _mesh)                                 { removeTemporaryPropertiesOfOpenMesh(_mesh);                 }

void FileVTKPlugin::addVertexNormal(TriMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)   { addVertexNormalToOpenMesh(_mesh, _index, _normal);          }
void FileVTKPlugin::addVertexNormal(PolyMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)  { addVertexNormalToOpenMesh(_mesh, _index, _normal);          }

void FileVTKPlugin::addFaceNormal(TriMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)     { addFaceNormalToOpenMesh(_mesh, _index, _normal);            }
void FileVTKPlugin::addFaceNormal(PolyMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)    { addFaceNormalToOpenMesh(_mesh, _index, _normal);            }

void FileVTKPlugin::setNormalsOfDuplicatedVertices(TriMesh*& _mesh)                             { setNormalsOfDuplicatedVerticesOfOpenMesh(_mesh);            }
void FileVTKPlugin::setNormalsOfDuplicatedVertices(PolyMesh*& _mesh)                            { setNormalsOfDuplicatedVerticesOfOpenMesh(_mesh);            }

bool FileVTKPlugin::writeASCIIData(std::ostream &_out, TriMesh &_mesh)                          { return writeASCIIDataOfOpenMesh(_out, _mesh);               }
bool FileVTKPlugin::writeASCIIData(std::ostream &_out, PolyMesh &_mesh)                         { return writeASCIIDataOfOpenMesh(_out, _mesh);               }


#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

template <typename MeshT>
int FileVTKPlugin::addTetraCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices)
{
    if (_indices.size() != 4)
    {   /*emit log(LOGWARN,tr("Unsupported Cell Type Tetra") );*/ }

    std::vector< std::vector<OpenVolumeMesh::VertexHandle> > faces;
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[0].push_back(_indices[0]);
    faces[0].push_back(_indices[1]);
    faces[0].push_back(_indices[2]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[1].push_back(_indices[0]);
    faces[1].push_back(_indices[3]);
    faces[1].push_back(_indices[1]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[2].push_back(_indices[1]);
    faces[2].push_back(_indices[3]);
    faces[2].push_back(_indices[2]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[3].push_back(_indices[0]);
    faces[3].push_back(_indices[2]);
    faces[3].push_back(_indices[3]);

    std::vector<OpenVolumeMesh::HalfFaceHandle> halffacehandles;
    for (unsigned int i = 0; i < faces.size(); i++)
        halffacehandles.push_back(_mesh->halfface(faces[i]));

    for (unsigned int i = 0; i < halffacehandles.size(); i++)
        if (halffacehandles[i] == OpenVolumeMesh::HalfFaceHandle(-1))
        {
            // face didn't exist
            OpenVolumeMesh::FaceHandle fh = _mesh->add_face(faces[i]);
            halffacehandles[i] = _mesh->halfface_handle(fh, 0);
        }

    return _mesh->add_cell(halffacehandles).idx();

}

template <typename MeshT>
int FileVTKPlugin::addHexaCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices)
{
    if (_indices.size() != 8)
    {   emit log(LOGWARN,tr("Expected 8 indices to add Hexahedron but got %1").arg(_indices.size()) ); }

    std::vector< std::vector<OpenVolumeMesh::VertexHandle> > faces;
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[0].push_back(_indices[0]);
    faces[0].push_back(_indices[1]);
    faces[0].push_back(_indices[2]);
    faces[0].push_back(_indices[3]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[1].push_back(_indices[0]);
    faces[1].push_back(_indices[4]);
    faces[1].push_back(_indices[5]);
    faces[1].push_back(_indices[1]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[2].push_back(_indices[0]);
    faces[2].push_back(_indices[3]);
    faces[2].push_back(_indices[7]);
    faces[2].push_back(_indices[4]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[3].push_back(_indices[1]);
    faces[3].push_back(_indices[5]);
    faces[3].push_back(_indices[6]);
    faces[3].push_back(_indices[2]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[4].push_back(_indices[2]);
    faces[4].push_back(_indices[6]);
    faces[4].push_back(_indices[7]);
    faces[4].push_back(_indices[3]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[5].push_back(_indices[4]);
    faces[5].push_back(_indices[7]);
    faces[5].push_back(_indices[6]);
    faces[5].push_back(_indices[5]);

    std::vector<OpenVolumeMesh::HalfFaceHandle> halffacehandles;
    for (unsigned int i = 0; i < faces.size(); i++)
        halffacehandles.push_back(_mesh->halfface(faces[i]));

    for (unsigned int i = 0; i < halffacehandles.size(); i++)
        if (halffacehandles[i] == OpenVolumeMesh::HalfFaceHandle(-1))
        {
            // face didn't exist
            OpenVolumeMesh::FaceHandle fh = _mesh->add_face(faces[i]);
            halffacehandles[i] = _mesh->halfface_handle(fh, 0);
        }

    return _mesh->add_cell(halffacehandles).idx();
}

template <typename MeshT>
int FileVTKPlugin::addWedgeCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices)
{
    if (_indices.size() != 6)
    {   emit log(LOGWARN,tr("Expected 6 indices to add Hexahedron but got %1").arg(_indices.size()) ); }

    std::vector< std::vector<OpenVolumeMesh::VertexHandle> > faces;
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[0].push_back(_indices[0]);
    faces[0].push_back(_indices[1]);
    faces[0].push_back(_indices[4]);
    faces[0].push_back(_indices[3]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[1].push_back(_indices[0]);
    faces[1].push_back(_indices[2]);
    faces[1].push_back(_indices[1]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[2].push_back(_indices[3]);
    faces[2].push_back(_indices[4]);
    faces[2].push_back(_indices[5]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[3].push_back(_indices[0]);
    faces[3].push_back(_indices[3]);
    faces[3].push_back(_indices[5]);
    faces[3].push_back(_indices[2]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[4].push_back(_indices[1]);
    faces[4].push_back(_indices[2]);
    faces[4].push_back(_indices[5]);
    faces[4].push_back(_indices[4]);

    std::vector<OpenVolumeMesh::HalfFaceHandle> halffacehandles;
    for (unsigned int i = 0; i < faces.size(); i++)
        halffacehandles.push_back(_mesh->halfface(faces[i]));

    for (unsigned int i = 0; i < halffacehandles.size(); i++)
        if (halffacehandles[i] == OpenVolumeMesh::HalfFaceHandle(-1))
        {
            // face didn't exist
            OpenVolumeMesh::FaceHandle fh = _mesh->add_face(faces[i]);
            halffacehandles[i] = _mesh->halfface_handle(fh, 0);
        }

    return _mesh->add_cell(halffacehandles).idx();
}

template <typename MeshT>
int FileVTKPlugin::addPyramidCellToOpenVolumeMesh(MeshT _mesh, std::vector<quint32> _indices)
{
    if (_indices.size() != 6)
    {   emit log(LOGWARN,tr("Expected 6 indices to add Hexahedron but got %1").arg(_indices.size()) ); }

    std::vector< std::vector<OpenVolumeMesh::VertexHandle> > faces;
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[0].push_back(_indices[0]);
    faces[0].push_back(_indices[1]);
    faces[0].push_back(_indices[2]);
    faces[0].push_back(_indices[3]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[1].push_back(_indices[0]);
    faces[1].push_back(_indices[4]);
    faces[1].push_back(_indices[1]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[2].push_back(_indices[0]);
    faces[2].push_back(_indices[3]);
    faces[2].push_back(_indices[4]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[3].push_back(_indices[2]);
    faces[3].push_back(_indices[4]);
    faces[3].push_back(_indices[3]);
    faces.push_back(std::vector<OpenVolumeMesh::VertexHandle>());
    faces[4].push_back(_indices[1]);
    faces[4].push_back(_indices[4]);
    faces[4].push_back(_indices[2]);

    std::vector<OpenVolumeMesh::HalfFaceHandle> halffacehandles;
    for (unsigned int i = 0; i < faces.size(); i++)
        halffacehandles.push_back(_mesh->halfface(faces[i]));

    for (unsigned int i = 0; i < halffacehandles.size(); i++)
        if (halffacehandles[i] == OpenVolumeMesh::HalfFaceHandle(-1))
        {
            // face didn't exist
            OpenVolumeMesh::FaceHandle fh = _mesh->add_face(faces[i]);
            halffacehandles[i] = _mesh->halfface_handle(fh, 0);
        }

    return _mesh->add_cell(halffacehandles).idx();
}

template <typename MeshT>
int FileVTKPlugin::addFaceToOpenVolumeMesh(MeshT*& _mesh, std::vector<quint32> _indices)
{
    if (_indices.size() < 3)
    {   emit log(LOGWARN,tr("Expected at least 3 indices to add a face but got %1").arg(_indices.size()) ); }

    std::vector<OpenVolumeMesh::VertexHandle> face;
    for (unsigned int i = 0; i < _indices.size(); i++)
        face.push_back(OpenVolumeMesh::VertexHandle(_indices[i]));


    OpenVolumeMesh::HalfFaceHandle halffacehandle;
    halffacehandle = _mesh->halfface(face);
    if (halffacehandle == -1) // face didn't exist
    {
        OpenVolumeMesh::FaceHandle fh = _mesh->add_face(face).idx();
        halffacehandle = _mesh->halfface_handle(fh, 0);
    }
    return halffacehandle.idx();
}

template <typename MeshT>
int FileVTKPlugin::addFaceToOpenVolumeMesh(MeshT*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3)
{
    std::vector<OpenVolumeMesh::VertexHandle> face;
    face.push_back(OpenVolumeMesh::VertexHandle(_index1));
    face.push_back(OpenVolumeMesh::VertexHandle(_index2));
    face.push_back(OpenVolumeMesh::VertexHandle(_index3));

    OpenVolumeMesh::HalfFaceHandle halffacehandle;
    halffacehandle = _mesh->halfface(face);
    if (halffacehandle == -1) // face didn't exist
    {
        OpenVolumeMesh::FaceHandle fh = _mesh->add_face(face).idx();
        halffacehandle = _mesh->halfface_handle(fh, 0);
    }
    return halffacehandle.idx();
}

template <typename MeshT>
void FileVTKPlugin::addVertexNormalToOpenVolumeMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal)
{
    OpenVolumeMesh::VertexPropertyT< typename OpenMesh::Vec3d > vertexNormals = _mesh->template request_vertex_property< typename OpenMesh::Vec3d >("vertex_normals");
    vertexNormals[OpenVolumeMesh::VertexHandle(_index)] = _normal;
}

template <typename MeshT>
void FileVTKPlugin::addFaceNormalToOpenVolumeMesh(MeshT _mesh, quint32 _index, OpenMesh::Vec3d _normal)
{
    OpenVolumeMesh::FacePropertyT< OpenMesh::Vec3d > faceNormals = _mesh->template request_face_property< OpenMesh::Vec3d >("face_normals");

    OpenVolumeMesh::HalfFaceHandle hfh = OpenVolumeMesh::HalfFaceHandle(_index);
    OpenVolumeMesh::FaceHandle      fh = _mesh->face_handle(hfh);
    if (hfh == _mesh->halfface_handle(fh, 0))
        faceNormals[_mesh->face_handle(_index)] = _normal;
    else
        faceNormals[_mesh->face_handle(_index)] = -_normal;

}

template <typename MeshT>
int getCellType(MeshT& _mesh, OpenVolumeMesh::CellHandle _cellHandle)
{
    int incidentFaces = _mesh.cell(_cellHandle).halffaces().size();
    int incidentVertices = 0;
    for (OpenVolumeMesh::CellVertexIter cvit = OpenVolumeMesh::CellVertexIter(_cellHandle, &_mesh); cvit.valid(); ++cvit)
        incidentVertices++;

    if      ((incidentFaces == 4) && (incidentVertices == 4))
        return 10; // tetra
    else if ((incidentFaces == 6) && (incidentVertices == 8))
        return 12; // hexahedron
    else if ((incidentFaces == 5) && (incidentVertices == 6))
        return 13; // wedge
    else if ((incidentFaces == 5) && (incidentVertices == 5))
        return 14; // pyramid
    else
        return -1;
}

template <typename MeshT>
int getCellType(MeshT& _mesh, OpenVolumeMesh::FaceHandle _faceHandle)
{
    std::vector<OpenVolumeMesh::HalfEdgeHandle> halfedges = _mesh.face(_faceHandle).halfedges();
    if (halfedges.size() == 3)
        return 5; // triangle;
    else
        return 7; // polygon
}

template <typename MeshT>
std::vector<int> getCellVertexIndices(MeshT& _mesh, OpenVolumeMesh::CellHandle _cellHandle, int _cellType)
{
    if (_cellType == 10) //tetra
    {
        // first three vertices are the three vertices of an arbitrary face
        // the last vertex is the one that is left
        OpenVolumeMesh::OpenVolumeMeshCell cell = _mesh.cell(_cellHandle);
        OpenVolumeMesh::OpenVolumeMeshFace face = _mesh.halfface(cell.halffaces()[0]);
        std::vector<OpenVolumeMesh::HalfEdgeHandle> halfedges = face.halfedges();
        std::vector<int> indices;
        for (unsigned int i = 0; i < halfedges.size(); i++)
            indices.push_back(_mesh.halfedge(halfedges[i]).from_vertex().idx());
        for (OpenVolumeMesh::CellVertexIter cvit = OpenVolumeMesh::CellVertexIter(_cellHandle, &_mesh); cvit.valid(); ++cvit)
            if ((cvit->idx() != indices[0]) && ((*cvit).idx() != indices[1]) && ((*cvit).idx() != indices[2]))
                indices.push_back(cvit->idx());
        return indices;
    }
    else if (_cellType == 12) // hexahedron
    {
        // take an arbitrary face and add its vertices
        OpenVolumeMesh::OpenVolumeMeshCell cell = _mesh.cell(_cellHandle);
        OpenVolumeMesh::HalfFaceHandle hfh = cell.halffaces()[0];
        OpenVolumeMesh::OpenVolumeMeshFace face = _mesh.halfface(hfh);
        OpenVolumeMesh::HalfEdgeHandle heh = face.halfedges()[0];
        std::vector<int> indices;
        for (unsigned int i = 0; i < 4; i++)
        {
            OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.halfedge(heh);
            indices.push_back(edge.from_vertex().idx());
            heh = _mesh.next_halfedge_in_halfface(heh, hfh);
        }
        // we now added four vertices and heh is a handle to the edge coming out of the first added vertex
        // we will now navigate to the oppsite face
        OpenVolumeMesh::HalfFaceHandle intermediateHalfFaceHandle = _mesh.adjacent_halfface_in_cell(hfh, heh);
        heh = _mesh.opposite_halfedge_handle(heh);
        heh = _mesh.next_halfedge_in_halfface(heh, intermediateHalfFaceHandle);
        heh = _mesh.next_halfedge_in_halfface(heh, intermediateHalfFaceHandle);
        OpenVolumeMesh::HalfFaceHandle oppositeHalfFaceHandle = _mesh.adjacent_halfface_in_cell(intermediateHalfFaceHandle, heh);
        heh = _mesh.opposite_halfedge_handle(heh);
        // heh is now the halfedge pointing to vertex 4 and lies in the opposite half face.
        // we now have to add the four vertices of that face but in clockwise order
        for (unsigned int i = 0; i < 4; i++)
        {
            OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.halfedge(heh);
            indices.push_back(edge.to_vertex().idx());
            heh = _mesh.prev_halfedge_in_halfface(heh, oppositeHalfFaceHandle);
        }
        return indices;
    }
    else if (_cellType == 13) // wedge
    {
        // take a face with three vertices and add its vertices in clockwise order
        OpenVolumeMesh::OpenVolumeMeshCell cell = _mesh.cell(_cellHandle);
        OpenVolumeMesh::HalfFaceHandle hfh;
        for (unsigned int i = 0; i < cell.halffaces().size(); i++)
            if (_mesh.halfface(cell.halffaces()[i]).halfedges().size() == 3)
                hfh = cell.halffaces()[i];
        OpenVolumeMesh::OpenVolumeMeshFace face = _mesh.halfface(hfh);
        OpenVolumeMesh::HalfEdgeHandle heh = face.halfedges()[0];
        std::vector<int> indices;
        for (unsigned int i = 0; i < 3; i++)
        {
            OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.halfedge(heh);
            indices.push_back(edge.from_vertex().idx());
            heh = _mesh.prev_halfedge_in_halfface(heh, hfh);
        }
        // we now added three vertices and heh is a handle to the edge coming out of the first added vertex
        // we will now navigate to the oppsite face
        OpenVolumeMesh::HalfFaceHandle intermediateHalfFaceHandle = _mesh.adjacent_halfface_in_cell(hfh, heh);
        heh = _mesh.opposite_halfedge_handle(heh);
        heh = _mesh.next_halfedge_in_halfface(heh, intermediateHalfFaceHandle);
        heh = _mesh.next_halfedge_in_halfface(heh, intermediateHalfFaceHandle);
        OpenVolumeMesh::HalfFaceHandle oppositeHalfFaceHandle = _mesh.adjacent_halfface_in_cell(intermediateHalfFaceHandle, heh);
        heh = _mesh.opposite_halfedge_handle(heh);
        // heh is now the halfedge pointing to vertex 3 and lies in the opposite half face.
        // we now have to add the three vertices of that face but in counter clockwise order
        for (unsigned int i = 0; i < 3; i++)
        {
            OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.halfedge(heh);
            indices.push_back(edge.to_vertex().idx());
            heh = _mesh.next_halfedge_in_halfface(heh, oppositeHalfFaceHandle);
        }
        return indices;

    }
    else if (_cellType == 14) // pyramid
    {
        // the first four vertices are the for vertices of the face with for vertices
        // the other vertex is the one that is left
        OpenVolumeMesh::OpenVolumeMeshCell cell = _mesh.cell(_cellHandle);
        std::vector<OpenVolumeMesh::HalfFaceHandle> halffaces = cell.halffaces();
        OpenVolumeMesh::OpenVolumeMeshFace face = _mesh.halfface(halffaces[0]);
        for (unsigned int i = 0; i < halffaces.size(); i++)
            if (_mesh.halfface(halffaces[i]).halfedges().size() == 4)
                face = _mesh.halfface(halffaces[i]);
        std::vector<OpenVolumeMesh::HalfEdgeHandle> halfedges = face.halfedges();
        std::vector<int> indices;
        for (unsigned int i = 0; i < halfedges.size(); i++)
            indices.push_back(_mesh.halfedge(halfedges[i]).from_vertex().idx());
        for (OpenVolumeMesh::CellVertexIter cvit = OpenVolumeMesh::CellVertexIter(_cellHandle, &_mesh); cvit.valid(); ++cvit)
            if ((cvit->idx() != indices[0]) && ((*cvit).idx() != indices[1]) && ((*cvit).idx() != indices[2]) && ((*cvit).idx() != indices[3]))
                indices.push_back(cvit->idx());
        return indices;
    }
    else // unsupported type
        return std::vector<int>();
}

template <typename MeshT>
bool FileVTKPlugin::writeASCIIDataOfOpenVolumeMesh(std::ostream& _out, MeshT& _mesh)
{
    if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
        _out.precision(savePrecision_->value());

    _out << "DATASET UNSTRUCTURED_GRID\n";


    // save all vertices

    OpenVolumeMesh::VertexIter vit = _mesh.vertices_begin();
    OpenVolumeMesh::VertexIter end_vit = _mesh.vertices_end();
    _out << "POINTS " << _mesh.n_vertices() << " float\n";
    for (; vit != end_vit; ++vit) {
        // Write vertex p[0] p[1] p[2]
        ACG::Vec3d p = _mesh.vertex(*vit);
        _out << p[0] << " " << p[1] << " " << p[2];
        _out << "\n";
    }



    // Remember which faces have been added, so we can later save the isolated faces.
    OpenVolumeMesh::FacePropertyT<bool> fpAlreadyStored = _mesh.template request_face_property<bool>();

    // Remember which edges have been added, so we can later save the isolated edges;
    OpenVolumeMesh::EdgePropertyT<bool> epAlreadyStored = _mesh.template request_edge_property<bool>();

    int cellCount = 0;

    // count the cell list size (for each cell: 1 + adjacent vertices)

    quint32 listSize = 0;
    for (OpenVolumeMesh::CellIter cit = _mesh.cells_begin(); cit != _mesh.cells_end(); ++cit)
    {
        int cellType = getCellType(_mesh, *cit);

        if (cellType == 10 )
            listSize += 1 + 4;
        else if (cellType == 12 )
            listSize += 1 + 8;
        else if (cellType == 13 )
            listSize += 1 + 6;
        else if (cellType == 14 )
            listSize += 1 + 5;
        else // type not supported by vtk file -> ignore
            continue;

        cellCount++;

        std::vector<OpenVolumeMesh::HalfFaceHandle> halffaces = _mesh.cell(*cit).halffaces();
        for (unsigned int i = 0; i < halffaces.size(); i++)
        {
            OpenVolumeMesh::FaceHandle fh = _mesh.face_handle(halffaces[i]);
            if (!(fpAlreadyStored[fh]))
            {
                // mark this face and its edges as already stored
                // if the user wants to save face normals we cannot skip adding the faces
                if (!(userWriteOptions_ & FileVTKPlugin::FACENORMALS))
                    fpAlreadyStored[fh] = true;
                //mark the edges as already added.
                OpenVolumeMesh::OpenVolumeMeshFace face = _mesh.halfface(halffaces[i]);
                for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator heit = face.halfedges().begin();
                     heit != face.halfedges().end();
                     ++heit)
                {
                    epAlreadyStored[_mesh.edge_handle(*heit)] = true;
                }
            }
        }
    }

    for (OpenVolumeMesh::FaceIter fit = _mesh.faces_begin(); fit != _mesh.faces_end(); ++fit)
    {
        if (!(fpAlreadyStored[*fit]))
        {
            //mark edges as already stored
            std::vector<OpenVolumeMesh::HalfEdgeHandle> halfedges = _mesh.face(*fit).halfedges();
            for (unsigned int i = 0; i < halfedges.size(); i++)
                epAlreadyStored[_mesh.edge_handle(halfedges[i])] = true;

            // number of incident vertices == number of incident halfedges
            listSize += 1 + halfedges.size();

            cellCount++;
        }
    }

    for (OpenVolumeMesh::EdgeIter eit = _mesh.edges_begin(); eit != _mesh.edges_end(); ++eit)
    {
        if (!(epAlreadyStored[*eit]))
        {
            listSize += 1 + 2;
            cellCount++;
        }
    }

    _out << "CELLS " << cellCount << " " << listSize << "\n";


    // save all cellCells.

    for (OpenVolumeMesh::CellIter cit = _mesh.cells_begin(); cit != _mesh.cells_end(); ++cit)
    {
        //std::vector<OpenVolumeMesh::VertexHandle> vertices;

        int cellType = getCellType(_mesh, *cit);

        // TODO:
        if (cellType == 10 )
            listSize += 1 + 4;
        else if (cellType == 12 )
            listSize += 1 + 8;
        else if (cellType == 13 )
            listSize += 1 + 6;
        else if (cellType == 14 )
            listSize += 1 + 5;
        else // type not supported by vtk file -> ignore
            continue;

        if (cellType == -1)// type not supported by vtk file -> ignore
            continue;
        else
        {
            std::vector<int> indices = getCellVertexIndices(_mesh, *cit, cellType);
            _out << indices.size();
            for (unsigned int i = 0; i < indices.size(); i++)
                _out << " " << indices[i];
            _out << "\n";
        }

    }

    // save isolated faces

    for (OpenVolumeMesh::FaceIter fit = _mesh.faces_begin(); fit != _mesh.faces_end(); ++fit)
    {
        if (!(fpAlreadyStored[*fit]))
        {
            std::vector<OpenVolumeMesh::HalfEdgeHandle> halfedges = _mesh.face(*fit).halfedges();

            _out << halfedges.size();
            for (unsigned int i = 0; i < halfedges.size(); i++)
            {
                OpenVolumeMesh::HalfEdgeHandle heh = halfedges[i];
                OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.halfedge(heh);
                _out << " " << edge.from_vertex().idx();
            }
            _out << "\n";
        }
    }


    // save isolated edges


    for (OpenVolumeMesh::EdgeIter eit = _mesh.edges_begin(); eit != _mesh.edges_end(); ++eit)
    {
        if (!(epAlreadyStored[*eit]))
        {
            OpenVolumeMesh::OpenVolumeMeshEdge edge = _mesh.edge(*eit);
            _out << "2";
            _out << " " << edge.from_vertex().idx();
            _out << " " << edge.to_vertex().idx();
            _out << "\n";
        }
    }



    // write cell types to file

    _out << "CELL_TYPES " << cellCount << "\n";

    // cell cellTypes
    for (OpenVolumeMesh::CellIter cit = _mesh.cells_begin(); cit != _mesh.cells_end(); ++cit)
    {
        int cellType = getCellType(_mesh, *cit);
        if (cellType == -1) //type not supported by vtk file, so this cell was not added
            continue;
        else
            _out << cellType << "\n";
    }

    // face cell types

    for (OpenVolumeMesh::FaceIter fit = _mesh.faces_begin(); fit != _mesh.faces_end(); ++fit)
    {
        if (!(fpAlreadyStored[*fit]))
        {
            int cellType = getCellType(_mesh,*fit);
            if (cellType == -1) //type not supported by vtk file, so this cell was not added
                continue;
            else
                _out << cellType << "\n";
        }
    }


    // edge cell type
    for (OpenVolumeMesh::EdgeIter eit = _mesh.edges_begin(); eit != _mesh.edges_end(); ++eit)
    {
        if (!(epAlreadyStored[*eit]))
        {
        // all edges have type 3
            _out << "2\n";
        }
    }



    //*************//
    //OPTIONAL DATA//
    //*************//

    //writing of tex coords also not supported yet.


    //Write vertex normals
    if ((userWriteOptions_ & FileVTKPlugin::VERTEXNORMALS))
    {
        _out << "POINT_DATA "<< _mesh.n_vertices() << "\n";

        OpenVolumeMesh::VertexPropertyT< typename OpenMesh::Vec3d > vertexNormals = _mesh.template request_vertex_property< typename OpenMesh::Vec3d >("vertex_normals");
        _out << "NORMALS vertex_normals float\n";
        for (OpenVolumeMesh::VertexIter vit = _mesh.vertices_begin(); vit != _mesh.vertices_end(); ++vit) {
            typename OpenMesh::Vec3d n = vertexNormals[*vit];
            _out << n[0] << " " << n[1] << " " << n[2];
            _out << "\n";
        }
    }


    //Write face normals
    if ((userWriteOptions_ & FileVTKPlugin::FACENORMALS))
    {
        _out << "CELL_DATA "<< cellCount << "\n";
        _out << "NORMALS face_normals float\n";
        // write dummy normal for cells

        for (OpenVolumeMesh::CellIter cit = _mesh.cells_begin(); cit != _mesh.cells_end(); ++cit)
        {
            int cellType = getCellType(_mesh, *cit);

            if (cellType == -1 )// type not supported by vtk file -> ignore
                continue;
            else
            {
                _out << 1 << " " << 0 << " " << 0;
                _out << "\n";
            }
        }


        // write face normals
        OpenVolumeMesh::FacePropertyT< typename OpenMesh::Vec3d > faceNormals = _mesh.template request_face_property< typename OpenMesh::Vec3d >("face_normals");

        for (OpenVolumeMesh::FaceIter fit = _mesh.faces_begin(); fit != _mesh.faces_end(); ++fit) {
            typename OpenMesh::Vec3d n = faceNormals[*fit];
            _out << n[0] << " " << n[1] << " " << n[2];
            _out << "\n";
        }


        // write dummy normals for edges
        for (OpenVolumeMesh::EdgeIter eit = _mesh.edges_begin(); eit != _mesh.edges_end(); ++eit)
        {
            if (!(epAlreadyStored[*eit]))
            {
                _out << 1 << " " << 0 << " " << 0;
                _out << "\n";
            }
        }


    }

    return true;
}

#endif //ENABLE_OPENVOLUMEMESH_SUPPORT

#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
int FileVTKPlugin::addTetraCell(HexahedralMesh*& _mesh, std::vector<quint32> indices)                 { return addTetraCellToOpenVolumeMesh(_mesh, indices);              }
int FileVTKPlugin::addHexaCell(HexahedralMesh*& _mesh, std::vector<quint32> indices)                  { return addHexaCellToOpenVolumeMesh(_mesh, indices);               }
int FileVTKPlugin::addWedgeCell(HexahedralMesh*& _mesh, std::vector<quint32> indices)                 { return addWedgeCellToOpenVolumeMesh(_mesh, indices);              }
int FileVTKPlugin::addPyramidCell(HexahedralMesh*& _mesh, std::vector<quint32> indices)               { return addPyramidCellToOpenVolumeMesh(_mesh, indices);            }
int FileVTKPlugin::addFace(HexahedralMesh*& _mesh, std::vector<quint32> indices)                      { return addFaceToOpenVolumeMesh(_mesh, indices);                   }
int FileVTKPlugin::addFace(HexahedralMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3) { return addFaceToOpenVolumeMesh(_mesh, _index1, _index2, _index3); }
void FileVTKPlugin::addVertexNormal(HexahedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)  { addVertexNormalToOpenVolumeMesh(_mesh, _index, _normal);          }
void FileVTKPlugin::addFaceNormal(HexahedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)    { addFaceNormalToOpenVolumeMesh(_mesh, _index, _normal);            }
bool FileVTKPlugin::writeASCIIData(std::ostream& _out, HexahedralMesh& _mesh)                         { return writeASCIIDataOfOpenVolumeMesh(_out, _mesh);               }
#endif //ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT


#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
int FileVTKPlugin::addTetraCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices)                 { return addTetraCellToOpenVolumeMesh(_mesh, indices);              }
int FileVTKPlugin::addHexaCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices)                  { return addHexaCellToOpenVolumeMesh(_mesh, indices);               }
int FileVTKPlugin::addWedgeCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices)                 { return addWedgeCellToOpenVolumeMesh(_mesh, indices);              }
int FileVTKPlugin::addPyramidCell(PolyhedralMesh*& _mesh, std::vector<quint32> indices)               { return addPyramidCellToOpenVolumeMesh(_mesh, indices);            }
int FileVTKPlugin::addFace(PolyhedralMesh*& _mesh, std::vector<quint32> indices)                      { return addFaceToOpenVolumeMesh(_mesh, indices);                   }
int FileVTKPlugin::addFace(PolyhedralMesh*& _mesh, quint32 _index1, quint32 _index2, quint32 _index3) { return addFaceToOpenVolumeMesh(_mesh, _index1, _index2, _index3); }
void FileVTKPlugin::addVertexNormal(PolyhedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)  { addVertexNormalToOpenVolumeMesh(_mesh, _index, _normal);          }
void FileVTKPlugin::addFaceNormal(PolyhedralMesh*& _mesh, quint32 _index, OpenMesh::Vec3d _normal)    { addFaceNormalToOpenVolumeMesh(_mesh, _index, _normal);            }
bool FileVTKPlugin::writeASCIIData(std::ostream& _out, PolyhedralMesh& _mesh)                         { return writeASCIIDataOfOpenVolumeMesh(_out, _mesh);               }
#endif //ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//----------------------------------- end of cool helper function -------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

FileVTKPlugin::BestMeshType FileVTKPlugin::findBestObjectType(QString _filename)
{
    QFile file(_filename);

    if ( !file.open(QIODevice::ReadOnly) ) {
      return BMT_None;
    }

    QTextStream in(&file);

    QString line = in.readLine();

    QStringList header = line.split(" ",QString::SkipEmptyParts);
    if ( header.size() != 5 ) {
      return BMT_None;
    }

    QString version = header[4];

    header.removeLast();

    QString magic = header.join(" ");

    if ( magic != "# vtk DataFile Version" ) {
      return BMT_None;
    }

    QString description = in.readLine();

    QString fileTypeStr = in.readLine();

    fileTypeStr = fileTypeStr.simplified();

    if ( fileTypeStr.toLower() == "binary" ) {
      binary_ = true;
    } else if ( fileTypeStr.toLower() == "ascii" ) {
      binary_ = false;
    } else {
      return BMT_None;
    }

    line = "";

    while ( line.simplified() == "" )
      line = in.readLine();

    QStringList datasetList =  line.split(" ",QString::SkipEmptyParts);

    if ( datasetList.size() != 2 ) {
      return BMT_None;
    }

    Dataset dataset;

    datasetList[1] = datasetList[1].simplified();

    if ( datasetList[1] == "STRUCTURED_POINTS" )
      dataset = STRUCTURED_POINTS;
    else if ( datasetList[1] == "STRUCTURED_GRID" )
      dataset = STRUCTURED_GRID;
    else if ( datasetList[1] == "RECTILINEAR_GRID" )
      dataset = RECTILINEAR_GRID;
    else if ( datasetList[1] == "POLYDATA" )
      dataset = POLYDATA;
    else if ( datasetList[1] == "UNSTRUCTURED_GRID" )
      dataset = UNSTRUCTURED_GRID;
    else {
      return BMT_None;
    }


    if ((dataset == STRUCTURED_POINTS) || (dataset == STRUCTURED_GRID) || (dataset == RECTILINEAR_GRID) )
    {
        // Just points so every type is ok. Just use TriMesh. (Apperently these aren't even supported.)
        return BMT_TriMesh;
    }
    else if (dataset == POLYDATA)
    {
        // The file could contain vertices, lines, polygons, or triangle strips.
        // So we will use either TriMesh or PolyMesh.
        //
        // Test if all polygons are triangles

        line = in.readLine();

        // Search for line "POLYGONS n size"
        while ( !line.contains("POLYGONS") ) {

            // stop at end of file!
            if ( in.atEnd() ) {
                // we didn't find polygons -> all primitives are either vertices, lines or trianglestrips
                return BMT_TriMesh;
            }

            line = in.readLine();
        }

        QStringList polygonsLine = line.split(" ",QString::SkipEmptyParts);

        // It has to contain the Keyword POLYGONS , the number of polygons to read and the total number of values to read ( each polygon: 1 + valence )
        if ( polygonsLine.size() != 3 ) {
            emit log(LOGERR,tr("Expected to get Points line with exactly 3 entries, but %1 found!").arg(polygonsLine.size()));
            return BMT_None;
        }

        bool ok = true;

        // Number of polygons to read
        quint32 polygoncount = polygonsLine[1].toUInt(&ok);

        // number of ints in the whole polygon description
        quint32 entrycount = polygonsLine[2].toUInt(&ok);

        if ( ! ok) {
            emit log(LOGERR,tr("Expected to get number of points and entries, but read %1 !").arg(polygonsLine.join(" ")));
            return BMT_None;
        }

        quint32 read = 0;
        while ( read < polygoncount) {

            // Read first integer describing number of indices
            quint32 valence;
            if ( !binary_ )
                in >> valence;
            else
                 in.device()->read((char*)&valence,sizeof(quint32));

            if (valence != 3)
            {
                // Polygone with more than 3 vertices found -> TriMesh not possible
                return BMT_PolyMesh;
            }

            // read rest
            quint32 index;

            for ( unsigned int i = 0 ; i < valence; ++i ) {

                // Read one index
                if ( !binary_ )
                    in >> index;
                else
                    in.device()->read((char*)&valence,sizeof(quint32));
            }
            ++read;
        }

        // if we reach this point, all polygons were triangles
        return BMT_TriMesh;

    }
    else /*if (dataset == UNSTRUCTURED_GRID)*/
    {
        // The file could contain all different kind of primitives.
        // We need to check if all of them are triangles.
        // If OpenVolumeMesh is supported we additionally check whether there are
        // Cell primitives, and if so, whether all of them are hexaherdral.

        line = in.readLine();

        // Search for line "CELL_TYPES n"
        while ( !line.contains("CELL_TYPES") ) {

            // stop at end of file!
            if ( in.atEnd() ) {
                // we didn't find the line "CELL_TYPES n" -> bad file
                return BMT_None;
            }

            line = in.readLine();
        }

        // Split the header line into components
        QStringList cellLine = line.split(" ",QString::SkipEmptyParts);

        // It has to contain the Keyword CELL_TYPES , the number of cells to read
        if ( cellLine.size() != 2 ) {
          emit log(LOGERR,tr("Expected to get CELL_TYPES line with exactly 2 entries, but %1 found!").arg(cellLine.size()));
          return BMT_None;
        }

        bool ok = true;

        // Number of cells to read
        quint32 cellCountTypes = cellLine[1].toUInt(&ok);

        if ( ! ok) {
          emit log(LOGERR,tr("Expected to get number of cell types, but read %1 !").arg(cellLine[1]));
          return BMT_None;
        }

        bool triMeshPossible = true;
        bool polyMeshPossible = true;

#ifndef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
        bool hexahedralMeshPossible = false;
#else
        bool hexahedralMeshPossible = true;
#endif
#ifndef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
        bool polyhedralMeshPossible = false;
#else
        bool polyhedralMeshPossible = true;
#endif

        quint32 read = 0;
        while ( read < cellCountTypes) {

          // Read first integer describing number of indizes
          quint32 type;
          if ( !binary_ )
            in >> type;
          else
            in.device()->read((char*)&type,sizeof(quint32));


          if      (( 1 <= type ) && (type <=  6 ))
          {
              // vertex, poly vertex, line, poly line, triangle, triangle strip
              // all mesh types possible except hexahedral mesh
              hexahedralMeshPossible = false;
          }
          else if (( 7 <= type ) && (type <=  9 ))
          {
              // polygone, pixel (axis alligned quad), quad -> triMesh not possible
              // while polygone could be a triangle as well, we assume that it's not because
              // we hope the file author would have chosen type == 5 int that case
              triMeshPossible = false;
              hexahedralMeshPossible = false;
          }
          // TODO: ignore if no OpenVolumeMesh support
          else if (( 10 == type ) || ( 13 == type ) || (type ==  14 ))
          {
              // tetra, wedge, pyramid
              // cannot be represented by anything other than polyhedral mesh
              triMeshPossible = false;
              polyMeshPossible = false;
              hexahedralMeshPossible = false;
          }
          else if (( 11 == type ) || ( 12 == type ))
          {
              // voxel, hexahedron
              // cannot be represented by open mesh
              triMeshPossible = false;
              polyMeshPossible = false;
          }


          if ( ! in.status() == QTextStream::Ok ) {
            emit log(LOGERR,tr("Read corrupted cell type data!"));
            return BMT_None;
          }
          ++read;
        }

        if (triMeshPossible)
            return BMT_TriMesh;
        else if (polyMeshPossible)
            return BMT_PolyMesh;
        else if (hexahedralMeshPossible)
            return BMT_HexahedralMesh;
        else if (polyhedralMeshPossible)
            return BMT_PolyhedralMesh;
        else
            return BMT_None;

    }



}

int FileVTKPlugin::loadObject(QString _filename) {
  std::cerr << "Loading vtk file" << std::endl;

  BestMeshType bestType = findBestObjectType(_filename);

  QFile file(_filename);

  if ( !file.open(QIODevice::ReadOnly) ) {
    emit log(LOGERR,"Unable to open vtk file!");
    return -1;
  }

  QTextStream in(&file);

  std::cerr << "File is open!" << std::endl;

  QString line = in.readLine();

  std::cerr << "Got line: " << std::endl;
  std::cerr << line.toStdString() << std::endl;

  QStringList header = line.split(" ",QString::SkipEmptyParts);

  if ( header.size() != 5 ) {
    emit log(LOGERR,tr("Bad VTK header? ") + line);
    return -1;
  }

  QString version = header[4];

  header.removeLast();

  QString magic = header.join(" ");

  if ( magic != "# vtk DataFile Version" ) {
    emit log(LOGERR,tr("Bad VTK header magic? ") + magic);
    return -1;
  }

  QString description = in.readLine();

  QString fileTypeStr = in.readLine();

  fileTypeStr = fileTypeStr.simplified();

  if ( fileTypeStr.toLower() == "binary" ) {
    binary_ = true;
    emit log(LOGINFO,tr("Loading VTK binary file of version %1.").arg(version));
    emit log(LOGERR,tr("Not yet implemented!"));
  } else if ( fileTypeStr.toLower() == "ascii" ) {
    binary_ = false;
    emit log(LOGINFO,tr("Loading VTK ascii file of version %1.").arg(version));
  } else {
    emit log(LOGERR,tr("Bad VTK type? ") + fileTypeStr);
    return -1;
  }

  emit log(LOGINFO,description);

  line = "";

  while ( line.simplified() == "" )
    line = in.readLine();

  QStringList datasetList =  line.split(" ",QString::SkipEmptyParts);

  if ( datasetList.size() != 2 ) {
    emit log(LOGERR,tr("Bad dataset specification!"));
    return -1;
  }

  Dataset dataset;

  datasetList[1] = datasetList[1].simplified();

  if ( datasetList[1] == "STRUCTURED_POINTS" )
    dataset = STRUCTURED_POINTS;
  else if ( datasetList[1] == "STRUCTURED_GRID" )
    dataset = STRUCTURED_GRID;
  else if ( datasetList[1] == "RECTILINEAR_GRID" )
    dataset = RECTILINEAR_GRID;
  else if ( datasetList[1] == "POLYDATA" )
    dataset = POLYDATA;
  else if ( datasetList[1] == "UNSTRUCTURED_GRID" )
    dataset = UNSTRUCTURED_GRID;
  else {
    emit log(LOGERR,tr("Unknown dataset specification! %1").arg(datasetList[1]));
    return -1;
  }

  bool rpc_ok(false);
  emit functionExists( "colorplugin" , "setDefaultColor()", rpc_ok ) ;

    if ( (forceTriangleMesh_) || (bestType == BMT_TriMesh) ){

        // add a triangle mesh
        int id = -1;
        emit addEmptyObject(DATA_TRIANGLE_MESH, id);

        TriMeshObject* object(0);

        if(PluginFunctions::getObject( id, object)){

            TriMesh* _mesh;
            PluginFunctions::getMesh(id,_mesh);

            if ( _mesh != 0 ) {
                if ( !loadMesh(in,_mesh,dataset) ) {
                    emit log(LOGERR,"Unable to load mesh!");
                    return -1;
                }
            } else {
                emit log(LOGERR,"Unable to add empty triangle mesh!");
                return -1;
            }

            object->setFromFileName(_filename);
            object->setName(object->filename());

            object->update();

            //general stuff
            emit openedFile( object->id() );
            if (rpc_ok)
              RPC::callFunction("colorplugin", "setDefaultColor");

            PluginFunctions::viewAll();

            return id;
        }

    }
    // If no type can represent the object in the file just use PolyMesh
    // and load as much as possible
    else if ((bestType == BMT_PolyMesh) || (bestType == BMT_None))
    {

        int id = -1;
        emit addEmptyObject(DATA_POLY_MESH, id);


        PolyMeshObject* object(0);

        if(PluginFunctions::getObject( id, object)){

            PolyMesh* _mesh(0);
            PluginFunctions::getMesh(id, _mesh);

            if ( _mesh != 0 ) {
                if ( !loadMesh(in,_mesh,dataset) ) {
                    emit log(LOGERR,"Unable to load mesh!");
                    return -1;
                }
            } else {
                emit log(LOGERR,"Unable to add empty poly mesh!");
                return -1;
            }


            object->setFromFileName(_filename);
            object->setName(object->filename());

            object->update();

            //general stuff
            emit openedFile( object->id() );
            if (rpc_ok)
              RPC::callFunction("colorplugin", "setDefaultColor");

            PluginFunctions::viewAll();

            return id;

        }


    }
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    else if (bestType == BMT_PolyhedralMesh)
    {
        // add a Polyhedral mesh
        int id = -1;
        emit addEmptyObject(DATA_POLYHEDRAL_MESH, id);

        PolyhedralMeshObject* object(0);

        if(PluginFunctions::getObject( id, object)){

            PolyhedralMesh* _mesh;
            _mesh = PluginFunctions::polyhedralMesh(object);

            if ( _mesh != 0 ) {
                if ( !loadMesh(in,_mesh,dataset) ) {
                    emit log(LOGERR,"Unable to load mesh!");
                    return -1;
                }
            } else {
                emit log(LOGERR,"Unable to add empty polyhedral mesh!");
                return -1;
            }

            object->setFromFileName(_filename);
            object->setName(object->filename());

            object->update();

            object->setObjectDrawMode(ACG::SceneGraph::DrawModes::getDrawMode("Cells (flat shaded)"));

            //general stuff
            emit openedFile( object->id() );
            if (rpc_ok)
              RPC::callFunction("colorplugin", "setDefaultColor");

            PluginFunctions::viewAll();

            return id;
        }

    }
#endif //ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    else if (bestType == BMT_HexahedralMesh)
    {
        // add a heexahedral mesh
        int id = -1;
        emit addEmptyObject(DATA_HEXAHEDRAL_MESH, id);

        HexahedralMeshObject* object(0);

        if(PluginFunctions::getObject( id, object)){

            HexahedralMesh* _mesh;
            _mesh = PluginFunctions::hexahedralMesh(object);

            if ( _mesh != 0 ) {
                if ( !loadMesh(in,_mesh,dataset) ) {
                    emit log(LOGERR,"Unable to load mesh!");
                    return -1;
                }
            } else {
                emit log(LOGERR,"Unable to add empty hexahedral mesh!");
                return -1;
            }


            object->setFromFileName(_filename);
            object->setName(object->filename());

            object->update();

            object->setObjectDrawMode(ACG::SceneGraph::DrawModes::getDrawMode("Cells (flat shaded)"));

            //general stuff
            emit openedFile( object->id() );
            if (rpc_ok)
              RPC::callFunction("colorplugin", "setDefaultColor");

            PluginFunctions::viewAll();

            return id;
        }

    }
#endif //ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT

  emit log(LOGERR,tr("Error in load mesh!"));

  return -1;


}

template <typename MeshT>
bool FileVTKPlugin::loadMeshPoints(QString _spec, QTextStream& _in,MeshT*& _mesh){

  std::cerr << "loadMeshPoints" << std::endl;

  bool ok = true;

  // Split the header line into components
  QStringList pointsLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword POINTS , the number of points to read and the datatype of the points
  if ( pointsLine.size() != 3 ) {
    emit log(LOGERR,tr("Expected to get Points line with exactly 3 entries, but %1 found!").arg(pointsLine.size()));
    return false;
  }

  // Get the number of points to read
  quint32 points = pointsLine[1].toUInt(&ok);

  if ( ! ok) {
    emit log(LOGERR,tr("Expected to get number of points, but read %1 !").arg(pointsLine[1]));
    return false;
  }

  // In OpenMesh we use doubles for the point representation
  OpenMesh::Vec3d vec;

  quint32 read = 0;
  while ( read < points ) {


    if ( binary_ ) {

      // Read binary float values
      if ( pointsLine[2] == "float" ) {
        // Read floats
        OpenMesh::Vec3f vecf;
        _in.device()->read((char*)&vecf[0],sizeof(float));
        _in.device()->read((char*)&vecf[1],sizeof(float));
        _in.device()->read((char*)&vecf[2],sizeof(float));

        // convert
        vec = vecf;
      } else {
        emit log(LOGERR,tr("Not implemented data type %1 !").arg(pointsLine[2]));
        return false;
      }

    } else {
      // Don't care about original type, as we read text and convert it anyway.
      _in >> vec[0] >> vec[1] >> vec[2];
    }

    // Next point to read
    ++read;

    // Check if the stream is still ok
    if ( _in.status() == QTextStream::Ok ) {
      _mesh->add_vertex(vec);
    } else {
      emit log(LOGERR,tr("Read corrupted point data!"));
      return false;
    }

  }

  if (binary_) {
    if ( pointsLine[2] == "float" ) {
      // Reposition text stream. We read points * 3 * sizeof(float)
      // For the text stream we calculate the position in charactersso convert it witth 1.0 / sizeof(char)
      _in.seek(_in.pos() + read * 3 * sizeof(float) / sizeof(char) );
    }
  }

  return true;

}

template <typename MeshT>
bool FileVTKPlugin::loadMeshLines(QString _spec,QTextStream& _in,MeshT*& _mesh) {
  std::cerr << "loadMeshLines" << std::endl;

  bool ok = true;

  // Split the header line into components
  QStringList linesLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword LINES , the number of polygons to read and the total number of values to read ( each line: 1 + valence )
  if ( linesLine.size() != 3 ) {
    emit log(LOGERR,tr("Expected to get LINES line with exactly 3 entries, but %1 found!").arg(linesLine.size()));
    return false;
  }

  // umber of lines to read
  quint32 linecount = linesLine[1].toUInt(&ok);

  // number of ints in the whole line description!
  quint32 entrycount = linesLine[2].toUInt(&ok);

  if ( ! ok) {
    emit log(LOGERR,tr("Expected to get number of lines and entries, but read %1 !").arg(linesLine.join(" ")));
    return false;
  }

  quint32 read = 0;

  while ( read < linecount) {

    // Read first integer describing number of indizes in the current line
    quint32 valence;

    if ( !binary_ )
      _in >> valence;
    else
      _in.device()->read((char*)&valence,sizeof(quint32));


    quint32 index;

    if ( _in.status() == QTextStream::Ok ) {

      if ( !binary_ ) {
        for ( unsigned int i = 0 ; i < valence; ++i )
          _in >> index;
      } else {
        for ( unsigned int i = 0 ; i < valence; ++i )
          _in.device()->read((char*)&valence,sizeof(quint32));
      }

      if ( _in.status() == QTextStream::Ok ) {
        // TODO : Generate lines here!

        //_mesh->add_edge(handles);
      } else {
        emit log(LOGERR,tr("Read corrupted face data!"));
        return false;
      }

    } else {
      emit log(LOGERR,tr("Read corrupted POLYGONS data!"));
      return false;
    }

    ++read;
  }

  if ( binary_ ) {
    // Reposition text stream. We read entrycount * sizeof(quint32) .
    // For the text stream we calculate the position in charactersso convert it witth 1.0 / sizeof(char)
    _in.seek(_in.pos() + entrycount * sizeof(quint32) / sizeof(char) );
  }

  emit log(LOGWARN,tr("Lines not supported yet ... skipped!"));

  return true;
}

template <typename MeshT>
bool FileVTKPlugin::loadMeshPolygons(QString _spec,QTextStream& _in,MeshT*& _mesh) {
    std::cerr << "loadMeshPolygons" << std::endl;

    bool ok = true;

    // Split the header line into components
    QStringList polygonsLine = _spec.split(" ",QString::SkipEmptyParts);

    // It has to contain the Keyword POLYGONS , the number of polygons to read and the total number of values to read ( each polygon: 1 + valence )
    if ( polygonsLine.size() != 3 ) {
        emit log(LOGERR,tr("Expected to get Points line with exactly 3 entries, but %1 found!").arg(polygonsLine.size()));
        return false;
    }

    // Number of polygons to read
    quint32 polygoncount = polygonsLine[1].toUInt(&ok);

    // number of ints in the whole polygon description
    quint32 entrycount = polygonsLine[2].toUInt(&ok);

    if ( ! ok) {
        emit log(LOGERR,tr("Expected to get number of points and entries, but read %1 !").arg(polygonsLine.join(" ")));
        return false;
    }

    // Disable OpenMesh error logging
    bool errorEnabled = omerr().is_enabled();
    omerr().disable();

    quint32 read = 0;
    while ( read < polygoncount) {

        // Read first integer describing number of indices
        quint32 valence;
        if ( !binary_ )
            _in >> valence;
        else
            _in.device()->read((char*)&valence,sizeof(quint32));


        quint32 index;
        std::vector< quint32 > indices;

        for ( unsigned int i = 0 ; i < valence; ++i ) {

            // Read one index
            if ( !binary_ )
                _in >> index;
            else
                _in.device()->read((char*)&valence,sizeof(quint32));

            // Remember it
            indices.push_back( index );
        }

        if ( _in.status() == QTextStream::Ok ) {
            addFace(_mesh, indices);
        } else {
            emit log(LOGERR,tr("Read corrupted face data!"));

            // Restore error logging state before returning
            if ( errorEnabled )
                omerr().enable();

            return false;
        }
        ++read;
    }

    // Restore error logging state.
    if ( errorEnabled )
        omerr().enable();

    if (binary_) {
        // Reposition text stream. We read entrycount * sizeof(qint32)
        // For the text stream we calculate the position in charactersso convert it witth 1.0 / sizeof(char)
        _in.seek(_in.pos() + entrycount * sizeof(qint32) / sizeof(char) );
    }

    return true;
}

template <typename MeshT>
int FileVTKPlugin::add_non_manifold_face(MeshT*& _mesh, std::vector< OpenMesh::VertexHandle >& _vhandles) {

    // Duplicate vertices of non-manifold faces
    // and add them as new isolated face
    if (_vhandles.empty()) return -1;

    OpenMesh::VPropHandleT< OpenMesh::VertexHandle > originalVertexIdx;
    if (! _mesh->get_property_handle(originalVertexIdx,"FileVTKPlugin_originalVertexIdx")) {
        _mesh->add_property( originalVertexIdx, "FileVTKPlugin_originalVertexIdx" );

        //mark all vertices
        typename MeshT::VertexIter vit = _mesh->vertices_begin();
        typename MeshT::VertexIter vend = _mesh->vertices_end();
        for(; vit != vend; ++vit) {
            _mesh->property (originalVertexIdx, vit) = OpenMesh::VertexHandle(-1);
        }
    }

    // double vertices
    for (unsigned int j = 0; j < _vhandles.size(); ++j)
    {
        typename MeshT::Point p = _mesh->point(_vhandles[j]);
        OpenMesh::VertexHandle original_handle = _vhandles[j];
        _vhandles[j] = _mesh->add_vertex(p);
        // DO STORE p, reference may not work since vertex array
        // may be relocated after adding a new vertex !

        // Mark vertices of failed face as non-manifold
        if (_mesh->has_vertex_status()) {
            _mesh->status(_vhandles[j]).set_fixed_nonmanifold(true);
        }
        // To assign other properties to this vertex later, save its "original" in property
        _mesh->property (originalVertexIdx, _vhandles[j]) = original_handle;
    }

    // add face
    OpenMesh::FaceHandle fh = _mesh->add_face(_vhandles);

    // Mark failed face as non-manifold
    if (_mesh->has_face_status())
        _mesh->status(fh).set_fixed_nonmanifold(true);

    // Mark edges of failed face as non-two-manifold
    if (_mesh->has_edge_status()) {
        typename MeshT::FaceEdgeIter fe_it = _mesh->fe_iter(fh);
        for (; fe_it; ++fe_it) {
            _mesh->status(fe_it).set_fixed_nonmanifold(true);
        }
    }

    return fh.idx();
}


template <typename MeshT>
bool FileVTKPlugin::loadMeshTriangleStrips(QString _spec,QTextStream& _in,MeshT*& _mesh) {
  std::cerr << "loadMeshTriangleStrips" << std::endl;

  bool ok = true;

  // Split the header line into components
  QStringList triStripsLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword TRIANGLE_STRIPS , the number of strips to read and the total number of values to read ( each strip: 1 + valence )
  if ( triStripsLine.size() != 3 ) {
    emit log(LOGERR,tr("Expected to get TRIANGLE_STRIPS line with exactly 3 entries, but %1 found!").arg(triStripsLine.size()));
    return false;
  }

  // Number of polygons to read
  quint32 stripcount = triStripsLine[1].toUInt(&ok);

  // number of ints in the whole polygon description
  quint32 entrycount = triStripsLine[2].toUInt(&ok);

  if ( ! ok) {
    emit log(LOGERR,tr("Expected to get number of strips and entries, but read %1 !").arg(triStripsLine.join(" ")));
    return false;
  }

  quint32 read = 0;
  while ( read < stripcount) {

    // Read first integer describing number of indizes in the strip
    quint32 valence;
    if ( !binary_ )
      _in >> valence;
    else
      _in.device()->read((char*)&valence,sizeof(quint32));


    quint32 index;
    QList< quint32 > indices;

    // Read first two of strip:
    for ( unsigned int i = 0 ; i < 2; ++i ) {

      // Read one index
      if ( !binary_ )
        _in >> index;
      else
        _in.device()->read((char*)&valence,sizeof(quint32));

      // Remember it
      indices.push_back( index );
    }

    // Read rest of strip while adding faces
    for ( unsigned int i = 2 ; i < valence; ++i ) {

      // Read one index
      if ( !binary_ )
        _in >> index;
      else
        _in.device()->read((char*)&valence,sizeof(quint32));

      // Remember it
      indices.push_back( index );

      if ( _in.status() == QTextStream::Ok ) {
        // TODO : handle non manifold cases!
        if ( i % 2 == 0 )
          addFace(_mesh, indices[i],indices[i-1],indices[i-2]);
        else
          addFace(_mesh, indices[i],indices[i-2],indices[i-1]);
      } else {
        emit log(LOGERR,tr("Read corrupted face data!"));
        return false;
      }


    }

    ++read;
  }


  if (binary_) {
    // Reposition text stream. We read entrycount * sizeof(qint32)
    // For the text stream we calculate the position in charactersso convert it witth 1.0 / sizeof(char)
    _in.seek(_in.pos() + entrycount * sizeof(qint32) / sizeof(char) );
  }

  return true;
}

template <typename MeshT>
bool FileVTKPlugin::loadMeshNormals(QString _spec,QTextStream& _in,MeshT*& _mesh, std::vector<CellType>& _cells, bool _pointNormal, quint32 _count) {
  std::cerr << "loadMeshNormals" << std::endl;

  // Split the header line into components
  QStringList normalsLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword NORMALS , the name of the dataset and the datatype
  if ( normalsLine.size() != 3 ) {
    emit log(LOGERR,tr("Expected to get NORMALS line with exactly 3 entries, but %1 found!").arg(normalsLine.size()));
    return false;
  }

  // In OpenMesh we use doubles for the normal representation
  OpenMesh::Vec3d normal;

  quint32 read = 0;
  while ( read < _count) {

    if ( binary_ ) {

      // Read binary float values
      if ( normalsLine[2] == "float" ) {

        // Read floats
        OpenMesh::Vec3f vecf;
        _in.device()->read((char*)&vecf[0],sizeof(float));
        _in.device()->read((char*)&vecf[1],sizeof(float));
        _in.device()->read((char*)&vecf[2],sizeof(float));

        // convert
        normal = vecf;
      } else {
        emit log(LOGERR,tr("Not implemented data type %1 !").arg(normalsLine[2]));
        return false;
      }

    } else {
      // Don't care about original type, as we read text and convert it anyway.
      _in >> normal[0] >> normal[1] >> normal[2];
    }

    // Check if the stream is still ok
    if ( _in.status() == QTextStream::Ok ) {

      if ( _pointNormal ) {
          addVertexNormal(_mesh, read, normal);

      } else {
          addCellNormal(_mesh, _cells[read], normal);
      }

    } else {
      emit log(LOGERR,tr("Read corrupted point data!"));
      return false;
    }

    // Next normal to read
    ++read;
  }

  if (binary_) {
    if ( normalsLine[2] == "float" ) {
      // Reposition text stream. We read points * 3 * sizeof(float)
      // For the text stream we calculate the position in charactersso convert it with 1.0 / sizeof(char)
      _in.seek(_in.pos() + read * 3 * sizeof(float) / sizeof(char) );
    }
  }

  setNormalsOfDuplicatedVertices(_mesh);


  return true;
}

template <typename MeshT>
bool FileVTKPlugin::loadMeshCells(QString _spec,QTextStream& _in,MeshT*& _mesh, std::vector<CellType>& _cells) {

  std::cerr << "loadMeshCells" << std::endl;

  // Split the header line into components
  QStringList cellLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword CELLS , the number of cells to read and the total number of values to read ( each cell: 1 + valence )
  if ( cellLine.size() != 3 ) {
    emit log(LOGERR,tr("Expected to get CELLS line with exactly 3 entries, but %1 found!").arg(cellLine.size()));
    return false;
  }

  bool ok = true;

  // Number of cells to read
  quint32 cellCount = cellLine[1].toUInt(&ok);

  // number of ints in the whole cell description
  quint32 entryCount = cellLine[2].toUInt(&ok);


  if ( ! ok) {
    emit log(LOGERR,tr("Expected to get number of cells and entries, but read %1 !").arg(cellLine.join(" ")));
    return false;
  }

  quint32 read = 0;
  while ( read < cellCount) {

    CellType currentCell;

    // Read first integer describing number of indizes
    quint32 valence;
    if ( !binary_ )
      _in >> valence;
    else
      _in.device()->read((char*)&valence,sizeof(quint32));


    quint32 index;

    for ( unsigned int i = 0 ; i < valence; ++i ) {

      // Read one index
      if ( !binary_ )
        _in >> index;
      else
        _in.device()->read((char*)&valence,sizeof(quint32));

      // Remember it
      currentCell.indices.push_back( index );
    }

    if ( _in.status() == QTextStream::Ok ) {
      _cells.push_back(currentCell);
    } else {
      emit log(LOGERR,tr("Read corrupted face data!"));
      return false;
    }
    ++read;

  }

  if (binary_) {
    // Reposition text stream. We read  entryCount * sizeof(quint32)
    // For the text stream we calculate the position in charactersso convert it with 1.0 / sizeof(char)
    _in.seek(_in.pos() + entryCount * sizeof(quint32) / sizeof(char) );
  }


  //=================================================================================
  //=================================================================================
  // Read the cell types now
  //=================================================================================
  //=================================================================================
  _spec ="";

  // Read lines until we get something usefull
  while ( _spec.simplified().size() == 0 ) {

    // stop at end of file!
    if ( _in.atEnd() ) {
      emit log(LOGERR,tr("File end when reading cell specification!"));
      return false;
    }

    _spec = _in.readLine();
  }

  if ( ! _spec.contains("CELL_TYPES") ) {
    emit log(LOGERR,tr("Wrong token! Expected CELL_TYPES but got : %1").arg(_spec));
    return false;
  }

  // Split the header line into components
  cellLine = _spec.split(" ",QString::SkipEmptyParts);

  // It has to contain the Keyword CELL_TYPES , the number of cells to read
  if ( cellLine.size() != 2 ) {
    emit log(LOGERR,tr("Expected to get CELL_TYPES line with exactly 2 entries, but %1 found!").arg(cellLine.size()));
    return false;
  }

  // Number of cells to read
  quint32 cellCountTypes = cellLine[1].toUInt(&ok);

  if ( ! ok) {
    emit log(LOGERR,tr("Expected to get number of cell types, but read %1 !").arg(cellLine[1]));
    return false;
  }

  if (cellCountTypes != cellCount ) {
    emit log(LOGERR,tr("cellCountTypes != cellCount !").arg(cellLine.size()));
    return false;
  }

  read = 0;
  while ( read < cellCount) {

    // Read first integer describing number of indizes
    quint32 type;
    if ( !binary_ )
      _in >> type;
    else
      _in.device()->read((char*)&type,sizeof(quint32));


    // Remember it
    _cells[read].type = type;

    if ( ! _in.status() == QTextStream::Ok ) {
      emit log(LOGERR,tr("Read corrupted cell type data!"));
      return false;
    }
    ++read;
  }

  if (binary_) {
    // Reposition text stream. We read cells * sizeof(quint32)
    // For the text stream we calculate the position in charactersso convert it with 1.0 / sizeof(char)
    _in.seek(_in.pos() + read * sizeof(quint32) / sizeof(char) );
  }

  //=================================================================================
  //=================================================================================
  // Now add the cells
  //=================================================================================
  //=================================================================================
  for ( unsigned int i = 0 ; i < _cells.size() ; ++i ) {
    if ( _cells[i].type == 1 ) {

      //VERTEX
      // Nothing to do for meshes ... already added as vertex

    } else if ( _cells[i].type == 2 ) {

      //POLY_VERTEX
      // Nothing to do for meshes ... already added as vertex

    } else if ( _cells[i].type == 3 ) {

      //LINE
      emit log(LOGWARN,tr("Unsupported Cell Type LINE") );

    } else if ( _cells[i].type == 4 ) {
      //POLY_LINE

      emit log(LOGWARN,tr("Unsupported Cell Type POLY_LINE") );

    } else if ( _cells[i].type == 5 ) {
      //TRIANGLE
        _cells[i].index = addFace(_mesh, _cells[i].indices);
    } else if ( _cells[i].type == 6 ) {
      //TRIANGLE_STRIP

      for ( unsigned int j = 2 ; j < _cells[i].indices.size() ; ++j) {
        if ( (j % 2) == 0 )
          _cells[i].index = addFace(_mesh, _cells[i].indices[j-2],_cells[i].indices[j],_cells[i].indices[j-1]);
        else
          _cells[i].index = addFace(_mesh, _cells[i].indices[j-2],_cells[i].indices[j-1],_cells[i].indices[j]);
      }

      //TODO : handle non manifold cases!

    } else if ( _cells[i].type == 7 ) {
      //POLYGON

        _cells[i].index = addFace(_mesh, _cells[i].indices);

    } else if ( _cells[i].type == 8 ) {
      //PIXEL

      emit log(LOGWARN,tr("Unsupported Cell Type PIXEL") );

    } else if ( _cells[i].type == 9 ) {
      //QUAD

        _cells[i].index = addFace(_mesh, _cells[i].indices);

    } else if ( _cells[i].type == 10 ) {
      //Tetra
        _cells[i].index = addTetraCell(_mesh, _cells[i].indices);

    } else if ( _cells[i].type == 11 ) {
      //VOXEL

      emit log(LOGWARN,tr("Unsupported Cell Type VOXEL") );

    } else if ( _cells[i].type == 12 ) {
      //HEXAHEDRON

        _cells[i].index = addHexaCell(_mesh, _cells[i].indices);
    } else if ( _cells[i].type == 13 ) {
      //WEDGE

        _cells[i].index = addWedgeCell(_mesh, _cells[i].indices);
    } else if ( _cells[i].type == 14 ) {
      //PYRAMID

        _cells[i].index = addPyramidCell(_mesh, _cells[i].indices);

    } else {
      emit log(LOGERR,tr("Unknown cell type").arg(_cells[i].type) );
    }
  }

  std::cerr << "Read " << read << " Cells " << std::endl;
  std::cerr << "Vector has size: " << _cells.size() << std::endl;


  return true;
}

template <typename MeshT>
bool FileVTKPlugin::loadMesh(QTextStream& _in,MeshT*& _mesh, Dataset _type){


  if ( _type != POLYDATA && _type != UNSTRUCTURED_GRID ) {
    emit log(LOGERR,"Unsupported DATASET" );
    return false;
  }

  QString line = "";

  bool ok = true;

  // Flag if normals have been read from files
  bool pointNormalsRead = false;
  bool faceNormalsRead = false;

  // flag if we are in Point data mode
  bool pointData = false;

  // Size of the point data
  quint32 pointDataSize = 0;

  // flag if we are in cell data mode
  bool cellData = false;

  // Size of the cell data
  quint32 cellDataSize = 0;

  std::vector<CellType> cells;

  while (ok) {

    line = _in.readLine();

    // Read lines until we get something usefull
    while ( line.simplified().size() == 0 ) {

      // stop at end of file!
      if ( _in.atEnd() ) {
        ok = false;
        std::cerr << "atEnd" << std::endl;
        break;
      }

      line = _in.readLine();
    }

    // Stop if something is wrong or we are at the end of the file
    if ( !ok )
      break;

    std::cerr << "Line is: " << line.toStdString() << std::endl;

    // if we got a points token:
    if ( line.contains("POINTS") ) {

      ok = loadMeshPoints(line,_in,_mesh);
    } else if ( line.contains("POINT_DATA") ) {

        bool ok = true;

        // Split the header line into components
        QStringList pointDataLine = line.split(" ",QString::SkipEmptyParts);

        // It has to contain the Keyword POINT_DATA , and the number of datasets
        if ( pointDataLine.size() != 2 ) {
          emit log(LOGERR,tr("Expected to get POINT_DATA line with exactly 2 entries, but %1 found!").arg(line.size()));
          return false;
        }

        // Number of polygons to read
        pointDataSize = pointDataLine[1].toUInt(&ok);

        pointData = true;
        cellData  = false;

        std::cerr <<  "Point data mode with " << pointDataSize << "Elements" << std::endl;

    } else if ( line.contains("CELL_DATA") ) {

      bool ok = true;

      // Split the header line into components
      QStringList cellDataLine = line.split(" ",QString::SkipEmptyParts);

      // It has to contain the Keyword CELL_DATA , and the number of datasets
      if ( cellDataLine.size() != 2 ) {
        emit log(LOGERR,tr("Expected to get CELL_DATA line with exactly 2 entries, but %1 found!").arg(line.size()));
        return false;
      }

      // Number of polygons to read
      cellDataSize = cellDataLine[1].toUInt(&ok);

      cellData  = true;
      pointData = false;

      std::cerr <<  "Cell data mode with " << cellDataSize << "Elements" << std::endl;

    } else if ( line.contains("VERTICES") ) {
      std::cerr << "Vertices will be skipped as they are already added!" << std::endl;
    } else if ( line.contains("LINES") ) {
      ok = loadMeshLines(line,_in,_mesh);
    } else if ( line.contains("POLYGONS") ) {
      ok = loadMeshPolygons(line,_in,_mesh);
    } else if ( line.contains("TRIANGLE_STRIPS") ) {
      ok = loadMeshTriangleStrips(line,_in,_mesh);
    } else if ( line.contains("CELL") ) {
      ok = loadMeshCells(line,_in,_mesh, cells);
    } else  if ( line.contains("NORMALS") ) {
      // load per point normals or per face normals
      if ( pointData ) {
        ok = loadMeshNormals(line,_in,_mesh,cells,true,pointDataSize);
        pointNormalsRead = true;
      } else if (cellData) {
        ok = loadMeshNormals(line,_in,_mesh,cells,false,cellDataSize);
        faceNormalsRead = true;
      } else {
        emit log(LOGERR,tr("Got normals keyword but we are neither in pointdata nor celldata mode") );
        return false;
      }

    } else {
      std::cerr << "Unrecognized keyword : " << line.toStdString() << std::endl;
    }

  }

  if ( !faceNormalsRead )
    updateFaceNormals(_mesh);

  if ( !pointNormalsRead )
      updateVertexNormals(_mesh);

  removeTemporaryProperties(_mesh);

  return true;
}

//-----------------------------------------------------------------------------------------------------

/// load a obj and force mesh datatype
int FileVTKPlugin::loadObject(QString _filename, DataType _type){

  if ( _type == DATA_TRIANGLE_MESH )
    forceTriangleMesh_ = true;
  else if ( _type == DATA_POLY_MESH )
      forcePolyMesh_ = true;

  return loadObject(_filename);
}

//-----------------------------------------------------------------------------------------------------

bool FileVTKPlugin::saveObject(int _id, QString _filename) {

    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);

    std::string filename = std::string( _filename.toUtf8() );

    std::fstream ofs( filename.c_str(), std::ios_base::out );

    if (!ofs) {

        emit log(LOGERR, tr("saveObject : Cannot not open file %1 for writing!").arg(_filename) );
        return false;
    }

    // Get user specified options
    updateUserOptions();

    if ( object->dataType( DATA_POLY_MESH ) ) {

        object->setFromFileName(_filename);
        object->setName(object->filename());

        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );

        if (writeMesh(ofs, *polyObj->mesh())){
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        }else{
            emit log(LOGERR, tr("Unable to save ") + _filename);
            ofs.close();
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

        object->setFromFileName(_filename);
        object->setName(object->filename());

        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );

        if (writeMesh(ofs, *triObj->mesh())) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename);
            ofs.close();
            return false;
        }
    }
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    else if ( object->dataType( DATA_POLYHEDRAL_MESH ) )
    {

        object->setFromFileName(_filename);
        object->setName(object->filename());

        PolyhedralMeshObject* polyhedralObj = dynamic_cast<PolyhedralMeshObject* >( object );

        if (writeMesh(ofs, *polyhedralObj->mesh())) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename);
            ofs.close();
            return false;
        }
    }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    else if ( object->dataType( DATA_HEXAHEDRAL_MESH ) )
    {

        object->setFromFileName(_filename);
        object->setName(object->filename());

        HexahedralMeshObject* polyhedralObj = dynamic_cast<HexahedralMeshObject* >( object );

        if (writeMesh(ofs, *polyhedralObj->mesh())) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename);
            ofs.close();
            return false;
        }
    }
#endif

    else {
           emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
           ofs.close();
           return false;
    }

}

//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileVTKPlugin::writeMesh(std::ostream& _out, MeshT& _mesh ) {

    /*****************
    * HEADER
    ******************/
    //every VTK file has to start with this
    _out << "# vtk DataFile Version 2.0\n";
    //write header info. Max. 256 characters!
    _out << "Mesh saved from OpenFlipper - www.openflipper.org\n";
    //only ASCII is supported right now
    _out << "ASCII\n";

    /*****************
    * DATA
    ******************/

    // Call corresponding write routine
    return writeASCIIData(_out, _mesh);
}

//------------------------------------------------------------------------------------------------------


void FileVTKPlugin::updateUserOptions() {

    // If the options dialog has not been initialized, keep
    // the initial values

    if( OpenFlipper::Options::nogui() )
        return;

    // Save options
    if(saveBinary_) {
        if(saveBinary_->isChecked()) userWriteOptions_ |= BINARY;
        else { if(userWriteOptions_ & BINARY) userWriteOptions_ -= BINARY; }
    }
    if(saveFaceNormals_) {
        if(saveFaceNormals_->isChecked()) userWriteOptions_ |= FACENORMALS;
        else { if(userWriteOptions_ & FACENORMALS) userWriteOptions_ -= FACENORMALS; }
    }
    if(saveVertexNormals_) {
        if(saveVertexNormals_->isChecked()) userWriteOptions_ |= VERTEXNORMALS;
        else { if(userWriteOptions_ & VERTEXNORMALS) userWriteOptions_ -= VERTEXNORMALS; }
    }
    if(saveVertexTexCoords_) {
        if(saveVertexTexCoords_->isChecked()) userWriteOptions_ |= VERTEXTEXCOORDS;
        else { if(userWriteOptions_ & VERTEXTEXCOORDS) userWriteOptions_ -= VERTEXTEXCOORDS; }
    }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( filevtkplugin , FileVTKPlugin );
#endif




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

#include <QtGui>
#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FileVTK.hh"

#include <iostream>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


class CellType {

public:

  CellType() :
          type(0)
  {
  }

  // The type of the cell
  unsigned int type;

  // list of indices in the cell
  std::vector<OpenMesh::VertexHandle> indices;
};

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
};

//-----------------------------------------------------------------------------------------------------

QString FileVTKPlugin::getSaveFilters() {
   return QString( tr("Visualization Toolkit ASCII ( *.vtk )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileVTKPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH | DATA_GROUP;

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
        
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);     
        
        saveOptions_->setLayout(layout);
        
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
  OpenFlipperSettings().setValue( "FileOff/Save/Binary",          saveBinary_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/FaceNormals",     saveFaceNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/VertexNormals",   saveVertexNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/VertexTexCoords", saveVertexTexCoords_->isChecked()  );
}

//-----------------------------------------------------------------------------------------------------
  
int FileVTKPlugin::loadObject(QString _filename) {
  std::cerr << "Loading vtk file" << std::endl;

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

  if ( forceTriangleMesh_  ){

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

      object->update();

      //general stuff
      emit openedFile( object->id() );

      PluginFunctions::viewAll();

      return id;
    }

  } else {

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);


    PolyMeshObject* object(0);

    if(PluginFunctions::getObject( id, object)){

      PolyMesh* _mesh;
      PluginFunctions::getMesh(id,_mesh);

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

      object->update();

      //general stuff
      emit openedFile( object->id() );

      PluginFunctions::viewAll();

      return id;

    }


  }

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
      std::vector< OpenMesh::VertexHandle > handles;
      
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
    std::vector< std::vector< OpenMesh::VertexHandle > > invalidFaces;

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
        std::vector< OpenMesh::VertexHandle > handles;

        for ( unsigned int i = 0 ; i < valence; ++i ) {

            // Read one index
            if ( !binary_ )
                _in >> index;
            else
                _in.device()->read((char*)&valence,sizeof(quint32));

            // Remember it
            handles.push_back( _mesh->vertex_handle(index) );
        }

        if ( _in.status() == QTextStream::Ok ) {
            OpenMesh::FaceHandle fh = _mesh->add_face(handles);

            // Try the other direction
            if ( ! fh.is_valid() ) {

                std::vector< OpenMesh::VertexHandle > inverseHandles;
                for ( int i = handles.size()-1 ; i >= 0 ; --i)
                    inverseHandles.push_back(handles[i]);

                fh = _mesh->add_face(inverseHandles);

                if ( !fh.is_valid() ) {
                    add_non_manifold_face(_mesh, handles);
                }
            }
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
void FileVTKPlugin::add_non_manifold_face(MeshT*& _mesh, std::vector< OpenMesh::VertexHandle >& _vhandles) {

    // Duplicate vertices of non-manifold faces
    // and add them as new isolated face
    if (_vhandles.empty()) return;

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
    QList< OpenMesh::VertexHandle > handles;
    
    // Read first two of strip:
    for ( unsigned int i = 0 ; i < 2; ++i ) {
      
      // Read one index
      if ( !binary_ )
        _in >> index;
      else
        _in.device()->read((char*)&valence,sizeof(quint32));
      
      // Remember it
      handles.push_back( _mesh->vertex_handle(index) );
    }
    
    // Read rest of strip while adding faces
    for ( unsigned int i = 2 ; i < valence; ++i ) {
      
      // Read one index
      if ( !binary_ )
        _in >> index;
      else
        _in.device()->read((char*)&valence,sizeof(quint32));
      
      // Remember it
      handles.push_back( _mesh->vertex_handle(index) );
      
      if ( _in.status() == QTextStream::Ok ) {
        // TODO : handle non manifold cases!
        if ( i % 2 == 0 )
          _mesh->add_face(handles[i],handles[i-1],handles[i-2]); 
        else
          _mesh->add_face(handles[i],handles[i-2],handles[i-1]); 
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
bool FileVTKPlugin::loadMeshNormals(QString _spec,QTextStream& _in,MeshT*& _mesh,bool _pointNormal, quint32 _count) {
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
        
        OpenMesh::VertexHandle vh = _mesh->vertex_handle(read);
        if ( vh.is_valid() )
          _mesh->set_normal( vh , normal ); 
        
      } else {
        
        OpenMesh::FaceHandle fh = _mesh->face_handle(read);
        if ( fh.is_valid() )
           _mesh->set_normal( fh, normal ); 
        
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
  
  //iterate over all contained vertices and set the normals
  OpenMesh::VPropHandleT< OpenMesh::VertexHandle > originalVertexIdx;
  if (! _mesh->get_property_handle(originalVertexIdx,"FileVTKPlugin_originalVertexIdx")) {
    _mesh->add_property( originalVertexIdx, "FileVTKPlugin_originalVertexIdx" );
  }
  
  typename MeshT::VertexIter vit = _mesh->vertices_begin();
  typename MeshT::VertexIter vend = _mesh->vertices_end();
    
  for(; vit != vend; ++vit) {
    if ( _mesh->property(originalVertexIdx, vit).is_valid() ) {
        //copied vertex found
        _mesh->set_normal( vit, _mesh->normal(_mesh->property (originalVertexIdx, vit) ) );
    }
  }
  
  return true;
}

template <typename MeshT>
bool FileVTKPlugin::loadMeshCells(QString _spec,QTextStream& _in,MeshT*& _mesh) {
  
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
  
  std::vector< CellType > cells;
  
  
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
      currentCell.indices.push_back( _mesh->vertex_handle(index) );
    }
    
    if ( _in.status() == QTextStream::Ok ) {
      cells.push_back(currentCell);
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
    emit log(LOGERR,tr("Expected to get CELL_TYPES line with exactly 3 entries, but %1 found!").arg(cellLine.size()));
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
    cells[read].type = type;
    
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
  for ( unsigned int i = 0 ; i < cells.size() ; ++i ) {
    if ( cells[i].type == 1 ) {
      
      //VERTEX
      // Nothing to do for meshes ... already added as vertex
      
    } else if ( cells[i].type == 2 ) {
      
      //POLY_VERTEX
      // Nothing to do for meshes ... already added as vertex
      
    } else if ( cells[i].type == 3 ) {
      
      //LINE
      emit log(LOGWARN,tr("Unsupported Cell Type LINE") );
      
    } else if ( cells[i].type == 4 ) {
      //POLY_LINE
      
      emit log(LOGWARN,tr("Unsupported Cell Type POLY_LINE") );
      
    } else if ( cells[i].type == 5 ) {
      //TRIANGLE
      
      
      OpenMesh::FaceHandle fh = _mesh->add_face(cells[i].indices);
      
      // Try the other direction
      if ( ! fh.is_valid() ) {
        
        std::vector< OpenMesh::VertexHandle > inverseHandles;
        for ( int j = cells[i].indices.size()-1 ; j > 0 ; --j)
          inverseHandles.push_back(cells[i].indices[j]);
        
        fh = _mesh->add_face(inverseHandles);
        
        if ( !fh.is_valid() ) {
          emit log(LOGERR,tr("Unable to add non-manifold triangle configuration!"));
          
          // TODO : handle complex non manifold cases!
        }
      }
      
    } else if ( cells[i].type == 6 ) {
      //TRIANGLE_STRIP
      
      for ( unsigned int j = 2 ; j < cells[i].indices.size() ; ++j) {
        if ( (j % 2) == 0 )
          _mesh->add_face(cells[i].indices[j-2],cells[i].indices[j],cells[i].indices[j-1]);
        else
          _mesh->add_face(cells[i].indices[j-2],cells[i].indices[j-1],cells[i].indices[j]);
      }
      
      //TODO : handle non manifold cases!
      
    } else if ( cells[i].type == 7 ) {
      //POLYGON
      
      OpenMesh::FaceHandle fh = _mesh->add_face(cells[i].indices);
      
      // Try the other direction
      if ( ! fh.is_valid() ) {
        
        std::vector< OpenMesh::VertexHandle > inverseHandles;
        for ( int j = cells[i].indices.size()-1 ; j > 0 ; --j)
          inverseHandles.push_back(cells[i].indices[j]);
        
        fh = _mesh->add_face(inverseHandles);
        
        if ( !fh.is_valid() ) {
          emit log(LOGERR,tr("Unable to add non-manifold polygon configuration!"));
          
          // TODO : handle complex non manifold cases!
        }
      }
      
    } else if ( cells[i].type == 8 ) {
      //PIXEL
      
      emit log(LOGWARN,tr("Unsupported Cell Type PIXEL") );
      
    } else if ( cells[i].type == 9 ) {
      //QUAD
      
      OpenMesh::FaceHandle fh = _mesh->add_face(cells[i].indices);
      
      // Try the other direction
      if ( ! fh.is_valid() ) {
        
        std::vector< OpenMesh::VertexHandle > inverseHandles;
        for ( int j = cells[i].indices.size()-1 ; j > 0 ; --j)
          inverseHandles.push_back(cells[i].indices[j]);
        
        fh = _mesh->add_face(inverseHandles);
        
        if ( !fh.is_valid() ) {
          emit log(LOGERR,tr("Unable to add non-manifold quad configuration!"));
          
          // TODO : handle complex non manifold cases!
        }
      }
        
    } else if ( cells[i].type == 10 ) {
      //Tetra
      
      emit log(LOGWARN,tr("Unsupported Cell Type Tetra") );
      
    } else if ( cells[i].type == 11 ) {
      //VOXEL
      
      emit log(LOGWARN,tr("Unsupported Cell Type VOXEL") );
      
    } else if ( cells[i].type == 12 ) {
      //HEXAHEDRON
      
      emit log(LOGWARN,tr("Unsupported Cell Type HEXAHEDRON") );
    } else if ( cells[i].type == 13 ) {
      //WEDGE
      
      emit log(LOGWARN,tr("Unsupported Cell Type WEDGE") );
    } else if ( cells[i].type == 14 ) {
      //PYRAMID
      
      emit log(LOGWARN,tr("Unsupported Cell Type PYRAMID") );
    } else {
      emit log(LOGERR,tr("Unknown cell type").arg(cells[i].type) );
    }
  }
  
  std::cerr << "Read " << read << "Cells " << std::endl;
  std::cerr << "Vector has size: " << cells.size() << std::endl;
  
  
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
      ok = loadMeshCells(line,_in,_mesh);
    } else  if ( line.contains("NORMALS") ) {
      // load per point normals or per face normals
      if ( pointData ) {
        ok = loadMeshNormals(line,_in,_mesh,true,pointDataSize);  
        pointNormalsRead = true;
      } else if (cellData) {
        ok = loadMeshNormals(line,_in,_mesh,false,cellDataSize);  
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
    _mesh->update_face_normals(); 
  
  if ( !pointNormalsRead )
    _mesh->update_vertex_normals();
  
  //if we added this property temporarily, we need to remove it now
  OpenMesh::VPropHandleT< OpenMesh::VertexHandle > originalVertexIdx;
  if (! _mesh->get_property_handle(originalVertexIdx,"FileVTKPlugin_originalVertexIdx")) {
    _mesh->remove_property( originalVertexIdx );
  }

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
    } else {
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

template< class MeshT >
bool FileVTKPlugin::writeASCIIData(std::ostream& _out, MeshT& _mesh ) {

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



Q_EXPORT_PLUGIN2( filevtkplugin , FileVTKPlugin );


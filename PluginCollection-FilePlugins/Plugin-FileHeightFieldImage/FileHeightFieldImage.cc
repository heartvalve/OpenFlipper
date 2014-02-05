/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#include "FileHeightFieldImage.hh"
#include "ImageDialog.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

void FileHeightFieldPNGPlugin::initializePlugin() {
}

QString FileHeightFieldPNGPlugin::getLoadFilters() {
  QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
  QString formats;

  if ( imageFormats.contains("jpeg")) {
    formats += "*.jpeg *.jpg ";
  }

  if ( imageFormats.contains("tiff")) {
    formats += "*.tiff ";
  }

  if ( imageFormats.contains("tif")) {
    formats += "*.tif ";
  }

  if ( imageFormats.contains("png")) {
    formats += "*.png";
  }

  return QString( tr("HeightField PNG files ( ")+ formats + ")" );
};

QString FileHeightFieldPNGPlugin::getSaveFilters() {
  return QString( "" );
};

DataType  FileHeightFieldPNGPlugin::supportedType() {
  DataType type = DATA_TRIANGLE_MESH;
  return type;
}



int FileHeightFieldPNGPlugin::loadObject(QString _filename)
{

  QFile file(_filename);
  if ( !file.exists() ) {
    emit log(LOGERR,tr("Unable to load file: ") + _filename);
    return -1;
  }

  QImage image(_filename);

  // Prepare for taking specific regions from the file (defaults to full size)
  int minX = 0;
  int maxX = image.width();

  int minY = 0;
  int maxY = image.height();

  // Try to calculate a reasonable height
  double height = image.height() / 100;

  if ( OpenFlipper::Options::gui() ) {
    ImageDialog* imageDialog = new ImageDialog(image);
    imageDialog->heightValue->setValue(height);

    int result = imageDialog->exec();

    if ( result == QDialog::Accepted) {
      minX   = imageDialog->minX->value();
      maxX   = imageDialog->maxX->value();
      minY   = imageDialog->minY->value();
      maxY   = imageDialog->maxY->value();
      height = imageDialog->heightValue->value();
    } else {
      return -1;
    }
  }


  int id = -1;
  emit addEmptyObject( DATA_TRIANGLE_MESH, id );

  TriMeshObject* object = 0;
  if(PluginFunctions::getObject( id, object))
  {
    TriMesh* mesh = object->mesh();

    if ( mesh ) {


      // Load the data into the mesh.
      loadImageAsTriangleMesh(image,mesh,minX ,maxX , minY ,maxY, height);

      // Make sure everything is ready
      object->update();

      // Tell core about update
      emit openedFile( id );

      // Update viewport
      PluginFunctions::viewAll();
    }
  }

  return id;
}

void FileHeightFieldPNGPlugin::loadImageAsTriangleMesh(QImage& _image,TriMesh* _mesh,int _minX , int _maxX , int _minY , int _maxY, double _height) {

  const int mWidth  = _maxY - _minY;
  const int mHeigth = _maxX - _minX;

  // Reserve to get reasonable memory usage
  _mesh->reserve( mWidth*mHeigth, mWidth*mHeigth * 4, mWidth*mHeigth * 2  );

  for ( int i = _minX ; i < _maxX ; ++i ) {
    for ( int j = _minY ; j < _maxY ; ++j ) {
      const QColor currentColor = _image.pixel(i,j);
      double value = std::max(currentColor.redF(),currentColor.blueF());
      value = std::max(currentColor.greenF(),value);
      _mesh->add_vertex(TriMesh::Point(i,j,-value * _height));
    }
  }

    // Triangulate
    for ( int i = 0 ; i < mHeigth - 1 ; ++i ) {
      const int upperStart = mWidth * i;
      const int lowerStart = mWidth * (i + 1);

      for ( int j = 0 ; j < mWidth -1 ; ++j ) {
        std::vector<TriMesh::VertexHandle> handles;
        handles.push_back( _mesh->vertex_handle(upperStart + j) );
        handles.push_back( _mesh->vertex_handle(lowerStart + j + 1) );
        handles.push_back( _mesh->vertex_handle(lowerStart + j) );


        _mesh->add_face(handles);

        handles.clear();

        handles.push_back( _mesh->vertex_handle(upperStart + j) );
        handles.push_back( _mesh->vertex_handle(upperStart + j + 1) );
        handles.push_back( _mesh->vertex_handle(lowerStart + j + 1) );

        _mesh->add_face(handles);
      }
    }

  //Calculate some normals
  _mesh->update_normals();
}


bool FileHeightFieldPNGPlugin::saveObject(int _id, QString _filename) {
  return false;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( fileheightfieldPpngplugin , FileHeightFieldPNGPlugin );
#endif



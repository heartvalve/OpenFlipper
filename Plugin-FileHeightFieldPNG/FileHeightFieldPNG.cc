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


#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileHeightFieldPNG.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

void FileHeightFieldPNGPlugin::initializePlugin() {
}

QString FileHeightFieldPNGPlugin::getLoadFilters() {
  return QString( tr("HeightField PNG files ( *.png )") );
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
  int id = -1;
  emit addEmptyObject( DATA_TRIANGLE_MESH, id );

  TriMeshObject* object = 0;
  if(PluginFunctions::getObject( id, object))
  {
    TriMesh* mesh = object->mesh();

    if ( mesh ) {

      QFile file(_filename);
      if ( !file.exists() ) {
        emit log(LOGERR,tr("Unable to load file: ") + _filename);
        return -1;
      }

      QImage image(_filename);

      for ( int i = 0 ; i < image.width() ; ++i ) {
        for ( int j = 0 ; j < image.height() ; ++j ) {
          QColor currentColor = image.pixel(i,j);
          double value = std::max(currentColor.redF(),currentColor.blueF());
          value = std::max(currentColor.greenF(),value);
          mesh->add_vertex(TriMesh::Point(i,j,-value));
        }
      }

      // Triangulate
      for ( int i = 0 ; i < image.height() - 1 ; ++i ) {
        const int upperStart = image.width() * i;
        const int lowerStart = image.width() * (i + 1);

        for ( int j = 0 ; j < image.width() - 1 ; ++j ) {
          std::vector<TriMesh::VertexHandle> handles;
          handles.push_back( mesh->vertex_handle(upperStart + j) );
          handles.push_back( mesh->vertex_handle(lowerStart + j + 1) );
          handles.push_back( mesh->vertex_handle(lowerStart + j) );


          mesh->add_face(handles);

          handles.clear();

          handles.push_back( mesh->vertex_handle(upperStart + j) );
          handles.push_back( mesh->vertex_handle(upperStart + j + 1) );
          handles.push_back( mesh->vertex_handle(lowerStart + j + 1) );

          mesh->add_face(handles);
        }
      }

      //Calculate some normals
      mesh->update_normals();

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

bool FileHeightFieldPNGPlugin::saveObject(int _id, QString _filename) {
  return false;
}


Q_EXPORT_PLUGIN2( fileheightfieldPpngplugin , FileHeightFieldPNGPlugin );


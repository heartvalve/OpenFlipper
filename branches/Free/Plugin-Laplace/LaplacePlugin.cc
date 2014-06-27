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


#include "LaplacePlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

#define UNIFORM_LAPLACE_NAME "Uniform Laplace Length"
#define UNIFORM_LAPLACE_SQUARED_NAME "Uniform Laplace Squared Length"

void LaplaceLengthPlugin::pluginsInitialized()
{
  emit addTexture( UNIFORM_LAPLACE_NAME , "laplace_length.png" , 1 );
  emit setTextureMode(UNIFORM_LAPLACE_NAME,"clamp=true,center=true,repeat=false,clamp_min=-20,clamp_max=20");

  emit addTexture( UNIFORM_LAPLACE_SQUARED_NAME , "laplace_length.png" , 1 );
  emit setTextureMode(UNIFORM_LAPLACE_SQUARED_NAME,"clamp=true,center=true,repeat=false,clamp_min=-20,clamp_max=20");
}

void LaplaceLengthPlugin::slotUpdateTexture( QString _textureName , int _identifier )
{
   if ( (_textureName != UNIFORM_LAPLACE_SQUARED_NAME) && (_textureName != UNIFORM_LAPLACE_NAME ) ) {
      return;
   }

   BaseObjectData* object;
   if (! PluginFunctions::getObject(  _identifier , object ) ) {
      return;
   }

   if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      if ( _textureName == UNIFORM_LAPLACE_NAME ) {
         computeLaplaceLength(mesh);
         emit updatedTextures(UNIFORM_LAPLACE_NAME,_identifier);
      }
      if ( _textureName == UNIFORM_LAPLACE_SQUARED_NAME ) {
         computeLaplaceSquaredLength(mesh);
         emit updatedTextures(UNIFORM_LAPLACE_SQUARED_NAME,_identifier);
      }
   }

   if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      if ( _textureName == UNIFORM_LAPLACE_NAME ) {
         computeLaplaceLength(mesh);
         emit updatedTextures(UNIFORM_LAPLACE_NAME,_identifier);
      }
      if ( _textureName == UNIFORM_LAPLACE_SQUARED_NAME ) {
         computeLaplaceSquaredLength(mesh);
         emit updatedTextures(UNIFORM_LAPLACE_SQUARED_NAME,_identifier);
      }
   }
}

template< typename MeshT >
void LaplaceLengthPlugin::computeLaplaceLength(MeshT* _mesh) {
  OpenMesh::VPropHandleT< ACG::Vec3d > laplace_vector_property;
  OpenMesh::VPropHandleT< double > laplace_length_property;

  if(!_mesh->get_property_handle( laplace_vector_property , "Laplace Vector" ))
    _mesh->add_property( laplace_vector_property, "Laplace Vector" );

  if(!_mesh->get_property_handle( laplace_length_property , UNIFORM_LAPLACE_NAME ))
    _mesh->add_property( laplace_length_property, UNIFORM_LAPLACE_NAME );


  QTime time;
  time.start();
  std::vector< typename MeshT::VertexHandle > handles;
  handles.reserve(_mesh->n_vertices());
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
    handles.push_back( *v_it );

  #ifdef USE_OPENMP
    #pragma omp parallel for
  #endif
  for ( int i = 0 ; i < (int)handles.size(); ++i ) {
    const typename MeshT::VertexHandle handle = handles[i];

    ACG::Vec3d laplace(0.0,0.0,0.0);
    for ( typename MeshT::VertexVertexIter vv_it(*_mesh , handle) ; vv_it.is_valid() ; ++vv_it )
        laplace += _mesh->point(*vv_it) - _mesh->point(handle);

    laplace = 1.0 /(double)_mesh->valence(handle) * laplace;
    _mesh->property(laplace_vector_property,handle) = laplace;
    _mesh->property(laplace_length_property,handle) = laplace.norm();
  }

//   #ifdef USE_OPENMP
//     std::cerr << "Laplace parallel took : " << time.elapsed() << std::endl;
//   #else
//     std::cerr << "Laplace sequential took : " << time.elapsed() << std::endl;
//   #endif

}

template< typename MeshT >
void LaplaceLengthPlugin::computeLaplaceSquaredLength(MeshT* _mesh) {
  computeLaplaceLength(_mesh);

  OpenMesh::VPropHandleT< ACG::Vec3d > laplace_property;
  OpenMesh::VPropHandleT< double > laplace_squared;

  if(!_mesh->get_property_handle( laplace_property , "Laplace Vector" )) {
    std::cerr << "LaplaceLengthPlugin : Unable to get Laplace Vector property" << std::endl;
    return;
  }

  if(!_mesh->get_property_handle( laplace_squared , UNIFORM_LAPLACE_SQUARED_NAME ))
    _mesh->add_property( laplace_squared, UNIFORM_LAPLACE_SQUARED_NAME );

  QTime time;
  time.start();
  std::vector< typename MeshT::VertexHandle > handles;
  handles.reserve(_mesh->n_vertices());
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
    handles.push_back( *v_it );

  #ifdef USE_OPENMP
    #pragma omp parallel for
  #endif
  for ( int i = 0 ; i < (int)handles.size(); ++i ) {
    const typename MeshT::VertexHandle handle = handles[i];

    ACG::Vec3d laplace(0.0,0.0,0.0);
    for ( typename MeshT::VertexVertexIter vv_it(*_mesh , handle) ; vv_it.is_valid() ; ++vv_it )
        laplace += _mesh->property(laplace_property,*vv_it) - _mesh->property(laplace_property,handle);
    laplace = 1.0 /(double)_mesh->valence(handle) * laplace;
    _mesh->property(laplace_squared,handle) = laplace.norm();
  }

//   #ifdef USE_OPENMP
//     std::cerr << "Laplace Squared  parallel took : " << time.elapsed() << std::endl;
//   #else
//     std::cerr << "Laplace Squared sequential took : " << time.elapsed() << std::endl;
//   #endif
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( laplacelengthplugin , LaplaceLengthPlugin );
#endif




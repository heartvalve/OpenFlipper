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


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "MeanCurvature.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <MeshTools/Curvature.hh>

#ifdef USE_OPENMP
#include <omp.h>
#endif

MeanCurvaturePlugin::MeanCurvaturePlugin()
{
}


MeanCurvaturePlugin::~MeanCurvaturePlugin()
{
}


void MeanCurvaturePlugin::pluginsInitialized()
{
  emit addTexture( "Mean Curvature" , "mean_curvature.png" , 1 );
  emit setTextureMode("Mean Curvature","clamp=true,center=true,repeat=false,clamp_min=-20,clamp_max=20");

  emit setSlotDescription(tr("computeMeanCurvature(int)"), tr("Compute the mean curvature on a mesh. The curvature will be stored on the mesh on the vertex property called \"Mean Curvature\""),
        QStringList(tr("ObjectId")), QStringList(tr("Id of the mesh")));
}

void MeanCurvaturePlugin::slotUpdateTexture( QString _textureName , int _identifier )
{
   if ( _textureName != "Mean Curvature") {
      return;
  }

   BaseObjectData* object;
   if (! PluginFunctions::getObject(  _identifier , object ) ) {
      return;
   }

   if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      computeMeanCurvature(mesh);
   }

   if ( object->dataType( DATA_POLY_MESH ) ) {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);
      computeMeanCurvature(mesh);
   }

   emit updatedTextures("Mean Curvature",_identifier);
}

bool MeanCurvaturePlugin::computeMeanCurvature(int _objectId) {
  BaseObjectData* object;
  if (! PluginFunctions::getObject(  _objectId , object ) ) {
    return false;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    computeMeanCurvature(mesh);
    return true;
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    computeMeanCurvature(mesh);
    return true;
  }

  return false;
}

template< typename MeshT >
void MeanCurvaturePlugin::computeMeanCurvature(MeshT* _mesh) {

  OpenMesh::VPropHandleT< double > mean;

  if(!_mesh->get_property_handle( mean, "Mean Curvature"))
    _mesh->add_property( mean, "Mean Curvature" );

  //QTime time;
  //time.start();
  std::vector< typename MeshT::VertexHandle > handles;
  handles.reserve(_mesh->n_vertices());
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
    handles.push_back( *v_it );

  #ifdef USE_OPENMP
    #pragma omp parallel for
  #endif
  for ( int i = 0 ; i < (int)handles.size(); ++i ) {

    const typename MeshT::VertexHandle handle = handles[i];
    ACG::Vec3d curva(0.0,0.0,0.0);
    double area = 0.0;
    curvature::discrete_mean_curv_op<MeshT,ACG::Vec3d,double>(*_mesh,handle,curva,area);
    double curv  = curva.norm();

    if ( (curva | _mesh->normal(handle)) <0 )
      curv = -curv;

    _mesh->property(mean,handle) =  curv;
  }

}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( meancurvatureplugin , MeanCurvaturePlugin );
#endif




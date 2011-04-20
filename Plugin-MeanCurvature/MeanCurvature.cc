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

void MeanCurvaturePlugin::pluginsInitialized()
{
  emit addTexture( "Mean Curvature" , "mean_curvature.png" , 1 );
  emit setTextureMode("Mean Curvature","clamp=true,center=true,repeat=false,clamp_min=-20,clamp_max=20");
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
    handles.push_back( v_it.handle() );

  /*
   * OMP parallelization fails for some
   * reason on Darwin architectures.
   */
  #if defined(USE_OPENMP) && !defined(__APPLE__)
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
/*
  #ifdef USE_OPENMP
    std::cerr << "MeanCurvature parallel took : " << time.elapsed() << std::endl;
  #else
    std::cerr << "MeanCurvature sequential took : " << time.elapsed() << std::endl;
  #endif*/

}

Q_EXPORT_PLUGIN2( meancurvatureplugin , MeanCurvaturePlugin );


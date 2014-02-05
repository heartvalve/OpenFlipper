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


#include "GaussCurvature.hh"

#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <MeshTools/Curvature.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef USE_OPENMP
#include <omp.h>
#endif


GaussCurvaturePlugin::GaussCurvaturePlugin()
{
}


GaussCurvaturePlugin::~GaussCurvaturePlugin()
{
}



void GaussCurvaturePlugin::pluginsInitialized()
{
  emit addTexture( "Gaussian Curvature" , "gauss_curvature.png" , 1 );
  emit setTextureMode("Gaussian Curvature","clamp=true,clamp_min=-1,clamp_max=1,center=true");

  emit setSlotDescription(tr("computeGaussCurvature(int)"), tr("Compute the gaussian curvature on a mesh. The curvature will be stored on the mesh on the vertex property called \"Gaussian Curvature\""),
          QStringList(tr("ObjectId")), QStringList(tr("Id of the mesh")));
}

void GaussCurvaturePlugin::slotUpdateTexture( QString _textureName , int _identifier )
{
   if ( _textureName != "Gaussian Curvature")
      return;

   BaseObjectData* object;
   if (! PluginFunctions::getObject(  _identifier , object ) ) {
      return;
   }

   if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMesh* mesh = PluginFunctions::triMesh(object);
      computeGaussianCurvature(mesh);
   }

   if ( object->dataType( DATA_POLY_MESH ) ) {
     PolyMesh* mesh = PluginFunctions::polyMesh(object);
      computeGaussianCurvature(mesh);
   }

   emit updatedTextures("Gaussian Curvature",_identifier);
}


bool GaussCurvaturePlugin::computeGaussCurvature(int _objectId) {
  BaseObjectData* object;
  if (! PluginFunctions::getObject(  _objectId , object ) ) {
    return false;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);
    computeGaussianCurvature(mesh);
    return true;
  }

  if ( object->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    computeGaussianCurvature(mesh);
    return true;
  }

  return false;
}


template< typename MeshT >
void GaussCurvaturePlugin::computeGaussianCurvature( MeshT* _mesh) {
  OpenMesh::VPropHandleT< double > gauss;

  if(!_mesh->get_property_handle( gauss, "Gaussian Curvature"))
    _mesh->add_property( gauss, "Gaussian Curvature" );

#ifdef USE_OPENMP
  std::vector<  typename MeshT::VertexHandle > handles;
  handles.reserve(_mesh->n_vertices());
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
    handles.push_back( *v_it );


  #pragma omp parallel for
  for ( int i = 0 ; i < (int)handles.size(); ++i )
    _mesh->property(gauss,handles[i]) = curvature::gauss_curvature(*_mesh,handles[i]);

#else
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
      _mesh->property(gauss,*v_it) =  curvature::gauss_curvature(*_mesh,*v_it);
#endif

}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( gausscurvatureplugin , GaussCurvaturePlugin );
#endif


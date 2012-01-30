
#include <QtGui>

#include "GaussCurvature.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <MeshTools/Curvature.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef USE_OPENMP
#include <omp.h>
#endif


void GaussCurvaturePlugin::pluginsInitialized()
{
  emit addTexture( "Gaussian Curvature" , "gauss_curvature.png" , 1 );
  emit setTextureMode("Gaussian Curvature","clamp=true,clamp_min=-1,clamp_max=1,center=true");
}

void GaussCurvaturePlugin::exit()
{
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
//       std::cerr << "Gauss : PolyMeshes not supported" << std::endl;
   }

   emit updatedTextures("Gaussian Curvature",_identifier);
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
    handles.push_back( v_it.handle() );


  #pragma omp parallel for
  for ( int i = 0 ; i < (int)handles.size(); ++i )
    _mesh->property(gauss,handles[i]) = curvature::gauss_curvature(*_mesh,handles[i]);

#else
  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end(); ++v_it)
      _mesh->property(gauss,v_it) =  curvature::gauss_curvature(*_mesh,v_it);
#endif

}

Q_EXPORT_PLUGIN2( gausscurvatureplugin , GaussCurvaturePlugin );


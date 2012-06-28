//=============================================================================
//
//  CLASS CatmullClarkPlugin - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <Qt>
#include <QtGui>
#include <QSpacerItem>

#include "CatmullClarkPlugin.hh"

#include <iostream>
#include <ACG/Utils/StopWatch.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

// #include <PhySim/QtWidgets/QwtFunctionPlot.hh>
// #include <PhySim/Meshes/PrincipalCurvatureT.hh>
// #include <PhySim/Meshes/PrincipalCurvatureJetT.hh>
// #include <PhySim/Math/Algorithms.hh>


#undef min
#undef max


//== IMPLEMENTATION ==========================================================


void CatmullClarkPlugin::initializePlugin()
{
  tool_ = new CatmullClarkToolbar();
  
  QSize size(300,300);
  tool_->resize(size);
  
  // connect button press event to function slotCompute()
  connect(tool_->compute_pb, SIGNAL( clicked() ), this, SLOT( slotCompute() ) );
  
  emit addToolbox(tr("CatMullClark Subdivider"),tool_);
}



//-----------------------------------------------------------------------------


void CatmullClarkPlugin::pluginsInitialized() 
{
//   emit addPickMode("Separator");
}


//-----------------------------------------------------------------------------

void CatmullClarkPlugin::slotCompute() 
{
  // iterate over all target poly meshes
  for ( PluginFunctions::ObjectIterator
      o_it(PluginFunctions::TARGET_OBJECTS,DATA_POLY_MESH) ; o_it != PluginFunctions::objectsEnd(); ++o_it) 
  {
    // get pointer to mesh
    PolyMesh* mesh = dynamic_cast< PolyMeshObject* >( *o_it )->mesh();
    if ( mesh )
    {
      OpenMesh::Subdivider::Uniform::CatmullClarkT<PolyMesh> CC;

      CC.attach(*mesh);
      CC(tool_->iters_spinBox->value());
      CC.detach();

      emit updatedObject( o_it->id() );

      // get feature line object and call compute function
     // get_catmullclark_object( *o_it )->compute();
    }
  }
}


//-----------------------------------------------------------------------------


Q_EXPORT_PLUGIN2( catmullclarkplugin , CatmullClarkPlugin );


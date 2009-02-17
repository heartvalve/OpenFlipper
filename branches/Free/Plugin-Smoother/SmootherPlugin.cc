//=============================================================================
//
//  CLASS SmootherPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include <QtGui>

#include "SmootherPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/ManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "SmootherObject.hh"

#define SMOOTHER "SmootherData"

//== IMPLEMENTATION ==========================================================


bool
SmootherPlugin::
initializeToolbox(QWidget*& _widget)
{
  tool_ = new SmootherToolbarWidget();
  _widget = tool_;
  QSize size(100, 100);
  tool_->resize(size);

  // connect signals->slots
	connect(tool_->pB_smooth,SIGNAL(clicked() ),this,SLOT(slot_smooth()));

  return true;
}

//-----------------------------------------------------------------------------

void
SmootherPlugin::
slot_smooth()
{
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    TriMeshObject* object = PluginFunctions::triMeshObject(*o_it);

    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      continue;
    }

    SmootherObject* data = dynamic_cast< SmootherObject* > ( o_it->objectData(SMOOTHER) );

    if (data == 0){
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);
      data = new SmootherObject(mesh);
      o_it->setObjectData(SMOOTHER, data);
    }


    OpenMesh::Smoother::SmootherT< TriMesh >::Component component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    if( tool_->rbTangential_and_Normal->isChecked() )
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    else if( tool_->rbNormal->isChecked() )
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Normal;
    else if( tool_->rbTangential->isChecked() )
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential;

    OpenMesh::Smoother::SmootherT< TriMesh >::Continuity continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
    if( tool_->rB_c0->isChecked() )
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
    else if( tool_->rB_c1->isChecked() )
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C1;

    // Read maximum distance Error from lineEdit
    if ( tool_->cbDistance->isChecked() ) {
      QString value;
      value = tool_->distance->text();
      bool ok = false;

      double absoluteError = value.toDouble(&ok);

      if ( ok )
        data->smoother->set_absolute_local_error( absoluteError );
      else
        emit log(LOGWARN , "Unable to read distance error from LineEdit");
    }

    data->smoother->initialize(component,continuity);

    data->smoother->smooth( tool_->sB_iter->value() );

    TriMesh* mesh = PluginFunctions::triMesh(*o_it);

    if (mesh != 0)
      mesh->garbage_collection();

    emit updatedObject( o_it->id() );
  }

}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( SmootherPlugin , SmootherPlugin );


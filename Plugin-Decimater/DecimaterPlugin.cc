//=============================================================================
//
//  CLASS DecimaterPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include <QtGui>

#include "DecimaterPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/ManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "DecimaterInfo.hh"

#define DECIMATER "DecimaterData"

//== IMPLEMENTATION ==========================================================

bool
DecimaterPlugin::
initializeToolbox(QWidget*& _widget)
{
  tool_ = new DecimaterToolbarWidget();
  _widget = tool_;
  QSize size(100, 100);
  tool_->resize(size);

  // connect signals->slots
	connect(tool_->pbDecimate,SIGNAL(clicked() ),this,SLOT(slot_decimate()));

   return true;
}

//-----------------------------------------------------------------------------

void
DecimaterPlugin::
slot_decimate()
{

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    //initialize
    TriMeshObject* object = PluginFunctions::triMeshObject(*o_it);

    if ( object == 0 )
      emit log(LOGWARN , "Unable to get object");

    DecimaterInfo* decimater = dynamic_cast< DecimaterInfo* > ( o_it->objectData(DECIMATER) );

    if (decimater == 0){
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);
      decimater = new DecimaterInfo( tool_, mesh );
      o_it->setObjectData(DECIMATER, decimater);
    }


    decimater->update();

    if( ! decimater->decimater()->is_initialized() ){
      emit log(LOGWARN, "Decimater could not be initialized");
      continue;
    }

    //decimate
    if ( tool_->cbTriangles->isChecked() )
      decimater->decimater()->decimate_to( tool_->triangleCount->value() );         // do decimation
    else
      decimater->decimater()->decimate();         // do decimation

    object->mesh()->garbage_collection();
    object->mesh()->update_normals();
    object->update();

  }

  emit updateView();
}


//-----------------------------------------------------------------------------

void DecimaterPlugin::decimate(int _objID, QString _constraints, QString _values){


  BaseObjectData* baseObjectData;
  if ( ! PluginFunctions::getObject(_objID,baseObjectData) ) {
    emit log(LOGERR,"Unable to get Object");
    return;
  }

  if ( baseObjectData->dataType() == DATA_TRIANGLE_MESH ) {
    TriMeshObject* object = PluginFunctions::triMeshObject(baseObjectData);

    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      return;
    }

    DecimaterInfo* decimater = dynamic_cast< DecimaterInfo* > ( object->objectData(DECIMATER) );

    if (decimater == 0){
      TriMesh* mesh = PluginFunctions::triMesh(baseObjectData);
      decimater = new DecimaterInfo( tool_, mesh );
      object->setObjectData(DECIMATER, decimater);
    }

    decimater->update();

    if( ! decimater->decimater()->is_initialized() ){
      emit log(LOGWARN, "Decimater could not be initialized");
      return;
    }

    //decimate
    if ( tool_->cbTriangles->isChecked() )
      decimater->decimater()->decimate_to( tool_->triangleCount->value() );         // do decimation
    else
      decimater->decimater()->decimate();         // do decimation

    object->mesh()->garbage_collection();
    object->mesh()->update_normals();
    object->update();


  } else {
    emit log(LOGERR,"Unsupported object type for decimater");
    return;
  }

}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( DecimaterPlugin , DecimaterPlugin );


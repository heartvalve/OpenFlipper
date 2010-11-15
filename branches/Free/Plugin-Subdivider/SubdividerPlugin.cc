
#include <QtGui>

#include "SubdividerPlugin.hh"

#include <iostream>
#include <algorithm>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>
#include <ACG/Scenegraph/PointNode.hh>

void SubdividerPlugin::initializePlugin() 
{
  if ( OpenFlipper::Options::gui() ) {
    tool_ = new subdividerToolbarWidget();
    QSize size(300, 300);
    tool_->resize(size);
    
    connect(tool_->subdivide_toolButton,    SIGNAL( clicked() ),  this, SLOT( slot_subdivide() ) );
    
    emit addToolbox( tr("Subdivider") , tool_ );
  }
}

void SubdividerPlugin::slotPickModeChanged( const std::string& /*_mode*/) 
{
}

void SubdividerPlugin::pluginsInitialized() 
{
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::slot_subdivide()
{
  std::vector< int > ids;
  if ( PluginFunctions::getTargetIdentifiers( ids ) )
  {
    for (unsigned int i = 0; i < ids.size(); ++i)
    {
      BaseObjectData* object;
      if(!test_trimesh_object(ids[i], object))
        return;

      int steps  = tool_->subdivision_steps_spinBox->value();
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if(tool_->loop_radioButton->isChecked())
      {
        OpenMesh::Subdivider::Uniform::LoopT<TriMesh,double> subdivider;
        
        subdivider.attach(*mesh);
        subdivider(steps);
        subdivider.detach();
      }
      else
      {
        OpenMesh::Subdivider::Uniform::Sqrt3T<TriMesh,double> subdivider;
        
        subdivider.attach(*mesh);
        subdivider(steps);
        subdivider.detach();
      }

      mesh->update_face_normals();
      mesh->update_vertex_normals();
      TriMeshObject* tmo = PluginFunctions::triMeshObject(object);
      tmo->update();
    
      // Create backup
      emit createBackup(object->id(), "Subdivider");
      emit updatedObject(object->id(), UPDATE_TOPOLOGY);
    }
  }
  emit updateView();
}


//-----------------------------------------------------------------------------


bool SubdividerPlugin::test_trimesh_object(int _identifier, BaseObjectData*& _object)
{
  if ( _identifier == -1)
    return false;

  if (! PluginFunctions::getObject(_identifier,_object) ) 
    return false;  

  if (!_object->dataType(DATA_TRIANGLE_MESH) )
    return false;
  return true;
}


//-----------------------------------------------------------------------------

void SubdividerPlugin::slotMouseEvent( QMouseEvent* /*_event*/ )
{
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::slotKeyEvent( QKeyEvent* _event )
{
  switch (_event->key()) 
  {
    default:
      break;
  }
} // END slotKeyEvent

Q_EXPORT_PLUGIN2( subdividerplugin , SubdividerPlugin );
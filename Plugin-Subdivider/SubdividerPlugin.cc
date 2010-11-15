
#include <QtGui>

#include "SubdividerPlugin.hh"

#include <iostream>
#include <algorithm>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/INIFile/INIFile.hh>
#include <ACG/Scenegraph/PointNode.hh>

#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/Sqrt3T.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/Sqrt3InterpolatingSubdividerLabsikGreinerT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/ModifiedButterFlyT.hh>

void SubdividerPlugin::initializePlugin() 
{
  if ( OpenFlipper::Options::gui() ) {
    tool_ = new subdividerToolbarWidget();
    QSize size(300, 300);
    tool_->resize(size);
    
    connect(tool_->subdivide_uniform_toolButton,    SIGNAL( clicked() ),  this, SLOT( slotSubdivideUniform() ) );
    
    emit addToolbox( tr("Subdivider") , tool_ );
  }
}

void SubdividerPlugin::pluginsInitialized() 
{
  emit setSlotDescription("subdivide(int,QString,int)", "Smooth a triangular mesh",
                          QString("object_id,algorithm,iterations").split(","),
                          QString("id of an object, algorithm to use (loop | sqrt3 | interpolating_sqrt3 | modifiedButterfly ), number of iterations").split(","));
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::slotSubdivideUniform()
{
  std::vector< int > ids;
  if ( PluginFunctions::getTargetIdentifiers( ids ) )
  {
    for (unsigned int i = 0; i < ids.size(); ++i)
    {
     
      if(tool_->loop_radioButton->isChecked())
      {
        subdivide(ids[i],"loop",tool_->subdivision_steps_spinBox->value());
      }
      else if ( tool_->sqrt3_radioButton->isChecked() )
      {
        subdivide(ids[i],"sqrt3",tool_->subdivision_steps_spinBox->value());
      }
      else if ( tool_->LabsikGreiner_radioButton->isChecked()  )
      {
        subdivide(ids[i],"interpolating_sqrt3",tool_->subdivision_steps_spinBox->value());
      }
      else if ( tool_->modifiedButterfly_radioButton->isChecked()  )
      {
        subdivide(ids[i],"modifiedButterfly",tool_->subdivision_steps_spinBox->value());
      }
    }
  }
  emit updateView();
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::subdivide(int _objectId, QString _algorithm , int _steps) {

  BaseObjectData* object;
  if(!test_trimesh_object(_objectId , object))
    return;

  TriMesh* mesh = PluginFunctions::triMesh(object);

  if(_algorithm.contains("loop",Qt::CaseInsensitive))
  {
    OpenMesh::Subdivider::Uniform::LoopT<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps);
    subdivider.detach();
  }
  else if ( _algorithm.contains("sqrt3",Qt::CaseInsensitive) )
  {
    OpenMesh::Subdivider::Uniform::Sqrt3T<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps);
    subdivider.detach();
  }
  else if ( _algorithm.contains("interpolating_sqrt(3)",Qt::CaseInsensitive)  )
  {
    OpenMesh::Subdivider::Uniform::InterpolatingSqrt3LGT<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps);
    subdivider.detach();
  }
  else if ( _algorithm.contains("modifiedButterfly",Qt::CaseInsensitive)  )
  {
    OpenMesh::Subdivider::Uniform::ModifiedButterflyT<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps);
    subdivider.detach();
  }


  mesh->update_face_normals();
  mesh->update_vertex_normals();
  TriMeshObject* tmo = PluginFunctions::triMeshObject(object);
  tmo->update();

  // Create backup
  emit createBackup(object->id(), "Subdivider");
  
  // Geometry and topology changed!
  emit updatedObject(object->id(), UPDATE_ALL);

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

Q_EXPORT_PLUGIN2( subdividerplugin , SubdividerPlugin );

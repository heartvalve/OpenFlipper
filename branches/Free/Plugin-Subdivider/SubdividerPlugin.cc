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


SubdividerPlugin::SubdividerPlugin() :
        tool_(0),
        toolIcon_(0)
{

}


void SubdividerPlugin::initializePlugin() 
{
  if ( OpenFlipper::Options::gui() ) {
    tool_ = new subdividerToolbarWidget();
    QSize size(300, 300);
    tool_->resize(size);
    
    connect(tool_->subdivide_uniform_toolButton,    SIGNAL( clicked() ),  this, SLOT( slotSubdivideUniform() ) );
    
    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"subdivider.png");
    emit addToolbox( tr("Subdivider") , tool_, toolIcon_ );
  }
}

void SubdividerPlugin::pluginsInitialized() 
{
  emit setSlotDescription("subdivide(int,QString,int)", "Smooth a triangular mesh",
                          QString("object_id,algorithm,iterations").split(","),
                          QString("id of an object, algorithm to use (linear | loop | sqrt3 | interpolating_sqrt3 | modifiedButterfly ), number of iterations").split(","));
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
    subdivider(*mesh,_steps,tool_->updatePoints->isChecked());
    subdivider.detach();
  }
  else if ( _algorithm.contains("sqrt3",Qt::CaseInsensitive) )
  {
    OpenMesh::Subdivider::Uniform::Sqrt3T<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps,tool_->updatePoints->isChecked());
    subdivider.detach();
  }
  else if ( _algorithm.contains("interpolating_sqrt(3)",Qt::CaseInsensitive)  )
  {
    OpenMesh::Subdivider::Uniform::InterpolatingSqrt3LGT<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps,tool_->updatePoints->isChecked());
    subdivider.detach();
  }
  else if ( _algorithm.contains("modifiedButterfly",Qt::CaseInsensitive)  )
  {
    OpenMesh::Subdivider::Uniform::ModifiedButterflyT<TriMesh,double> subdivider;
    
    subdivider.attach(*mesh);
    subdivider(_steps,tool_->updatePoints->isChecked());
    subdivider.detach();
  }


  mesh->update_face_normals();
  mesh->update_vertex_normals();
  TriMeshObject* tmo = PluginFunctions::triMeshObject(object);
  tmo->update();

  // Create backup
  emit createBackup(object->id(), "Subdivider", UPDATE_TOPOLOGY);
  
  // Geometry and topology changed!
  emit updatedObject(object->id(), UPDATE_TOPOLOGY);

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

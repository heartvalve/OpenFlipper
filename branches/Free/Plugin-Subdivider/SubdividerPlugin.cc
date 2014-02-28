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
#include <OpenMesh/Tools/Subdivider/Uniform/LongestEdgeT.hh>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>


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
    
    connect(tool_->subdivide_uniform_toolButton,    SIGNAL( clicked() ),  this, SLOT( slotSubdivideUniformButton() ) );
    connect(tool_->simpleButton,                    SIGNAL( clicked() ),  this, SLOT( slotSimpleSubdivideButton() ) );


    
    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"subdivider.png");
    emit addToolbox( tr("Subdivider") , tool_, toolIcon_ );
  }
}

void SubdividerPlugin::pluginsInitialized() 
{
  emit setSlotDescription("subdivide(int,QString,int,bool)", "Subdivide a triangular or polygonal mesh. For polygonal meshes use catmullClark, the other algorithms are for triangular meshes.",
                          QString("object_id,algorithm,iterations,update_points").split(","),
                          QString(" id of an object, algorithm to use ( loop | sqrt3 | interpolating_sqrt3 | modifiedButterfly | catmullClark ), number of iterations, update original points").split(","));

  emit setSlotDescription("subdivide(int,QString,int)", "Subdivide a triangular or polygonal mesh. For polygonal meshes use catmullClark, the other algorithms are for triangular meshes. This function will modify the original point locations.",
                            QString("object_id,algorithm,iterations").split(","),
                            QString(" id of an object, algorithm to use ( loop | sqrt3 | interpolating_sqrt3 | modifiedButterfly | catmullClark ), number of iterations").split(","));

  emit setSlotDescription("simpleSubdivide(int,QString,int,double,bool)", "Subdivide a triangular mesh.",
                          QString("object_id,algorithm,iterations,parameter,update_points").split(","),
                          QString(" id of an object, algorithm to use ( longest ), number of iterations, additional parameter(e.g. maximal edge length for longest), update original points").split(","));

  emit setSlotDescription("simpleSubdivide(int,QString,int,double)", "Subdivide a triangular mesh. This function will modify the original point locations.",
                          QString("object_id,algorithm,iterations,parameter").split(","),
                          QString(" id of an object, algorithm to use ( longest ), number of iterations, additional parameter(e.g. maximal edge length for longest)").split(","));
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::slotSubdivideUniformButton()
{
  std::vector< int > ids;
  if ( PluginFunctions::getTargetIdentifiers( ids ) )
  {

    for (unsigned int i = 0; i < ids.size(); ++i)
    {
      if(tool_->loop_radioButton->isChecked())
      {
        subdivide(ids[i],"loop",tool_->subdivision_steps_spinBox->value(), tool_->updatePoints->isChecked());
      }
      else if ( tool_->sqrt3_radioButton->isChecked() )
      {
        subdivide(ids[i],"sqrt3",tool_->subdivision_steps_spinBox->value(), tool_->updatePoints->isChecked());
      }
      else if ( tool_->LabsikGreiner_radioButton->isChecked()  )
      {
        subdivide(ids[i],"interpolating_sqrt3",tool_->subdivision_steps_spinBox->value(), tool_->updatePoints->isChecked());
      }
      else if ( tool_->modifiedButterfly_radioButton->isChecked()  )
      {
        subdivide(ids[i],"modifiedButterfly",tool_->subdivision_steps_spinBox->value(), tool_->updatePoints->isChecked());
      }
      else if ( tool_->catmullClark_radioButton->isChecked()  )
      {
        subdivide(ids[i],"catmullClark",tool_->subdivision_steps_spinBox->value(), tool_->updatePoints->isChecked());
      }

      // Create backup
      emit createBackup(ids[i], "Subdivider", UPDATE_TOPOLOGY);
    }

  }
  emit updateView();
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::slotSimpleSubdivideButton()
{
  std::vector< int > ids;
  if ( PluginFunctions::getTargetIdentifiers( ids ) )
  {
    for (unsigned int i = 0; i < ids.size(); ++i)
    {
      if(tool_->longestEdgeSplit->isChecked())
      {
        simpleSubdivide(ids[i],"longest",tool_->subdivision_steps_spinBox->value(),
                tool_->maximalEdgeLength->value(), tool_->updatePoints->isChecked());

        // Create backup
        emit createBackup(ids[i], "Subdivider", UPDATE_TOPOLOGY);
      }
    }
  }
  emit updateView();
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::simpleSubdivide(int _objectId, QString _algorithm , int _steps, double _parameter, bool _update_points) {

  BaseObjectData* object;
  if (!test_trimesh_object(_objectId, object)) {
    emit log(LOGERR,"The simple subdivision algorithms only work on triangular meshes.");
    return;
  }

  TriMesh* mesh = PluginFunctions::triMesh(object);

  if (_algorithm.contains("longest", Qt::CaseInsensitive)) {
    OpenMesh::Subdivider::Uniform::LongestEdgeT<TriMesh, double> subdivider;

    subdivider.attach(*mesh);
    subdivider.set_max_edge_length(_parameter);
    subdivider(*mesh, _steps, _update_points);
    subdivider.detach();
  } else {
    emit log(LOGERR,"Unsupported algorithm in simpleSubdivide: " + _algorithm);
    return;
  }

  mesh->garbage_collection();

  mesh->update_face_normals();
  mesh->update_vertex_normals();

  // Geometry and topology changed!
  emit updatedObject(object->id(), UPDATE_TOPOLOGY);
}

//-----------------------------------------------------------------------------

void SubdividerPlugin::subdivide(int _objectId, QString _algorithm , int _steps, bool _update_points) {

  BaseObjectData* object;
  PluginFunctions::getObject(_objectId,object);

  if(!object) {
    emit log(LOGERR,"Unable to get Object in SubdividerPlugin::subdivide");
    return;
  }


  // Catmull clark:
  if ( _algorithm.contains("catmullClark",Qt::CaseInsensitive)  ) {

    PolyMesh* polyMesh = PluginFunctions::polyMesh(object);
    if ( ! polyMesh ) {
      emit log(LOGERR,"Error: Catmull Clark only works on Poly Meshes!");
      return;
    }

    OpenMesh::Subdivider::Uniform::CatmullClarkT<PolyMesh> subdivider;

    subdivider.attach(*polyMesh);
    subdivider(_steps,_update_points);
    subdivider.detach();

    polyMesh->garbage_collection();

    polyMesh->update_face_normals();
    polyMesh->update_vertex_normals();

    // Geometry and topology changed!
    emit updatedObject(object->id(), UPDATE_TOPOLOGY);

  } else {

    //=======================================================
    // All other algorithms work on triangular meshes here
    //=======================================================

    TriMesh*  mesh = PluginFunctions::triMesh(object);

    if ( ! mesh ) {
      emit log(LOGERR,"Error: Unable to get trisngular mesh in subdivider!");
      return;
    }

    if(_algorithm.contains("loop",Qt::CaseInsensitive))
    {
      OpenMesh::Subdivider::Uniform::LoopT<TriMesh,double> subdivider;

      subdivider.attach(*mesh);
      subdivider(*mesh,_steps,_update_points);
      subdivider.detach();
    }
    else if ( _algorithm.contains("sqrt3",Qt::CaseInsensitive) )
    {
      OpenMesh::Subdivider::Uniform::Sqrt3T<TriMesh,double> subdivider;

      subdivider.attach(*mesh);
      subdivider(_steps,_update_points);
      subdivider.detach();
    }
    else if ( _algorithm.contains("interpolating_sqrt(3)",Qt::CaseInsensitive)  )
    {
      OpenMesh::Subdivider::Uniform::InterpolatingSqrt3LGT<TriMesh,double> subdivider;

      subdivider.attach(*mesh);
      subdivider(_steps,_update_points);
      subdivider.detach();
    }
    else if ( _algorithm.contains("modifiedButterfly",Qt::CaseInsensitive)  )
    {
      OpenMesh::Subdivider::Uniform::ModifiedButterflyT<TriMesh,double> subdivider;

      subdivider.attach(*mesh);
      subdivider(_steps,_update_points);
      subdivider.detach();
    } else {
      emit log(LOGERR,"Unsupported or unknown subdivider for triangular meshes: " + _algorithm);
    }

    mesh->garbage_collection();

    mesh->update_face_normals();
    mesh->update_vertex_normals();

    // Geometry and topology changed!
     emit updatedObject(object->id(), UPDATE_TOPOLOGY);

  }
  

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

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( subdividerplugin , SubdividerPlugin );
#endif



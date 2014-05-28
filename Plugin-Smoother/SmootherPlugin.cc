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

//=============================================================================
//
//  CLASS SmootherPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


#include "SmootherPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <ACG/Scenegraph/ManipulatorNode.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "SmootherObject.hh"

#define SMOOTHER "SmootherData"

//== IMPLEMENTATION ==========================================================

SmootherPlugin::SmootherPlugin() :
        tool_(0),
        toolIcon_(0)
{
}

//-----------------------------------------------------------------------------

void
SmootherPlugin::
initializePlugin()
{
  if ( OpenFlipper::Options::gui() ) {
    tool_ = new SmootherToolbarWidget();
    QSize size(100, 100);
    tool_->resize(size);

    // connect signals->slots
    connect(tool_->pB_smooth,SIGNAL(clicked() ),this,SLOT(slot_smooth()));

    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"smoother2.png");
    emit addToolbox( tr("Smoother") , tool_, toolIcon_ );
  }
}

//-----------------------------------------------------------------------------

/** \brief Set the scripting slot descriptions
 *
 */
void
SmootherPlugin::pluginsInitialized(){

  emit setSlotDescription("smooth(int,int,QString,QString,double)", "Smooth an object",
                          QString("object_id,iterations,direction,continuity,maxDistance").split(","),
                          QString("id of an object, number of smoothing iterations, Smoothing direction. (tangential;normal;tangential+normal), Continuity. (C1 or C2), max distance the smoothed mesh is allowed to differ from the original").split(","));

  emit setSlotDescription("smooth(int,int,QString,QString)", "Smooth an object",
                          QString("object_id,iterations,direction,continuity").split(","),
                          QString("id of an object, number of smoothing iterations, Smoothing direction. (tangential;normal;tangential+normal), Continuity. (C1 or C2)").split(","));

}


//-----------------------------------------------------------------------------

/** \brief Smooth all target objects
 *
 * Parameters for the smoothing are retrieved from the toolbox
 *
 */
void
SmootherPlugin::
slot_smooth()
{
  bool found = false;
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH) ;
        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    QString jobDescription = "Smoothed (";
  
    TriMeshObject* object = PluginFunctions::triMeshObject(*o_it);

    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      continue;
    }

    found = true;

    SmootherObject* data = dynamic_cast< SmootherObject* > ( o_it->objectData(SMOOTHER) );

    // Get triangle mesh
    TriMesh* mesh = PluginFunctions::triMesh(*o_it);
    
    if ( mesh == NULL ) {
      emit log(LOGERR, "Unable to get mesh from object( Only Triangle Meshes supported)");
      return;
    }

    if (data == 0){
      data = new SmootherObject();
      o_it->setObjectData(SMOOTHER, data);
    }
    
    // Create smoother
    SmootherType smoother(*mesh);

    OpenMesh::Smoother::SmootherT< TriMesh >::Component component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    if( tool_->rbTangential_and_Normal->isChecked() ) {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
      jobDescription += "tangential and normal,";
    } else if( tool_->rbNormal->isChecked() ) {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Normal;
      jobDescription += "normal,";
    } else if( tool_->rbTangential->isChecked() ) {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential;
      jobDescription += "tangential,";
    }

    // Set perObjectData
    data->component(component);

    OpenMesh::Smoother::SmootherT< TriMesh >::Continuity continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
    if( tool_->rB_c0->isChecked() ) {
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
      jobDescription += "C0";
    } else if( tool_->rB_c1->isChecked() ) {
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C1;
      jobDescription += "C1";
    }
    
    // Set perObjectData
    data->continuity(continuity);

    // Read maximum distance Error from lineEdit
    if ( tool_->cbDistance->isChecked() ) {
      QString value;
      value = tool_->distance->text();
      bool ok = false;

      double absoluteError = value.toDouble(&ok);

      if ( ok ) {
        data->distance(absoluteError);
        smoother.set_absolute_local_error( absoluteError );
      } else {
        emit log(LOGWARN , "Unable to read distance error from LineEdit");
      }
      
      jobDescription += ",max_error: " + QString::number(absoluteError);
    }
    
    // Set perObjectData
    data->features(tool_->respectFeatures->isChecked());
    data->iterations(tool_->sB_iter->value());
    
    // Initialize smoother

    if(tool_->cbReinitialize->isChecked() || !data->initialized())
    {
         smoother.initialize(component,continuity );
         data->initialized(true);
    }

    smoother.skip_features(data->features());

    smoother.smooth( data->iterations() );
    
    jobDescription +=  ") " + QString::number(tool_->sB_iter->value()) + " iterations";

    mesh->update_normals();

    emit updatedObject( o_it->id(), UPDATE_GEOMETRY );
    emit createBackup(o_it->id(), "Smoothing", UPDATE_GEOMETRY );
  }

  if ( !found )
    emit log(LOGERR , tr("Unable to smooth. No triangle mesh selected as target!") );

}


//-----------------------------------------------------------------------------

/** \brief  Smooth object
*
* @param _objectId Object to smooth
* @param _iterations Number of smoothing iterations
* @param _direction tangential/normal/tangential+normal
* @param _continuity C0/C1
* @param _maxDistance the maximum distance that the smoothed mesh is allowed to differ from the original mesh
*/
void SmootherPlugin::smooth(int _objectId , int _iterations , QString _direction , QString _continuity, double _maxDistance) {

  BaseObjectData* baseObjectData;
  if ( ! PluginFunctions::getObject(_objectId,baseObjectData) ) {
    emit log(LOGERR,"Unable to get Object");
    return;
  }

  if ( baseObjectData->dataType() == DATA_TRIANGLE_MESH ) {
    TriMeshObject* object = PluginFunctions::triMeshObject(baseObjectData);

    QString jobDescription = "Smoothed (";
    
    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      return;
    }

    SmootherObject* data = dynamic_cast< SmootherObject* > ( object->objectData(SMOOTHER) );

    // Get triangle mesh
    TriMesh* mesh = PluginFunctions::triMesh(object);
    
    if ( mesh == NULL ) {
      emit log(LOGERR, "Unable to get mesh from object( Only Triangle Meshes supported)");
      return;
    }

    if (data == 0){
      data = new SmootherObject();
      object->setObjectData(SMOOTHER, data);
    }
    
    SmootherType smoother(*mesh);

    OpenMesh::Smoother::SmootherT< TriMesh >::Component component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    bool tangential = _direction.contains("tangential");
    bool normal     = _direction.contains("normal");

    if ( tangential && normal ) {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
      jobDescription += "tangential and normal,";
    } else if ( tangential ) {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential;
      jobDescription += "normal,";
    } else {
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Normal;
      jobDescription += "tangential,";
    }
    
    // Set perObjectData
    data->component(component);

    OpenMesh::Smoother::SmootherT< TriMesh >::Continuity continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
    bool c0 = _continuity.contains("C0");
    bool c1 = _continuity.contains("C1");

    if ( c0 && c1 )
      std::cerr << "Continuity C0 + C1 ? Using C1" << std::endl;
    if( c1 ) {
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C1;
      jobDescription += "C1";
    } else if( c0 ) {
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
      jobDescription += "C0";
    }
    
    // Set perObjectData
    data->continuity(continuity);

    if ( _maxDistance > 0.0) {
      // Set perObjectData
      data->distance(_maxDistance);
      smoother.set_absolute_local_error( _maxDistance );
      jobDescription += ",max_error: " + QString::number(_maxDistance);
    } else {
      // Set perObjectData
      data->distance( FLT_MAX );
      smoother.set_absolute_local_error( FLT_MAX );
    }

    smoother.initialize(component,continuity);

    smoother.smooth( _iterations );

    jobDescription +=  ") " + QString::number(_iterations) + " iterations";
    
    mesh->update_normals();

    emit updatedObject( object->id(), UPDATE_GEOMETRY );

    // Create backup
    emit createBackup(object->id(),"Smoothing", UPDATE_GEOMETRY );
    
    emit scriptInfo(tr("smooth(%1, %2, %3, %4, %5)").arg(QString::number(_objectId), QString::number(_iterations),
                                                    _direction, _continuity, QString::number(_maxDistance)));

  } else {
    emit log(LOGERR,"Unsupported object type for smoother");
    return;
  }

}

//-----------------------------------------------------------------------------

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( SmootherPlugin , SmootherPlugin );
#endif


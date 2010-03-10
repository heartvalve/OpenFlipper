/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

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


void
SmootherPlugin::
initializePlugin()
{
  tool_ = new SmootherToolbarWidget();
  QSize size(100, 100);
  tool_->resize(size);

  // connect signals->slots
  connect(tool_->pB_smooth,SIGNAL(clicked() ),this,SLOT(slot_smooth()));

  emit addToolbox( tr("Smoother") , tool_ );
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

    TriMeshObject* object = PluginFunctions::triMeshObject(*o_it);

    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      continue;
    }

    found = true;

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

    data->smoother->initialize(component,continuity );
    data->smoother->skip_features(tool_->respectFeatures->isChecked());

    data->smoother->smooth( tool_->sB_iter->value() );

    TriMesh* mesh = PluginFunctions::triMesh(*o_it);

    if (mesh != 0)
      mesh->garbage_collection();

    emit updatedObject( o_it->id(), UPDATE_GEOMETRY );
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

    if ( object == 0 ) {
      emit log(LOGWARN , "Unable to get object ( Only Triangle Meshes supported)");
      return;
    }

    SmootherObject* data = dynamic_cast< SmootherObject* > ( object->objectData(SMOOTHER) );

    if (data == 0){
      TriMesh* mesh = PluginFunctions::triMesh(object);
      data = new SmootherObject(mesh);
      object->setObjectData(SMOOTHER, data);
    }

    OpenMesh::Smoother::SmootherT< TriMesh >::Component component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    bool tangential = _direction.contains("tangential");
    bool normal     = _direction.contains("normal");

    if ( tangential && normal )
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential_and_Normal;
    else if ( tangential )
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Tangential;
    else
      component = OpenMesh::Smoother::SmootherT< TriMesh >::Normal;

    OpenMesh::Smoother::SmootherT< TriMesh >::Continuity continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;
    bool c0 = _continuity.contains("C0");
    bool c1 = _continuity.contains("C1");

    if ( c0 && c1 )
      std::cerr << "Continuity C0 + C1 ? Using C1" << std::endl;
    if( c1 )
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C1;
    else if( c0 )
      continuity = OpenMesh::Smoother::SmootherT< TriMesh >::C0;

    if ( _maxDistance > 0.0)
      data->smoother->set_absolute_local_error( _maxDistance );
    else
      data->smoother->set_absolute_local_error( FLT_MAX );

    data->smoother->initialize(component,continuity);

    data->smoother->smooth( _iterations );
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if (mesh != 0) {
      mesh->garbage_collection();
      mesh->update_normals();
    }

    emit updatedObject( object->id(), UPDATE_GEOMETRY );


  } else {
    emit log(LOGERR,"Unsupported object type for smoother");
    return;
  }

}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( SmootherPlugin , SmootherPlugin );


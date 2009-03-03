//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================


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

/** \brief Initialize the Toolbox
 *
 * @param _widget reference to the generated toolbox
 * @return return if the toolbox was generated successfully
 */
bool DecimaterPlugin::initializeToolbox(QWidget*& _widget)
{
  tool_ = new DecimaterToolbarWidget();
  _widget = tool_;
  QSize size(100, 100);
  tool_->resize(size);

  // connect signals->slots
	connect(tool_->pbDecimate,SIGNAL(clicked() ),this,SLOT(slot_decimate()));

  connect(tool_->roundness,SIGNAL(valueChanged(double) ),this,SLOT(updateRoundness(double)) );
  connect(tool_->roundnessSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateRoundness(int)) );

  return true;
}


//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::updateRoundness(int _value)
{
  tool_->roundness->setValue( (double) _value / 100.0 );
}


//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::updateRoundness(double _value)
{
  tool_->roundnessSlider->setValue( (int) (_value * 100) );
}

//-----------------------------------------------------------------------------

/** \brief Decimation called by toolbox
 *
 */
void DecimaterPlugin::slot_decimate()
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
      decimater = new DecimaterInfo( mesh );
      o_it->setObjectData(DECIMATER, decimater);
    }

    //remove old constraints
    decimater->removeConstraints();

    //and set new constraints
    if ( tool_->cbDistance->isChecked() )
      decimater->setDistanceConstraint( tool_->distance->value() );

    if ( tool_->cbNormalDev->isChecked() )
      decimater->setNormalDeviationConstraint( tool_->normalDeviation->value() );

    if ( tool_->cbRoundness->isChecked() )
      decimater->setRoundnessConstraint( tool_->roundness->value() );

    //init the decimater
    if( ! decimater->decimater()->initialize() ){
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

/** \brief Decimation called by Scripting
 *
 * @param _objID id of an object
 * @param _constraints A string containing a comma separated list of constraints (distance,normal_deviation,roundness,triangles)
 * @param _values a string containing a comma separated list of constraint values suited to the _constraints parameter
 */
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
      decimater = new DecimaterInfo( mesh );
      object->setObjectData(DECIMATER, decimater);
    }

    //remove old constraints
    decimater->removeConstraints();

    //and set new constraints
    QStringList constraints = _constraints.toLower().split(",");
    QStringList values      = _values.split(",");

    //distance constraint
    if ( constraints.contains("distance") ){

      int i = constraints.indexOf("distance");

      if ( i >= 0 && i < values.count() ){
        bool ok;

        double value = values[i].toDouble(&ok);

        if (ok)
          decimater->setDistanceConstraint( value );
      }
    }

    //normal deviation constraint
    if ( constraints.contains("normal_deviation") ){

      int i = constraints.indexOf("normal_deviation");

      if ( i >= 0 && i < values.count() ){
        bool ok;

        int value = values[i].toInt(&ok);

        if (ok)
          decimater->setNormalDeviationConstraint( value );
      }
    }

    //roundness constraint
    if ( constraints.contains("roundness") ){

      int i = constraints.indexOf("roundness");

      if ( i >= 0 && i < values.count() ){
        bool ok;

        double value = values[i].toDouble(&ok);

        if (ok)
          decimater->setRoundnessConstraint( value );
      }
    }

    //triangleCount constraint
    bool triangleCount = false;
    int triangles = 0;

    if ( constraints.contains("triangles") ){

      int i = constraints.indexOf("triangles");

      if ( i >= 0 && i < values.count() ){
        bool ok;

        int value = values[i].toInt(&ok);

        if (ok){
          triangleCount = true;
          triangles = value;
        }
      }
    }

    //init the decimater
    if( ! decimater->decimater()->initialize() ){
      emit log(LOGWARN, "Decimater could not be initialized");
      return;
    }

    //decimate
    if ( triangleCount )
      decimater->decimater()->decimate_to( triangles ); // do decimation
    else
      decimater->decimater()->decimate(); // do decimation

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


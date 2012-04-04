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

#define DECIMATER "DecimaterData"

//== IMPLEMENTATION ==========================================================

DecimaterPlugin::DecimaterPlugin() :
        tool_(0),
        toolIcon_(0)
{

}

void DecimaterPlugin::initializePlugin()
{
  tool_ = new DecimaterToolbarWidget();
  QSize size(100, 100);
  tool_->resize(size);

  // connect signals->slots
  connect(tool_->pbDecimate,SIGNAL(clicked() ),this,SLOT(slot_decimate()));

  connect(tool_->roundness,SIGNAL(valueChanged(double) ),this,SLOT(updateRoundness(double)) );
  connect(tool_->roundnessSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateRoundness(int)) );
  connect(tool_->aspectRatio,SIGNAL(valueChanged(double) ),this,SLOT(updateAspectRatio(double)) );
  connect(tool_->aspectRatioSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateAspectRatio(int)) );
  connect(tool_->distance,SIGNAL(valueChanged(double) ),this,SLOT(updateDistance()) );
  connect(tool_->edgeLength,SIGNAL(valueChanged(double) ),this,SLOT(updateEdgeLength()) );
  connect(tool_->normalDeviation,SIGNAL(valueChanged(int) ),this,SLOT(updateNormalDev()) );
  connect(tool_->normalDeviationSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateNormalDev()) );
  connect(tool_->verticesCount,SIGNAL(valueChanged(int) ),this,SLOT(updateVertices()) );
  connect(tool_->verticesCountSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateVertices()) );
  connect(tool_->trianglesCount,SIGNAL(valueChanged(int) ),this,SLOT(updateTriangles()) );
  connect(tool_->trianglesCountSlider,SIGNAL(valueChanged(int) ),this,SLOT(updateTriangles()) );

  // Force update if the Toolbox gets visible
  connect(tool_, SIGNAL(showing()), this, SLOT( slotUpdateNumVertices() ) );
  connect(tool_, SIGNAL(showing()), this, SLOT( slotUpdateNumTriangles() ) );

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"decimater.png");
  emit addToolbox( tr("Decimater") , tool_, toolIcon_ );
}

/** \brief Initialization of the plugin when it is loaded by the core
 * 
 */
void DecimaterPlugin::pluginsInitialized() {

  emit setSlotDescription("decimate(int,QVariantMap)",tr("Decimate a given object"),
                          QString(tr("objectId,constraints")).split(","),
                          QString(tr("ID of an object; Object that can has one or more constraint properties (decimation_order,distance,edge_length,normal_deviation,roundness,aspect_ratio,independent_sets,vertices,triangles)")).split(";"));
}


//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::updateRoundness(int _value)
{
  tool_->roundness->setValue( (double) _value / 100.0 );
  tool_->cbRoundness->setChecked (true);
}


//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::updateRoundness(double _value)
{
  tool_->roundnessSlider->setValue( (int) (_value * 100) );
  tool_->cbRoundness->setChecked (true);
}

//-----------------------------------------------------------------------------


/** \brief sync between values of aspect ratio slider and spinbox in the toolbox
 *
 * @param _value new aspect ratio value
 */
void DecimaterPlugin::updateAspectRatio(int _value)
{
  tool_->aspectRatio->setValue( (double) _value / 100.0 );
  tool_->cbAspectRatio->setChecked (true);
}


//-----------------------------------------------------------------------------

/** \brief sync between values of aspect ratio slider and spinbox in the toolbox
 *
 * @param _value new aspect ratio value
 */
void DecimaterPlugin::updateAspectRatio(double _value)
{
  tool_->aspectRatioSlider->setValue( (int) (_value * 100) );
  tool_->cbAspectRatio->setChecked (true);
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
      emit log(LOGWARN , tr("Unable to get object"));

    DecimaterInfo* decimater = dynamic_cast< DecimaterInfo* > ( o_it->objectData(DECIMATER) );

    TriMesh* mesh = PluginFunctions::triMesh(*o_it);
  
    if (decimater == 0){
      decimater = new DecimaterInfo();
      o_it->setObjectData(DECIMATER, decimater);
    }
    
    // constraint handles for decimation
    ModAspectRatioH     hModAspectRatio;
    ModEdgeLengthH      hModEdgeLength;
    ModHausdorffH       hModHausdorff;
    ModIndependentH     hModIndependent;
    ModNormalDeviationH hModNormalDeviation;
    ModNormalFlippingH  hModNormalFlipping;
    ModQuadricH         hModQuadric;
    ModRoundnessH       hModRoundness;
    
    // Create decimater
    DecimaterType decimater_object( *mesh );
    
    // Remove old constraints
    if(decimater->distance()) {
        decimater->removeDistanceConstraint();
        decimater_object.remove(hModHausdorff);
    }
    if(decimater->normalDeviation()) {
        decimater->removeNormalDeviationConstraint();
        decimater_object.remove(hModNormalDeviation);
    }
    if(decimater->normalFlipping()) {
        decimater->removeNormalFlippingConstraint();
        decimater_object.remove(hModNormalFlipping);
    }
    if(decimater->roundness()) {
        decimater->removeRoundnessConstraint();
        decimater_object.remove(hModRoundness);
    }
    if(decimater->aspectRatio()) {
      decimater->removeAspectRatioConstraint();
      decimater_object.remove(hModAspectRatio);
    }
    if(decimater->edgeLength()) {
      decimater->removeEdgeLengthConstraint();
      decimater_object.remove(hModEdgeLength);
    }
    if(decimater->independentSets()) {
      decimater->removeIndependentSetsConstraint();
      decimater_object.remove(hModIndependent);
    }

    // set priority module: quadric, normal deviation or edge length
    if (tool_->rbByDistance->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::DISTANCE);
      decimater_object.add( hModQuadric );
      decimater_object.module( hModQuadric ).unset_max_err();
    } else if (tool_->rbByNormalDeviation->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::NORMALDEV);
      decimater_object.add(hModNormalDeviation);
      decimater_object.module(hModNormalDeviation).set_binary(false);
    } else if (tool_->rbByEdgeLength->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::EDGELENGTH);
      decimater_object.add(hModEdgeLength);
      decimater_object.module(hModEdgeLength).set_binary(false);
    }

    // and set new constraints
    if ( tool_->cbDistance->isChecked() ) {
      if (  decimater_object.add( hModHausdorff ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setDistanceConstraint( tool_->distance->value() );
          decimater_object.module( hModHausdorff ).set_tolerance( decimater->distanceValue() );
      }
    }

    if ( tool_->cbNormalDev->isChecked() ) {
      if (  decimater_object.add( hModNormalDeviation ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setNormalDeviationConstraint( tool_->normalDeviation->value() );
          decimater_object.module( hModNormalDeviation ).set_normal_deviation( decimater->normalDeviationValue() );
      }
    } else {
      if ( decimater_object.add( hModNormalFlipping ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setNormalFlippingConstraint();
          // decimater_object.module( hModNormalFlipping ).set_max_normal_deviation( decimater->normalDeviationValue() ); ?
      }
    }

    if ( tool_->cbRoundness->isChecked() ) {      
      if (  decimater_object.add( hModRoundness ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setRoundnessConstraint( tool_->roundness->value() );
          decimater_object.module( hModRoundness ).set_min_roundness( decimater->roundnessValue(), true );
      }
    }

    if ( tool_->cbAspectRatio->isChecked() ) {
      if ( decimater_object.add( hModAspectRatio ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setAspectRatioConstraint( tool_->aspectRatio->value() );
          decimater_object.module( hModAspectRatio ).set_aspect_ratio( decimater->aspectRatioValue() );
      }
    }

    if ( tool_->cbEdgeLength->isChecked() ) {
      if ( decimater_object.add( hModEdgeLength ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setEdgeLengthConstraint( tool_->edgeLength->value() );
          decimater_object.module( hModEdgeLength ).set_edge_length( decimater->edgeLengthValue() );
      }
    }

    if ( tool_->cbIndependentSets->isChecked() ) {
      if ( decimater_object.add( hModIndependent ) || tool_->rbConstraintsOnly->isChecked() ) {
          decimater->setIndependentSetsConstraint();
      }
    }

    // Initialize the decimater
    if( ! decimater_object.initialize() ){
      emit log(LOGWARN, tr("Decimater could not be initialized"));
      continue;
    }

    //decimate
    if ( tool_->rbVertices->isChecked() )
        decimater_object.decimate_to(tool_->verticesCount->value());
    else if (tool_->rbTriangles->isChecked() )
        decimater_object.decimate_to_faces(0, tool_->trianglesCount->value());
    else // constraints only
        decimater_object.decimate_to_faces(0, 1);

    object->mesh()->garbage_collection();
    object->mesh()->update_normals();
    object->update();
    
    // Create backup
    emit createBackup(o_it->id(), "Decimation");
    emit updatedObject( o_it->id() , UPDATE_TOPOLOGY );
  }

  emit updateView();
}


//-----------------------------------------------------------------------------

/** \brief Decimation called by Scripting
 *
 * @param _objID id of an object
 * @param _constraints A string containing a comma separated list of constraints (distance,normal_deviation,roundness,triangles)
 */
void DecimaterPlugin::decimate(int _objID, QVariantMap _constraints) {


  BaseObjectData* baseObjectData;
  if ( ! PluginFunctions::getObject(_objID,baseObjectData) ) {
    emit log(LOGERR,tr("Unable to get Object"));
    return;
  }

  if ( baseObjectData->dataType() == DATA_TRIANGLE_MESH ) {
    TriMeshObject* object = PluginFunctions::triMeshObject(baseObjectData);

    if ( object == 0 ) {
      emit log(LOGWARN , tr("Unable to get object ( Only Triangle Meshes supported)"));
      return;
    }

    DecimaterInfo* decimater = dynamic_cast< DecimaterInfo* > ( object->objectData(DECIMATER) );

    TriMesh* mesh = PluginFunctions::triMesh(baseObjectData);
    
    if (decimater == 0){
      decimater = new DecimaterInfo();
      object->setObjectData(DECIMATER, decimater);
    }
    
    // constraint handles for decimation
    ModAspectRatioH     hModAspectRatio;
    ModEdgeLengthH      hModEdgeLength;
    ModHausdorffH       hModHausdorff;
    ModIndependentH     hModIndependent;
    ModNormalDeviationH hModNormalDeviation;
    ModNormalFlippingH  hModNormalFlipping;
    ModQuadricH         hModQuadric;
    ModRoundnessH       hModRoundness;
    
    // Create decimater
    DecimaterType decimater_object( *mesh );

    // Remove old constraints
    if(decimater->distance()) {
      decimater->removeDistanceConstraint();
      decimater_object.remove(hModHausdorff);
    }
    if(decimater->normalDeviation()) {
      decimater->removeNormalDeviationConstraint();
      decimater_object.remove(hModNormalDeviation);
    }
    if(decimater->normalFlipping()) {
      decimater->removeNormalFlippingConstraint();
      decimater_object.remove(hModNormalFlipping);
    }
    if(decimater->roundness()) {
      decimater->removeRoundnessConstraint();
      decimater_object.remove(hModRoundness);
    }
    if(decimater->aspectRatio()) {
      decimater->removeAspectRatioConstraint();
      decimater_object.remove(hModAspectRatio);
    }
    if(decimater->edgeLength()) {
      decimater->removeEdgeLengthConstraint();
      decimater_object.remove(hModEdgeLength);
    }
    if(decimater->independentSets()) {
      decimater->removeIndependentSetsConstraint();
      decimater_object.remove(hModIndependent);
    }

    // set priority module: quadric, normal deviation or edge length
    if ( _constraints.contains("decimation_order") ){
      bool ok;

      int value = _constraints["decimation_order"].toInt(&ok);

      if (ok) {
        switch (value) {
        case 0:
          decimater->setDecimationOrder(DecimaterInfo::DISTANCE);
          decimater_object.add( hModQuadric );
          decimater_object.module( hModQuadric ).unset_max_err();
          break;
        case 1:
          decimater->setDecimationOrder(DecimaterInfo::NORMALDEV);
          decimater_object.add(hModNormalDeviation);
          decimater_object.module(hModNormalDeviation).set_binary(false);
          break;
        case 2:
          decimater->setDecimationOrder(DecimaterInfo::EDGELENGTH);
          decimater_object.add(hModEdgeLength);
          decimater_object.module(hModEdgeLength).set_binary(false);
          break;
        default:
          emit log(LOGERR,tr("Invalid Decimation Order"));
          return;
        }
      }
    } else {
      emit log(LOGERR,tr("No Decimation Order set"));
      return;
    }

    // stock options (triangle and vertices count) constraint
    bool verticesCount = false;
    bool trianglesCount = false;
    int vertices = 0;
    int triangles = 0;

    if ( _constraints.contains("vertices") ){

      bool ok;

      int value = _constraints["vertices"].toInt(&ok);

      if (ok){
        verticesCount = true;
        vertices = value;
      }
    } else if ( _constraints.contains("triangles") ){

      bool ok;

      int value = _constraints["triangles"].toInt(&ok);

      if (ok){
        trianglesCount = true;
        triangles = value;
      }
    }

    //distance constraint
    if ( _constraints.contains("distance") ){

      bool ok;

      double value = _constraints["distance"].toDouble(&ok);

      if (ok) {
        if (  decimater_object.add( hModHausdorff ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setDistanceConstraint( value );
            decimater_object.module( hModHausdorff ).set_tolerance( decimater->distanceValue() );
        }
      }
    }

    //normal deviation constraint
    if ( _constraints.contains("normal_deviation") ){

      bool ok;

      int value = _constraints["normal_deviation"].toInt(&ok);

      if (ok) {
        if (  decimater_object.add( hModNormalDeviation ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setNormalDeviationConstraint( value );
            decimater_object.module( hModNormalDeviation ).set_normal_deviation( decimater->normalDeviationValue() );
        }
      }
    } else { // flipping constraint
      if (  decimater_object.add( hModNormalFlipping ) || (!verticesCount && !trianglesCount)  ) {
        decimater->setNormalFlippingConstraint();
        // decimater_object.module( hModNormalFlipping ).set_max_normal_deviation( decimater->normalDeviationValue() ); ?
      }
    }

    //roundness constraint
    if ( _constraints.contains("roundness") ){

      bool ok;

      double value = _constraints["roundness"].toDouble(&ok);

      if (ok) {
        if (  decimater_object.add( hModRoundness ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setRoundnessConstraint( value );
            decimater_object.module( hModRoundness ).set_min_roundness( decimater->roundnessValue(), true );
        }
      }
    }

    //aspect ratio constraint
    if ( _constraints.contains("aspect_ratio") ){

      bool ok;

      double value = _constraints["aspect_ratio"].toDouble(&ok);

      if (ok) {
        if (  decimater_object.add( hModAspectRatio ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setAspectRatioConstraint( value );
            decimater_object.module( hModAspectRatio ).set_aspect_ratio( decimater->aspectRatioValue() );
        }
      }
    }

    //edge length constraint
    if ( _constraints.contains("edge_length") ){

      bool ok;

      double value = _constraints["edge_length"].toDouble(&ok);

      if (ok) {
        if (  decimater_object.add( hModEdgeLength ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setEdgeLengthConstraint( value );
            decimater_object.module( hModEdgeLength ).set_edge_length( decimater->edgeLengthValue() );
        }
      }
    }

    //independent sets constraint
    if ( _constraints.contains("independent_sets") ){

      bool value = _constraints["independent_sets"].toBool();

      if (value) {
        if (  decimater_object.add( hModIndependent ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setIndependentSetsConstraint();
        }
      }
    }

    //init the decimater
    if( ! decimater_object.initialize() ){
      emit log(LOGWARN, tr("Decimater could not be initialized"));
      return;
    }

    //decimate
    if ( verticesCount )
      decimater_object.decimate_to( vertices ); // do decimation (vertices)
    else if (trianglesCount )
      decimater_object.decimate_to_faces(0, triangles); // do decimation (triangles)
    else
      decimater_object.decimate_to_faces(0, 1); // do decimation

    object->mesh()->garbage_collection();
    object->mesh()->update_normals();
    object->update();

    // Create backup
    emit createBackup(_objID, "Decimation");
    
    // Create QVariantMap parameter string
    QString param = "("  + (_constraints.contains("decimation_order") ? tr("decimation_order =  %1").arg(_constraints["decimation_order"].toString()) : "") +
                    ", " + (_constraints.contains("distance") ? tr("distance = %1").arg(_constraints["distance"].toString()) : "") +
                    ", " + (_constraints.contains("normal_deviation") ? tr("normal_deviation = %1").arg(_constraints["normal_deviation"].toString()) : "") +
                    ", " + (_constraints.contains("edge_length") ? tr("edge_length = %1").arg(_constraints["edge_length"].toString()) : "") +
                    ", " + (_constraints.contains("roundness") ? tr("roundness = %1").arg(_constraints["roundness"].toString()) : "") +
                    ", " + (_constraints.contains("aspect_ratio") ? tr("aspect_ratio = %1").arg(_constraints["aspect_ratio"].toString()) : "") +
                    ", " + (_constraints.contains("independent_sets") ? tr("independent_sets = %1").arg(_constraints["independent_sets"].toString()) : "") +
                    ", " + (_constraints.contains("triangles") ? tr("triangles = %1").arg(_constraints["triangles"].toString()) : "") +
                    ", " + (_constraints.contains("vertices") ? tr("vertices = %1").arg(_constraints["vertices"].toString()) : "") + ")";
    
    emit scriptInfo( "decimate(" + QString::number(_objID) + ", " + param + ")" );
    
    emit updatedObject( baseObjectData->id() , UPDATE_TOPOLOGY);

  } else {
    emit log(LOGERR,tr("Unsupported object type for decimater"));
    return;
  }

  emit updateView();
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotUpdateNumVertices()
{
  // Only update if tool is visible
  if ( !tool_->isVisible() ) {
    return;
  }

  int max = 0;
  int div = 0;

  bool ok;
  emit functionExists( "infomeshobject" , "vertexCount(int)", ok  ) ;
  if (!ok)
  {
    tool_->currentNumVertices->setText ("<not available>");
    return;
  }

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {


    max = std::max( RPC::callFunctionValue<int>   ("infomeshobject" , "vertexCount",o_it->id()) , max );
    div++;
  }

  if (div <= 0)
    tool_->currentNumVertices->setText ("<not available>");
  else {
    tool_->verticesCount->blockSignals(true);
    tool_->verticesCountSlider->blockSignals(true);

    tool_->currentNumVertices->setText (QString::number(max));
    tool_->verticesCount->setMaximum(max);
    tool_->verticesCountSlider->setMaximum(max);

    if ( tool_->verticesCount->value() < 2 )
    {
      tool_->verticesCount->setValue( max / 2 );
      tool_->verticesCountSlider->setValue( max / 2);
    }

    tool_->verticesCount->blockSignals(false);
    tool_->verticesCountSlider->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------

/** \brief gets and sets the current maximum number of triangles
 *
 */
void DecimaterPlugin::slotUpdateNumTriangles() {
  // only update if the tool is visible
  if (!tool_->isVisible())
    return;

  uint max = 0;
  int meshN = 0;

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH));
                                       o_it != PluginFunctions::objectsEnd(); ++o_it) {
    TriMesh* mesh = PluginFunctions::triMesh(o_it->id());
    max = std::max(mesh->n_faces(), max);
    meshN++;
  }

  tool_->trianglesCount->blockSignals(true);
  tool_->trianglesCountSlider->blockSignals(true);

  tool_->trianglesCount->setMinimum(1);
  tool_->trianglesCount->setMaximum(max);
  tool_->trianglesCountSlider->setMinimum(1);
  tool_->trianglesCountSlider->setMaximum(max);

  if (tool_->trianglesCount->value() < 2)
  {
    tool_->trianglesCount->setValue (max / 2 );
    tool_->trianglesCountSlider->setValue( max / 2);
  }

  tool_->trianglesCount->blockSignals(false);
  tool_->trianglesCountSlider->blockSignals(false);
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotObjectSelectionChanged(int /*_identifier*/)
{
  slotUpdateNumVertices ();
  slotUpdateNumTriangles ();
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotObjectUpdated(int /*_identifier*/ , const UpdateType& _type )
{
  if ( _type.contains(UPDATE_TOPOLOGY) ) {
    slotUpdateNumVertices ();
    slotUpdateNumTriangles ();
  }
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::updateVertices()
{
  tool_->rbVertices->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::updateTriangles()
{
  tool_->rbTriangles->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::updateNormalDev()
{
  tool_->cbNormalDev->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::updateEdgeLength()
{
  tool_->cbEdgeLength->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::updateDistance()
{
  tool_->cbDistance->setChecked (true);
}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2(DecimaterPlugin , DecimaterPlugin );


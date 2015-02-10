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
//  CLASS DecimaterPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "DecimaterPlugin.hh"
#include <ACG/GL/GLState.hh>
#include <ACG/Scenegraph/ManipulatorNode.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <memory>
#include <iostream>

#include <QStringList>


#define DECIMATER "DecimaterData"

//== IMPLEMENTATION ==========================================================

DecimaterPlugin::DecimaterPlugin() :
        tool_(0),
        decimater_objects_(),
        toolIcon_(0),
        runningJobs_(0)
{

}

void DecimaterPlugin::initializePlugin()
{

  if ( OpenFlipper::Options::gui()) {
    tool_ = new DecimaterToolbarWidget();
    QSize size(100, 100);
    tool_->resize(size);

    // connect signals->slots
    connect(tool_->pbDecimate,SIGNAL(clicked() ),this,SLOT(slot_decimate()));
    connect(tool_->pbInitialize,SIGNAL(clicked() ), this, SLOT(slot_initialize()));

    connect(tool_->roundness,SIGNAL(valueChanged(double) ),this,SLOT(slotUpdateRoundness(double)) );
    connect(tool_->roundnessSlider,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateRoundness(int)) );
    connect(tool_->aspectRatio,SIGNAL(valueChanged(double) ),this,SLOT(slotUpdateAspectRatio(double)) );
    connect(tool_->aspectRatioSlider,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateAspectRatio(int)) );
    connect(tool_->distance,SIGNAL(valueChanged(double) ),this,SLOT(slotUpdateDistance()) );
    connect(tool_->edgeLength,SIGNAL(valueChanged(double) ),this,SLOT(slotUpdateEdgeLength()) );
    connect(tool_->normalDeviation,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateNormalDev()) );
    connect(tool_->normalDeviationSlider,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateNormalDev()) );
    connect(tool_->verticesCount,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateVertices()) );
    connect(tool_->verticesCountSlider,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateVertices()) );
    connect(tool_->trianglesCount,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateTriangles()) );
    connect(tool_->trianglesCountSlider,SIGNAL(valueChanged(int) ),this,SLOT(slotUpdateTriangles()) );

    // Force update if the Toolbox gets visible
    connect(tool_, SIGNAL(showing()), this, SLOT( slotUpdateNumVertices() ) );
    connect(tool_, SIGNAL(showing()), this, SLOT( slotUpdateNumTriangles() ) );
    connect(tool_->mixedFactorCounter, SIGNAL(valueChanged(double)), this, SLOT(slotMixedCounterValueChanged(double)) );
    connect(tool_->mixedFactorSlider, SIGNAL(valueChanged(int)), this, SLOT(slotMixedSliderValueChanged(int)) );
    connect(tool_->cbDistance, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->cbNormalDev, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->cbEdgeLength, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->cbIndependentSets, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->cbRoundness, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->cbAspectRatio, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbByDistance, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbByEdgeLength, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbByNormalDeviation, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbConstraintsOnly, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbTriangles, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbUseDecimater, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbUseMC, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbUseMixed, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));
    connect(tool_->rbVertices, SIGNAL(toggled(bool)), this, SLOT(slotDisableDecimation()));

    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"decimater.png");
    emit addToolbox( tr("Decimater") , tool_, toolIcon_ );

    WhatsThisGenerator generator("Decimater");

    tool_->pbDecimate->setWhatsThis(tool_->pbDecimate->whatsThis()+generator.generateLink("quick_tutorial"));
    tool_->pbInitialize->setWhatsThis(tool_->pbInitialize->whatsThis()+generator.generateLink("quick_tutorial"));
    tool_->rbUseDecimater->setWhatsThis(tool_->rbUseDecimater->whatsThis()+generator.generateLink("incremental"));
    tool_->rbUseMC->setWhatsThis(tool_->rbUseMC->whatsThis()+generator.generateLink("multiple_choice"));
    tool_->rbUseMixed->setWhatsThis(tool_->rbUseMixed->whatsThis()+generator.generateLink("mixed"));
    tool_->cbDistance->setWhatsThis(tool_->cbDistance->whatsThis()+generator.generateLink());
    tool_->cbNormalDev->setWhatsThis(tool_->cbNormalDev->whatsThis()+generator.generateLink());
    tool_->cbEdgeLength->setWhatsThis(tool_->cbEdgeLength->whatsThis()+generator.generateLink());
    tool_->cbIndependentSets->setWhatsThis(tool_->cbIndependentSets->whatsThis()+generator.generateLink());
    tool_->cbRoundness->setWhatsThis(tool_->cbRoundness->whatsThis()+generator.generateLink());
    tool_->cbAspectRatio->setWhatsThis(tool_->cbAspectRatio->whatsThis()+generator.generateLink());
    tool_->rbByDistance->setWhatsThis(tool_->rbByDistance->whatsThis()+generator.generateLink());
    tool_->rbByEdgeLength->setWhatsThis(tool_->rbByEdgeLength->whatsThis()+generator.generateLink());
    tool_->rbByNormalDeviation->setWhatsThis(tool_->rbByNormalDeviation->whatsThis()+generator.generateLink());
    tool_->rbConstraintsOnly->setWhatsThis(tool_->rbConstraintsOnly->whatsThis()+generator.generateLink());
    tool_->rbTriangles->setWhatsThis(tool_->rbTriangles->whatsThis()+generator.generateLink());
    tool_->rbVertices->setWhatsThis(tool_->rbVertices->whatsThis()+generator.generateLink());


    tool_->randomSamplesCounter->setWhatsThis(tool_->randomSamplesCounter->whatsThis()+generator.generateLink("multiple_choice"));
    tool_->mixedFactorCounter->setWhatsThis(tool_->mixedFactorCounter->whatsThis()+generator.generateLink("mixed"));
    tool_->roundness->setWhatsThis(tool_->roundness->whatsThis()+generator.generateLink());
    tool_->aspectRatio->setWhatsThis(tool_->aspectRatio->whatsThis()+generator.generateLink());
    tool_->distance->setWhatsThis(tool_->distance->whatsThis()+generator.generateLink());
    tool_->edgeLength->setWhatsThis(tool_->edgeLength->whatsThis()+generator.generateLink());
    tool_->normalDeviation->setWhatsThis(tool_->normalDeviation->whatsThis()+generator.generateLink());
  }

}

/** \brief Initialization of the plugin when it is loaded by the core
 *
 */
void DecimaterPlugin::pluginsInitialized() {

  emit setSlotDescription("decimate(int,QVariantMap)",tr("Decimate a given object"),
                          QString(tr("objectId,constraints")).split(","),
                          QString(tr("ID of an object; Object that can has one or more constraint properties ("
                              "decimater_type [0 (Incremental), 1 (MC), 2 (Mixed)], "
                              "random_samples [For MC/Mixed], "
                              "incremental_percentage [For Mixed], "
                              "decimation_order [0 (by distance), 1 (by normal deviation), 2 (by edge length)], "
                              "distance, "
                              "edge_length, "
                              "normal_deviation, "
                              "roundness, "
                              "aspect_ratio,independent_sets, "
                              "vertices, "
                              "triangles)")).split(";"));

  if ( OpenFlipper::Options::gui()) {
    tool_->decTypeOps->setVisible(false);
  }
}


//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::slotUpdateRoundness(int _value)
{
  tool_->roundness->setValue( (double) _value / 100.0 );
  tool_->cbRoundness->setChecked (true);
}

void DecimaterPlugin::slotMixedCounterValueChanged(double _value)
{
  tool_->mixedFactorLabel->setText(QString::number(100-_value)+QString("%"));
  tool_->mixedFactorSlider->setValue(100-_value);
}
void DecimaterPlugin::slotMixedSliderValueChanged(int _value)
{
  tool_->mixedFactorLabel->setText(QString::number(_value)+QString("%"));
  tool_->mixedFactorCounter->setValue(100.0-_value);
}

//-----------------------------------------------------------------------------

/** \brief sync between values of roundness slider and spinbox in the toolbox
 *
 * @param _value new roundness value
 */
void DecimaterPlugin::slotUpdateRoundness(double _value)
{
  tool_->roundnessSlider->setValue( (int) (_value * 100) );
  tool_->cbRoundness->setChecked (true);
}

//-----------------------------------------------------------------------------


/** \brief sync between values of aspect ratio slider and spinbox in the toolbox
 *
 * @param _value new aspect ratio value
 */
void DecimaterPlugin::slotUpdateAspectRatio(int _value)
{
  tool_->aspectRatio->setValue( (double) _value / 100.0 );
  tool_->cbAspectRatio->setChecked (true);
}


//-----------------------------------------------------------------------------

/** \brief sync between values of aspect ratio slider and spinbox in the toolbox
 *
 * @param _value new aspect ratio value
 */
void DecimaterPlugin::slotUpdateAspectRatio(double _value)
{
  tool_->aspectRatioSlider->setValue( (int) (_value * 100) );
  tool_->cbAspectRatio->setChecked (true);
}
//-----------------------------------------------------------------------------


/** \brief Init called by toolbox
 *
 */
void DecimaterPlugin::slot_initialize()
{

  if ( ! OpenFlipper::Options::gui())
    return;

  decimater_objects_.clear();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DATA_TRIANGLE_MESH) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)  {

	  initialize_object(*o_it);

  }
  tool_->pbDecimate->setEnabled(true);
}

void DecimaterPlugin::initialize_object(BaseObjectData *obj) {
    //initialize
    TriMeshObject* object = PluginFunctions::triMeshObject(obj);

    if ( object == 0 )
      emit log(LOGWARN , tr("Unable to get object"));

    DecimaterInfo* decimater = dynamic_cast< DecimaterInfo* > ( obj->objectData(DECIMATER) );

    TriMesh* mesh = PluginFunctions::triMesh(obj);

    if (decimater == 0){
      decimater = new DecimaterInfo();
      obj->setObjectData(DECIMATER, decimater);
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
    ptr::shared_ptr<BaseDecimaterType> decimater_object;
    if (tool_->rbUseDecimater->isChecked())
      decimater_object = ptr::shared_ptr<DecimaterType>(new DecimaterType(*mesh));
    else if(tool_->rbUseMC->isChecked())
      decimater_object = ptr::shared_ptr<McDecimaterType>(new McDecimaterType(*mesh));
    else if(tool_->rbUseMixed->isChecked())
      decimater_object = ptr::shared_ptr<MixedDecimaterType>(new MixedDecimaterType(*mesh));


    // Remove old constraints
    if(decimater->distance()) {
      decimater->removeDistanceConstraint();
      decimater_object->remove(hModHausdorff);
    }
    if(decimater->normalDeviation()) {
      decimater->removeNormalDeviationConstraint();
      decimater_object->remove(hModNormalDeviation);
    }
    if(decimater->normalFlipping()) {
      decimater->removeNormalFlippingConstraint();
      decimater_object->remove(hModNormalFlipping);
    }
    if(decimater->roundness()) {
      decimater->removeRoundnessConstraint();
      decimater_object->remove(hModRoundness);
    }
    if(decimater->aspectRatio()) {
      decimater->removeAspectRatioConstraint();
      decimater_object->remove(hModAspectRatio);
    }
    if(decimater->edgeLength()) {
      decimater->removeEdgeLengthConstraint();
      decimater_object->remove(hModEdgeLength);
    }
    if(decimater->independentSets()) {
      decimater->removeIndependentSetsConstraint();
      decimater_object->remove(hModIndependent);
    }

    // set priority module: quadric, normal deviation or edge length
    if (tool_->rbByDistance->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::DISTANCE);
      decimater_object->add( hModQuadric );
      decimater_object->module( hModQuadric ).unset_max_err();
    } else if (tool_->rbByNormalDeviation->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::NORMALDEV);
      decimater_object->add(hModNormalDeviation);
      decimater_object->module(hModNormalDeviation).set_binary(false);
    } else if (tool_->rbByEdgeLength->isChecked()) {
      decimater->setDecimationOrder(DecimaterInfo::EDGELENGTH);
      decimater_object->add(hModEdgeLength);
      decimater_object->module(hModEdgeLength).set_binary(false);
    }

    // and set new constraints
    ptr::shared_ptr<DecimaterInit> decInit (new DecimaterInit);
    if ( tool_->cbDistance->isChecked() ) {
      if (  decimater_object->add( decInit->hModHausdorff ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setDistanceConstraint( tool_->distance->value() );
        decimater_object->module( decInit->hModHausdorff ).set_tolerance( decimater->distanceValue() );
      }
    }

    if ( tool_->cbNormalDev->isChecked() ) {
      if (  decimater_object->add( decInit->hModNormalDeviation ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setNormalDeviationConstraint( tool_->normalDeviation->value() );
        decimater_object->module( decInit->hModNormalDeviation ).set_normal_deviation( decimater->normalDeviationValue() );
      }
    } else {
      if ( decimater_object->add( decInit->hModNormalFlipping ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setNormalFlippingConstraint();
      }
    }

    if ( tool_->cbRoundness->isChecked() ) {
      if (  decimater_object->add( decInit->hModRoundness ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setRoundnessConstraint( tool_->roundness->value() );
        decimater_object->module( decInit->hModRoundness ).set_min_roundness( decimater->roundnessValue(), true );
      }
    }

    if ( tool_->cbAspectRatio->isChecked() ) {
      if ( decimater_object->add( decInit->hModAspectRatio ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setAspectRatioConstraint( tool_->aspectRatio->value() );
        decimater_object->module( decInit->hModAspectRatio ).set_aspect_ratio( decimater->aspectRatioValue() );
      }
    }

    if ( tool_->cbEdgeLength->isChecked() ) {
      if ( decimater_object->add( decInit->hModEdgeLength ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setEdgeLengthConstraint( tool_->edgeLength->value() );
        decimater_object->module( decInit->hModEdgeLength ).set_edge_length( decimater->edgeLengthValue() );
      }
    }

    if ( tool_->cbIndependentSets->isChecked() ) {
      if ( decimater_object->add( decInit->hModIndependent ) || tool_->rbConstraintsOnly->isChecked() ) {
        decimater->setIndependentSetsConstraint();
      }
    }


    // Initialize the decimater
    if( ! decimater_object->initialize() ){
      emit log(LOGWARN, tr("Decimater could not be initialized"));
      return;
    }

    decInit->decimater = decimater_object;
    decInit->objId = obj->id();

    decimater_objects_.push_back(decInit);
}

void DecimaterPlugin::slot_initialize_object(int obj_id, bool clear) {
	if (clear)
		decimater_objects_.clear();

	BaseObjectData *obj = 0;
	PluginFunctions::getObject(obj_id, obj);
	if (!obj) return;

	initialize_object(obj);

	tool_->pbDecimate->setEnabled(true);
}

//-----------------------------------------------------------------------------
/** \brief Decimation called by toolbox
 *
 */
void DecimaterPlugin::slot_decimate()
{

  if ( ! OpenFlipper::Options::gui())
    return;

  //decimate
  runningJobs_ = decimater_objects_.size();
  for (std::vector< ptr::shared_ptr<DecimaterInit> >::iterator decIter = decimater_objects_.begin();
      decIter != decimater_objects_.end(); ++decIter)
  {
    ptr::shared_ptr<DecimaterInit> decInit = *decIter;
    ptr::shared_ptr<BaseDecimaterType> decimater = decInit->decimater;

    // set values for constraints
    if ( tool_->cbDistance->isChecked() ) {
        decimater->module( decInit->hModHausdorff ).set_tolerance( tool_->distance->value() );
    }

    if ( tool_->cbNormalDev->isChecked() ) {
      decimater->module( decInit->hModNormalDeviation ).set_normal_deviation( tool_->normalDeviation->value() );
    }

    if ( tool_->cbRoundness->isChecked() ) {
      decimater->module( decInit->hModRoundness ).set_min_roundness( tool_->roundness->value(), true );
    }

    if ( tool_->cbAspectRatio->isChecked() ) {
      decimater->module( decInit->hModAspectRatio ).set_aspect_ratio( tool_->aspectRatio->value() );
    }

    if ( tool_->cbEdgeLength->isChecked() ) {
      decimater->module( decInit->hModEdgeLength ).set_edge_length( tool_->edgeLength->value() );
    }

    // fill data for the decimate thread
    DecimateThread::Params params;
    params.dec = (tool_->rbUseDecimater->isChecked()) ? dynamic_cast<DecimaterType*>(decimater.get()) : NULL;
    params.mcDec = (tool_->rbUseMC->isChecked())   ? dynamic_cast<McDecimaterType*>(decimater.get()) : NULL;
    params.mixedDec = (tool_->rbUseMixed->isChecked())   ? dynamic_cast<MixedDecimaterType*>(decimater.get()) : NULL;

    params.facesCount = (tool_->rbTriangles->isChecked()) ? tool_->trianglesCount->value() : -1;
    params.verticesCount = (tool_->rbVertices->isChecked() ) ? tool_->verticesCount->value() : -1;
    params.samples = tool_->randomSamplesCounter->value();
    params.mc_factor = 1.0 - (tool_->mixedFactorCounter->value()*0.01);

    // create and start decimate thread
    QString jobId = QString("Decimate_Object_%1").arg(decInit->objId);
    DecimateThread* th = new DecimateThread(params, jobId, decInit->objId);
    connect(th, SIGNAL(finished(QString)), this,SIGNAL(finishJob(QString)));
    connect(th, SIGNAL(finished(QString)), this, SLOT(slot_decimate_finished(QString)));
    connect(th, SIGNAL(state(QString, int)), this, SIGNAL(setJobState(QString, int)));
    connect(this, SIGNAL(jobCanceled(QString)), th, SLOT(slotCancel(QString)));

    tool_->pbDecimate->setEnabled(false);
    tool_->pbInitialize->setEnabled(false);

    emit startJob(jobId , QString("Decimate Object with Id %1").arg(decInit->objId) , 0, 100, false);

    th->start();
    th->startProcessing();

  }


}

void DecimaterPlugin::canceledJob (QString _job )
{
  emit jobCanceled(_job);
}

void DecimaterPlugin::slot_decimate_finished(QString _jobId)
{
  //This function is executed by the main thread! but the sender is the finished thread
  DecimateThread* thread = dynamic_cast<DecimateThread*>(sender());

  if (!thread)
    return;
  if (!thread->baseDecimater())
    return;

  //update mesh
  thread->baseDecimater()->mesh().garbage_collection();
  thread->baseDecimater()->mesh().update_normals();

  emit updatedObject( thread->objectId() , UPDATE_TOPOLOGY );
  emit createBackup( thread->objectId(), "Decimation");

  //cleanup when all threads are done
  --runningJobs_;//running in main thread, so no race condition
  if (runningJobs_ == 0)
  {
    tool_->pbDecimate->setEnabled(true);
    tool_->pbInitialize->setEnabled(true);
    emit updateView();
  }
}


//-----------------------------------------------------------------------------

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
    ptr::shared_ptr<BaseDecimaterType> decimater_object;
    if (_constraints.contains("decimater_type"))
    {
      bool ok;
      int value = _constraints["decimater_type"].toInt(&ok);
      if (ok)
      {
        std::cout << "value: " << value << std::endl;
        if (value == 0)
          decimater_object = ptr::shared_ptr<DecimaterType>(new DecimaterType(*mesh));
        else if (value == 1)
          decimater_object = ptr::shared_ptr<McDecimaterType>(new McDecimaterType(*mesh));
        else if (value == 2)
          decimater_object = ptr::shared_ptr<MixedDecimaterType>(new MixedDecimaterType(*mesh));
      }

    }

    if (!decimater_object)
      decimater_object = ptr::shared_ptr<DecimaterType>(new DecimaterType(*mesh));

    // Remove old constraints
    if(decimater->distance()) {
      decimater->removeDistanceConstraint();
      decimater_object->remove(hModHausdorff);
    }
    if(decimater->normalDeviation()) {
      decimater->removeNormalDeviationConstraint();
      decimater_object->remove(hModNormalDeviation);
    }
    if(decimater->normalFlipping()) {
      decimater->removeNormalFlippingConstraint();
      decimater_object->remove(hModNormalFlipping);
    }
    if(decimater->roundness()) {
      decimater->removeRoundnessConstraint();
      decimater_object->remove(hModRoundness);
    }
    if(decimater->aspectRatio()) {
      decimater->removeAspectRatioConstraint();
      decimater_object->remove(hModAspectRatio);
    }
    if(decimater->edgeLength()) {
      decimater->removeEdgeLengthConstraint();
      decimater_object->remove(hModEdgeLength);
    }
    if(decimater->independentSets()) {
      decimater->removeIndependentSetsConstraint();
      decimater_object->remove(hModIndependent);
    }

    // set priority module: quadric, normal deviation or edge length
    if ( _constraints.contains("decimation_order") ){
      bool ok;

      int value = _constraints["decimation_order"].toInt(&ok);

      if (ok) {
        switch (value) {
        case 0:
          decimater->setDecimationOrder(DecimaterInfo::DISTANCE);
          decimater_object->add( hModQuadric );
          decimater_object->module( hModQuadric ).unset_max_err();
          break;
        case 1:
          decimater->setDecimationOrder(DecimaterInfo::NORMALDEV);
          decimater_object->add(hModNormalDeviation);
          decimater_object->module(hModNormalDeviation).set_binary(false);
          break;
        case 2:
          decimater->setDecimationOrder(DecimaterInfo::EDGELENGTH);
          decimater_object->add(hModEdgeLength);
          decimater_object->module(hModEdgeLength).set_binary(false);
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
        if (  decimater_object->add( hModHausdorff ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setDistanceConstraint( value );
            decimater_object->module( hModHausdorff ).set_tolerance( decimater->distanceValue() );
        }
      }
    }

    //normal deviation constraint
    if ( _constraints.contains("normal_deviation") ){

      bool ok;

      int value = _constraints["normal_deviation"].toInt(&ok);

      if (ok) {
        if (  decimater_object->add( hModNormalDeviation ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setNormalDeviationConstraint( value );
            decimater_object->module( hModNormalDeviation ).set_normal_deviation( decimater->normalDeviationValue() );
        }
      }
    } else { // flipping constraint
      if (  decimater_object->add( hModNormalFlipping ) || (!verticesCount && !trianglesCount)  ) {
        decimater->setNormalFlippingConstraint();
        // decimater_object->module( hModNormalFlipping ).set_max_normal_deviation( decimater->normalDeviationValue() ); ?
      }
    }

    //roundness constraint
    if ( _constraints.contains("roundness") ){

      bool ok;

      double value = _constraints["roundness"].toDouble(&ok);

      if (ok) {
        if (  decimater_object->add( hModRoundness ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setRoundnessConstraint( value );
            decimater_object->module( hModRoundness ).set_min_roundness( decimater->roundnessValue(), true );
        }
      }
    }

    //aspect ratio constraint
    if ( _constraints.contains("aspect_ratio") ){

      bool ok;

      double value = _constraints["aspect_ratio"].toDouble(&ok);

      if (ok) {
        if (  decimater_object->add( hModAspectRatio ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setAspectRatioConstraint( value );
            decimater_object->module( hModAspectRatio ).set_aspect_ratio( decimater->aspectRatioValue() );
        }
      }
    }

    //edge length constraint
    if ( _constraints.contains("edge_length") ){

      bool ok;

      double value = _constraints["edge_length"].toDouble(&ok);

      if (ok) {
        if (  decimater_object->add( hModEdgeLength ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setEdgeLengthConstraint( value );
            decimater_object->module( hModEdgeLength ).set_edge_length( decimater->edgeLengthValue() );
        }
      }
    }

    //independent sets constraint
    if ( _constraints.contains("independent_sets") ){

      bool value = _constraints["independent_sets"].toBool();

      if (value) {
        if (  decimater_object->add( hModIndependent ) || (!verticesCount && !trianglesCount)  ) {
            decimater->setIndependentSetsConstraint();
        }
      }
    }

    //init the decimater
    if( ! decimater_object->initialize() ){
      emit log(LOGWARN, tr("Decimater could not be initialized"));
      return;
    }

    float mc_factor = 0.5;
    size_t randomSamples = 8;

    if (_constraints.contains("random_samples"))
    {
      bool ok;
      unsigned value =_constraints["random_samples"].toUInt(&ok);
      if (ok)
        randomSamples = value;
    }

    if (_constraints.contains("incremental_percentage"))
    {
      bool ok;
      unsigned value =_constraints["incremental_percentage"].toUInt(&ok);
      if (ok)
        mc_factor = 1.f - (value*0.01f);
    }

    //decimate
    DecimaterType* dec = dynamic_cast<DecimaterType*>(decimater_object.get());
    McDecimaterType* mcDec = dynamic_cast<McDecimaterType*>(decimater_object.get());
    MixedDecimaterType* mixedDec = dynamic_cast<MixedDecimaterType*>(decimater_object.get());

    if(dec && !mixedDec)
    {
      if ( verticesCount )
        dec->decimate_to(vertices);
      else if (trianglesCount )
        dec->decimate_to_faces(0, triangles);
      else // constraints only
        dec->decimate_to_faces(0, 1);
    }
    else if (mcDec && !mixedDec)
    {
      mcDec->set_samples(randomSamples);
      if ( verticesCount )
        mcDec->decimate_to(vertices);
      else if (trianglesCount)
        mcDec->decimate_to_faces(0, triangles);
      else // constraints only
        mcDec->decimate_to_faces(0, 1);
    }
    else if (mixedDec)
    {
      mixedDec->set_samples(randomSamples);
      if ( verticesCount )
        mixedDec->decimate_to(vertices,mc_factor);
      else if (trianglesCount)
        mixedDec->decimate_to_faces(0, triangles,mc_factor);
      else // constraints only
        mixedDec->decimate_to_faces(0, 1,mc_factor);
    }else
    {
      emit log(LOGERR,tr("Could not find Decimater Type"));
    }

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
  if ( !OpenFlipper::Options::gui() || !tool_->isVisible() ) {
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
  if (!OpenFlipper::Options::gui() || !tool_->isVisible())
    return;

  size_t max = 0;
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

void DecimaterPlugin::objectDeleted(int _id)
{
  slotDisableDecimation();
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotAboutToRestore(int _id)
{
  slotDisableDecimation();
}

//-----------------------------------------------------------------------------

void DecimaterPlugin::slotDisableDecimation()
{
  if ( ! OpenFlipper::Options::gui())
    return;

  decimater_objects_.clear();
  tool_->pbDecimate->setEnabled(false);
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
void DecimaterPlugin::slotUpdateVertices()
{
  tool_->rbVertices->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::slotUpdateTriangles()
{
  tool_->rbTriangles->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::slotUpdateNormalDev()
{
  tool_->cbNormalDev->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::slotUpdateEdgeLength()
{
  tool_->cbEdgeLength->setChecked (true);
}

//-----------------------------------------------------------------------------

// activate checkbox if value has changed
void DecimaterPlugin::slotUpdateDistance()
{
  tool_->cbDistance->setChecked (true);
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(DecimaterPlugin , DecimaterPlugin );
#endif

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
*   $Date$                    *
*                                                                            *
\*===========================================================================*/


#include "RemesherPlugin.hh"

#include "Algorithms/AdaptiveRemesherT.hh"
#include "Algorithms/UniformRemesherT.hh"

/*
 * STILL \TODO:
 *
 *  - Emit status updates when remeshing
 */

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

// ----------------------------------------------------------------------------------------

RemesherPlugin::RemesherPlugin() :
progress_(0) {

  progress_ = new ProgressEmitter();

  connect(progress_, SIGNAL(signalJobState(QString,int)), this, SIGNAL(setJobState(QString,int)), Qt::QueuedConnection);
  connect(progress_, SIGNAL(changeDescription(QString,QString)), this, SIGNAL(setJobDescription(QString,QString)), Qt::QueuedConnection);
}

// ----------------------------------------------------------------------------------------

RemesherPlugin::~RemesherPlugin() {

  delete progress_;
}

// ----------------------------------------------------------------------------------------

/// Initialize the plugin
void RemesherPlugin::pluginsInitialized(){

  emit setSlotDescription("adaptiveRemeshing(int,double,double,double,int,bool)", "Adaptive Remeshing",
                          QString("object_id,error,min_edge_length,max_edge_length,iterations,use_projection").split(","),
                          QString("id of an object,error,minimal target edge length,maximal target edge length,iterations,use projection method").split(","));
  emit setSlotDescription("uniformRemeshing(int,double,unsigned int,unsigned int,bool)", "Uniform Remeshing",
                          QString("object_id,edge_length,iterations,area_iterations,use_projection").split(","),
                          QString("id of an object,target edge length,iterations,area iterations,use projection method").split(","));
}

// ----------------------------------------------------------------------------------------

/// init the Toolbox
void RemesherPlugin::initializePlugin() {

  if ( OpenFlipper::Options::gui() ) {
    tool_ = new RemesherToolBox();

    // Connect buttons
    connect(tool_->adaptive_button,    SIGNAL(clicked()), this, SLOT(adaptiveRemeshingButtonClicked()) );
    //connect(tool_->anisotropic_button, SIGNAL(clicked()), this, SLOT(anisotropicRemeshingButtonClicked()) );
    connect(tool_->uniform_button,     SIGNAL(clicked()), this, SLOT(uniformRemeshingButtonClicked()) );

    connect(tool_->adaptive_initial_values, SIGNAL(clicked()), this, SLOT(computeInitValues()));
    connect(tool_->uniform_initial_values, SIGNAL(clicked()), this, SLOT(computeInitValues()));

    tool_->tabWidget->setCurrentIndex(0);

    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"remesher.png");
    emit addToolbox( tr("Remesher") , tool_ , toolIcon_);
  }

  connect(this,   SIGNAL( finishJob(QString)),    this, SLOT(threadFinished(QString)), Qt::QueuedConnection);
}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::threadFinished(QString _jobId) {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    if ( operation_ == REMESH_ADAPTIVE ) {
      emit updatedObject(o_it->id(), UPDATE_TOPOLOGY );
      emit createBackup(o_it->id(), "Adaptive remeshing", UPDATE_TOPOLOGY);
    } else if ( operation_ == REMESH_UNIFORM ) {
      emit updatedObject(o_it->id(), UPDATE_TOPOLOGY );
      emit createBackup(o_it->id(), "Uniform remeshing", UPDATE_TOPOLOGY);
    }
  }

  // Detach job from progress emitter
  progress_->detachJob();
}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::computeInitValues() {

  if(OpenFlipper::Options::nogui()) return;

  double mean_edge = 0.0;
  double max_feature_angle = 0.0;

  //read one target objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    if(o_it->dataType() == DATA_TRIANGLE_MESH) {

      TriMesh* mesh = PluginFunctions::triMesh(o_it->id());
      if(!mesh) return;

      mesh->update_face_normals();

      for(TriMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {

        TriMesh::HalfedgeHandle he = mesh->halfedge_handle(e_it, 0);

        ACG::Vec3d vec_e = mesh->point(mesh->to_vertex_handle(he)) - mesh->point(mesh->from_vertex_handle(he));

        mean_edge += vec_e.length();

        // Estimate feature angle
        TriMesh::FaceHandle fh1 = mesh->face_handle(he);
        TriMesh::FaceHandle fh2 = mesh->face_handle(mesh->opposite_halfedge_handle(he));

        // Boundary halfedge?
        if ( !fh2.is_valid() || !fh2.is_valid() )
          continue;

        TriMesh::Normal n1 = mesh->normal(fh1);
        TriMesh::Normal n2 = mesh->normal(fh2);

        double feature_angle = (1.0 - (n1 | n2));

        if(feature_angle > max_feature_angle) max_feature_angle = feature_angle;
      }

      mean_edge /= (double)mesh->n_edges();

    } else {
      // DATA_POLY_MESH

      PolyMesh* mesh = PluginFunctions::polyMesh(o_it->id());
      if(!mesh) return;

      mesh->update_face_normals();

      for(PolyMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {

        PolyMesh::HalfedgeHandle he = mesh->halfedge_handle(e_it, 0);

        ACG::Vec3d vec_e = mesh->point(mesh->to_vertex_handle(he)) - mesh->point(mesh->from_vertex_handle(he));

        mean_edge += vec_e.length();

        // Estimate feature angle
        PolyMesh::FaceHandle fh1 = mesh->face_handle(he);
        PolyMesh::FaceHandle fh2 = mesh->face_handle(mesh->opposite_halfedge_handle(he));

        // Boundary halfedge?
        if ( !fh2.is_valid() || !fh2.is_valid() )
          continue;

        PolyMesh::Normal n1 = mesh->normal(fh1);
        PolyMesh::Normal n2 = mesh->normal(fh2);

        double feature_angle = (1.0 - (n1 | n2));

        if(feature_angle > max_feature_angle) max_feature_angle = feature_angle;
      }

      mean_edge /= (double)mesh->n_edges();
    }

    // Convert feature angle to radian
    max_feature_angle = max_feature_angle * (M_PI / 2.0);

    // Set adaptive values
    tool_->adaptive_error->setValue(mean_edge * 0.1); // 10% of mean value
    tool_->adaptive_min_edge->setValue(mean_edge - (mean_edge * 0.1)); // mean - 10%
    tool_->adaptive_max_edge->setValue(mean_edge + (mean_edge * 0.1)); // mean + 10%

    // Set uniform values
    tool_->uniform_edge_length->setValue(mean_edge);

    return; // Just take first object
  }
}

// ----------------------- Adaptive Remeshing ---------------------------------------------

void RemesherPlugin::adaptiveRemeshingButtonClicked() {
    
  QString jobId = name() + "AdaptiveRemeshing";
  OpenFlipperThread* thread = new OpenFlipperThread(jobId);

  connect(thread, SIGNAL( finished(QString)),     this, SIGNAL(finishJob(QString)));
  connect(thread, SIGNAL( function() ),           this, SLOT(adaptiveRemeshing()),Qt::DirectConnection);


  emit startJob( jobId, "Adaptive remeshing" , 0 , 100 , true);

  // Attach job to progress emitter
  progress_->attachJob(jobId);

  thread->start();
  thread->startProcessing();

}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::adaptiveRemeshing() {

  if(OpenFlipper::Options::nogui()) return;

  //read one target objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH)) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    // Check data type.
    // Note that adaptive remeshing is restricted
    // to triangle meshes only since it relies
    // on edge flips which are only defined
    // for triangle configurations.

    // Get parameters from GUI
    double error = tool_->adaptive_error->value();
    double min_edge = tool_->adaptive_min_edge->value();
    double max_edge = tool_->adaptive_max_edge->value();
    unsigned int iters = tool_->adaptive_iters->text().toInt();
    bool projection = tool_->adaptive_projection->isChecked();

    slotAdaptiveRemeshing(o_it->id(), error, min_edge, max_edge, iters, projection);

  }
}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::slotAdaptiveRemeshing(int           _objectID,
                                           double        _error,
                                           double        _min_edge_length,
                                           double        _max_edge_length,
                                           unsigned int  _iters,
                                           bool          _use_projection) {
    
  operation_ = REMESH_ADAPTIVE;

  BaseObjectData* object = 0;

  if (PluginFunctions::getObject(_objectID, object)) {

    // Check data type
    if (object->dataType(DATA_TRIANGLE_MESH)) {

      TriMesh* mesh = PluginFunctions::triMesh(object);

      Remeshing::AdaptiveRemesherT<TriMesh> remesher(*mesh, progress_);

      remesher.remesh(_error, _min_edge_length, _max_edge_length, _iters, _use_projection);

      mesh->update_normals();


      QString projectionString = "\"FALSE\"";
      if (_use_projection)
        projectionString = "\"TRUE\"";

      emit scriptInfo("adaptiveRemeshing(" + QString::number(_objectID) + ", "
                                           + QString::number(_error) + ", "
                                           + QString::number(_min_edge_length) + ", "
                                           + QString::number(_max_edge_length) + ", "
                                           + QString::number(_iters) + ", "
                                           + projectionString + ")");

      return;
    }
  }
}

// ----------------------- Uniform Remeshing ---------------------------------------------

void RemesherPlugin::uniformRemeshingButtonClicked() {
    
  OpenFlipperThread* thread = new OpenFlipperThread(name() + "UniformRemeshing");

  connect(thread, SIGNAL( state(QString, int)),   this, SIGNAL(setJobState(QString, int)));
  connect(thread, SIGNAL( finished(QString)),     this, SIGNAL(finishJob(QString)));
  connect(thread, SIGNAL( function() ),           this, SLOT(uniformRemeshing()),Qt::DirectConnection);

  emit startJob( name() + "UniformRemeshing", "Uniform remeshing" , 0 , 100 , true);

  thread->start();
  thread->startProcessing();

}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::uniformRemeshing(){

  if(OpenFlipper::Options::nogui()) return;

  // Get parameters from GUI
  double edge_length      = tool_->uniform_edge_length->value();
  unsigned int iters      = tool_->uniform_iters->text().toInt();
  unsigned int area_iters = tool_->uniform_area_iters->text().toInt();
  bool projection         = tool_->uniform_projection->isChecked();

  //read one target objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_TRIANGLE_MESH)) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    // Set incident vertices to feature edges to be feature vertices
    TriMesh* mesh = PluginFunctions::triMesh(o_it->id());
    if(!mesh) continue;
    for(TriMesh::EdgeIter e_it = mesh->edges_begin(); e_it != mesh->edges_end(); ++e_it) {
      if(mesh->status(e_it).feature()) {
        mesh->status(mesh->to_vertex_handle(mesh->halfedge_handle(e_it, 0))).set_feature(true);
        mesh->status(mesh->from_vertex_handle(mesh->halfedge_handle(e_it, 0))).set_feature(true);
      }
    }

    // Check data type.
    // Note that uniform remeshing is restricted
    // to triangle meshes only since it also relies
    // on edge flips which are only defined
    // for triangle configurations.

    slotUniformRemeshing(o_it->id(), edge_length, iters, area_iters, projection);
  }
}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::slotUniformRemeshing(int           _objectID,
                                          double        _edge_length,
                                          unsigned int  _iters,
                                          unsigned int  _area_iters,
                                          bool          _use_projection) {
    
  operation_ = REMESH_UNIFORM;

  BaseObjectData* object = 0;

  if (PluginFunctions::getObject(_objectID, object)) {

    // Check data type
    if (object->dataType(DATA_TRIANGLE_MESH)) {

      TriMesh* mesh = PluginFunctions::triMesh(object);

      Remeshing::UniformRemesherT<TriMesh> remesher(*mesh, progress_);

      remesher.remesh(_edge_length, _iters, _area_iters, _use_projection);

      mesh->update_normals();

      QString projectionString = "\"FALSE\"";
      if (_use_projection)
        projectionString = "\"TRUE\"";

      emit scriptInfo("uniformRemeshing(" + QString::number(_objectID) + ", "
                                          + QString::number(_edge_length) + ", "
                                          + QString::number(_iters) + ", "
                                          + QString::number(_area_iters) + ", "
                                          + QString::number(_iters) + ", "
                                          + projectionString + ")");

      return;
    }
  }

}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::adaptiveRemeshing(int           _objectID,
                                       double        _error,
                                       double        _min_edge_length,
                                       double        _max_edge_length,
                                       unsigned int  _iters,
                                       bool          _use_projection) {

  slotAdaptiveRemeshing(_objectID,_error,_min_edge_length,_max_edge_length,_iters,_use_projection);
  emit updatedObject(_objectID, UPDATE_TOPOLOGY );
  emit createBackup(_objectID, "Adaptive remeshing", UPDATE_TOPOLOGY);

}

// ----------------------------------------------------------------------------------------

void RemesherPlugin::uniformRemeshing(int           _objectID,
                                      double        _edge_length,
                                      unsigned int  _iters,
                                      unsigned int  _area_iters,
                                      bool          _use_projection) {

  slotUniformRemeshing(_objectID,_edge_length,_iters,_area_iters,_use_projection);
  emit updatedObject(_objectID, UPDATE_TOPOLOGY );
  emit createBackup(_objectID, "Uniform remeshing", UPDATE_TOPOLOGY);

}

// ----------------------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( remesherplugin, RemesherPlugin );


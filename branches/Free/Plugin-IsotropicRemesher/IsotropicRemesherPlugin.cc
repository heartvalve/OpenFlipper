
#include <QtGui>

#include "IsotropicRemesherPlugin.hh"
#include "IsotropicRemesherT.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

/// init the Toolbox
void IsotropicRemesherPlugin::initializePlugin() {
  tool_ = new IsotropicRemesherToolBox();

  QSize size(300, 300);
  tool_->resize(size);

  connect(tool_->remeshButton, SIGNAL(clicked()), this, SLOT(slotRemeshButtonClicked()) );

  connect(tool_->minEdgeLength, SIGNAL(clicked()), this, SLOT(slotSetMinLength()) );
  connect(tool_->maxEdgeLength, SIGNAL(clicked()), this, SLOT(slotSetMaxLength()) );
  connect(tool_->meanEdgeLength, SIGNAL(clicked()), this, SLOT(slotSetMeanLength()) );

  emit addToolbox( tr("Isotropic Remesher") , tool_ );
}

void IsotropicRemesherPlugin::slotRemeshButtonClicked() {
  edgeLength_ = tool_->targetEdgeLength->value();
  
  OpenFlipperThread* thread = new OpenFlipperThread(name() + "IsotropicRemesh");                         // Create your thread containing a unique id \n
  
  connect(thread,SIGNAL( state(QString, int)), this,SIGNAL(setJobState(QString, int)));                  // connect your threads state info to the global one \n
  connect(thread,SIGNAL( finished(QString)), this,SIGNAL(finishJob(QString)));                           // connect your threads finish info to the global one ( you can do the same for a local one ) \n
  connect(thread,SIGNAL( function() ), this,SLOT(slotRemesh()),Qt::DirectConnection);           // You can directly give a slot of your app that gets called \n
  connect(this,SIGNAL( finishJob(QString)), this, SLOT(threadFinished(QString)), Qt::QueuedConnection);
  
  emit startJob( name() + "IsotropicRemesh", "Isotropic remeshing" , 0 , 100 , true);  // As long as meshes cannot be locked, this thread has to be blocking. Otherwise, during operation the mesh could be deleted. We don't want that!
  
  thread->start();                                                                                       // start thread
  thread->startProcessing();                                                                             // start processing

}


void IsotropicRemesherPlugin::slotRemesh(){

  //read one target objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    //check dataType
    if ( o_it->dataType(DATA_TRIANGLE_MESH) ) {
      TriMesh* mesh = PluginFunctions::triMesh(o_it);

      ProgressEmitter prgEmt(name() + "IsotropicRemesh");
      connect (&prgEmt, SIGNAL(changeDescription(QString,QString)), this, SIGNAL(setJobDescription(QString,QString)) );
      connect (&prgEmt, SIGNAL(signalJobState(QString,int)), this, SIGNAL(setJobState(QString,int)) );
      IsotropicRemesher< TriMesh > remesher(&prgEmt);

      remesher.remesh(*mesh, edgeLength_);

      mesh->update_normals();

    }else{
      emit log("Remeshing currently only implemented for triangle Meshes");
    }
  }
}

void IsotropicRemesherPlugin::threadFinished(QString _jobId) {
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

      emit updatedObject( o_it->id(), UPDATE_ALL );
  
      emit createBackup(o_it->id(),"Isotropic Remeshing with Target length: " + QString::number(edgeLength_) );
  
      emit updateView();
    
  }
}

void IsotropicRemesherPlugin::slotSetMinLength()
{
  double current = 0.0;
  bool first = true;

  bool ok;
  emit functionExists( "info" , "minEdgeLength(int)", ok  ) ;
  if (!ok)
    return;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    if (first)
    {
      current = RPC::callFunctionValue<double>("info" , "minEdgeLength",o_it->id() );
      first = false;
    }
    else
      current = qMin (current, RPC::callFunctionValue<double>("info" , "minEdgeLength",o_it->id() ));
    
  }

  if (!first)
    tool_->targetEdgeLength->setValue (current);
}

void IsotropicRemesherPlugin::slotSetMaxLength()
{
  double current = 0.0;
  bool first = true;

  bool ok;
  emit functionExists( "info" , "maxEdgeLength(int)", ok  ) ;
  if (!ok)
    return;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {

    if (first)
    {
      current = RPC::callFunctionValue<double>("info" , "maxEdgeLength",o_it->id() );
      first = false;
    }
    else
      current = qMax (current, RPC::callFunctionValue<double>("info" , "maxEdgeLength",o_it->id() ));
    
  }

  if (!first)
    tool_->targetEdgeLength->setValue (current);
}

void IsotropicRemesherPlugin::slotSetMeanLength()
{
  double current = 0.0;
  int  div = 0;

  bool ok;
  emit functionExists( "info" , "edgeCount(int)", ok  ) ;
  if (!ok)
    return;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType(DATA_TRIANGLE_MESH)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {


    current += RPC::callFunctionValue<int>   ("info" , "edgeCount",o_it->id()) *
               RPC::callFunctionValue<double>("info" , "meanEdgeLength",o_it->id() );
    div     += RPC::callFunctionValue<int>   ("info" , "edgeCount",o_it->id()      );
  }

  if (div > 0)
    tool_->targetEdgeLength->setValue (current / div);
}


/// Initialize the plugin
void IsotropicRemesherPlugin::pluginsInitialized(){

  emit setSlotDescription("isotropicRemesh(int,double)", "Isotropic Remeshing",
                          QString("object_id,targetEdgeLength").split(","),
                          QString("id of an object, target edge length").split(","));
}


void IsotropicRemesherPlugin::isotropicRemesh(int _objectID, double _targetEdgeLength ){
  BaseObjectData* object  = 0;


  if ( PluginFunctions::getObject(_objectID, object) ){

    //check dataType
    if ( object->dataType(DATA_TRIANGLE_MESH)) {

      TriMesh* mesh = PluginFunctions::triMesh(object);

      IsotropicRemesher< TriMesh > remesher;

      remesher.remesh(*mesh, _targetEdgeLength);

      mesh->update_normals();

      emit updatedObject( object->id(), UPDATE_ALL );
      
      emit createBackup(object->id(),"Isotropic Remeshing with Target length: " + QString::number(edgeLength_) );

      emit updateView();

      return;

    }else{
      emit log("Remeshing currently only implemented for triangle Meshes");
      return;
    }
  }else{
    emit log("Unable to get object");
  }
}

Q_EXPORT_PLUGIN2( isotropicRemesherPlugin , IsotropicRemesherPlugin );


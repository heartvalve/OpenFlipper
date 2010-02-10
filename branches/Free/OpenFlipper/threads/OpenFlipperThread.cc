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


#include "OpenFlipperThread.hh"
#include <iostream>


OpenFlipperThread::OpenFlipperThread( QString _jobId ) :
  job_(0),
  jobId_(_jobId)
{
  startup_.lock();
  
}

OpenFlipperThread::~OpenFlipperThread() {
  std::cerr << "Destructor Thread" << std::endl;
}

void OpenFlipperThread::run()
{
  if ( job_ == 0 ) {
    // Create a job wrapper to run a slot from within this thread
    job_ = new OpenFlipperJob();

    // Connect the slot which should run in this thread. This has to be a DirectConnection !
    // Otherwisse the slot will run inside its owner context which will be the main loop!!
    connect(job_, SIGNAL(process()),this,SIGNAL(function() ),Qt::DirectConnection);
    
    // Connect the jobs finished function
    connect(job_, SIGNAL(finished()),this,SLOT(slotJobFinished() ) );
    
    // connect the function to start the job
    connect(this,SIGNAL(startProcessingInternal()),job_,SLOT(startJobProcessing()),Qt::QueuedConnection);
  }
  
  std::cerr << "Start Event Loop" << std::endl;

  startup_.unlock();
  
  // Start event queue
  exec();
  
  std::cerr << "End Event Loop " << std::endl;
  
  
// TODO: Self destuction sometimes does not work! 
// Seems to be a race condition!!!

//   std::cerr << "Delete thread Object " << std::endl;
//   
//   deleteLater();
  
//   std::cerr << "Deleted Thread Object" << std::endl;
}

void OpenFlipperThread::cancel() {
  std::cerr << "Cancel not implemented" << std::endl;
}

void OpenFlipperThread::slotCancel( QString _jobId) {
 std::cerr << "Thread : cancel received" << std::endl;
 if ( _jobId == jobId_ )
  cancel(); 
}

void OpenFlipperThread::slotJobFinished( ) {
  emit finished( jobId_ );
  
  quit();
  
}


void OpenFlipperThread::startProcessing() {
  std::cerr << "Thread emit startProcessing" << std::endl;
  
  // Wait for thread to come up with event loop ... otherwise the signals might get lost
  startup_.lock();
  
  // Tell internal wrapper to start with the processing
  emit startProcessingInternal();
  
  startup_.unlock();
  std::cerr << "Thread emit startProcessing done" << std::endl;
}

OpenFlipperJob::~OpenFlipperJob() 
{ 
  std::cerr << "Destructor job called" << std::endl;
}

void OpenFlipperJob::startJobProcessing() {
  std::cerr << "job start process" << std::endl;
  // Actually start the process ( This function will block as it uses a direct connection )
  // But it only blocks the current thread.
  emit process(); 
  
  // Tell thread that the job is done.
  emit finished();
  
  // Cleanup this object
  deleteLater();
  
  std::cerr << "processing done" << std::endl;
}

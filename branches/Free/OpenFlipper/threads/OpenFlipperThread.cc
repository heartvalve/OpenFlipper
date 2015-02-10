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


#include "OpenFlipperThread.hh"
#include <iostream>


OpenFlipperThread::OpenFlipperThread( QString _jobId ) :
  job_(0),
  jobId_(_jobId)
{
}

OpenFlipperThread::~OpenFlipperThread() {
}

QString OpenFlipperThread::jobId() {
  return jobId_;
}

void OpenFlipperThread::run()
{
  if ( job_ == 0 ) {
    // Create a job wrapper to run a slot from within this thread
    job_ = new OpenFlipperJob( jobId_ );

    // Connect the slot which should run in this thread. This has to be a DirectConnection !
    // Otherwise the slot will run inside its owner context which will be the main loop!!
    connect(job_, SIGNAL(process(QString)),this,SIGNAL(function(QString) ),Qt::DirectConnection);
    
    // Connect the jobs finished function
    connect(job_, SIGNAL(finished()),this,SLOT(slotJobFinished() ) );
    
    // connect the function to start the job
    connect(this,SIGNAL(startProcessingInternal()),job_,SLOT(startJobProcessing()),Qt::QueuedConnection);
  }

  // Thread is ready for processing now, tell core that we can continue.
  startup_.release(1);

  // Start event queue (possibly added events are already queued here
  exec();
  
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
  job_ = 0;
  quit();
}


void OpenFlipperThread::startProcessing() {
  
  // Wait for thread to come up and connect its signals ... otherwise the signals might get lost
  startup_.acquire(1);
  
  // Tell internal wrapper to start with the processing
  emit startProcessingInternal();
}

OpenFlipperJob::~OpenFlipperJob() 
{ 
}

void OpenFlipperJob::startJobProcessing() {
  
  // Actually start the process ( This function will block as it uses a direct connection )
  // But it only blocks the current thread.
  emit process(jobId_); 
  
  // Tell thread that the job is done.
  emit finished();
  
  // Cleanup this object
  deleteLater();
}

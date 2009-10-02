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
//   $Revision: 5074 $
//   $Author: wilden $
//   $Date: 2009-02-25 18:27:57 +0100 (Mi, 25. Feb 2009) $
//
//=============================================================================


#include "OpenFlipperThread.hh"
#include <iostream>

OpenFlipperThread::OpenFlipperThread( QString _jobId ) :
  jobId_(_jobId)
{
  
}

OpenFlipperThread::~OpenFlipperThread() {
  
}

void OpenFlipperThread::run()
{
  std::cerr << "Start Function" << std::endl;

  // This starts the function in our local thread if DirectConnection is used!
  emit function();
  
  // Emit a message that our job is finished
  emit finished(jobId_);
  
  std::cerr << "Done " << std::endl;
  
  // start event loop of thread
  //   exec();
}

void OpenFlipperThread::cancel() {
  std::cerr << "Cancel not implemented" << std::endl;
}

void OpenFlipperThread::slotCancel( QString _jobId) {
 if ( _jobId == jobId_ )
  cancel(); 
}

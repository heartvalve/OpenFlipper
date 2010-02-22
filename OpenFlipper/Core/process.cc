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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

//== IMPLEMENTATION ==========================================================


// A job has been started by a plugin
void Core::slotStartJob( QString _jobId, QString _description , int _min , int _max, bool _blocking) {
  std::cerr << "StartJob: " << _jobId.toStdString() << " " << _description.toStdString() << " " << _min << " " << _max  << " " << _blocking <<std::endl;
  
  // Create process manager window if it has not been created before
  if(!processManager_) {
      processManager_ = new ProcessManagerWidget();
      
      // Connect cancel buttons to local slot for further treatment
      connect(processManager_, SIGNAL(cancelJobRequested(QString)),
              this,            SLOT(slotJobCancelRequested(QString)));
  }
  // Add new item
  processManager_->addJob(_jobId, _description, _min, _max);
  
  // Show window
  processManager_->show();
  
  JobInfo* info        = new JobInfo();
  info->id             = _jobId;
  info->description    = _description;
  info->currentStep    =  0;
  info->minSteps       = _min;
  info->maxSteps       = _max;
  info->blocking       = _blocking;
  
  currentJobs.push_back(info);
}

//-----------------------------------------------------------------------------
bool Core::getJob(QString _jobId, int& _index) {
  
  for ( int i = 0 ; i < currentJobs.size() ; ++i) {
      if ( currentJobs[i]->id == _jobId ) {
        _index = i;
        return true;
      }
  }
  
  emit log(LOGERR,tr("Unable to find Job %1.").arg(_jobId));
  _index = -1;
  return false;
}

//-----------------------------------------------------------------------------

// A job state has been updated by a plugin
void Core::slotSetJobState(QString _jobId, int _value ) {
  int id;
  
  if (  getJob(_jobId, id) ) {
    currentJobs[id]->currentStep = _value;
    processManager_->updateStatus(_jobId, _value);
  }
}

//-----------------------------------------------------------------------------

// A job's caption has been updated by a plugin
void Core::slotSetJobName(QString _jobId, QString _name ) {
    int id;
    
    if (  getJob(_jobId, id) ) {
        currentJobs[id]->id = _name;
        processManager_->setJobName(_jobId, _name);
    }
}
//-----------------------------------------------------------------------------

// A job's widget's status text has been updated by a plugin
void Core::slotSetJobDescription(QString _jobId, QString _text ) {
    int id;
    
    if (  getJob(_jobId, id) ) {
        currentJobs[id]->description = _text;
        processManager_->setJobDescription(_jobId, _text);
    }
}

//-----------------------------------------------------------------------------

// A job state has been canceled by a plugin
void Core::slotCancelJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {
    processManager_->removeJob(_jobId);
    currentJobs.removeAt(id);
  }
}

//-----------------------------------------------------------------------------

// A job state has been finished by a plugin
void Core::slotFinishJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {
    processManager_->removeJob(_jobId);
    currentJobs.removeAt(id);
  }
}

//-----------------------------------------------------------------------------

// A job has shall be canceled since the user pushed
// the cancel button
void Core::slotJobCancelRequested(QString /*_jobId*/) {
    
    // Cancel job still to be implemented...
}

//=============================================================================

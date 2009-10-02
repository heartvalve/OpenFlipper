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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
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
  
  JobInfo* info        = new JobInfo();
  info->jobId          = _jobId;
  info->description    = _description;
  info->min            = _min;
  info->max            = _max;
  info->blocking       = _blocking;
  info->progressDialog = new QProgressDialog(coreWidget_);
  info->progressDialog->setLabelText(_description);
  info->progressDialog->setMinimum(_min);
  info->progressDialog->setMaximum(_max);
  info->progressDialog->setValue(_min);
  info->progressDialog->resize(300,130);
  info->progressDialog->setMinimumDuration(1);
  if ( _blocking )
    info->progressDialog->setWindowModality(Qt::WindowModal);
  
  connect( info->progressDialog, SIGNAL(canceled()),
           this,SLOT(slotJobCancelButtons()));
  
  currentJobs.push_back(info);
  
}

//-----------------------------------------------------------------------------
bool Core::getJob(QString _jobId, int& _index) {
  
  for ( int i = 0 ; i < currentJobs.size() ; ++i) {
      if ( currentJobs[i]->jobId == _jobId ) {
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
    currentJobs[id]->currentState = _value;
    currentJobs[id]->progressDialog->setValue(_value);
  }
}

//-----------------------------------------------------------------------------

// A job state has been canceled by a plugin
void Core::slotCancelJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {
    currentJobs[id]->progressDialog->reset();
    currentJobs[id]->progressDialog->hide();
    delete currentJobs[id]->progressDialog;
    currentJobs.removeAt(id);
  }
}

//-----------------------------------------------------------------------------

// A job state has been finished by a plugin
void Core::slotFinishJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {
    currentJobs[id]->progressDialog->reset();
    currentJobs[id]->progressDialog->hide();
    delete currentJobs[id]->progressDialog;
    currentJobs.removeAt(id);
  }
}

// The user canceled a job
void Core::slotJobCancelButtons( ) {
  for ( int i = 0 ; i < currentJobs.size() ; ++i) {
    if ( currentJobs[i]->progressDialog == sender() ) {
      QString id = currentJobs[i]->jobId;
      slotCancelJob(id);
      emit jobCanceled( id );
    }
  }
}




//=============================================================================

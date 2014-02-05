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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

#include <OpenFlipper/widgets/processManagerWidget/BlockingWidget.hh>

//== IMPLEMENTATION ==========================================================


// A job has been started by a plugin
void Core::slotStartJob( QString _jobId, QString _description , int _min , int _max, bool _blocking) {
  std::cerr << "StartJob: " << _jobId.toStdString() << " " << _description.toStdString() << " " << _min << " " << _max  << " " << _blocking <<std::endl;

  // Create job information
  JobInfo* info        = new JobInfo();
  info->id             = _jobId;
  info->description    = _description;
  info->currentStep    =  0;
  info->minSteps       = _min;
  info->maxSteps       = _max;
  info->blocking       = _blocking;
  info->blockingWidget = 0;

  // Add job to local job list
  currentJobs.push_back(info);

  if (OpenFlipper::Options::gui()) {
    // Don't show process status in process manager
    // if blocking is enabled
    if(_blocking) {
      // Create blocking widget
      BlockingWidget* widget = new BlockingWidget(_jobId, _description,
              _min, _max);

      // Connect cancel button to local slot for further treatment
      connect(widget,     SIGNAL(cancelRequested(QString)),
              this,       SLOT(slotJobCancelRequested(QString)));

      info->blockingWidget = widget;

      int x = (coreWidget_->width()  / 2) - (widget->width()  / 2);
      int y = (coreWidget_->height() / 2) - (widget->height() / 2);

      widget->setGeometry( x, y, widget->width(), widget->height());

      // Show blocking widget
      widget->show();

    } else {
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
    }
  }
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

    if (!OpenFlipper::Options::gui())
        return;
    // Update gui
    if(!currentJobs[id]->blocking)
    	processManager_->updateStatus(_jobId, _value);
    else {
    	BlockingWidget* w = 0;
    	w = dynamic_cast<BlockingWidget*>(currentJobs[id]->blockingWidget);
    	if(w != 0) {
    		w->updateStatus(_value);
    	}
    }
  }
}

//-----------------------------------------------------------------------------

// A job's caption has been updated by a plugin
void Core::slotSetJobName(QString _jobId, QString _name ) {
    int id;
    
    if (  getJob(_jobId, id) ) {
        currentJobs[id]->id = _name;

        if (!OpenFlipper::Options::gui())
            return;
        // Update gui
        if(!currentJobs[id]->blocking)
        	processManager_->setJobName(_jobId, _name);
        else {
           	BlockingWidget* w = 0;
           	w = dynamic_cast<BlockingWidget*>(currentJobs[id]->blockingWidget);
           	if(w != 0) {
           		w->setJobId(_name);
           	}
        }
    }
}
//-----------------------------------------------------------------------------

// A job's widget's status text has been updated by a plugin
void Core::slotSetJobDescription(QString _jobId, QString _text ) {
    int id;
    
    if (  getJob(_jobId, id) ) {
        currentJobs[id]->description = _text;

        if (!OpenFlipper::Options::gui())
            return;
        // Update gui
        if(!currentJobs[id]->blocking)
        	processManager_->setJobDescription(_jobId, _text);
        else {
           	BlockingWidget* w = 0;
           	w = dynamic_cast<BlockingWidget*>(currentJobs[id]->blockingWidget);
            if(w != 0) {
            	w->setJobDescription(_text);
            }
        }
    }
}

//-----------------------------------------------------------------------------

// A job state has been canceled by a plugin
void Core::slotCancelJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {

    if (OpenFlipper::Options::gui()) {
        // Update gui
        if(!currentJobs[id]->blocking)
            processManager_->removeJob(_jobId);
        else {
            BlockingWidget* w = 0;
            w = dynamic_cast<BlockingWidget*>(currentJobs[id]->blockingWidget);
            if(w != 0) {
                w->hide();
                delete w;
            }
        }
    }

	currentJobs.removeAt(id);
  }
}

//-----------------------------------------------------------------------------

// A job state has been finished by a plugin
void Core::slotFinishJob(QString _jobId ) {
  int id;
  
  if (  getJob(_jobId, id) ) {

    if (OpenFlipper::Options::gui()) {
        // Update gui
        if(!currentJobs[id]->blocking) {
            processManager_->removeJob(_jobId);

            // Hide widget if there's no job left
            if(processManager_->getNumJobs() == 0) processManager_->hide();

        } else {
            BlockingWidget* w = 0;
            w = dynamic_cast<BlockingWidget*>(currentJobs[id]->blockingWidget);
            if(w != 0) {
                w->hide();
                delete w;
            }
        }
    }

    currentJobs.removeAt(id);
  }
}

//-----------------------------------------------------------------------------

// A job will be canceled by user request
void Core::slotJobCancelRequested(QString _jobId ) {
    
  std::cerr << "Cancel requested!" << std::endl;
  
  emit jobCanceled(_jobId);
    // Cancel job still to be implemented...
	
}

//=============================================================================

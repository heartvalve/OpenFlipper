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


/*
 * processManagerWidget.cc
 *
 *  Created on: Apr 7, 2009
 *      Author: kremer
 */

#include "processManagerWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

void ProcessManagerWidget::updateStatus(QString _id, int _status) {
    
    JobContainer job;
    std::map<QString, JobContainer>::iterator it = processMap_.begin();
    
    // Find item
    bool found = false;
    for(; it != processMap_.end(); ++it) {
        if((*it).first == _id) {
            found = true;
            job = (*it).second;
            break;
        }
    }
           
    // No such item has been found -> returning
    if(!found) return;

    // Set new status
    (job.progress)->setValue(_status);
}

void ProcessManagerWidget::setJobName(QString _id, QString _name) {
    
    JobContainer job;
    std::map<QString, JobContainer>::iterator it = processMap_.begin();
    
    // Find item
    bool found = false;
    for(; it != processMap_.end(); ++it) {
        if((*it).first == _id) {
            found = true;
            job = (*it).second;
            break;
        }
    }
    
    // No such item found -> return
    if(!found) return;
    
    (job.id)->setText(_name);
}

void ProcessManagerWidget::setJobDescription(QString _id, QString _desc) {
    
    JobContainer job;
    std::map<QString, JobContainer>::iterator it = processMap_.begin();
    
    // Find item
    bool found = false;
    for(; it != processMap_.end(); ++it) {
        if((*it).first == _id) {
            found = true;
            job = (*it).second;
            break;
        }
    }
           
    // No such item has been found -> returning
    if(!found) return;
    
    // Set new description
    (job.description)->setText(_desc);
}

void ProcessManagerWidget::addJob(QString _id, QString _description,
                                      int _minSteps, int _maxSteps) {
    
    std::map<QString, JobContainer>::iterator it = processMap_.begin();
    
    // Find item
    bool found = false;
    for(; it != processMap_.end(); ++it) {
        if((*it).first == _id) {
            found = true;
            break;
        }
    }
           
    // Item with the same id has already been added
    if(found) return;
    
    QTableWidgetItem* name      = new QTableWidgetItem(_id);
    QTableWidgetItem* desc      = new QTableWidgetItem(_description);
    JobCancelButton* button     = new JobCancelButton("Cancel", _id, this->processList);
    QProgressBar* progressBar   = new QProgressBar(this->processList);
    progressBar->setMaximum(_maxSteps);
    progressBar->setMinimum(_minSteps);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    
    // Connect cancel button to event handler
    connect(button, SIGNAL(pressed()), this, SLOT(cancelButtonPressed()));
    
    int newRow = this->processList->rowCount();
    
    this->processList->insertRow(newRow);
    
    this->processList->setItem(newRow, 0, name);
    this->processList->setItem(newRow, 1, desc);
    this->processList->setCellWidget(newRow, 2, progressBar);
    this->processList->setCellWidget(newRow, 3, button);
    
    // Insert job into local map
    JobContainer job;
    job.id          = name;
    job.description = desc;
    job.progress    = progressBar;
    job.button      = button;
    
    processMap_.insert(std::pair<QString, JobContainer>(_id, job));
}

void ProcessManagerWidget::cancelButtonPressed() {
    
    // Get pushed button
    JobCancelButton* button = 0;
    button = dynamic_cast<JobCancelButton*>(QObject::sender());
    
    // Some error has occurred
    if(button == 0) return;
    
    // Emit signal to cancel job
    emit cancelJobRequested(button->jobId());
}

void ProcessManagerWidget::removeJob(QString _id) {

    JobContainer job;
    std::map<QString, JobContainer>::iterator it = processMap_.begin();
    
    // Find item
    bool found = false;
    for(; it != processMap_.end(); ++it) {
        if((*it).first == _id) {
            found = true;
            job = (*it).second;
            break;
        }
    }
           
    // No such item has been found -> returning
    if(!found) return;

    this->processList->removeRow(this->processList->row(job.id));
    
    // Remove from local map
    processMap_.erase(_id);
}

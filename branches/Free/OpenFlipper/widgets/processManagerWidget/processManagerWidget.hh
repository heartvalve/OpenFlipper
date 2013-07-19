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
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


/*
 * processManagerWidget.hh
 *
 *  Created on: Apr 7, 2009
 *      Author: kremer
 */

#ifndef PROCESSMANAGERWIDGET_HH_
#define PROCESSMANAGERWIDGET_HH_


#include "ui_processManagerWidget.hh"

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

// A button class that additionally stores
// an attached job's id.
class JobCancelButton : public QPushButton {
    Q_OBJECT
    
public:
    JobCancelButton(QString _caption, QString _jobId, QWidget* _parent = 0) :
    QPushButton(_caption, _parent),
    jobId_(_jobId) {};
    
    // Set job's id
    void setJobId(const QString& _jobId) { jobId_ = _jobId; };
    
    // Get job's id
    QString jobId() { return jobId_; }

private:
    QString jobId_;
};

class ProcessManagerWidget : public QWidget, public Ui::ProcessManagerWidget
{
  Q_OBJECT
  
  signals:
      
    void cancelJobRequested(QString _jobId);

  public:

    ProcessManagerWidget(QWidget* parent = 0) : QWidget(parent) {
        
        setupUi(this);
    };

    virtual ~ProcessManagerWidget() {};
    
    void updateStatus(QString _id, int _status);
    
    void setJobName(QString _id, QString _desc);
    
    void setJobDescription(QString _id, QString _desc);
    
    void addJob(QString _id, QString _description = "",
                    int _minSteps = 0, int _maxSteps = 100);
                    
    void removeJob(QString _jobName);
    
    int getNumJobs() { return processMap_.size(); }

  private slots:
      
    void cancelButtonPressed();
    
  private:
      
    // A container to hold the widget items
    struct JobContainer {
        int               row;
        QTableWidgetItem* id;
        QTableWidgetItem* description;
        QProgressBar*     progress;
        JobCancelButton*  button;
    };
    
    std::map<QString, JobContainer> processMap_;
};


#endif /* PROCESSMANAGERWIDGET_HH_ */

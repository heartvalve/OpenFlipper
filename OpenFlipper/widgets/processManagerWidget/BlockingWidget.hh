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

#ifndef BLOCKINGWIDGET_HH
#define BLOCKINGWIDGET_HH

#include "processManagerWidget.hh"

class BlockingWidget : public QWidget {
Q_OBJECT

signals:
	void cancelRequested(QString _jobId);

private slots:

	void cancelPressed() {
		JobCancelButton* button = 0;
		button = dynamic_cast<JobCancelButton*>(QObject::sender());
		if(button != 0) {
			emit cancelRequested(button->jobId());
		}
	};

public:
	BlockingWidget(QString _jobId, QString _description,
			int _minSteps, int _maxSteps, QWidget* _parent = 0) : QWidget(_parent),
			layout_(0),
			description_(0),
			progress_(0),
			cancelButton_(0) {

        // Block all other windows of application
        setWindowModality(Qt::ApplicationModal);
		setWindowTitle(_jobId);
		// Set window size
		QSize size(300, 150);
		resize(size);

		layout_ = new QVBoxLayout();
		description_ = new QLabel(_description);
		layout_->addWidget(description_);

		progress_ = new QProgressBar();
		progress_->setMinimum(_minSteps);
		progress_->setMaximum(_maxSteps);
		progress_->setValue(0);
		progress_->setTextVisible(true);
		layout_->addWidget(progress_);

		cancelButton_ = new JobCancelButton("Cancel", _jobId, this);
		layout_->addWidget(cancelButton_);

		setLayout(layout_);

		// Connect cancel button
		connect(cancelButton_, SIGNAL(pressed()), this, SLOT(cancelPressed()));
	};

	void updateStatus(int _value) { progress_->setValue(_value); };

	void setJobId(QString _jobId) { setWindowTitle(_jobId); };

	void setJobDescription(QString _description) { description_->setText(_description); };

private:
	QVBoxLayout* 		layout_;
	QLabel* 			description_;
	QProgressBar* 		progress_;
	JobCancelButton*	cancelButton_;
};

#endif // BLOCKINGWIDGET_HH

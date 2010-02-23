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

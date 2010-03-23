#ifndef PROGRESSEMITTER_HH
#define PROGRESSEMITTER_HH

#include <QObject>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

class ProgressEmitter : public QObject
{
  Q_OBJECT
  
  signals:
    void signalJobState(QString _jobId, int _value);
    void changeDescription(QString _jobId, QString _description);
  
  public:
    ProgressEmitter(QString _jobId) : jobId_(_jobId) {}
    
    void sendProgressSignal(double _percent) {
      emit signalJobState(jobId_, (int)_percent);
//       std::cerr << "Job State for " << jobId_.toStdString() << ": " << (int)_percent << "%" << std::endl;
    }
    
    void sendChangeDescriptionSignal(QString _description) {
      emit changeDescription(jobId_, _description);
//       std::cerr << "Job description for " << jobId_.toStdString() << ": '" << _description.toStdString() << "'" << std::endl;
    }
    
  private:
    QString jobId_;
};


#endif //PROGRESSEMITTER_HH
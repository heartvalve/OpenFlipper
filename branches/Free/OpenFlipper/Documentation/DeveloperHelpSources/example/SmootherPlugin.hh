#ifndef SMOOTHERPLUGIN_HH
#define SMOOTHERPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>

class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)

  signals:
    //BaseInterface
    void updateView();
    void updatedObject(int _identifier, const UpdateType& _type);

    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ToolboxInterface
    void addToolbox( QString _name, QWidget* _widget );

  public:

    SmootherPlugin();

    // BaseInterface
    QString name() { return (QString("Simple Smoother")); };
    QString description( ) { return (QString("Smooths the active Mesh")); };

   private:

    /// SpinBox for Number of iterations
    QSpinBox* iterationsSpinbox_;

   private slots:

    // BaseInterface
    void initializePlugin();


    void simpleLaplace();

   public slots:
      QString version() { return QString("1.0"); };
};

#endif //SMOOTHERPLUGIN_HH

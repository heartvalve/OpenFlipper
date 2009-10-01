
#ifndef ISOTROPICREMESHERPLUGIN_HH
#define ISOTROPICREMESHERPLUGIN_HH

#include <QtGui>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "IsotropicRemesherToolbox.hh"


class IsotropicRemesherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, RPCInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(RPCInterface)


signals:
  
  //BaseInterface
  void updateView();
  void updatedObject(int);

  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  //LoggingInterface:
  void log( Logtype _type, QString _message );
  void log( QString _message );

  // RPC Interface
  void pluginExists( QString _pluginName , bool& _exists  ) ;
  void functionExists( QString _pluginName , QString _functionName , bool& _exists  );
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget );   

private slots:

  // BaseInterface
  void initializePlugin();  
  void pluginsInitialized(); // BaseInterface

public :

  ~IsotropicRemesherPlugin() {};

  QString name() { return (QString("IsotropicRemesher")); };
  QString description( ) { return (QString("an isotropic remesher")); };

//GUI
private :
  IsotropicRemesherToolBox* tool_;

private slots:
  void slotRemesh();

  void slotSetMinLength();
  void slotSetMaxLength();
  void slotSetMeanLength();

//scripting functions
public slots:
  void isotropicRemesh(int _objectID, double _targetEdgeLength );


public slots:
  QString version() { return QString("1.0"); };
};

#endif //ISOTROPICREMESHERPLUGIN_HH

#ifndef SLICEPLUGIN_HH
#define SLICEPLUGIN_HH

#include <QObject>
#include <QtGui>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <ACG/Scenegraph/ClippingNode.hh>
// #include <OpenFlipper/common/Types.hh>

#include "SliceToolbox.hh"

class SlicePlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)

//BaseInterface
signals:
  void updateView();
  void updatedObject(int);

//LoggingInterface:
  void log( Logtype _type, QString _message );
  void log( QString _message );
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget );   

private slots:

  void initializePlugin(); // BaseInterface
  
public :

  ~SlicePlugin() {};

  QString name() { return (QString("Slice")); };
  QString description( ) { return (QString("Slice objects")); };

private :
  SliceToolBox* tool_;
  QButtonGroup* bbGroup;
  QButtonGroup* axisGroup;

  void getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax);

private slots:
  void resetParameters();
  void updateSlice();
  void updateSlice(int bla);

private:
  ACG::SceneGraph::ClippingNode* node_;

public slots:
  QString version() { return QString("1.0"); };
};

#endif //SLICEPLUGIN_HH

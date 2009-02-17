//=============================================================================
//
// SmootherPlugin
//
//=============================================================================

#ifndef SMOOTHERPLUGIN_HH
#define SMOOTHERPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

#include "SmootherToolbarWidget.hh"


//== CLASS DEFINITION =========================================================


/** Plugin for Smoother Support
 */
class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)

signals:

  /// Force Examiner widget to update their views
  void updateView();

  void log(Logtype _type, QString _message);
  void log(QString _message);

  /// Emit this Signal, if object list has changed (e.g. Source or Target changed)
  void updatedObject(int);

  void activeObjectChanged();

public :

  // default constructor
  SmootherPlugin() : tool_(0) {};

  // default destructor
  ~SmootherPlugin() {};

  /** Initialize the toolbar (create a widget in the right side toolbox) */
  bool initializeToolbox(QWidget*& _widget);

  /// Name of the Plugin
  QString name(){ return (QString("Smoother")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Smoothing ...")); };

  /// User selected plugins Toolbox
  void toolboxActivated( bool /*_activated*/  ) {};


private :

  /// Widget for Toolbox
  SmootherToolbarWidget* tool_;

private slots:

  void slot_smooth();


public slots:
   QString version() { return QString("1.0"); };

};

#endif //SMOOTHERPLUGIN_HH

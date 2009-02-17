//=============================================================================
//
// CLASS DecimaterPlugin
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Author$
//   $Date: 03-10-2007$
//
//=============================================================================

#ifndef DECIMATERPLUGIN_HH
#define DECIMATERPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>


#include "DecimaterToolbarWidget.hh"


//== CLASS DEFINITION =========================================================

/** Plugin for Decimater Support
 */
class DecimaterPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface
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
  DecimaterPlugin()
    : tool_(0)

  {};

  // default destructor
  ~DecimaterPlugin() {};

  /** Initialize the toolbar (create a widget in the right side toolbox) */
  bool initializeToolbox(QWidget*& _widget);

  /// Name of the Plugin
  QString name(){ return (QString("Decimater")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Decimation ...")); };

  /// User selected plugins Toolbox
  void toolboxActivated( bool /*_activated*/  ) {};


private :

  /// Widget for Toolbox
  DecimaterToolbarWidget* tool_;

private slots:

  void slot_decimate();

public slots:
   QString version() { return QString("1.0"); };

};

#endif //DECIMATERPLUGIN_HH

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

  // BaseInterface
  void updateView();
  void updatedObject(int);
  void activeObjectChanged();
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

public :

  /// Default constructor
  DecimaterPlugin() : tool_(0) {};

  /// Default destructor
  ~DecimaterPlugin() {};

  /// Initialize the toolbar (create a widget in the right side toolbox)
  bool initializeToolbox(QWidget*& _widget);

  /// Name of the Plugin
  QString name(){ return (QString("Decimater")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Decimation ...")); };

private :

  /// Widget for Toolbox
  DecimaterToolbarWidget* tool_;

private slots:

  /// decimating called from button in toolbox
  void slot_decimate();

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================

public slots:
  void decimate(int _objID, QString _constraints, QString _values);

public slots:
   QString version() { return QString("1.0"); };


/** @} */

};

#endif //DECIMATERPLUGIN_HH

#ifndef CoordsysPlugin_HH
#define CoordsysPlugin_HH

#include <QObject>
#include <QtGui>
#include <QCheckBox>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ACG/Scenegraph/CoordsysNode.hh>

class CoordsysPlugin : public QObject, BaseInterface, KeyInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(KeyInterface)

signals:

  void updateView();

private slots:
  void slotObjectUpdated(int /*_identifier*/){};

//   void slotKeyEvent( QKeyEvent* _event ) {};

  void pluginsInitialized();

public :

  ~CoordsysPlugin() {};

  QString name() { return (QString("Coordsys")); };
  QString description( ) { return (QString("Paints a coordinate system frame")); };

private :

  ACG::SceneGraph::CoordsysNode* node_;

public slots:
  QString version() { return QString("0.1"); };
};

#endif //CoordsysPlugin_HH

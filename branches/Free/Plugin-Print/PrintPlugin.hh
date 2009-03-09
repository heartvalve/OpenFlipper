#ifndef PRINTPLUGIN
#define PRINTPLUGIN

#include <QObject>
#include <QMenuBar>
#include "PrintPlugin.hh"

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>

class PrintPlugin : public QObject, BaseInterface, MenuInterface, KeyInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(MenuInterface)
   Q_INTERFACES(KeyInterface)

   signals:
     void addMenubarAction(QAction* _action, MenuActionType _type );

  private slots:
    void pluginsInitialized();

    void slotKeyEvent( QKeyEvent* _event );

  public :
    QString name() { return (QString("Printing Plugin")); };
    QString description( ) { return (QString("Print the current view")); };

  public slots:
    void printView();

  public slots:
    QString version() { return QString("1.0"); };
};

#endif //PRINTPLUGIN

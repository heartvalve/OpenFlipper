#ifndef COLORPLUGIN_HH
#define COLORPLUGIN_HH

#include <QObject>
#include <QMenuBar>
#include "ColorPlugin.hh"


#include <ACG/QtWidgets/QtExaminerViewer.hh>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include "OpenFlipper/BasePlugin/StatusbarInterface.hh"

class ColorPlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, ToolbarInterface, StatusbarInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(MenuInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(ToolbarInterface)
   Q_INTERFACES(StatusbarInterface)

   signals:
     void update_view();

     void scriptInfo( QString _functionName );

     void addMenu(QMenu* _menu, MenuType _type);

     void addToolbar(QToolBar* _toolbar);
     void removeToolbar(QToolBar* _toolbar);

     void showStatusMessage(QString _message, int _timeout = 0);

  private slots:

     void pluginsInitialized();

   public :
     void init();

     QString name() { return (QString("Color Plugin")); };
     QString description( ) { return (QString("Sets the Default colors e.g. for slides, paper,... snapshots")); };

   public slots:
      void setDefaultColor();
      void setPowerpointColor();
      void setPaperColor();

      /// Set the Background Color
      void setBackgroundColor( Vector _color );

      QString version() { return QString("1.0"); };

   private :
      QAction* AC_set_Default_color;
      QAction* AC_set_Powerpoint_color;
      QAction* AC_set_Paper_color;
};

#endif //COLORPLUGIN_HH

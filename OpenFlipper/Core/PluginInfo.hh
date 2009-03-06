
#ifndef PLUGININFO_HH
#define PLUGININFO_HH

#include <Qt>
#include <QString>
#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>

struct KeyBinding{
  int                   key;
  Qt::KeyboardModifiers modifiers;
  QString               description;
  bool                  multiUse;
  bool                  slot;
};

struct SlotInfo{
  QString slotName;
  QString slotDescription;
  QStringList parameters;
  QStringList descriptions;
};

/** Type defining a currently loaded Plugin */
class PluginInfo{

  public :

  PluginInfo() {
     plugin = 0;
     name = "";
     description = "";
     version = "";
     path = "";
     rpcName = "";
     rpcFunctions.clear();
     slotInfos.clear();
     keys.clear();
     widget = 0;
     optionsWidget = 0;
  }



  PluginInfo( const PluginInfo& _i) {
     plugin = _i.plugin;
     name = _i.name;
     description = _i.description;
     version = _i.version;
     path = _i.path;
     rpcName = _i.rpcName;
     rpcFunctions = _i.rpcFunctions;
     slotInfos = _i.slotInfos;
     keys = _i.keys;
     widget = _i.widget;
     optionsWidget = _i.optionsWidget;
  }

  /// Pointer to the loaded plugin (Already casted when loading it)
  QObject*    plugin;

  /// Name of the plugin ( requested from the plugin on load)
  QString     name;

  /// Description of the plugin ( requested from the plugin on load)
  QString     description;

  /// Version of the plugin
  QString     version;

  /// Path to the plugin ( set on load )
  QString     path;

  /// Clean rpc name of the plugin
  QString     rpcName;

  /// List of exported rpc slots
  QStringList rpcFunctions;

  /// This list contains Descriptions about public slots if available
  QList< SlotInfo > slotInfos;

  /// List of registered keys with description
  QList< KeyBinding > keys;

  /// Pointer to plugins toolbar widget (if available)
  QWidget* widget;

  /// Pointer to plugins options widget (if available)
  QWidget* optionsWidget;
};

#endif //PLUGININFO_HH

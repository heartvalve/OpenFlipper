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
};

/** Type defining a currently loaded Plugin */
struct PluginInfo{

  /// Pointer to the loaded plugin (Already casted when loading it)
  QObject*    plugin;

  /// Name of the plugin ( requested from the plugin on load)
  QString     name;

  /// Description of the plugin ( requested from the plugin on load)
  QString     description;

  /// Path to the plugin ( set on load )
  QString     path;

  /// Clean rpc name of the plugin
  QString     rpcName;

  /// List of exported rpc slots
  QStringList rpcFunctions;

  /// List of registered keys with description
  QList< KeyBinding > keys;

  /// Pointer to plugins toolbar widget (if available)
  QDockWidget* widget;
};

#endif //PLUGININFO_HH

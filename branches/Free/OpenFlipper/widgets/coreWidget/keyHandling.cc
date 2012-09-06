/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

KeyBinding CoreWidget::getKeyBinding(QObject* _plugin, int _keyIndex ){
  if (_plugin == 0)
    return coreKeys_[_keyIndex];

  for (uint i=0; i < plugins_.size(); i++){
    if (plugins_[i].plugin == _plugin)
      return plugins_[i].keys[_keyIndex];
  }

  emit log(LOGERR,tr("ERROR: could not get KeyBinding"));
  return KeyBinding();
}

QString CoreWidget::getRPCName(QObject* _plugin ){
  if (_plugin == 0)
    return "";

  for (uint i=0; i < plugins_.size(); i++){
    if (plugins_[i].plugin == _plugin)
      return plugins_[i].rpcName;
  }

  emit log(LOGERR,tr("ERROR: could not get rpcname"));
  return "";
}

/// passes keyPressEvents to either the Core or a Plugin depending on who has registered the key
void CoreWidget::keyPressEvent(QKeyEvent* _e)
{
  std::pair< int,Qt::KeyboardModifiers > key = std::make_pair(_e->key(), _e->modifiers() );

  //iterate over all assigned keys
  KeyRange range = keys_.equal_range(key);

  KeyMap::iterator it;
  for (it=range.first; it != range.second; ++it){

    QObject* plugin = (*it).second.first;
    KeyBinding binding = getKeyBinding( plugin, (*it).second.second );

    //check if its a core Key
    if (plugin == 0){

      //the key belongs to a slot
      if (binding.slot){
        bool ok;
        emit call("core." + binding.description, ok);
        return;
      }

      // =================================================================================
      // Map event to the cores key and modifier.
      // Call the core key handler with the mapped event.
      // =================================================================================
      QKeyEvent* mappedEvent = new QKeyEvent(_e->type(),binding.key, binding.modifiers,
                                             _e->text(), _e->isAutoRepeat(), _e->count() );

      coreKeyPressEvent(mappedEvent);

      delete mappedEvent;

      //if the key is multiUse also check other assigned keys
      if (binding.multiUse)
        continue;
      else
        return;
    }

    //it's a plugin key

    //the key belongs to a slot
    if (binding.slot){
      bool ok;
      emit call(getRPCName(plugin) +"."+ binding.description, ok);
      return;
    }

    //the key was specified through keyInterface
    KeyInterface* keyPlugin = qobject_cast< KeyInterface * >(plugin);

    if (keyPlugin){

      // =================================================================================
      // Map event to the plugins key and modifier.
      // Call it with the mapped event.
      // =================================================================================
      QKeyEvent* mappedEvent = new QKeyEvent(_e->type(),binding.key, binding.modifiers,
                                             _e->text(), _e->isAutoRepeat(), _e->count() );

      keyPlugin->slotKeyEvent(mappedEvent);

      delete mappedEvent ;
    }

    //if its not a multiUse key we are ready
    if (!binding.multiUse)
      return;
  }
}

//-----------------------------------------------------------------------------

/// passes keyReleaseEvents to either the Core or a Plugin depending on who has registered the key
void CoreWidget::keyReleaseEvent(QKeyEvent* _e) {

  if (_e->isAutoRepeat()) return; //consider only "real" release events

  std::pair< int,Qt::KeyboardModifiers > key = std::make_pair(_e->key(), _e->modifiers() );

  //iterate over all assigned keys
  KeyRange range = keys_.equal_range(key);

  KeyMap::iterator it;
  for (it=range.first; it != range.second; ++it){

    QObject* plugin = (*it).second.first;
    KeyBinding binding = getKeyBinding( plugin, (*it).second.second );

    if (plugin == 0){

      // =================================================================================
      // Map event to the cores key and modifier.
      // Call the core key handler with the mapped event.
      // =================================================================================
      QKeyEvent* mappedEvent = new QKeyEvent(_e->type(),binding.key, binding.modifiers,
                                             _e->text(), _e->isAutoRepeat(), _e->count() );
      coreKeyReleaseEvent(mappedEvent);

      delete mappedEvent;

      //if the key is multiUse also check other assigned keys
      if (binding.multiUse)
        continue;
      else
        return;
    }

    //it's a plugin key
    KeyInterface* keyPlugin = qobject_cast< KeyInterface * >(plugin);

    if (keyPlugin){

      // =================================================================================
      // Map event to the plugins key and modifier.
      // Call the plugin with the mapped event.
      // =================================================================================

      QKeyEvent* mappedEvent = new QKeyEvent(_e->type(),binding.key, binding.modifiers,
                                             _e->text(), _e->isAutoRepeat(), _e->count() );

      keyPlugin->slotKeyReleaseEvent(mappedEvent);

      delete mappedEvent;
    }

    //if its not a multiUse key we are ready
    if (!binding.multiUse)
      return;
  }
}

//-----------------------------------------------------------------------------

/// Register a key to the core or a plugin
void CoreWidget::slotRegisterKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse){

  //first check if the key is already registered by the coreWidget
  bool found = false;
  bool multi = false;
  QString name;
  for (uint i=0; i < coreKeys_.size(); i++)
    if (coreKeys_[i].key == _key && coreKeys_[i].modifiers == _modifiers){
      found = true;
      multi = coreKeys_[i].multiUse;
      name = "Core";
      break;
    }

  //then check if the key is already registered by a different plugin
  if (!found)
    for (uint i=0; i < plugins_.size(); i++)
      for (int k=0; k < plugins_[i].keys.count(); k++)
        if (plugins_[i].keys[k].key == _key
        && plugins_[i].keys[k].modifiers == _modifiers){
          found = true;
          multi = plugins_[i].keys[k].multiUse;
          name = plugins_[i].name;
          break;
        }

  if (found && !multi)
    emit log(LOGERR, tr("Key already registered by '%1'").arg( name ) );

  //check if its a key for the core
  if (sender() == this){
    KeyBinding kb;
    kb.key = _key;
    kb.modifiers = _modifiers;
    kb.description = _description;
    kb.multiUse = multi || _multiUse;
    kb.slot = false;

    if (multi && !_multiUse)
      emit log(LOGERR, tr("Key already registered by '%1'. Forced registration as multiUse key.").arg( name ));

    coreKeys_.push_back( kb );

    keys_.insert( std::make_pair( std::make_pair(_key, _modifiers) , std::make_pair ((QObject*)0, coreKeys_.size()-1 ) )) ;
    invKeys_.insert( std::make_pair( std::make_pair ((QObject*)0, coreKeys_.size()-1 ) , std::make_pair(_key, _modifiers) ) );
    return;
  }

  //find plugin
 PluginInfo* pluginInfo = 0;

  for (uint i=0; i < plugins_.size(); i++)
    if (plugins_[i].plugin == sender())
      pluginInfo = &plugins_[i];

  if (pluginInfo == 0){
    emit log(LOGERR, tr("Unable to register key. Plugin not found!"));
    return;
  }

  KeyBinding kb;
  kb.key = _key;
  kb.modifiers = _modifiers;
  kb.description = _description;
  kb.multiUse = multi || _multiUse;
  kb.slot = false;

  if (multi && !_multiUse)
    emit log(LOGERR, tr("Key already registered by '%1'. Forced registration as multiUse key.").arg( name ));

  pluginInfo->keys.append( kb );

  keys_.insert( std::make_pair( std::make_pair(_key, _modifiers) , std::make_pair(pluginInfo->plugin, pluginInfo->keys.size()-1) ) );
  invKeys_.insert( std::make_pair( std::make_pair(pluginInfo->plugin, pluginInfo->keys.size()-1) , std::make_pair(_key, _modifiers) ) );
}

///scripting slots will automatically registered to be assigned  to an invalid key(-1)
void CoreWidget::slotRegisterSlotKeyBindings(){

  //check the core slots
  for (int i=0; i < coreSlots_.count(); i++){

    //only consider functions without arguments
    if ( !coreSlots_.at(i).slotName.contains( "()" ) )
      continue;

    KeyBinding kb;
    kb.key = -1;
    kb.modifiers = 0;
    kb.description = coreSlots_.at(i).slotName;
    kb.multiUse = true;
    kb.slot = true;

    coreKeys_.push_back( kb );

    keys_.insert( std::make_pair( std::make_pair(-1, 0) , std::make_pair ((QObject*)0, coreKeys_.size()-1 ) )) ;
    invKeys_.insert( std::make_pair( std::make_pair ((QObject*)0, coreKeys_.size()-1 ) , std::make_pair(-1, 0) ) );
  }

  //check all plugins
  for (uint i=0; i < plugins_.size(); i++)

    for (int j=0; j < plugins_[i].rpcFunctions.count(); j++){

      //only consider functions without arguments
      if ( !plugins_[i].rpcFunctions[j].contains( "()" )
         || plugins_[i].rpcFunctions[j] == "version()")
        continue;

      KeyBinding kb;
      kb.key = -1;
      kb.modifiers = 0;
      kb.description = plugins_[i].rpcFunctions[j];
      kb.multiUse = true;
      kb.slot = true;

      plugins_[i].keys.append( kb );

      keys_.insert( std::make_pair( std::make_pair(-1, 0) , std::make_pair(plugins_[i].plugin, plugins_[i].keys.size()-1) ) );
      invKeys_.insert( std::make_pair( std::make_pair(plugins_[i].plugin, plugins_[i].keys.size()-1) , std::make_pair(-1, 0) ) );
    }
}

/// add a new keyMapping (keyBindingID is the id of the keyBinding in the pluginInfo of _plugin :)
void CoreWidget::slotAddKeyMapping(int _key, Qt::KeyboardModifiers _modifiers, QObject* _plugin, int _keyBindingID){

  std::pair< int,Qt::KeyboardModifiers > keyCombi = std::make_pair(_key, _modifiers );
  std::pair< int,Qt::KeyboardModifiers > oldCombi;
  std::pair< QObject*, int > oldTarget;

  bool replace = false;

  //and check if the key is already assigned without multiUse
  KeyMap::iterator it;
  for (it=keys_.begin(); it != keys_.end(); ++it){

    int key = (*it).first.first;
    Qt::KeyboardModifiers modifiers = (*it).first.second;
    QObject* plugin = (*it).second.first;
    int bindingID = (*it).second.second;
    KeyBinding binding = getKeyBinding(plugin, bindingID);

    //check if its the keyBinding we want to map/replace
    if (plugin == _plugin && bindingID == _keyBindingID){
      replace = true;
      oldCombi = (*it).first;
      oldTarget = (*it).second;
      continue;
    }

    //check if the mapping is conflicting with other mappings
    if (_key == key && _modifiers == modifiers ){

      if (!binding.multiUse){
        if (plugin == 0)
          emit log(LOGERR, tr("Could not add key mapping. Key already assigned to the core."));
        else{
          BaseInterface* basePlugin = qobject_cast< BaseInterface * >(plugin);

          if (basePlugin)
            emit log(LOGERR, tr("Could not add key mapping. Key already assigned to %1").arg( basePlugin->name() ) );
          else
            emit log(LOGERR, tr("Could not add key mapping. Key already assigned to an unknown plugin."));
        }
        return;
      }
    }
  }

  KeyBinding keyBinding = getKeyBinding(_plugin, _keyBindingID);

  //check if new binding doesn't allow multiUse but other assignments for the key exist
  if (!keyBinding.multiUse)
    if ( (replace && keys_.count(keyCombi) > 1)  || (!replace && keys_.count(keyCombi) > 0) ){
      emit log(LOGERR, tr("Could not add (single usage) key mapping. Key already assigned."));
      return;
    }

  if (replace){
    keys_.erase(oldCombi);
    invKeys_.erase(oldTarget);
  }

  //now we can add the mapping
  keys_.insert   ( std::make_pair( keyCombi , std::make_pair(_plugin, _keyBindingID) ));
  invKeys_.insert( std::make_pair( std::make_pair(_plugin, _keyBindingID),  keyCombi ));

}

///Load key assignments from a given INI file
void CoreWidget::loadKeyBindings(INIFile& _ini){

  QVector< int > keys;
  QVector< int > modifiers;
  QStringList pluginNames;
  QVector< int > bindingIDs;

  //first load everything from INI file
  if ( !_ini.section_exists("KeyBindings") )
     return;

  int keyCount;
  if (_ini.get_entry(keyCount,"KeyBindings","KeyCount") ){

    int key;
    int mod;
    QString name;
    int binding;

    for (int i=0; i < keyCount; i++){

      if (!_ini.get_entry(key,     "KeyBindings","Key" + QString::number(i) ) ) continue;
      if (!_ini.get_entry(mod,     "KeyBindings","KeyModifiers" + QString::number(i) ) )  continue;
      if (!_ini.get_entry(name,    "KeyBindings","KeyTarget" + QString::number(i) ) )  continue;
      if (!_ini.get_entry(binding, "KeyBindings","KeyBinding" + QString::number(i) ) ) continue;

      keys.push_back( key );
      modifiers.push_back( mod );
      pluginNames.push_back( name );
      bindingIDs.push_back( binding );
    }
  }

  //add the keyMapping
  for (int i=0; i < keys.count(); i++){

    //first we need the plugin
    QObject* plugin = 0;

    if (pluginNames[i] != "Core" ){
      //search for the plugin
      for (uint i=0; i < plugins_.size(); i++)
        if (plugins_[i].rpcName == pluginNames[i] ){
          plugin = plugins_[i].plugin;
          break;
        }

      if (plugin == 0)
        continue; //because plugin was not found
    }

    slotAddKeyMapping( keys[i], (Qt::KeyboardModifiers) modifiers[i], plugin, bindingIDs[i] );
  }

}

///Store current key assignments to a given INI file
void CoreWidget::saveKeyBindings(INIFile& _ini){

  QVector< int > keys;
  QVector< int > modifiers;
  QStringList pluginNames;
  QVector< int > bindingIDs;

  //first get all keys with custom assignments
  KeyMap::iterator it;
  for (it=keys_.begin(); it != keys_.end(); ++it){

    int key = (*it).first.first;
    Qt::KeyboardModifiers mod = (*it).first.second;
    QObject* plugin = (*it).second.first;
    int bindingID = (*it).second.second;
    KeyBinding binding = getKeyBinding(plugin, bindingID);

    //check if current key assignment and original assignment differ
    if (key != binding.key || mod != binding.modifiers){

      //get the pluginName
      QString name;

      if (plugin == 0)
        name = "Core";
      else
        name = getRPCName(plugin);

      //store key assignment
      keys.push_back( key );
      modifiers.push_back( mod );
      pluginNames.push_back( name );
      bindingIDs.push_back( bindingID );
    }
  }

  //finally store everything to INI file
  _ini.add_entry("KeyBindings","KeyCount", keys.count());

  for (int i=0; i < keys.count(); i++){

    _ini.add_entry("KeyBindings","Key" + QString::number(i)         , keys[i] );
    _ini.add_entry("KeyBindings","KeyModifiers" + QString::number(i), modifiers[i] );
    _ini.add_entry("KeyBindings","KeyTarget" + QString::number(i)   , pluginNames[i] );
    _ini.add_entry("KeyBindings","KeyBinding" + QString::number(i)  , bindingIDs[i] );
  }
}

/// if a keyPressEvent belongs to the core this functions is called
void CoreWidget::registerCoreKeys() {

  //register keys for coreWidget
  connect(this , SIGNAL( registerKey(int, Qt::KeyboardModifiers, QString, bool) ),
          this , SLOT(slotRegisterKey(int, Qt::KeyboardModifiers, QString, bool)) );

  emit registerKey(Qt::Key_Print  , Qt::NoModifier, "Create Snapshot");
  emit registerKey(Qt::Key_Escape , Qt::NoModifier, "Switch to last action mode ( Move,Picking,Light or Info Mode)");
  emit registerKey(Qt::Key_Space  , Qt::NoModifier, "Toggle between multiview and single view");

  emit registerKey(Qt::Key_S      , Qt::ControlModifier, "Save Object");
  emit registerKey(Qt::Key_O      , Qt::ControlModifier, "Open Object");
  emit registerKey(Qt::Key_L      , Qt::ControlModifier, "Show/Hide Logger");
  emit registerKey(Qt::Key_T      , Qt::ControlModifier, "Show/Hide Toolbox");
  emit registerKey(Qt::Key_F      , Qt::ControlModifier, "Toggle Fullscreen");
  emit registerKey(Qt::Key_B      , Qt::ControlModifier, "Show/Hide StatusBar");


  if ( OpenFlipper::Options::isLinux() ) {
    emit registerKey(Qt::Key_Meta , Qt::MetaModifier, "Use Navigation mode while key is pressed");
    emit registerKey(Qt::Key_Meta , Qt::NoModifier, "Use Navigation mode while key is pressed");
  } else {
    emit registerKey(Qt::Key_Alt  , Qt::AltModifier, "Use Navigation mode while key is pressed");
    emit registerKey(Qt::Key_Alt  , Qt::NoModifier, "Use Navigation mode while key is pressed");
  }

  emit registerKey(Qt::Key_Shift  , Qt::ShiftModifier, "Apply context menu action to all Viewers", true);
  emit registerKey(Qt::Key_Shift  , Qt::NoModifier, "Apply context menu action to all Viewers", true);
  
  
  emit registerKey(Qt::Key_A      , Qt::NoModifier, "First Person view strafe left");
  emit registerKey(Qt::Key_D      , Qt::NoModifier, "First Person view strafe right");
  emit registerKey(Qt::Key_W      , Qt::NoModifier, "First Person view move forward");
  emit registerKey(Qt::Key_S      , Qt::NoModifier, "First Person view move back");
  
}

/// if a keyPressEvent belongs to the core this functions is called
void CoreWidget::coreKeyPressEvent  (QKeyEvent* _e){

  //emit log(LOGERR,"Key Press");

  if ( ( _e->key() == Qt::Key_Meta ) && OpenFlipper::Options::isLinux() ) {
    if ( _e->type() == QEvent::KeyPress ) {

      if ( lastActionMode() == actionMode()) {
        if (actionMode() == Viewer::PickingMode)
          setActionMode( Viewer::ExamineMode );
        else
          setActionMode( Viewer::PickingMode );
      } else
        setActionMode( lastActionMode() );


    }
  }

  if ( ( _e->key() == Qt::Key_Alt ) && ! OpenFlipper::Options::isLinux() ) {
    if ( _e->type() == QEvent::KeyPress ) {
      //emit log(LOGERR,"Switch to examine mode");
      if ( lastActionMode() == actionMode()) {
        if (actionMode() == Viewer::PickingMode)
          setActionMode( Viewer::ExamineMode );
        else
          setActionMode( Viewer::PickingMode );
      } else
        setActionMode( lastActionMode() );
    }
  }


  if (_e->modifiers() & Qt::ControlModifier ) {
    switch (_e->key()) {
      case Qt::Key_B :
          toggleStatusBar();
        return;

      case Qt::Key_F :
          toggleFullscreen();
        return;

      case Qt::Key_L :
          toggleLogger();
        return;

      case Qt::Key_T :
          toggleToolbox();
        return;

      case Qt::Key_O :
        loadMenu();

      case Qt::Key_S :
        saveMenu();

      default:
        return;
    }

  }

  switch (_e->key()) {
    
    case Qt::Key_Escape:
        setActionMode( lastActionMode() );
      break;

    case Qt::Key_Print:
      std::cerr << "Todo : On Print Screen, create a snapshot for all viewers" << std::endl;
      break;

    case Qt::Key_Space:
      nextViewerLayout();
      break;
      
    case Qt::Key_A:
      strafeLeft();
      break;
      
    case Qt::Key_D:
      strafeRight();
      break;
      
    case Qt::Key_W:
      moveForward();
      break;      

    case Qt::Key_S:
      moveBack();
      break;      
      
    case Qt::Key_Shift :
      shiftPressed_ = true;
      break;

    default:
      shiftPressed_ = false;
      return;
  }
}

/// if a keyReleaseEvent belongs to the core this functions is called
void CoreWidget::coreKeyReleaseEvent(QKeyEvent* _e){


  if ( ( _e->key() == Qt::Key_Meta ) && OpenFlipper::Options::isLinux() ) {
    if ( _e->type() == QEvent::KeyRelease ) {
      setActionMode( lastActionMode() );
    }
  }

  //emit log(LOGERR,"Key release");

  if ( ( _e->key() == Qt::Key_Alt ) && !OpenFlipper::Options::isLinux() ) {
    //emit log(LOGERR,"Key alt release");
    if ( _e->type() == QEvent::KeyRelease ) {
      //emit log(LOGERR,"Key alt release toggle");
      setActionMode( lastActionMode() );
    }
  }


  switch (_e->key()) {
    case Qt::Key_Shift :
      shiftPressed_ = false;
      break;
    default:
      return;
  }
}

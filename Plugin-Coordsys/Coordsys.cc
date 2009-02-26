
#include <QtGui>

#include "Coordsys.hh"

#include <iostream>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <ACG/Scenegraph/BaseNode.hh>


void CoordsysPlugin::pluginsInitialized() {

  std::cerr << "Initializing coordsys plugin" << std::endl;

  //init the slice node
  node_ = new ACG::SceneGraph::CoordsysNode(PluginFunctions::getSceneGraphRootNode(),"Coordsys Node");


}

Q_EXPORT_PLUGIN2( coordsysplugin , CoordsysPlugin );


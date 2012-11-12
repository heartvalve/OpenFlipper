/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "SelectionBasePlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Standard selection mode icon files
#define TOGGLE_IMG          "selection_toggle.png"
#define LASSO_IMG           "selection_lasso.png"
#define VOLUME_LASSO_IMG    "selection_lasso2.png"
#define SURFACE_LASSO_IMG   "surface-lasso.png"
#define SPHERE_IMG          "selection_paintSphere.png"
#define BOUNDARY_IMG        "selection_boundary.png"
#define FLOODFILL_IMG       "selection_floodFill.png"
#define COMPONENTS_IMG      "selection_connected.png"

// Standard selection mode descriptions
#define TOGGLE_DESC         "Toggle Selection"
#define LASSO_DESC          "Lasso Selection"
#define VOLUME_LASSO_DESC   "Volume Lasso Selection"
#define SURFACE_LASSO_DESC  "Surface Lasso Selection"
#define SPHERE_DESC         "Sphere Selection"
#define BOUNDARY_DESC       "Boundary Selection"
#define FLOODFILL_DESC      "Floodfill Selection"
#define COMPONENTS_DESC     "Selected Components Selection"

// Standard selection mode names
#define SB_TOGGLE           "sb_toggle"
#define SB_LASSO            "sb_lasso"
#define SB_VOLUME_LASSO     "sb_volumelasso"
#define SB_SURFACE_LASSO    "sb_surfacelasso"
#define SB_SPHERE           "sb_sphere"
#define SB_BOUNDARY         "sb_closestboundary"
#define SB_FLOODFILL        "sb_floodfill"
#define SB_COMPONENTS       "sb_components"

// Constant to set if no selection picking is enabled
#define NO_SELECTION_PICKING    "No_Selection_Picking"
#define NO_SELECTION_MODE       "No_Selection_Mode"

#define SELECTION_PICKING   "SelectionBasePicking"

// Constructor
SelectionBasePlugin::SelectionBasePlugin() :
toolBar_(0),
tool_(0),
primitivesBarGroup_(0),
pickModeToolBar_(0),
selectionModesGroup_(0),
toggleSelectionAction_(0),
lassoSelectionAction_(0),
volumeLassoSelectionAction_(0),
surfaceLassoSelectionAction_(0),
sphereSelectionAction_(0),
boundarySelectionAction_(0),
floodFillSelectionAction_(0),
componentsSelectionAction_(0),
nextFreePrimitiveType_(1u),
sphere_mat_node_(0),
sphere_node_(0),
line_node_(0),
lassoSelection_(false),
toolIcon_(0),
availableObjectTypes_(0u)
{
    
    // Reset active pick mode
    currentPickMode_            = NO_SELECTION_PICKING;
    lastPickMode_               = NO_SELECTION_PICKING;
    currentSelectionMode_       = NO_SELECTION_MODE;
    
    currentPrimitiveType_ = 0u;
    
    // We initially want selection, not deselection
    deselection_ = false;
    
    sphere_radius_ = 0.0;
}

//============================================================================================

void SelectionBasePlugin::initializePlugin() {
    
    // Skip if no gui is available
    if (OpenFlipper::Options::nogui())
        return;
  
    // Create tool box
    tool_ = new SelectionBaseToolboxWidget();
    QSize size(300, 300);
    tool_->resize(size);
    
    // Connect load/save buttons
    connect(tool_->loadSelection, SIGNAL(clicked()), this, SLOT(slotLoadSelectionButton()));
    connect(tool_->saveSelection, SIGNAL(clicked()), this, SLOT(slotSaveSelectionButton()));
    
    // Add toolbox to OpenFlipper main window
    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"selection_base.png");
    emit addToolbox("Selections", tool_, toolIcon_ );
    
    // Create toolbar that holds the selection environment buttons
    toolBar_ = new QToolBar(tr("Selection Base"));
    emit addToolbar(toolBar_);
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QAction* selectionEnvironmentButton = new QAction(QIcon(iconPath + "selection_base.png"), "Selections", toolBar_);
    toolBar_->addAction(selectionEnvironmentButton);
    
    // Connect QAction to local function that switches mode
    connect(selectionEnvironmentButton, SIGNAL(triggered(bool)), this, SLOT(slotSelectionEnvironmentRequested(bool)));
    
    // Register keys
    emit registerKey(Qt::Key_Control, Qt::NoModifier, tr("Deselection"), true);
    emit registerKey(Qt::Key_Control, Qt::ControlModifier, tr("Deselection"), true);
    
    emit registerKey(Qt::Key_Shift, Qt::NoModifier, tr("Source/Target Selection"), true);
    emit registerKey(Qt::Key_Shift, Qt::ShiftModifier, tr("Source/Target Selection"), true);
    
    // Add pickmode for the selection environment
    emit addPickMode(SELECTION_PICKING);
    // Enable mouse tracking for all pick modes
    emit setPickModeMouseTracking(SELECTION_PICKING, true);
    
    // Set tabs widget movable
    tool_->typeTabWidget->setMovable(true);

    // Set pick mode toolbar
    pickModeToolBar_ = new QToolBar("Selection Picking Toolbar");
    pickModeToolBar_->setObjectName("Selection_Picking_Toolbar");

    // Create primitive toolbar
    primitivesBarGroup_ = new QActionGroup(pickModeToolBar_);
    primitivesBarGroup_->setExclusive(true);


    // Create default selection mode actions
    selectionModesGroup_ = new QActionGroup(pickModeToolBar_);
    selectionModesGroup_->setExclusive(true);
    toggleSelectionAction_ = new HandleAction(QIcon(iconPath + TOGGLE_IMG), TOGGLE_DESC, selectionModesGroup_);
    toggleSelectionAction_->setCheckable(true);
    toggleSelectionAction_->selectionModeHandle(SB_TOGGLE);
    connect(toggleSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    lassoSelectionAction_ = new HandleAction(QIcon(iconPath + LASSO_IMG), LASSO_DESC, selectionModesGroup_);
    lassoSelectionAction_->setCheckable(true);
    lassoSelectionAction_->selectionModeHandle(SB_LASSO);
    connect(lassoSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    volumeLassoSelectionAction_ = new HandleAction(QIcon(iconPath + VOLUME_LASSO_IMG), VOLUME_LASSO_DESC, selectionModesGroup_);
    volumeLassoSelectionAction_->setCheckable(true);
    volumeLassoSelectionAction_->selectionModeHandle(SB_VOLUME_LASSO);
    connect(volumeLassoSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    surfaceLassoSelectionAction_ = new HandleAction(QIcon(iconPath + SURFACE_LASSO_IMG), SURFACE_LASSO_DESC, selectionModesGroup_);
    surfaceLassoSelectionAction_->setCheckable(true);
    surfaceLassoSelectionAction_->selectionModeHandle(SB_SURFACE_LASSO);
    connect(surfaceLassoSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    sphereSelectionAction_ = new HandleAction(QIcon(iconPath + SPHERE_IMG), SPHERE_DESC, selectionModesGroup_);
    sphereSelectionAction_->setCheckable(true);
    sphereSelectionAction_->selectionModeHandle(SB_SPHERE);
    connect(sphereSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    boundarySelectionAction_ = new HandleAction(QIcon(iconPath + BOUNDARY_IMG), BOUNDARY_DESC, selectionModesGroup_);
    boundarySelectionAction_->setCheckable(true);
    boundarySelectionAction_->selectionModeHandle(SB_BOUNDARY);
    connect(boundarySelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    floodFillSelectionAction_ = new HandleAction(QIcon(iconPath + FLOODFILL_IMG), FLOODFILL_DESC, selectionModesGroup_);
    floodFillSelectionAction_->setCheckable(true);
    floodFillSelectionAction_->selectionModeHandle(SB_FLOODFILL);
    connect(floodFillSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    componentsSelectionAction_ = new HandleAction(QIcon(iconPath + COMPONENTS_IMG), COMPONENTS_DESC, selectionModesGroup_);
    componentsSelectionAction_->setCheckable(true);
    componentsSelectionAction_->selectionModeHandle(SB_COMPONENTS);
    connect(componentsSelectionAction_, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));

    pickModeToolBar_->clear();
    pickModeToolBar_->addActions(primitivesBarGroup_->actions());
    pickModeToolBar_->addSeparator();
    pickModeToolBar_->addActions(selectionModesGroup_->actions());

    emit setPickModeToolbar(SELECTION_PICKING, pickModeToolBar_);
}

//============================================================================================

void SelectionBasePlugin::pluginsInitialized() {
    
    // Initialize scenegraph nodes that are used for the
    // rendering of the selection tools (e.g. sphere, lasso, etc.)
    
    // Sphere node
    
    std::string nodeName = std::string( tr("Selection Base Plugin: Selection Sphere Material").toUtf8() );
    sphere_mat_node_  = new ACG::SceneGraph::MaterialNode(0, nodeName );
    PluginFunctions::addGlobalNode(sphere_mat_node_);
    sphere_mat_node_->applyProperties( MaterialNode::Blending  |
                                       MaterialNode::Material  |
                                       MaterialNode::AlphaTest |
                                       MaterialNode::BackFaceCulling );
                                       
    sphere_mat_node_->set_color(ACG::Vec4f(1.0, 0.0, 0.0, 0.3));
    sphere_mat_node_->enable_blending();
    sphere_mat_node_->disable_alpha_test();
    sphere_mat_node_->enable_backface_culling();

    nodeName = std::string( tr("Selection Base Plugin: Selection Sphere").toUtf8() );
    sphere_node_ = new ACG::SceneGraph::GlutPrimitiveNode(ACG::SceneGraph::GlutPrimitiveNode::SPHERE, sphere_mat_node_, nodeName);
    sphere_node_->drawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);
    sphere_node_->get_primitive(0).color = ACG::Vec4f(1.0, 0.0, 0.0,0.3);
    sphere_node_->hide();

    // Line node
    
    nodeName = std::string( tr("Selection Base Plugin: Lasso Selection Line").toUtf8() );
    line_node_ = new ACG::SceneGraph::LineNode (ACG::SceneGraph::LineNode::PolygonMode, 0, nodeName );
    PluginFunctions::addGlobalNode(line_node_);
    line_node_->set_line_width (2.0);
    line_node_->alwaysOnTop() = true;
    line_node_->setTraverseMode (BaseNode::NodeFirst | BaseNode::SecondPass);
    line_node_->set_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
    line_node_->set_base_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
    line_node_->hide();
}

//============================================================================================

SelectionTypeFrameWidget* SelectionBasePlugin::createNewTypeFrame(SelectionEnvironment& _env) {
    
    SelectionTypeFrameWidget* tab = new SelectionTypeFrameWidget(tool_->typeTabWidget);
    
    // Layout in tool box that contains the primitive buttons
    _env.primitivesBar = new QHBoxLayout(tab->toolPrimitivesBar);
        
    // Layout in tool box that contains the operation buttons
    _env.operationsBar = new QVBoxLayout(tab->toolOperationsWidget);
    
    QSizePolicy policy;
    policy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    tab->selectionGroup->setSizePolicy(policy);
    
    tab->toolPrimitivesBar->setLayout(_env.primitivesBar);
    tab->toolPrimitivesBar->setMinimumHeight(68);
    tab->toolOperationsWidget->setLayout(_env.operationsBar);
    tab->toolOperationsWidget->setMinimumHeight(150);
    
    _env.primitiveActions = new QActionGroup(0);
    _env.primitiveActions->setExclusive(true);
    
    return tab;
}

//============================================================================================

void SelectionBasePlugin::slotLoadSelectionButton() {
    // Load selection button has been clicked

    QString filename = QFileDialog::getOpenFileName(0, tr("Load Selection"), "selection.ini", tr("Selection files ( *.ini )"));

    if(filename.length() > 0) {
        
        INIFile file;
    
        if(!file.connect(filename, false)) {
            emit log(LOGERR, QString("Could not read file '%1'!").arg(filename));
            return;
        }
        
        // Pass ini-file to all object selection implementations
        emit loadSelection(file);
        
        // Close file
        file.disconnect();
    }
}

//============================================================================================

void SelectionBasePlugin::slotSaveSelectionButton() {
    // Save selection button has been clicked
    
    QString filename = QFileDialog::getSaveFileName(0, tr("Save Selection"), "selection.ini", tr("Selection files ( *.ini )"));

    if(filename.length() > 0) {
        
        INIFile file;
    
        if(!file.connect(filename, true)) {
            emit log(LOGERR, QString("Could not create file '%1'!").arg(filename));
            return;
        }
        
        // Pass ini-file to all object selection implementations
        emit saveSelection(file);
        
        // Write all data to the file
        file.disconnect();
    }
}

//============================================================================================

void SelectionBasePlugin::slotKeyEvent(QKeyEvent* _event) {
    
    // Check if we want to select or deselect
    if ((_event->modifiers() & Qt::ControlModifier) || (_event->key() == Qt::Key_Control)) {
        deselection_ = true;
        primitivesBarGroup_->setExclusive(false);
    } else {
        deselection_ = false;
        primitivesBarGroup_->setExclusive(true);
    }
    
    // Check if we want to source or target selection
    if ((_event->modifiers() & Qt::ShiftModifier) || (_event->key() == Qt::Key_Shift))
        tool_->restrictOnTargets->setChecked(true);
    else
        tool_->restrictOnTargets->setChecked(false);
    
    // We have to store and search for registered key combinations at this point
    // in order to preserve the possibility to register some key events exclusively
    // for SelectionBasePlugin (whithout passing them on to al type selection plugins...
    
    // Search for key combination
    std::set<std::pair<int,Qt::KeyboardModifiers> >::iterator f =
        registeredKeys_.find(std::pair<int,Qt::KeyboardModifiers>(_event->key(), _event->modifiers()));
        
    if(f != registeredKeys_.end()) {
        // Some registered key found
        emit keyShortcutEvent((*f).first,(*f).second);
    }
}

//============================================================================================

void SelectionBasePlugin::slotKeyReleaseEvent(QKeyEvent* _event) {
    
    //check if we want to select or deselect
    if ((_event->modifiers() & Qt::ControlModifier) || (_event->key() == Qt::Key_Control)) {
        deselection_ = false;
        primitivesBarGroup_->setExclusive(true);
    }
    
    // Check if we want to source or target selection
    if ((_event->modifiers() & Qt::ShiftModifier) || (_event->key() == Qt::Key_Shift))
        tool_->restrictOnTargets->setChecked(false);
}

//============================================================================================

void SelectionBasePlugin::slotAddSelectionEnvironment(QString _modeName, QString _description, QIcon _icon, QString& _handleName) {
    /*
    A new selection environment is to be added. We first test if the
    associated selection environment already exists. If not, we create
    a new button and hide it.
    */
    std::map<QString,SelectionEnvironment>::iterator it =
        selectionEnvironments_.begin();    
    
    for(; it != selectionEnvironments_.end(); ++it) {
        if(_modeName == (*it).second.name) {
            emit log(LOGINFO, QString("Selection environment %1 already exists.").arg(_modeName));
            return;
        }
    }

    // Create new selection environment
    SelectionEnvironment env;
    env.name = _modeName;
    
    SelectionTypeFrameWidget* tab = createNewTypeFrame(env);
    
    // Add type frame to tab widget
    int index = tool_->typeTabWidget->addTab(tab, _icon, _modeName);
    env.tabWidget = tool_->typeTabWidget->widget(index);
    
    // Disable type frame unless there's at least one
    // object of the desired type in the scene
    tool_->typeTabWidget->setTabEnabled(index, false);
    tool_->typeTabWidget->widget(index)->setEnabled(false);
    
    // Create a unique handle name for this selection environment
    _handleName = getUniqueHandleName("h_" + _modeName.replace(" ", "_"));
    
    // Set handle for selection environment
    env.handle = _handleName;
    
    // Update pick mode toolbar
    updatePickModeToolBar();
    
    // Add selection environment to local map
    selectionEnvironments_.insert(std::pair<QString,SelectionEnvironment>(_handleName, env));
}

//============================================================================================

void SelectionBasePlugin::slotRegisterType(QString _handleName, DataType _type) {
    /*
    Register new data type under the selection environment with
    handle name _handleName.
    */
    std::map<QString,SelectionEnvironment>::iterator it =
        selectionEnvironments_.find(_handleName);
        
    // If the associated selection environment has been found,
    // we add the specified data type to the list of supported data types
    if(it != selectionEnvironments_.end()) {
        // Search if the data type has already been added before...
        for(std::vector<DataType>::iterator t_it = (*it).second.types.begin();
            t_it != (*it).second.types.end(); ++t_it) {
            // If the type already exists in the list -> return
            if ((*t_it) == _type) return;
        }
        // ...if not, add it.
        (*it).second.types.push_back(_type);
        // And show selection environment button if at least one object
        // of the associated type already exists in the scenegraph
        if(typeExists(_type)) {
            // Show selection environment's tab widget
            tool_->typeTabWidget->setTabEnabled(tool_->typeTabWidget->indexOf((*it).second.tabWidget), true);
            (*it).second.tabWidget->setEnabled(true);
        }
        
    } else {
        emit log(LOGERR, "The specified selection environment has not been found! The data type could not be registered.");
    }
}

void SelectionBasePlugin::updatePickModeToolBar() {

    // Add newly added primitive and tool buttons
    QList<QAction*> primitivesList = primitivesBarGroup_->actions();
    
    for(QList<QAction*>::iterator it = primitivesList.begin(); it != primitivesList.end(); ++it) {
            (*it)->setEnabled(false);
            
        // If at least one object of this type exists in the scene,
        // don't grey out the button
        PrimitiveAction* act = dynamic_cast<PrimitiveAction*>(*it);
        if(act) {
            std::map<QString,SelectionEnvironment>::iterator sit =
                selectionEnvironments_.find(act->selectionEnvironmentHandle());
            if(sit != selectionEnvironments_.end()) {

                bool atLeastOne = false;
                for(std::vector<DataType>::iterator tit = (*sit).second.types.begin();
                    tit != (*sit).second.types.end(); ++tit) {
                    if(typeExists(*tit)) {
                        atLeastOne = true;
                        break;
                    }
                }
                if(atLeastOne) {
                    (*it)->setEnabled(true);
                }
            }
        }
    }
    
    // Only activate those tools, that are available for the current
    // active primitive type

    for(std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
        it != selectionEnvironments_.end(); ++it) {

        // Default selection modes
        toggleSelectionAction_->setEnabled(toggleSelectionAction_->isAssociated(currentPrimitiveType_, true));
        lassoSelectionAction_->setEnabled(lassoSelectionAction_->isAssociated(currentPrimitiveType_, true));
        volumeLassoSelectionAction_->setEnabled(volumeLassoSelectionAction_->isAssociated(currentPrimitiveType_, true));
        surfaceLassoSelectionAction_->setEnabled(surfaceLassoSelectionAction_->isAssociated(currentPrimitiveType_, true));
        sphereSelectionAction_->setEnabled(sphereSelectionAction_->isAssociated(currentPrimitiveType_, true));
        boundarySelectionAction_->setEnabled(boundarySelectionAction_->isAssociated(currentPrimitiveType_, true));
        floodFillSelectionAction_->setEnabled(floodFillSelectionAction_->isAssociated(currentPrimitiveType_, true));
        componentsSelectionAction_->setEnabled(componentsSelectionAction_->isAssociated(currentPrimitiveType_, true));

        // Custom selection modes
        for(std::set<HandleAction*>::iterator cit = (*it).second.customSelectionModes.begin();
            cit != (*it).second.customSelectionModes.end(); ++cit) {

            (*cit)->setEnabled((availableObjectTypes_ & (*cit)->objectTypeRestriction()) &&
                               (*cit)->isAssociated(currentPrimitiveType_, true));
        }
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseWheelEvent(QWheelEvent* _event, const std::string& _mode) {
    
    if(currentPickMode_ == NO_SELECTION_PICKING) return;
    
    // Increase sphere radius
    if (currentSelectionMode_ == SB_SPHERE) {
        
        float d = -(float)_event->delta() / 120.0 * 0.1;
        sphere_radius_ *= 1.0 + d;

        sphere_node_->set_size(sphere_radius_);
        sphere_node_->show();

        emit updateView();
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseEvent(QMouseEvent* _event) {
    
    if(currentPickMode_ == NO_SELECTION_PICKING) return;
    
    // Go into appropriate pick processing
    if(currentSelectionMode_ == SB_TOGGLE) {
        slotMouseToggleSelection(_event);
    } else if (currentSelectionMode_ == SB_LASSO) {
        slotMouseLassoSelection(_event);
    } else if (currentSelectionMode_ == SB_VOLUME_LASSO) {
        slotMouseVolumeLassoSelection(_event);
    } else if (currentSelectionMode_ == SB_SURFACE_LASSO) {
        slotMouseSurfaceLassoSelection(_event);
    } else if (currentSelectionMode_ == SB_SPHERE) {
        slotMouseSphereSelection(_event);
    } else if (currentSelectionMode_ == SB_BOUNDARY) {
        slotMouseBoundarySelection(_event);
    } else if (currentSelectionMode_ == SB_FLOODFILL) {
        slotMouseFloodFillSelection(_event);
    } else if (currentSelectionMode_ == SB_COMPONENTS) {
        slotMouseComponentsSelection(_event);
    } else {
        // Custom selection mode
        slotMouseCustomSelection(_event);
    }
}

//============================================================================================

void SelectionBasePlugin::slotAddPrimitiveType(QString _handleName, QString _name, QIcon _icon, SelectionInterface::PrimitiveType& _typeHandle) {
    
  if (  OpenFlipper::Options::nogui() )
    return;

    // Get selection environment
    std::map<QString,SelectionEnvironment>::iterator it =
        selectionEnvironments_.find(_handleName);
        
    if(it == selectionEnvironments_.end()) {
        emit log(LOGERR, QString("Could not find selection environment width handle '%1'!").arg(_handleName));
        return;
    }
    
    SelectionEnvironment& env = (*it).second;
    
    // Test if there's a free primitive type available
    // Note: This is actually limited to 31
    if(nextFreePrimitiveType_ > nextFreePrimitiveType_ << 1) {
        emit log(LOGERR, "Maximum number of custom primitive types for selection reached!");
        return;
    }
    
    // Test if there's a custom type with the same name already
    QList<QAction*>::const_iterator a_it = env.primitiveActions->actions().constBegin();
    for(; a_it != env.primitiveActions->actions().constEnd(); ++a_it) {
        if((*a_it)->text() == _name) {
            emit log(LOGERR, QString("A custom primitive type with name \"%1\" already exists!").arg(_name));
            return;
        }
    }
    
    // Add custom primitive type
    PrimitiveAction* action = new PrimitiveAction(_icon, _name, env.primitiveActions);
    action->setCheckable(true);
    action->selectionEnvironmentHandle(_handleName);
    primitivesBarGroup_->addAction(action);
    pickModeToolBar_->clear();
    pickModeToolBar_->addActions(primitivesBarGroup_->actions());
    pickModeToolBar_->addSeparator();
    pickModeToolBar_->addActions(selectionModesGroup_->actions());

    // Also add type button to tool box of environment tab
    ActionButton* button = new ActionButton(action);
    button->setMinimumSize(QSize(32,32));
    button->setMaximumSize(QSize(64,64));
    env.primitivesBar->addWidget(button);
    
    _typeHandle = nextFreePrimitiveType_;
    action->primitiveType(_typeHandle);
    
    // Add primitive type to environment
    env.primitiveTypes |= _typeHandle;
    
    primitiveTypeButtons_.insert(std::pair<PrimitiveType,QAction*>(_typeHandle,action));
    
    // Go over to next free primitive type
    nextFreePrimitiveType_ <<= 1;
    
    // Connect action to local slot in order to keep track of active primitive types
    connect(action, SIGNAL(toggled(bool)), this, SLOT(updateActivePrimitiveTypes(bool)));
    
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::updateActivePrimitiveTypes(bool _checked) {
    
    QObject* sender = QObject::sender();
    PrimitiveAction* clickedAction = 0;
    clickedAction = dynamic_cast<PrimitiveAction*>(sender);
    
    if(!clickedAction) return;
    
    // Change button state
    clickedAction->setChecked(_checked);
    
    // Reset types
    currentPrimitiveType_ = 0u;
    
    QList<QAction*> actions = primitivesBarGroup_->actions();
    for(int i = 0; i < actions.size(); ++i) {
        
        if(actions[i]->isChecked()) {
            PrimitiveAction* pa = 0;
            pa = dynamic_cast<PrimitiveAction*>(actions[i]);
            if(pa) {
                currentPrimitiveType_ |= pa->primitiveType();
            }
        }
    }
    
    // If the primitive type button has been activated,
    // automatically go into the associated selection environment's
    // picking mode
    if(_checked) {
        slotSelectionEnvironmentRequested(_checked);
    }
    
    // If currently selected selection mode is not available
    // for the currently active primitive types, reset selection mode
    QList<QAction*> sm_actions = selectionModesGroup_->actions();
    bool atLeastOneSelectionMode = false;
    for(int i = 0; i < sm_actions.size(); ++i) {

        if(sm_actions[i]->isChecked()) {
            atLeastOneSelectionMode = true;
            HandleAction* ha = 0;
            ha = dynamic_cast<HandleAction*>(sm_actions[i]);
            if(ha) {
                if(!ha->isAssociated(currentPrimitiveType_)) {
                    ha->blockSignals(true);
                    ha->setChecked(false);
                    ha->blockSignals(false);
                    toggleSelectionAction_->trigger();
                }
            }
        }
    }
    // If no selection mode is active, automatically go into toggle mode
    if(!atLeastOneSelectionMode) {
        toggleSelectionAction_->trigger();
    }

    // Automatically show tab widget associated to this primitive type
    std::map<QString,SelectionEnvironment>::iterator sit = selectionEnvironments_.find(clickedAction->selectionEnvironmentHandle());
    if(sit != selectionEnvironments_.end() && _checked) {
        tool_->typeTabWidget->setCurrentIndex(tool_->typeTabWidget->indexOf((*sit).second.tabWidget));
    }
    
    // Clear lines
    line_node_->clear();
    linePoints_.clear();
    lassoSelection_ = false;

    // Hide and show selection functions that are associated
    // with the currently active primitive types
    slotShowAndHideOperations();
    
    // Update pick modes bar
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotAddSelectionOperations(QString _handleName, QStringList _operationsList, QString _category, PrimitiveType _type) {
    
    // Get selection environment
    std::map<QString,SelectionEnvironment>::iterator e_it =
        selectionEnvironments_.find(_handleName);
        
    if(e_it == selectionEnvironments_.end()) {
        emit log(LOGERR, QString("Could not find selection environment with handle '%1'!").arg(_handleName));
        return;
    }
    
    SelectionEnvironment& env = (*e_it).second;
    
    // Find associated layout from category
    std::map<QString,std::pair<FillingLayout*,QGroupBox*> >::iterator it = env.categories.find(_category);
    if(it == env.categories.end()) {
        // Create new category
        FillingLayout* fillLayout = new FillingLayout(2);
        QGroupBox* group = new QGroupBox(_category);
        group->setLayout(fillLayout);
        // Insert newly created fillLayout into map
        std::pair<std::map<QString,std::pair<FillingLayout*,QGroupBox*> >::iterator,bool> ret;
        ret = env.categories.insert(std::pair<QString,std::pair<FillingLayout*,QGroupBox*> >(_category,
                                            std::pair<FillingLayout*,QGroupBox*>(fillLayout,group)));
        it = ret.first;
        // Add group box to vertical operations layout
        env.operationsBar->addWidget(group);
    }
    
    // Add buttons with function names to operations widget
    for(int i = 0; i < _operationsList.size(); ++i) {
        QPushButton* button = new QPushButton(_operationsList[i]);
        button->setDisabled(true);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        int width = button->fontMetrics().width(_operationsList[i]);
        button->setMinimumWidth(width);
        button->setFixedHeight(26);

        connect(button, SIGNAL(clicked()), this, SLOT(slotOperationRequested()));
        // Add operation to local list
        env.operations.insert(std::pair<PrimitiveType,QPushButton*>(_type, button));
        // Add button to operations widget in tool box
        (*it).second.first->addWidget(button);
    }
    // Show operations if in supported primitive type mode
    slotShowAndHideOperations();
}

//============================================================================================

void SelectionBasePlugin::slotOperationRequested() {
    
    QObject* sender = QObject::sender();
    QPushButton* button = 0;
    button = dynamic_cast<QPushButton*>(sender);
    
    if(!button) return;
    
    emit selectionOperation(button->text());
}

//============================================================================================
        
void SelectionBasePlugin::slotShowAndHideOperations() {
    
    for(std::map<QString,SelectionEnvironment>::iterator e_it = selectionEnvironments_.begin();
        e_it != selectionEnvironments_.end(); ++e_it) {
        
        for(std::multimap<PrimitiveType,QPushButton*>::iterator it = (*e_it).second.operations.begin();
            it != (*e_it).second.operations.end(); ++it) {
            
            if((currentPrimitiveType_ & (*it).first) || (*it).first == 0u) {
                // Type is currently active -> show button
                (*it).second->setDisabled(false);
            } else {
                // Type is currently not active -> hide button
                (*it).second->setDisabled(true);
            }
        }
    }
}

//============================================================================================

void SelectionBasePlugin::slotSelectionEnvironmentRequested(bool _checked) {
    
    updatePickModeToolBar();
    
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode(SELECTION_PICKING);
    
    currentPickMode_ = SELECTION_PICKING;
}

//============================================================================================

void SelectionBasePlugin::slotEnterSelectionMode(bool _checked) {
    
    QObject* obj = QObject::sender();
    HandleAction* act = 0;
    act = dynamic_cast<HandleAction*>(obj);
    
    if(act == 0) return;
    
    // Make button checked
    act->setChecked(_checked);
    
    // Keep active selection mode
    if(_checked) {
        currentSelectionMode_ = act->selectionModeHandle();
    }
    
    // Clear lines
    line_node_->clear();
    linePoints_.clear();
    lassoSelection_ = false;

    if(currentSelectionMode_ == SB_SPHERE) {
        // Adjust sphere radius to have size defined relatively to the scene radius
        sphere_radius_ = 0.03 * PluginFunctions::sceneRadius();
    }
}

//============================================================================================

void SelectionBasePlugin::slotPickModeChanged (const std::string& _pickmode) {
    
    // Test if current pickmode is a selection pick mode
    bool selectionPicking = (_pickmode == "SelectionBasePicking");
     
    // Show/hode line and sphere node
    if(currentSelectionMode_ == SB_SPHERE)
        sphere_node_->show();
    else
        sphere_node_->hide();
    
    if(currentSelectionMode_ == SB_LASSO || currentSelectionMode_ == SB_VOLUME_LASSO)
        line_node_->show();
    else
        line_node_->hide();
    
    bool resetPickToolBar = false;
    
    if(currentPickMode_ != NO_SELECTION_PICKING) {
        // We go into examiner (or some other) mode for the first time
        
        // Just hide line and sphere nodes
        sphere_node_->hide();
        line_node_->hide();
        
        // Save values
        lastPickMode_ = currentPickMode_;
        currentPickMode_ = NO_SELECTION_PICKING;
        
    } else if (selectionPicking && currentPickMode_ == NO_SELECTION_PICKING && lastPickMode_ != NO_SELECTION_PICKING) {
        // We come back from examiner mode (windows key hit)
        
        currentPickMode_ = lastPickMode_;
        
    } else if (!selectionPicking && currentPickMode_ == NO_SELECTION_PICKING) {
        // We go into some other picking mode
        
        // Clear line nodes
        linePoints_.clear();
        line_node_->clear_points();
        
        // A completely different pick mode has been chosen that
        // is not handled by this plugin, so reset current pickmode
        currentPickMode_            = NO_SELECTION_PICKING;
        currentSelectionMode_       = NO_SELECTION_MODE;
        lastPickMode_               = NO_SELECTION_PICKING;
        
        // We don't want no tool to be selected anymore
        resetPickToolBar = true;
        
    } else {
        // We return from some other pickimg mode (or mutliple examiner sessions)
        // to selection mode
        
        lastPickMode_ = currentPickMode_ = _pickmode.c_str();
        
        // Reset pick toolbar
        resetPickToolBar = true;
        
        // Clear line nodes
        linePoints_.clear();
        line_node_->clear_points();
    }
    
    // Make sure that the pick mode buttons are in correct state
    toggleSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_TOGGLE);
    lassoSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_LASSO);
    volumeLassoSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_VOLUME_LASSO);
    surfaceLassoSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_SURFACE_LASSO);
    sphereSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_SPHERE);
    boundarySelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_BOUNDARY);
    floodFillSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_FLOODFILL);
    componentsSelectionAction_->setChecked(!resetPickToolBar && currentSelectionMode_ == SB_COMPONENTS);
    
    for(std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
        it != selectionEnvironments_.end(); ++it) {
        
        // Custom selection modes
        for(std::set<HandleAction*>::iterator csm_it = (*it).second.customSelectionModes.begin();
            csm_it != (*it).second.customSelectionModes.end(); ++csm_it) {
            (*csm_it)->setChecked(!resetPickToolBar && currentSelectionMode_ == (*csm_it)->selectionModeHandle());
        }
    }

    if(selectionPicking)
        updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::showSelectionMode(QString _mode, QIcon _icon, QString _desc, QString _handleName,
                                            bool _show, SelectionInterface::PrimitiveType _associatedTypes,
                                            QString& _customIdentifier, bool _custom, DataType _objectTypeRestriction) {
    
  if (  OpenFlipper::Options::nogui() )
    return;

  // Find selection environment that is associated to _handleName
  std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.find(_handleName);

  // Return if the requested selection environment was not found
  if(it == selectionEnvironments_.end()) return;

  if(!_custom) {
    if(_mode == SB_TOGGLE) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(toggleSelectionAction_);
        toggleSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(toggleSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          toggleSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_LASSO) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(lassoSelectionAction_);
        lassoSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(lassoSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          lassoSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_VOLUME_LASSO) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(volumeLassoSelectionAction_);
        volumeLassoSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(volumeLassoSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          volumeLassoSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_SURFACE_LASSO) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(surfaceLassoSelectionAction_);
        surfaceLassoSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(surfaceLassoSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          surfaceLassoSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_SPHERE) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(sphereSelectionAction_);
        sphereSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(sphereSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          sphereSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_BOUNDARY) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(boundarySelectionAction_);
        boundarySelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(boundarySelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          boundarySelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_FLOODFILL) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(floodFillSelectionAction_);
        floodFillSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(floodFillSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          floodFillSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    } else if (_mode == SB_COMPONENTS) {
      if(_show) {
        (*it).second.defaultSelectionModes.insert(componentsSelectionAction_);
        componentsSelectionAction_->addAssociatedType(_associatedTypes);
      } else {
        std::set<HandleAction*>::iterator e = (*it).second.defaultSelectionModes.find(componentsSelectionAction_);
        if(e != (*it).second.defaultSelectionModes.end()) {
          (*it).second.defaultSelectionModes.erase(e);
          componentsSelectionAction_->removeAssociatedType(_associatedTypes);
        }
      }
    }
  } else {
    if(_show) {
      // Create custom function
      // Create action for associated function
      _customIdentifier = getUniqueIdentifierName(QString(_handleName + "_" + _mode).replace(" ", "_"));

      // Create action
      HandleAction* action = new HandleAction(_icon, _desc, selectionModesGroup_, _objectTypeRestriction);
      action->setCheckable(true);
      action->selectionEnvironmentHandle(_handleName);
      action->selectionModeHandle(_customIdentifier);
      action->addAssociatedType(_associatedTypes);

      // Add action to tools bar
      selectionModesGroup_->addAction(action);
      pickModeToolBar_->clear();
      pickModeToolBar_->addActions(primitivesBarGroup_->actions());
      pickModeToolBar_->addSeparator();
      pickModeToolBar_->addActions(selectionModesGroup_->actions());

      // Add pickmode name and button to selection environment's container
      (*it).second.customSelectionModes.insert(action);

      connect(action, SIGNAL(triggered(bool)), this, SLOT(slotEnterSelectionMode(bool)));
    } else {
      // Search custom selection mode
      std::set<HandleAction*>::iterator e = (*it).second.customSelectionModes.begin();
      for(; e != (*it).second.customSelectionModes.end(); ++e) {
        if((*e)->selectionEnvironmentHandle() == _handleName)
          break;
      }

      // Delete action from list
      if(e != (*it).second.customSelectionModes.end()) {
        (*e)->removeAssociatedType(_associatedTypes);
        (*it).second.customSelectionModes.erase(e);
      }
    }
  }
}

//============================================================================================

void SelectionBasePlugin::slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QIcon _icon,
                                                     SelectionInterface::PrimitiveType _associatedTypes, QString& _customIdentifier) {
    
    showSelectionMode(_modeName, _icon, _description, _handleName, true, _associatedTypes, _customIdentifier, true);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QIcon _icon,
                                                     SelectionInterface::PrimitiveType _associatedTypes, QString& _customIdentifier,
                                                     DataType _objectTypeRestriction) {

    showSelectionMode(_modeName, _icon, _description, _handleName, true, _associatedTypes, _customIdentifier, true, _objectTypeRestriction);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowToggleSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + TOGGLE_IMG);
    QString dummy;
    showSelectionMode(SB_TOGGLE, icon, TOGGLE_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + LASSO_IMG);
    QString dummy;
    showSelectionMode(SB_LASSO, icon, LASSO_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowVolumeLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + VOLUME_LASSO_IMG);
    QString dummy;
    showSelectionMode(SB_VOLUME_LASSO, icon, VOLUME_LASSO_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowSurfaceLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + SURFACE_LASSO_IMG);
    QString dummy;
    showSelectionMode(SB_SURFACE_LASSO, icon, SURFACE_LASSO_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowSphereSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + SPHERE_IMG);
    QString dummy;
    showSelectionMode(SB_SPHERE, icon, SPHERE_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowClosestBoundarySelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + BOUNDARY_IMG);
    QString dummy;
    showSelectionMode(SB_BOUNDARY, icon, BOUNDARY_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowFloodFillSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + FLOODFILL_IMG);
    QString dummy;
    showSelectionMode(SB_FLOODFILL, icon, FLOODFILL_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotShowComponentsSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes) {

    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    QIcon icon(iconPath + COMPONENTS_IMG);
    QString dummy;
    showSelectionMode(SB_COMPONENTS, icon, COMPONENTS_DESC, _handleName, _show, _associatedTypes, dummy);
    updatePickModeToolBar();
}

//============================================================================================

void SelectionBasePlugin::slotGetActiveDataTypes(TypeList& _types) {
    
    if(currentPickMode_ == NO_SELECTION_PICKING) {
        _types = TypeList();
    } else {
        std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.find(currentPickMode_);
        if(it == selectionEnvironments_.end()) {
            _types = TypeList();
        } else {
            _types = (*it).second.types;
        }
    }
}

//============================================================================================

void SelectionBasePlugin::slotGetActivePrimitiveType(SelectionInterface::PrimitiveType& _type) {
    
    _type = currentPrimitiveType_;
}

//============================================================================================

void SelectionBasePlugin::slotMouseToggleSelection(QMouseEvent* _event) {
    
    // Only emit toggleSelection if left mouse button was clicked
    if (_event->type() == QEvent::MouseButtonPress) {
        
        if (_event->button() == Qt::RightButton) return;
        
        emit toggleSelection(_event, currentPrimitiveType_, deselection_);   
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseLassoSelection(QMouseEvent* _event) {
    
    // Ignore context menu for lasso selection
    if (_event->button() == Qt::RightButton) return;
   
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    int y = PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y();
    if (!PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point))
        hit_point = PluginFunctions::viewerProperties().glState().unproject(ACG::Vec3d(_event->pos().x(), y, 0.5));
    
    // Do rendering of mouse tool...
    if(_event->type() == QEvent::MouseButtonPress && _event->button() == Qt::LeftButton) {
        // If mouse button has been pressed
        
        if(!lassoSelection_) {
            // Initiate lasso selection
            linePoints_.clear();
            lassoSelection_ = true;
        }
        
        if(line_node_->hidden())
            line_node_->show();
        
        linePoints_.push_back(hit_point);
        
        // Inform selection plugins about the performed action
        emit lassoSelection(_event, currentPrimitiveType_, deselection_);
        
    } else if(_event->type() == QEvent::MouseMove) {

        if(!lassoSelection_) return;
        
        if (deselection_) {
          line_node_->set_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
          line_node_->set_base_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
        } else {
          line_node_->set_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));
          line_node_->set_base_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));
        }

        line_node_->clear();
        for (std::vector< ACG::Vec3d >::iterator it = linePoints_.begin(); it != linePoints_.end(); ++it)
            line_node_->add_point(*it);
        
        line_node_->add_point(hit_point);
        
        // Close lasso
        if( !linePoints_.empty() )
            line_node_->add_point(linePoints_[0]);
        
    } else if(_event->type() == QEvent::MouseButtonDblClick) {
        
        // Double click
        lassoSelection_ = false;
        linePoints_.clear();
        line_node_->clear();
        line_node_->hide();
        
        // Inform selection plugins about the performed action
        emit lassoSelection(_event, currentPrimitiveType_, deselection_);
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseVolumeLassoSelection(QMouseEvent* _event) {
    
    // Ignore context menu for volume lasso selection
    if (_event->button() == Qt::RightButton) return;
   
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    int y = PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y();
    if (!PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx, target_idx, &hit_point))
        hit_point = PluginFunctions::viewerProperties().glState().unproject(ACG::Vec3d(_event->pos().x(), y, 0.5));
    
    // Do rendering of mouse tool...
    if(_event->type() == QEvent::MouseButtonPress && _event->button() == Qt::LeftButton) {
        // If mouse button has been pressed
        
        if(!lassoSelection_) {
            // Initiate lasso selection
            linePoints_.clear();
            lassoSelection_ = true;
            line_node_->show();
        }
        
        if(line_node_->hidden())
            line_node_->show();
        
        linePoints_.push_back(hit_point);
        
        // Inform selection plugins about the performed action
        emit volumeLassoSelection(_event, currentPrimitiveType_, deselection_);
        
    } else if(_event->type() == QEvent::MouseMove) {

        if(!lassoSelection_) return;
        
        if (deselection_) {
          line_node_->set_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
          line_node_->set_base_color(ACG::Vec4f(1.0,0.0,0.0,1.0));
        } else {
          line_node_->set_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));
          line_node_->set_base_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));
        }
        
        line_node_->clear();
        for (std::vector< ACG::Vec3d >::iterator it = linePoints_.begin(); it != linePoints_.end(); ++it)
            line_node_->add_point(*it);
        
        line_node_->add_point(hit_point);
        
        // Close lasso
        if( !linePoints_.empty() )
            line_node_->add_point(linePoints_[0]);
        
    } else if(_event->type() == QEvent::MouseButtonDblClick) {
        
        // Double click
        lassoSelection_ = false;
        linePoints_.clear();
        line_node_->clear();
        line_node_->hide();
        
        // Inform selection plugins about the performed action
        emit volumeLassoSelection(_event, currentPrimitiveType_, deselection_);
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseSurfaceLassoSelection(QMouseEvent* _event) {
    
    /* NOT IMPLEMENTED YET */
}

//============================================================================================

void SelectionBasePlugin::slotMouseSphereSelection(QMouseEvent* _event) {
    
    // Ignore context menu for sphere selection
    if (_event->button() == Qt::RightButton) return;
    
    // Do rendering of mouse tool...
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // Hide sphere node in order to avoid self picking
    sphere_node_->hide();

    // Pick anything to find all possible objects
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
            _event->pos(), node_idx, target_idx, &hit_point)) {
        
        BaseObjectData* object = 0;
        
        if (PluginFunctions::getPickedObject(node_idx, object)) {
            
            // update brush sphere
            sphere_node_->show();
            sphere_node_->set_position(hit_point);
            sphere_node_->set_size(sphere_radius_);
            sphere_node_->enablePicking(false);
            
            if( (_event->buttons() & Qt::LeftButton) //left button
              ||( (_event->buttons() == Qt::NoButton) && (_event->type() == QEvent::MouseButtonRelease)) // or release of left button
              ) {
                // Inform selection plugins about the performed action
                emit sphereSelection(_event, sphere_radius_, currentPrimitiveType_, deselection_);
            }
            
        }
    } else {
        // Hide sphere node if no object was picked
        sphere_node_->hide();
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseBoundarySelection(QMouseEvent* _event) {
    
    // Only emit toggleSelection if left mouse button was clicked
    if (_event->type() == QEvent::MouseButtonPress) {
        
        if (_event->button() == Qt::RightButton) return;
        
        emit closestBoundarySelection(_event, currentPrimitiveType_, deselection_);
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseFloodFillSelection(QMouseEvent* _event) {
    
    // Only emit toggleSelection if left mouse button was clicked
    if (_event->type() == QEvent::MouseButtonPress) {
        
        if (_event->button() == Qt::RightButton) return;
        
        double maxAngle = 2*M_PI;
        
        if(!OpenFlipper::Options::nogui())
            maxAngle = tool_->maxFloodFillAngle->value();
        
        emit floodFillSelection(_event, maxAngle, currentPrimitiveType_, deselection_);
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseComponentsSelection(QMouseEvent* _event) {

    // Only emit componentsSelection if left mouse button was clicked
    if (_event->type() == QEvent::MouseButtonPress) {

        if (_event->button() == Qt::RightButton) return;

        emit componentsSelection(_event, currentPrimitiveType_, deselection_);
    }
}

//============================================================================================

void SelectionBasePlugin::slotMouseCustomSelection(QMouseEvent* _event) {
 
    emit customSelection(_event, currentPrimitiveType_, currentSelectionMode_, deselection_);
}

//============================================================================================

void SelectionBasePlugin::addedEmptyObject (int _id) {
    /*
    If an empty object has been added we will check if
    there exists a selection environment for the associated
    object type. If so and if this selection environment is
    not yet available, we will make it available from now on.
    */
    std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
    bool found = false;
    
    BaseObjectData* obj;
    PluginFunctions::getObject(_id, obj);
    if (obj) {
        DataType t = obj->dataType();
        
        // Iterate over all selection environments
        for(;it != selectionEnvironments_.end(); ++it) {
            
            // Iterate over all supported data types per selection environment
            for(std::vector<DataType>::iterator t_it = (*it).second.types.begin();
                t_it != (*it).second.types.end(); ++t_it) {
                
                if(t == (*t_it)) {
                    found = true;
                    break;
                }
            }
            
            if(found) break;
        }

        // Keep track of all data types in the scene
        availableObjectTypes_ |= obj->dataType();

    } else {
      BaseObject* bObject = 0;
      PluginFunctions::getObject(_id, bObject);

      // Groups are ok, others will cause an error
      if (!bObject->isGroup()) {
        emit log(LOGERR, "Could not retrieve object type! Maybe a selection environment will be missing.");
      }

      return;
    }
    
    if(found) {
      // We have found a selection environment for the
      // recently loaded data type -> show tab widget
      tool_->typeTabWidget->setTabEnabled(tool_->typeTabWidget->indexOf((*it).second.tabWidget), true);
      (*it).second.tabWidget->setEnabled(true);
    }
    
    // Increase the number of available objects for that type
    QMap<DataType, int>::iterator iterator = typeCounter_.find(obj->dataType());
    if ( iterator != typeCounter_.end() ) {
      typeCounter_[obj->dataType()] = typeCounter_[obj->dataType()] + 1;
    } else {
      typeCounter_[obj->dataType()] = 1;
    }

    updatePickModeToolBar();

    updateTabsOrder();
}

//============================================================================================

void SelectionBasePlugin::objectDeleted (int _id) {
    /*
    If an object was deleted, we will check if
    there exists a selection environment for the associated
    object type. If so and if this selection environment is
    available, we will remove it if there's no object
    of the given data type left in the scenegraph.
    */
    std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
    bool found = false;
    DataType t;
    
    std::vector<DataType> typeVec;
    
    BaseObjectData* obj;
    PluginFunctions::getObject(_id, obj);
    if (obj) {
        t = obj->dataType();
        
        // Iterate over all selection environments
        for(;it != selectionEnvironments_.end(); ++it) {
            
            // Iterate over all supported data types of a selection environment
            for(std::vector<DataType>::iterator t_it = (*it).second.types.begin();
                t_it != (*it).second.types.end(); ++t_it) {
                
                if(t == (*t_it)) {
                    found = true;
                    break;
                }
            }
            
            if(found) break;
        }

        availableObjectTypes_ = (availableObjectTypes_ & ~obj->dataType().value());

    } else {
        emit log(LOGERR, "Could not retrieve object type!");
        return;
    }
    
    if(found) {
        // If we have found a selection environment for the
        // recently loaded data type AND if there's no
        // other object with the same data type in the
        // scene graph -> hide button
                
        // Search for other objects for *all* supported data types
        bool atLeastOne = false;
        for(std::vector<DataType>::iterator t_it = (*it).second.types.begin();
                t_it != (*it).second.types.end(); ++t_it) {
            
            if(typeExists(*t_it, _id)) {
                atLeastOne = true;
                break;
            }
        }
        // Show tab widget if at least one object of supported data type was found
        // Hide it otherwise
        tool_->typeTabWidget->setTabEnabled(tool_->typeTabWidget->indexOf((*it).second.tabWidget), atLeastOne);
        (*it).second.tabWidget->setEnabled(atLeastOne);
    }
    
    // Decrease the number of available objects for that type
    // We track that here because otherwise we had to iterate over all objects
    // which would cause a linear runtime which is bad for high object counts.
    QMap<DataType, int>::iterator iterator = typeCounter_.find(obj->dataType());
    if ( iterator != typeCounter_.end() ) {
      typeCounter_[obj->dataType()] = typeCounter_[obj->dataType()] - 1;
      if ( typeCounter_[obj->dataType()] < 0 ) {
        std::cerr << "====== ERROR =======" << std::endl;
        std::cerr << "Negative counter for type  " << obj->dataType().name().toStdString() << std::endl;
      }
    } else {
      std::cerr << "Error: No counter for type  " << obj->dataType().name().toStdString() << std::endl;
    }

    updatePickModeToolBar();

    updateTabsOrder();
}

//============================================================================================

void SelectionBasePlugin::updateTabsOrder() {

    std::map<int, int> newMappings;

    int firstFree = 0;
    for(int i = 0; i < tool_->typeTabWidget->count(); ++i) {
        if(tool_->typeTabWidget->isTabEnabled(i)) {
            tool_->typeTabWidget->insertTab(firstFree, tool_->typeTabWidget->widget(i), tool_->typeTabWidget->tabText(i));
            newMappings.insert(std::pair<int,int>(i,firstFree));
            firstFree++;
        } else {
            // Tab remains in old order
            newMappings.insert(std::pair<int,int>(i,i));
        }
    }

    // Choose first active tab
    if(tool_->typeTabWidget->count() > 0) tool_->typeTabWidget->setCurrentIndex(0);
}

//============================================================================================

void SelectionBasePlugin::slotTargetObjectsOnly(bool& _targetsOnly) {
    
    if(OpenFlipper::Options::nogui() || tool_ == 0) _targetsOnly = true;
    
    _targetsOnly = tool_->restrictOnTargets->isChecked();
}

//============================================================================================

QString SelectionBasePlugin::getUniqueIdentifierName(QString _name, int _num) {
    /*
    This makes sure that we always have unique pickmode names
    in order to avoid double mappings.
    
    So we iterate over all selection modes and look if
    _name already exists as pickmode name. If the name exists,
    we append a number at the end of the string.
    */
    QString needle = _name;;
    
    if(_num != 0) {
        needle.append(QString::number(_num));
    }
    
    // Iterate over all selection environments
    for(std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
        it != selectionEnvironments_.end(); ++it) {
        
        // Iterate over all selection modes for this selection environment
        // This also includes the standard selection modes (toggle, sphere, etc.)
        for(std::set<HandleAction*>::iterator dsm_it = (*it).second.defaultSelectionModes.begin();
            dsm_it != (*it).second.defaultSelectionModes.end(); ++dsm_it) {
            
            if((*dsm_it)->selectionModeHandle() == needle) {
                // Recursive call with increased number
                return getUniqueIdentifierName(_name, _num + 1);
            }
        }
    }
    
    return needle;
}

//============================================================================================

QString SelectionBasePlugin::getUniqueHandleName(QString _name, int _num) {
    /*
    This makes sure that we always have unique handle names
    in order to avoid double mappings.
    
    So we iterate over all selection environments and look if
    _name already exists as handle name. If the name exists,
    we append a number at the end of the string.
    */
    QString needle = _name;;
    
    if(_num != 0) {
        needle.append(QString::number(_num));
    }
    
    // Iterate over all selection environments
    for(std::map<QString,SelectionEnvironment>::iterator it = selectionEnvironments_.begin();
        it != selectionEnvironments_.end(); ++it) {
        
        if((*it).first == needle) {
            // Recursive call with increased number
            return getUniqueHandleName(_name, _num + 1);
        }
    }
    
    return needle;
}

//============================================================================================

bool SelectionBasePlugin::typeExists(DataType _type, int _excludeId) {

  // Check the number of available objects for that type
  QMap<DataType, int>::iterator iterator = typeCounter_.find(_type);
  if ( iterator != typeCounter_.end() ) {

    // We count objects of a specific type and decrease the number by one
    // which basically happens, if we are about to remove an object.
    if ( _excludeId != -1 ) {

      // Sanity check. If the object is of a different type, something went wrong.
      BaseObject* object;
      PluginFunctions::getObject(_excludeId,object);

      if ( object == 0 ) {
        std::cerr << "Unable to get Object for type exists" << std::endl;
      } else {
        // mismatching exclude type
        if (_type !=  object->dataType()) {
          // return as if not excluded
          if ( typeCounter_[_type] > 0) {
            return true; // At least one object of this type exists
          } else if ( typeCounter_[_type] == 0 ){
            return false; // No object of this type exists
          } else {
            std::cerr << "Type exists Error after mismatch exclude:  " << _type.name().toStdString() <<  " negative count" << std::endl;
            return false;
          }
        }
      }

      // Check the counter of the type
      if ( typeCounter_[_type] - 1 > 0) {
        return true; // At least one object of this type exists if we delete the excluded one
      } else if ( typeCounter_[_type] - 1 == 0 ){
        return false;  // No object of this type exists if we delete the excluded one
      } else {
        std::cerr << "Type exists Error  " << _type.name().toStdString() <<  " negative count" << std::endl;
        return false;
      }
    } else {

      // If we don't exclude items, we directly take the number we found.
      if ( typeCounter_[_type] > 0) {
        return true; // At least one object of this type exists
      } else if ( typeCounter_[_type] == 0 ){
        return false; // No object of this type exists
      } else {
        std::cerr << "Type exists Error  " << _type.name().toStdString() <<  " negative count" << std::endl;
        return false;
      }
    }

  } else {
    // No counter available, which basically means it is equal to zero
    return false;
  }

  // This should never be reached!
  return false;
}

//============================================================================================

void SelectionBasePlugin::slotRegisterKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers) {
    
    std::set<std::pair<int,Qt::KeyboardModifiers> >::iterator f =
        registeredKeys_.find(std::pair<int,Qt::KeyboardModifiers>(_key,_modifiers));
    
    if(f == registeredKeys_.end()) {
        // Register key shortcut
        emit registerKey(_key, _modifiers, QString("Selection base key %1").arg(_key), true);
        registeredKeys_.insert(std::pair<int,Qt::KeyboardModifiers>(_key,_modifiers));
    }
}

//============================================================================================

Q_EXPORT_PLUGIN2(selectionbaseplugin, SelectionBasePlugin);

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
 *   $Revision$                                                       *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#ifndef SELECTIONBASEPLUGIN_HH
#define SELECTIONBASEPLUGIN_HH

#include <QObject>
#include <QGroupBox>
#include <QMenuBar>

#include <set>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/SelectionInterface.hh>

#include <ACG/Scenegraph/GlutPrimitiveNode.hh>
#include <ACG/Scenegraph/LineNode.hh>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "widgets/SelectionBaseToolbox.hh"
#include "widgets/SelectionTypeFrame.hh"

#include <OpenFlipper/INIFile/INIFile.hh>

#include "HelperClasses.hh"

class SelectionBasePlugin : public QObject, BaseInterface , MouseInterface, KeyInterface, PickingInterface, ToolboxInterface, INIInterface,
    LoggingInterface, ScriptInterface, ContextMenuInterface, ToolbarInterface, LoadSaveInterface, SelectionInterface {
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(MouseInterface)
Q_INTERFACES(KeyInterface)
Q_INTERFACES(PickingInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(INIInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(ContextMenuInterface)
Q_INTERFACES(ToolbarInterface)
Q_INTERFACES(LoadSaveInterface)
Q_INTERFACES(SelectionInterface)

    public:
        
        /// Default constructor
        SelectionBasePlugin();
     
        // BaseInterface
        QString name() { return (QString(tr("SelectionBase"))); };
        QString description( ) { return (QString(tr("Provides basic selection functions"))); };

    signals:
        
        // BaseInterface
        void updateView();

        // LoggingInterface
        void log(Logtype _type, QString _message);
        void log(QString _message);
        
        // ToolbarInterface
        void addToolbar(QToolBar* _toolbar);
        
        // SelectionInterface
        void toggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void lassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void volumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void surfaceLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void sphereSelection(QMouseEvent* _event, double _radius, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void closestBoundarySelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void floodFillSelection(QMouseEvent* _event, double _maxAngle, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void componentsSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect);
        void customSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, QString _customIdentifier, bool _deselect);
        
        void selectionOperation(QString _operation);
        void loadSelection(const INIFile& _file);
        void saveSelection(INIFile& _file);
        
        void keyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier);
        
        // PickingInterface
        void addPickMode(const std::string& _mode);
        void setPickModeMouseTracking(const std::string& _mode, bool _mouseTracking);
        void setPickModeCursor(const std::string& _mode, QCursor _cursor);
        void setPickModeToolbar (const std::string& _mode, QToolBar * _toolbar);
        
        // KeyInterface
        void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);
        
        // ToolboxInterface
        void addToolbox(QString _name, QWidget* _widget, QIcon* _icon); 
      
    private slots:
        
        // BaseInterface
        void initializePlugin();
        void pluginsInitialized();
        
        //MouseInterface
        void slotMouseWheelEvent(QWheelEvent* _event, const std::string& _mode);
        void slotMouseEvent(QMouseEvent* _event);
        
        // SelectionInterface
        void slotAddSelectionEnvironment(QString _modeName, QString _description, QIcon _icon, QString& _handleName);
        void slotRegisterType(QString _handleName, DataType _type);
        void slotAddPrimitiveType(QString _handleName, QString _name, QIcon _icon, SelectionInterface::PrimitiveType& _typeHandle);
        void slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QIcon _icon,
                                        SelectionInterface::PrimitiveType _associatedTypes, QString& _customIdentifier);
        
        void slotAddSelectionOperations(QString _handleName, QStringList _operationsList, QString _category, SelectionInterface::PrimitiveType _type = 0u);
        
        void slotShowToggleSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowVolumeLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowSurfaceLassoSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowSphereSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowClosestBoundarySelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowFloodFillSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        void slotShowComponentsSelectionMode(QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes);
        
        void slotGetActiveDataTypes(SelectionInterface::TypeList& _types);
        void slotGetActivePrimitiveType(SelectionInterface::PrimitiveType& _type);
        
        void slotTargetObjectsOnly(bool& _targetsOnly);
        void slotRegisterKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers);
        
        // LoadSaveInterface
        void addedEmptyObject (int _id);
        void objectDeleted (int _id);
        
        // PickingInterface
        void slotPickModeChanged (const std::string& _pickmode);
        
        // KeyInterface
        void slotKeyEvent(QKeyEvent* _event);
        void slotKeyReleaseEvent(QKeyEvent* _event);
        
        // ============== Selection Actions ===============
        
        void slotMouseToggleSelection(QMouseEvent* _event);
        void slotMouseLassoSelection(QMouseEvent* _event);
        void slotMouseVolumeLassoSelection(QMouseEvent* _event);
        void slotMouseSurfaceLassoSelection(QMouseEvent* _event);
        void slotMouseSphereSelection(QMouseEvent* _event);
        void slotMouseBoundarySelection(QMouseEvent* _event);
        void slotMouseFloodFillSelection(QMouseEvent* _event);
        void slotMouseComponentsSelection(QMouseEvent* _event);
        void slotMouseCustomSelection(QMouseEvent* _event);
        
        void slotLoadSelectionButton();
        void slotSaveSelectionButton();
        
        void slotOperationRequested();
        void slotShowAndHideOperations();
        
        // ==================== GUI =======================
        
        void slotSelectionEnvironmentRequested(bool _checked);
        
        void slotEnterSelectionMode(bool _checked);
        
        void updateActivePrimitiveTypes(bool _checked);
        
    public slots:
        QString version() { return QString("1.0"); };
        
    private:
        
        struct SelectionEnvironment {
            // Name of the environment
            QString name;
            // List of supported types
            std::vector<DataType> types;
            // List of all default selection modes
            std::set<HandleAction*> defaultSelectionModes;
            // List of all default selection modes
            std::set<HandleAction*> customSelectionModes;
            // Selection environments handle
            QString handle;
            // Handle to tab widget's primitive's bar
            QHBoxLayout* primitivesBar;
            // Handle to tab widget's operations widget
            QVBoxLayout* operationsBar;
            // Keep track of operation categories
            std::map<QString,std::pair<FillingLayout*,QGroupBox*> > categories;
            // Action group for primitives
            QActionGroup* primitiveActions;
            // Store the supported primitive types (bitmask)
            unsigned int primitiveTypes;
            // Map that stores for each primitive type a set of
            // operations that are supported on it
            std::multimap<PrimitiveType, QPushButton*> operations;
            // Keep name of tab for selection environment
            QWidget* tabWidget;
        };
        
        /// Get a unique pickmode name
        QString getUniqueIdentifierName(QString _name, int _num = 0);
        
        /// Get a unique handle name
        QString getUniqueHandleName(QString _name, int _num = 0);
        
        /// Test if at least one object of type _type is in the scene graph
        bool typeExists(DataType _type, int _excludeId = -1);
        
        /// Show standard selection mode
        /// Use this method to show/hide standard selection actions (lasso, floodfill, sphere, etc.)
        void showSelectionMode(QString _mode, QIcon _icon, QString _desc,
                               QString _handleName, bool _show, SelectionInterface::PrimitiveType _associatedTypes,
                               QString& _customIdentifier, bool _custom = false);
         
        /// Create new type frame for tabs widget
        SelectionTypeFrameWidget* createNewTypeFrame(SelectionEnvironment& _env);        
        
        /// Update the pickmode toolbar
        void updatePickModeToolBar();
        
        // Update the tabs widgets' order
        void updateTabsOrder();

        // Map of registered selection environments (handle, environment)
        std::map<QString,SelectionEnvironment> selectionEnvironments_;
        
        //================== GUI ======================
        
        // List of primitive type buttons (primitivetype, button)
        std::map<PrimitiveType,QAction*> primitiveTypeButtons_;
        
        // Toolbar in OpenFlipper that contains the selection button
        QToolBar* toolBar_;
        
        // Widget for Toolbox
        SelectionBaseToolboxWidget* tool_;
        
        // Toolbar that displays ALL primitive types of all selection environments
        QActionGroup* primitivesBarGroup_;
        QToolBar* pickModeToolBar_;
        
        QActionGroup* selectionModesGroup_;

        // Actions for default selection modes
        HandleAction* toggleSelectionAction_;
        HandleAction* lassoSelectionAction_;
        HandleAction* volumeLassoSelectionAction_;
        HandleAction* surfaceLassoSelectionAction_;
        HandleAction* sphereSelectionAction_;
        HandleAction* boundarySelectionAction_;
        HandleAction* floodFillSelectionAction_;
        HandleAction* componentsSelectionAction_;
        
        std::set<std::pair<int,Qt::KeyboardModifiers> > registeredKeys_;
        
        //================== Status ===================
        
        // Keep the current pick mode
        QString currentPickMode_;
        
        // Keep the last used pick mode
        QString lastPickMode_;
        
        // Keep the current selection mode
        QString currentSelectionMode_;
        
        // Bitmask for currently selected primitive types
        SelectionInterface::PrimitiveType currentPrimitiveType_;
        
        // Next free primitive handle
        unsigned int nextFreePrimitiveType_;
        
        // Indicates whether we want to deselect primitives
        bool deselection_;
        
        //=============== Rendering ====================
        
        // Sphere Nodes material
        ACG::SceneGraph::MaterialNode* sphere_mat_node_;

        // Sphere Node
        ACG::SceneGraph::GlutPrimitiveNode* sphere_node_;
        
        // Sphere radius
        double sphere_radius_;
        
        // Line Node
        ACG::SceneGraph::LineNode* line_node_;
        
        // Indicates if lasso selection is currently active
        bool lassoSelection_;
        
        // List of points for line node
        std::vector<ACG::Vec3d> linePoints_;

        QIcon* toolIcon_;
};

#endif // SELECTIONBASEPLUGIN_HH

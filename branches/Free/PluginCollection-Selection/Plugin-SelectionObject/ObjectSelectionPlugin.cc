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

#include "ObjectSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Primitive type icons
#define OBJECT_TYPE         "selection_object.png"
// =======================================
// Define operations
// =======================================
// Vertices:
#define O_SELECT_ALL      "Select All Objects"
#define O_DESELECT_ALL    "Deselect All Objects"
#define O_INVERT          "Invert Object Selection"
#define O_DELETE          "Delete Selected Objects"

/// Default constructor
ObjectSelectionPlugin::ObjectSelectionPlugin() :
objectType_(0) {
}

//==============================================================================================
      
ObjectSelectionPlugin::~ObjectSelectionPlugin() {
}

//==============================================================================================

void ObjectSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
}

//==============================================================================================

void ObjectSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for polylines
    // and register polyline data type for the environment.
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    
    emit addSelectionEnvironment("Object Selections", "Select objects.",
                                 iconPath + OBJECT_TYPE, environmentHandle_);

    // Register mesh object types
    emit registerType(environmentHandle_, DATA_ALL);
    
    emit addPrimitiveType(environmentHandle_, "Select Objects", iconPath + OBJECT_TYPE, objectType_);
    
    // Determine, which selection modes are requested
    emit showToggleSelectionMode(environmentHandle_, true, objectType_);
    emit showVolumeLassoSelectionMode(environmentHandle_, true, objectType_);
    
    // Define object operations
    QStringList objectOperations;
    objectOperations.append(O_SELECT_ALL);
    objectOperations.append(O_DESELECT_ALL);
    objectOperations.append(O_INVERT);
    objectOperations.append(O_DELETE);
    
    emit addSelectionOperations(environmentHandle_, objectOperations,  "Object Operations",   objectType_);
    
    // Register key shortcuts:
    
    // Select (a)ll
    emit registerKeyShortcut(Qt::Key_A, Qt::ControlModifier);
    // (C)lear selection
    emit registerKeyShortcut(Qt::Key_C,      Qt::NoModifier);
    // (I)nvert selection
    emit registerKeyShortcut(Qt::Key_I,      Qt::NoModifier);
    // Delete selected entities
    emit registerKeyShortcut(Qt::Key_Delete, Qt::NoModifier);
}

//==============================================================================================

void ObjectSelectionPlugin::updateSlotDescriptions() {
    
    emit setSlotDescription("selectAllObjects(int)", tr("Select all objects"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllObjects(int)", tr("Deselect all objects"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertObjectSelection(int)", tr("Invert object selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedObjects(int)", tr("Delete selected objects"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectObjects(int,IdList)", tr("Select the specified objects"),
                            QString("objectId,objectList").split(","), QString("Id of object,List of objects").split(","));

    emit setSlotDescription("loadSelection(int,QString)", tr("Load selection from selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Selection file").split(","));
}

//==============================================================================================

void ObjectSelectionPlugin::slotSelectionOperation(QString _operation) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & objectType_) == 0)
        return;
            
    if(_operation == O_SELECT_ALL) {
        // Select all objects
        selectAllObjects();
    } else if (_operation == O_DESELECT_ALL) {
        // Deselect all objects
        deselectAllObjects();
    } else if (_operation == O_INVERT) {
        // Invert object selection
        invertObjectSelection();
    } else if (_operation == O_DELETE) {
        // Delete selected objects
        deleteSelectedObjects();
    }
}

//==============================================================================================

void ObjectSelectionPlugin::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & objectType_) == 0) return;

    // Return if mouse event is not a left-button click
    if(_event->button() != Qt::LeftButton) return;
     
    unsigned int node_idx = 0;
    unsigned int target_idx = 0;
    ACG::Vec3d hit_point;
    
    BaseObjectData* object = 0;

    bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx,
                              target_idx, &hit_point) && PluginFunctions::getPickedObject(node_idx, object);

    if(successfullyPicked) {

        if(object->target() || _deselect)
            object->target(false);
        else
            object->target(true);

        emit updatedObject(object->id(), UPDATE_SELECTION);
    }
}

//==============================================================================================

void ObjectSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & objectType_) == 0) return;
    
    ACG::Vec3d hit_point;
    
    if(_event->type() == QEvent::MouseButtonPress) {
        
        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        QPoint p(_event->pos().x(), state.viewport_height() - _event->pos().y());
        
        volumeLassoPoints_.append(p);
        
        return;

    } else if(_event->type() == QEvent::MouseButtonDblClick) {

        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        
        QPolygon polygon(volumeLassoPoints_);
        
        // Select all vertices that lie in this region
        for(PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            
            if (o_it->visible()) {

                // Look if COG of object lies within polygon
                ACG::Vec3d bbmin(0.0);
                ACG::Vec3d bbmax(0.0);
                o_it->getBoundingBox(bbmin, bbmax);
                ACG::Vec3d c = (bbmin + bbmax)/2.0;

                ACG::Vec3d pc = state.project(c);
                QPoint p((int)pc[0], (int)pc[1]);

                if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                    o_it->target(!_deselect);
                }
            }
        
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }

        
        // Clear lasso points
        volumeLassoPoints_.clear();
    }
}

//==============================================================================================

void ObjectSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

    // Load ini file
    INIFile file;

    if(!file.connect(_filename, false)) {
        emit log(LOGERR, QString("Could not read file '%1'!").arg(_filename));
        return;
    }

    // Load selection from file
    loadIniFile(file, _objId);
}

//==============================================================================================

void ObjectSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Load plugin specific settings
}

//==============================================================================================

void ObjectSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Save plugin specific settings
}

//==============================================================================================

void ObjectSelectionPlugin::slotLoadSelection(const INIFile& _file) {
    
    // Iterate over all polyline objects in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Read section for each object
        // Append object name to section identifier
        QString section = QString("ObjectSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }
        
        std::vector<int> ids;
        // Load vertex selection:
        _file.get_entry(ids, section, "Target");
        selectObjects(ids);
        ids.clear();
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
    }
}

//==============================================================================================

void ObjectSelectionPlugin::slotSaveSelection(INIFile& _file) {
    
    // Iterate over all polyline objects in the scene and save
    // the selections for all vertices
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Create section for each object
        // Append object name to section identifier
        QString section = QString("ObjectSelection") + "//" + o_it->name();
        
        // Store vertex selection:
        _file.add_entry(section, "Target", getObjectSelection());
    }
}

//==============================================================================================

void ObjectSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & objectType_) == 0) {
        // No supported type is active
        return;
    }

    if(_key == Qt::Key_A && _modifiers == Qt::ControlModifier) {
        // Select all objects
        selectAllObjects();
    } else if(_key == Qt::Key_C && _modifiers == Qt::NoModifier) {
        // Deselect all vertices
        deselectAllObjects();
    } else if(_key == Qt::Key_I && _modifiers == Qt::NoModifier) {
        // Invert object selection
        invertObjectSelection();
    } else if(_key == Qt::Key_Delete && _modifiers == Qt::NoModifier) {
        // Delete selected objects
        deleteSelectedObjects();
   }
}

//==============================================================================================

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(objectselectionplugin, ObjectSelectionPlugin);
#endif



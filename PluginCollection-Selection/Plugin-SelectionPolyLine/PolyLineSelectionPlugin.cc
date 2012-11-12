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

#include "PolyLineSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Primitive type icons
#define VERTEX_TYPE         "polyline_vertex.png"
#define EDGE_TYPE           "polyline_edge.png"
// =======================================
// Define operations
// =======================================
// Vertices:
#define V_SELECT_ALL      "Select All Vertices"
#define V_DESELECT_ALL    "Deselect All Vertices"
#define V_INVERT          "Invert Vertex Selection"
#define V_DELETE          "Delete Selected Vertices"
// Edges:
#define E_SELECT_ALL      "Select All Edges"
#define E_DESELECT_ALL    "Deselect All Edges"
#define E_INVERT          "Invert Edge Selection"
#define E_DELETE          "Delete Selected Edges"

/// Default constructor
PolyLineSelectionPlugin::PolyLineSelectionPlugin() :
vertexType_(0),
edgeType_(0),
allSupportedTypes_(0) {
}

//==============================================================================================
      
PolyLineSelectionPlugin::~PolyLineSelectionPlugin() {
}

//==============================================================================================

void PolyLineSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
}

//==============================================================================================

void PolyLineSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for polylines
    // and register polyline data type for the environment.
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    
    emit addSelectionEnvironment("PolyLine Selections", "Select polyline primitives.",
                                 iconPath + "polyline_type.png", environmentHandle_);

    // Register mesh object types
    emit registerType(environmentHandle_, DATA_POLY_LINE);
    
    emit addPrimitiveType(environmentHandle_, "Select Polyline Vertices", iconPath + VERTEX_TYPE, vertexType_);
    emit addPrimitiveType(environmentHandle_, "Select Polyline Edges", iconPath + EDGE_TYPE, edgeType_);
    
    allSupportedTypes_ = vertexType_ | edgeType_;
    
    // Determine, which selection modes are requested
    emit showToggleSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showVolumeLassoSelectionMode(environmentHandle_, true, allSupportedTypes_);
    
    // Define vertex operations
    QStringList vertexOperations;
    vertexOperations.append(V_SELECT_ALL);
    vertexOperations.append(V_DESELECT_ALL);
    vertexOperations.append(V_INVERT);
    vertexOperations.append(V_DELETE);
    
    QStringList edgeOperations;
    edgeOperations.append(E_SELECT_ALL);
    edgeOperations.append(E_DESELECT_ALL);
    edgeOperations.append(E_INVERT);
    edgeOperations.append(E_DELETE);
    
    emit addSelectionOperations(environmentHandle_, vertexOperations,  "Vertex Operations",   vertexType_);
    emit addSelectionOperations(environmentHandle_, edgeOperations,  "Edge Operations",   edgeType_);
    
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

void PolyLineSelectionPlugin::updateSlotDescriptions() {
    
    emit setSlotDescription("selectAllVertices(int)", tr("Select all vertices of a polyline"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllVertices(int)", tr("Deselect all vertices of a polyline"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertVertexSelection(int)", tr("Invert vertex selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedVertices(int)", tr("Delete selected vertices"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectVertices(int,IdList)", tr("Select the specified vertices"),
                            QString("objectId,vertexList").split(","), QString("Id of object,List of vertices").split(","));
                            
    emit setSlotDescription("selectAllEdges(int)", tr("Select all edges of a polyline"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllEdges(int)", tr("Deselect all edges of a polyline"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertEdgeSelection(int)", tr("Invert edge selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedEdges(int)", tr("Delete selected edges"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectEdges(int,IdList)", tr("Select the specified edges"),
                            QString("objectId,edgeList").split(","), QString("Id of object,List of edges").split(","));

    emit setSlotDescription("loadSelection(int,QString)", tr("Load selection from selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Selection file").split(","));
}

//==============================================================================================

void PolyLineSelectionPlugin::slotSelectionOperation(QString _operation) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & allSupportedTypes_) == 0)
        return;
    
    // Test if operation should be applied to target objects only
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);
            
    if(_operation == V_SELECT_ALL) {
        // Select all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllVertices(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == V_DESELECT_ALL) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllVertices(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == V_INVERT) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertVertexSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == V_DELETE) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deleteSelectedVertices(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if(_operation == E_SELECT_ALL) {
        // Select all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllEdges(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == E_DESELECT_ALL) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllEdges(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == E_INVERT) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertEdgeSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if (_operation == E_DELETE) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deleteSelectedEdges(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    }
}

//==============================================================================================

void PolyLineSelectionPlugin::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;

    // Return if mouse event is not a left-button click
    if(_event->button() != Qt::LeftButton) return;
     
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    
    BaseObjectData* object = 0;

    if(_currentType & vertexType_) {
        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx,
                                  target_idx, &hit_point) && PluginFunctions::getPickedObject(node_idx, object);
     
        if(successfullyPicked) {
            PolyLine* polyline = PluginFunctions::polyLine(object);

            if(!polyline)
                return;

            if(polyline->vertex_selected(target_idx) || _deselect)
                polyline->deselect_vertex(target_idx);
            else
                polyline->select_vertex(target_idx);
            
            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }
    
    if(_currentType & edgeType_) {
        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_EDGE, _event->pos(), node_idx,
                                  target_idx, &hit_point) && PluginFunctions::getPickedObject(node_idx, object);
     
        if(successfullyPicked) {
            PolyLine* polyline = PluginFunctions::polyLine(object);

            if(!polyline)
                return;

            if(polyline->edge_selected(target_idx) || _deselect)
                polyline->deselect_edge(target_idx);
            else
                polyline->select_edge(target_idx);
            
            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }
}

//==============================================================================================

void PolyLineSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    
    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    ACG::Vec3d hit_point;
    
    if(_event->type() == QEvent::MouseButtonPress) {
        
        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        QPoint p(_event->pos().x(), state.viewport_height() - _event->pos().y());
        
        volumeLassoPoints_.append(p);
        
        return;

    } else if(_event->type() == QEvent::MouseButtonDblClick) {

        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        
        QPolygon polygon(volumeLassoPoints_);
        
        bool targetsOnly = false;
        emit targetObjectsOnly(targetsOnly);
        PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);
        
        // Select all vertices that lie in this region
        for(PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            
            if (o_it->visible()) {
                
                PolyLineObject* plo = PluginFunctions::polyLineObject(o_it);
                
                if(plo) {
                    
                    PolyLine* polyline = plo->line();
                    
                    if(polyline) {
                        
                        if(_currentType & vertexType_) {
                            
                            for(unsigned int i = 0; i < polyline->n_vertices(); ++i) {
                                ACG::Vec3d pp = state.project(polyline->point(i));
                                QPoint p((int)pp[0], (int)pp[1]);
                                
                                if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                    if(_deselect)
                                        polyline->deselect_vertex(i);
                                    else
                                        polyline->select_vertex(i);
                                }
                            }
                        }
                        
                        if(_currentType & edgeType_) {
                            
                            for(unsigned int i = 0; i < polyline->n_vertices(); ++i) {
                                
                                ACG::Vec3d edgeMidPoint = polyline->point(i) + 0.5*polyline->edge_vector(i);
                                
                                ACG::Vec3d pp = state.project(edgeMidPoint);
                                QPoint p((int)pp[0], (int)pp[1]);
                                
                                if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                    if(_deselect)
                                        polyline->deselect_edge(i);
                                    else
                                        polyline->select_edge(i);
                                }
                            }
                        }
                    }
                }
            }
        
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }

        
        // Clear lasso points
        volumeLassoPoints_.clear();
    }
}

//==============================================================================================

void PolyLineSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

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

void PolyLineSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Load plugin specific settings
}

//==============================================================================================

void PolyLineSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Save plugin specific settings
}

//==============================================================================================

void PolyLineSelectionPlugin::slotLoadSelection(const INIFile& _file) {
    
    // Iterate over all polyline objects in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_POLY_LINE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Read section for each object
        // Append object name to section identifier
        QString section = QString("PolyLineSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }
        
        std::vector<int> ids;
        // Load vertex selection:
        _file.get_entry(ids, section, "VertexSelection");
        selectVertices(o_it->id(), ids);
        ids.clear();
        // Load edge selection:
        _file.get_entry(ids, section, "EdgeSelection");
        selectEdges(o_it->id(), ids);
        ids.clear();
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
    }
}

//==============================================================================================

void PolyLineSelectionPlugin::slotSaveSelection(INIFile& _file) {
    
    // Iterate over all polyline objects in the scene and save
    // the selections for all vertices
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_POLY_LINE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Create section for each object
        // Append object name to section identifier
        QString section = QString("PolyLineSelection") + "//" + o_it->name();
        
        // Store vertex selection:
        _file.add_entry(section, "VertexSelection", getVertexSelection(o_it->id()));
        _file.add_entry(section, "EdgeSelection", getEdgeSelection(o_it->id()));
    }
}

//==============================================================================================

void PolyLineSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & allSupportedTypes_) == 0) {
        // No supported type is active
        return;
    }
    
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);
    
    if(_key == Qt::Key_A && _modifiers == Qt::ControlModifier) {
        // Select all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    selectAllVertices(o_it->id());
                if(type & edgeType_)
                    selectAllEdges(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if(_key == Qt::Key_C && _modifiers == Qt::NoModifier) {
        // Deselect all vertices
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    deselectAllVertices(o_it->id());
                if(type & edgeType_)
                    deselectAllEdges(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if(_key == Qt::Key_I && _modifiers == Qt::NoModifier) {
        // Invert vertex selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    invertVertexSelection(o_it->id());
                if(type & edgeType_)
                    invertEdgeSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
        }
    } else if(_key == Qt::Key_Delete && _modifiers == Qt::NoModifier) {
        // Delete selected vertices and its children
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLY_LINE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    deleteSelectedVertices(o_it->id());
                if(type & edgeType_)
                    deleteSelectedEdges(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_ALL);
        }
   }
}

//==============================================================================================

Q_EXPORT_PLUGIN2(polylineselectionplugin, PolyLineSelectionPlugin);

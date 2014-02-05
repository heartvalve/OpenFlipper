/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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

#include "BSplineCurveSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Primitive type icons
#define CONTROL_POINT_TYPE  "controlpointselection.png"
#define KNOT_TYPE           "knotselection.png"
// =======================================
// Define operations
// =======================================
// General Operations:
#define G_CP_MODE          "Control Point Selection View"
#define G_K_MODE           "Knot Selection View"

// Control points:
#define CP_SELECT_ALL      "Select All Control Points"
#define CP_DESELECT_ALL    "Deselect All Control Points"
#define CP_INVERT          "Invert Control Point Selection"
#define CP_DELETE          "Delete Selected Control Points"

// Knots:
#define K_SELECT_ALL      "Select All Knots"
#define K_DESELECT_ALL    "Deselect All Knots"
#define K_INVERT          "Invert Knot Selection"
#define K_DELETE          "Delete Selected Knots"

/// Default constructor
BSplineCurveSelectionPlugin::BSplineCurveSelectionPlugin() :
controlPointType_(0),
knotType_(0),
allSupportedTypes_(0) {
}

//==============================================================================================
      
BSplineCurveSelectionPlugin::~BSplineCurveSelectionPlugin() {
}

//==============================================================================================

void BSplineCurveSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
}

//==============================================================================================

void BSplineCurveSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for skeletons
    // and register skeleton data type for the environment.

    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

    emit addSelectionEnvironment("B-Spline Curve Selections", "Select B-Spline curve primitives.",
                                 iconPath + "bsplinecurve.png", environmentHandle_);

    // Register mesh object types
    emit registerType(environmentHandle_, DATA_BSPLINE_CURVE);

    emit addPrimitiveType(environmentHandle_, "Select Control Points", iconPath + CONTROL_POINT_TYPE, controlPointType_);
    emit addPrimitiveType(environmentHandle_, "Select Knots", iconPath + KNOT_TYPE, knotType_);

    allSupportedTypes_ = controlPointType_ | knotType_;

    // Determine, which selection modes are requested
    emit showToggleSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showVolumeLassoSelectionMode(environmentHandle_, true, allSupportedTypes_);

    QStringList generalOperations;
    generalOperations.append(G_CP_MODE);
    generalOperations.append(G_K_MODE);

    // Define control point and knot operations
    QStringList controlPointOperations;
    controlPointOperations.append(CP_SELECT_ALL);
    controlPointOperations.append(CP_DESELECT_ALL);
    controlPointOperations.append(CP_INVERT);
    controlPointOperations.append(CP_DELETE);

    QStringList knotOperations;
    knotOperations.append(K_SELECT_ALL);
    knotOperations.append(K_DESELECT_ALL);
    knotOperations.append(K_INVERT);
    knotOperations.append(K_DELETE);

    emit addSelectionOperations(environmentHandle_, generalOperations, "Selection Operations");
    emit addSelectionOperations(environmentHandle_, controlPointOperations,  "Control Point Operations",   controlPointType_);
    emit addSelectionOperations(environmentHandle_, knotOperations,  "Knot Operations",   knotType_);

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

void BSplineCurveSelectionPlugin::updateSlotDescriptions() {
    
    emit setSlotDescription("selectAllControlPoints(int)", tr("Select all control points of a B-spline curve"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllControlPoints(int)", tr("Deselect all control points of a B-spline curve"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertControlPointSelection(int)", tr("Invert control point selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedControlPoints(int)", tr("Delete selected control points"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectControlPoints(int,IdList)", tr("Select the specified control points"),
                            QString("objectId,control pointList").split(","), QString("Id of object,List of control points").split(","));

    emit setSlotDescription("loadSelection(int,QString)", tr("Load selection from selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Selection file").split(","));
                            
    emit setSlotDescription("selectAllKnots(int)", tr("Select all knots of a B-spline curve"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllKnots(int)", tr("Deselect all knots of a B-spline curve"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertKnotSelection(int)", tr("Invert knot selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedKnots(int)", tr("Delete selected knots"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectKnots(int,IdList)", tr("Select the specified knots"),
                            QString("objectId,knotList").split(","), QString("Id of object,List of knots").split(","));
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotSelectionOperation(QString _operation) {
    
    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);
    
    if((type & allSupportedTypes_) == 0)
        return;
    
    // Test if operation should be applied to target objects only
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction =
            (targetsOnly ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS);

    if(_operation == G_CP_MODE) {
        // Set selection view mode
        setSelectionViewMode(CP);
    } else if(_operation == G_K_MODE) {
        // Set selection view mode
        setSelectionViewMode(K);
    } else if(_operation == CP_SELECT_ALL) {
        // Select all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllControlPoints(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All Controlpoints", UPDATE_SELECTION);
        }
    } else if (_operation == CP_DESELECT_ALL) {
        // Deselect all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllControlPoints(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Deselect All Controlpoints", UPDATE_SELECTION);
        }
    } else if (_operation == CP_INVERT) {
        // Invert control point selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertControlPointSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Controlpoint Selection", UPDATE_SELECTION);
        }
    } else if (_operation == CP_DELETE) {
        // Delete control point selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deleteSelectedControlPoints(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Delete Controlpoint Selection", UPDATE_SELECTION);
        }
    } else if(_operation == K_SELECT_ALL) {
        // Select all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllKnots(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All Knots", UPDATE_SELECTION);
        }
    } else if (_operation == K_DESELECT_ALL) {
        // Deselect all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllKnots(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Deselect All Knots", UPDATE_SELECTION);
        }
    } else if (_operation == K_INVERT) {
        // Invert knot selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertKnotSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Knot Selection", UPDATE_SELECTION);
        }
    } else if (_operation == K_DELETE) {
        // Delete knot selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deleteSelectedKnots(o_it->id());
            }

            emit updatedObject(o_it->id(), UPDATE_ALL);
            emit  createBackup(o_it->id(), "Delete Knot Selection", UPDATE_ALL);
        }
    }
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;
    
    BaseObjectData* object = 0;

    ACG::SceneGraph::PickTarget pickTarget;
    if(_currentType & controlPointType_) {
        // Control point selection
        pickTarget = ACG::SceneGraph::PICK_VERTEX;
    } else {
        // Knot selection
        pickTarget = ACG::SceneGraph::PICK_SPLINE;
    }

    // Perform picking
    bool successfullyPicked = PluginFunctions::scenegraphPick(pickTarget, _event->pos(), node_idx,
                              target_idx, &hit_point) && PluginFunctions::getPickedObject(node_idx, object);
                              
    if(successfullyPicked) {
        BSplineCurve* curve = PluginFunctions::splineCurve(object);
        BSplineCurveObject* co = PluginFunctions::bsplineCurveObject(object);

        if(!curve)
            return;

        if(_currentType & controlPointType_) {     
            // Control point selection
            if(!curve->controlpoint_selections_available()) return;
            
            // Enable control point texture mode
            co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::CONTROLPOINT);
            
            if(curve->controlpoint_selected(target_idx))
                curve->deselect_controlpoint(target_idx);
            else
                curve->select_controlpoint(target_idx);
        }
        
        if(_currentType & knotType_) {
            // Knot selection
            if(!curve->get_knotvector_ref()->selections_available()) return;
            
            // Enable control point texture mode
            co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::KNOTVECTOR);
            
            int numKnots = curve->n_knots();
            int order    = curve->degree() + 1;

            Knotvector* knotvec = curve->get_knotvector_ref();

            double minu = knotvec->getKnot(curve->degree());
            double maxu = knotvec->getKnot(numKnots - order);
            double udiff = maxu - minu;
            
            int texres = co->splineCurveNode()->pick_texture_res();
            double param = ((double)target_idx/(double)texres) * udiff + minu;
            
            ACG::Vec2i interval = curve->interval(param);
            
            int knotIdx = interval[0];

            if(knotvec->selected(knotIdx)) {
                knotvec->deselect(knotIdx);
            } else {
                knotvec->select(knotIdx);
            }
        }
        
        emit updatedObject(object->id(), UPDATE_SELECTION);
        emit  createBackup(object->id(), "Toogle Selection", UPDATE_SELECTION);
    }
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    
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
        
        // Select all primitives that lie in this region
        for(PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            
            if (o_it->visible()) {
                
                BSplineCurve* curve = PluginFunctions::splineCurve(o_it);
                BSplineCurveObject* co = PluginFunctions::bsplineCurveObject(o_it);
                
                if(curve) {
                    
                    if(_currentType & controlPointType_) {
                        // Select control points
                        if(!curve->controlpoint_selections_available()) return;
            
                        // Enable control point texture mode
                        co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::CONTROLPOINT);
                        
                        for(unsigned int i = 0; i < curve->n_control_points(); ++i) {
                            ACG::Vec3d c = curve->get_control_point(i);
                            ACG::Vec3d pc = state.project(c);
                            QPoint p((int)pc[0], (int)pc[1]);
                            if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                if(_deselect)
                                    curve->deselect_controlpoint(i);
                                else
                                    curve->select_controlpoint(i);
                            }
                        }
                    }
                    
                    if(_currentType & knotType_) {
                        // Select knots
                        if(!curve->get_knotvector_ref()->selections_available()) return;
                        
                        // Enable knot texture mode
                        co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::KNOTVECTOR);
                        
                        int numKnots = curve->n_knots();
                        int order    = curve->degree() + 1;

                        Knotvector* knotvec = curve->get_knotvector_ref();

                        // Go over all B-spline knot midpoints and test
                        // if computed position on curve lies within the polygon
                        if(curve->degree() == 0) return;
                        if((int)curve->n_knots() <= ((numKnots - order) + 1)) return;
                        
                        for(int i = curve->degree()-1; i < (numKnots - order)-1; ++i) {
                            
                            double baseKnot = knotvec->getKnot(i+1);
                            double dhalf = 0.5*(knotvec->getKnot(i+2) - baseKnot);
                            
                            double t = baseKnot + dhalf;
                            
                            // Project curve point
                            ACG::Vec3d pcp = state.project(curve->curvePoint(t));
                            
                            QPoint p((int)pcp[0], (int)pcp[1]);
                            
                            if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                if(_deselect) {
                                    knotvec->deselect(i+1);
                                } else {
                                    knotvec->select(i+1);
                                }
                            }
                        }
                    }
                }
            }

            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Lasso Selection", UPDATE_SELECTION);
        }

        // Clear lasso points
        volumeLassoPoints_.clear();
    }
}

//==============================================================================================

void BSplineCurveSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

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

void BSplineCurveSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Load plugin specific settings
}

//==============================================================================================

void BSplineCurveSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Save plugin specific settings
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotLoadSelection(const INIFile& _file) {
    
    // Iterate over all B-spline curves in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_CURVE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Read section for each object
        // Append object name to section identifier
        QString section = QString("BSplineCurveSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }
        
        std::vector<int> ids;
        // Read control point selection:
        _file.get_entry(ids, section, "ControlPointSelection");
        selectControlPoints(o_it->id(), ids);
        ids.clear();
        // Read knot selection:
        _file.get_entry(ids, section, "KnotSelection");
        selectKnots(o_it->id(), ids);
        ids.clear();
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit  createBackup(o_it->id(), "Load Selection", UPDATE_SELECTION);
    }
    
    setSelectionViewMode(CP);
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotSaveSelection(INIFile& _file) {
    
    // Iterate over all B-spline curves in the scene and save
    // the selections for all entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_CURVE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Create section for each object
        // Append object name to section identifier
        QString section = QString("BSplineCurveSelection") + "//" + o_it->name();
        
        // Store control point selection
        _file.add_entry(section, "ControlPointSelection", getControlPointSelection(o_it->id()));
        _file.add_entry(section, "KnotSelection", getKnotSelection(o_it->id()));
    }
}

//==============================================================================================

void BSplineCurveSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    
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
        // Select all
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & controlPointType_) selectAllControlPoints(o_it->id());
                if(type & knotType_) selectAllKnots(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All", UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_C && _modifiers == Qt::NoModifier) {
        // Deselect all
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & controlPointType_) deselectAllControlPoints(o_it->id());
                if(type & knotType_) deselectAllKnots(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Clear Selection", UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_I && _modifiers == Qt::NoModifier) {
        // Invert selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & controlPointType_) invertControlPointSelection(o_it->id());
                if(type & knotType_) invertKnotSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Selection", UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_Delete && _modifiers == Qt::NoModifier) {
        // Delete selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_CURVE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & controlPointType_) deleteSelectedControlPoints(o_it->id());
                if(type & knotType_) deleteSelectedKnots(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_ALL);
            emit  createBackup(o_it->id(), "Delete Selection", UPDATE_ALL);
        }
   }
}

void BSplineCurveSelectionPlugin::setSelectionViewMode(const SelectionViewMode _mode) {
    
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_CURVE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        BSplineCurveObject* co = PluginFunctions::bsplineCurveObject(o_it);
        if(_mode == CP) {
            co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::CONTROLPOINT);
        }
        else {
            co->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::KNOTVECTOR);
        }
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
    }
}

//==============================================================================================

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(bsplinecurveselectionplugin, BSplineCurveSelectionPlugin);
#endif



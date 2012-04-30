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

#include "BSplineSurfaceSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Primitive type icons
#define CONTROL_POINT_TYPE  "bscontrolpointselection.png"
#define KNOT_TYPE           "bsknotselection.png"
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
//#define CP_DELETE_U        "Delete Selected Control Points in U"
//#define CP_DELETE_V        "Delete Selected Control Points in V"

// Knots:
#define K_SELECT_ALL      "Select All Knots"
#define K_DESELECT_ALL    "Deselect All Knots"
#define K_INVERT          "Invert Knot Selection"
//#define K_DELETE_U        "Delete Selected Knots in U"
//#define K_DELETE_V        "Delete Selected Knots in V"

/// Default constructor
BSplineSurfaceSelectionPlugin::BSplineSurfaceSelectionPlugin() :
controlPointType_(0),
knotType_(0),
allSupportedTypes_(0) {
}

//==============================================================================================
      
BSplineSurfaceSelectionPlugin::~BSplineSurfaceSelectionPlugin() {
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for skeletons
    // and register skeleton data type for the environment.
    
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    
    emit addSelectionEnvironment("B-Spline Surface Selections", "Select B-Spline surface primitives.",
                                 QIcon(iconPath + "bsplinesurface.png"), environmentHandle_);

    // Register mesh object types
    emit registerType(environmentHandle_, DATA_BSPLINE_SURFACE);
    
    emit addPrimitiveType(environmentHandle_, "Select Control Points", QIcon(iconPath + CONTROL_POINT_TYPE), controlPointType_);
    emit addPrimitiveType(environmentHandle_, "Select Knots", QIcon(iconPath + KNOT_TYPE), knotType_);
    
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
//     controlPointOperations.append(CP_DELETE_U);
//     controlPointOperations.append(CP_DELETE_V);
    controlPointOperations.append(CP_INVERT);
    
    QStringList knotOperations;
    knotOperations.append(K_SELECT_ALL);
    knotOperations.append(K_DESELECT_ALL);
//     knotOperations.append(K_DELETE_U);
//     knotOperations.append(K_DELETE_V);
    knotOperations.append(K_INVERT);
    
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
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::updateSlotDescriptions() {
    
    emit setSlotDescription("selectAllControlPoints(int)", tr("Select all control points of a B-spline surface"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllControlPoints(int)", tr("Deselect all control points of a B-spline surface"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertControlPointSelection(int)", tr("Invert control point selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedControlPoints(int)", tr("Delete selected control points"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectControlPoints(int,IdList)", tr("Select the specified control points"),
                            QString("objectId,control pointList").split(","), QString("Id of object,List of control points").split(","));

    emit setSlotDescription("loadSelection(int,QString)", tr("Load selection from selection file"),
                            QString("objectId,filename").split(","), QString("Id of an object,Selection file").split(","));

    emit setSlotDescription("selectAllKnots(int)", tr("Select all knots of a B-spline surface"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deselectAllKnots(int)", tr("Deselect all knots of a B-spline surface"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("invertKnotSelection(int)", tr("Invert knot selection"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("deleteSelectedKnots(int)", tr("Delete selected knots"),
                            QStringList("objectId"), QStringList("Id of object"));
    emit setSlotDescription("selectKnots(int,IdList)", tr("Select the specified knots"),
                            QString("objectId,knotList").split(","), QString("Id of object,List of knots").split(","));
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotSelectionOperation(QString _operation) {
    
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
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllControlPoints(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All Controlpoints", UPDATE_SELECTION);
        }
    } else if (_operation == CP_DESELECT_ALL) {
        // Deselect all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllControlPoints(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);    
            emit  createBackup(o_it->id(), "Deselect All Controlpoints", UPDATE_SELECTION);
        }
    } else if (_operation == CP_INVERT) {
        // Invert control point selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertControlPointSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Controlpoint Selection", UPDATE_SELECTION);
        }
//     } else if (_operation == CP_DELETE_U) {
//         // Delete control point selection
//         for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
//             o_it != PluginFunctions::objectsEnd(); ++o_it) {
//             if (o_it->visible()) {
//                deleteSelectedControlPointsU(o_it->id());
//             }
//             
//             emit updatedObject(o_it->id(), UPDATE_SELECTION);    
//         }
//     } else if (_operation == CP_DELETE_V) {
//         // Delete control point selection
//         for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
//             o_it != PluginFunctions::objectsEnd(); ++o_it) {
//             if (o_it->visible()) {
//                deleteSelectedControlPointsV(o_it->id());
//             }
//             
//             emit updatedObject(o_it->id(), UPDATE_SELECTION);    
//         }
    } else if(_operation == K_SELECT_ALL) {
        // Select all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                selectAllKnots(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Select All Knots", UPDATE_SELECTION);
        }
    } else if (_operation == K_DESELECT_ALL) {
        // Deselect all control points
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               deselectAllKnots(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Deselect All Knots", UPDATE_SELECTION);
        }
    } else if (_operation == K_INVERT) {
        // Invert knot selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
               invertKnotSelection(o_it->id());
            }
            
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Knot Selection", UPDATE_SELECTION);
        }
//     } else if (_operation == K_DELETE_U) {
//         // Delete knot selection
//         for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
//             o_it != PluginFunctions::objectsEnd(); ++o_it) {
//             if (o_it->visible()) {
//                deleteSelectedKnotsU(o_it->id());
//             }
//             
//             emit updatedObject(o_it->id(), UPDATE_SELECTION);    
//         }
//     } else if (_operation == K_DELETE_V) {
//         // Delete knot selection
//         for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
//             o_it != PluginFunctions::objectsEnd(); ++o_it) {
//             if (o_it->visible()) {
//                deleteSelectedKnotsV(o_it->id());
//             }
//             
//             emit updatedObject(o_it->id(), UPDATE_SELECTION);    
//         }
    }
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotToggleSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0) return;
    
    if (_currentType & controlPointType_) {

        unsigned int node_idx, target_idx;
        ACG::Vec3d   hit_point;

        if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(), node_idx, target_idx, &hit_point)) {

            BaseObjectData* object = 0;

            if (PluginFunctions::getPickedObject(node_idx, object)) {
                
                BSplineSurfaceObject* bsso = PluginFunctions::bsplineSurfaceObject(object);
                
                // toggle selection
                if (bsso->splineSurface()->controlpoint_selections_available()) {
                    
                    unsigned int max = bsso->splineSurface()->n_control_points_m() * bsso->splineSurface()->n_control_points_n();

                    if ( target_idx < max) {
                        
                        int idx_m = target_idx / bsso->splineSurface()->n_control_points_n();
                        int idx_n = target_idx % bsso->splineSurface()->n_control_points_n();
                        
                        if (bsso->splineSurface()->controlpoint_selected(idx_m, idx_n) || _deselect) {
                            bsso->splineSurface()->deselect_controlpoint(idx_m, idx_n);
                        } else {
                            bsso->splineSurface()->select_controlpoint(idx_m, idx_n);
                        }
                    }
                }
                bsso->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
                emit updatedObject(bsso->id(), UPDATE_SELECTION);
                emit  createBackup(bsso->id(), "Toggle Selection", UPDATE_SELECTION);
            }
        }
    }
    
    if (_currentType & knotType_) {

        unsigned int node_idx, target_idx;
        ACG::Vec3d   hit_point;

        if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_SPLINE, _event->pos(),node_idx, target_idx, &hit_point)) {
            
            BaseObjectData* object = 0;

            if (PluginFunctions::getPickedObject(node_idx, object)) {
                
                BSplineSurfaceObject * bsso = PluginFunctions::bsplineSurfaceObject(object);
                BSplineSurface *       bss  = bsso->splineSurface();

                // toggle selection
                if(bss->get_knotvector_m_ref()->selections_available() &&
                        bss->get_knotvector_n_ref()->selections_available()) {
                    
                    // the target index we are getting here refers to the respective texel in the picking-texture
                    // hence, we have to compute the actual u,v coordinates from this texel
                    // given the uv coord, we finally compute the closes knot in u and v direction, respectively

                    int numKnots_m = bss->n_knots_m();
                    int numKnots_n = bss->n_knots_n();
                    int order_m    = bss->degree_m() + 1;
                    int order_n    = bss->degree_n() + 1;

                    Knotvector * knotvec_u = bss->get_knotvector_m_ref();
                    Knotvector * knotvec_v = bss->get_knotvector_n_ref();

                    double minu = knotvec_u->getKnot(bss->degree_m());
                    double minv = knotvec_v->getKnot(bss->degree_n());
                    double maxu = knotvec_u->getKnot(numKnots_m - order_m);
                    double maxv = knotvec_v->getKnot(numKnots_n - order_n);

                    double udiff = maxu - minu;
                    double vdiff = maxv - minv;

                    int texres = bsso->splineSurfaceNode()->pick_texture_res();
                    double curu = double (target_idx / texres) / double(texres) * udiff + minu;
                    double curv = double (target_idx % texres) / double(texres) * vdiff + minv;

                    ACG::Vec2d found_params(curu, curv);

                    // knots closest to parameters of hitpoint on the surface
                    ACG::Vec2i interval_m = bss->interval_m(curu);
                    ACG::Vec2i interval_n = bss->interval_n(curv);
            
                    int knotIdx_m = interval_m[0];
                    int knotIdx_n = interval_n[0];

                    if ( !bss->get_knotvector_m_ref()->selected( knotIdx_m ) ||
                         !bss->get_knotvector_n_ref()->selected( knotIdx_n )) {
                        
                        bss->get_knotvector_m_ref()->select( knotIdx_m );
                        bss->get_knotvector_n_ref()->select( knotIdx_n );
                    }
                    else {
                        bss->get_knotvector_m_ref()->deselect( knotIdx_m );
                        bss->get_knotvector_n_ref()->deselect( knotIdx_n );
                    }

                } // end of if selections available
                
                bsso->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
                emit updatedObject(bsso->id(), UPDATE_SELECTION);
                emit  createBackup(bsso->id(), "Toggle Selection", UPDATE_SELECTION);
            }
        }
    }
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event, SelectionInterface::PrimitiveType _currentType, bool _deselect) {
    
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
        for(PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            
            if (o_it->visible()) {
                
                BSplineSurface* surface = PluginFunctions::splineSurface(o_it);
                BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(o_it);
                
                if(surface) {
                    
                    if(_currentType & controlPointType_) {
                        // Select control points
                        if(!surface->controlpoint_selections_available()) return;
                        
                        // Enable control point texture mode
                        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
                        
                        for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
                            for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
                                
                                ACG::Vec3d c = surface->get_control_point(i, j);
                                ACG::Vec3d pc = state.project(c);
                                QPoint p((int)pc[0], (int)pc[1]);
                                if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                    if(_deselect)
                                        surface->deselect_controlpoint(i,j);
                                    else
                                        surface->select_controlpoint(i,j);
                                }
                            }
                        }
            
                    }
                    
                    if(_currentType & knotType_) {
                        // Select knots
                        if(!surface->get_knotvector_m_ref()->selections_available()) return;
                        if(!surface->get_knotvector_n_ref()->selections_available()) return;
                        
                        // Enable knot texture mode
                        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
                        
                        int numKnots_m = surface->n_knots_m();
                        int numKnots_n = surface->n_knots_n();
                        
                        int order_m    = surface->degree_m() + 1;
                        int order_n    = surface->degree_n() + 1;

                        Knotvector* knotvec_m = surface->get_knotvector_m_ref();
                        Knotvector* knotvec_n = surface->get_knotvector_n_ref();

                        // Go over all B-spline surface knot midpoints and test
                        // if computed position on surface lies within the polygon
                        if(surface->degree_m() == 0) return;
                        if(surface->degree_n() == 0) return;
                        
                        if((int)surface->n_knots_m() <= ((numKnots_m - order_m) + 1)) return;
                        if((int)surface->n_knots_n() <= ((numKnots_n - order_n) + 1)) return;
                        
                        for(int i = surface->degree_m()-1; i < (numKnots_m - order_m)-1; ++i) {
                            for(int j = surface->degree_n()-1; j < (numKnots_n - order_n)-1; ++j) {
                            
                                double baseKnot_m = knotvec_m->getKnot(i+1);
                                double baseKnot_n = knotvec_n->getKnot(j+1);
                                
                                double dhalfu = 0.5*(knotvec_m->getKnot(i+2) - baseKnot_m);
                                double dhalfv = 0.5*(knotvec_n->getKnot(j+2) - baseKnot_n);
                                
                                double t_u = baseKnot_m + dhalfu;
                                double t_v = baseKnot_n + dhalfv;
                                
                                // Project surface point
                                ACG::Vec3d pcp = state.project(surface->surfacePoint(t_u, t_v));
                                
                                QPoint p((int)pcp[0], (int)pcp[1]);
                                
                                if(polygon.containsPoint(p, Qt::OddEvenFill)) {
                                    if(_deselect) {
                                        knotvec_m->deselect(i+1);
                                        knotvec_n->deselect(j+1);
                                    } else {
                                        knotvec_m->select(i+1);
                                        knotvec_n->select(j+1);
                                    }
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

void BSplineSurfaceSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

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

void BSplineSurfaceSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Load plugin specific settings
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Save plugin specific settings
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotLoadSelection(const INIFile& _file) {
    
    // Iterate over all B-spline surfaces in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_SURFACE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Read section for each object
        // Append object name to section identifier
        QString section = QString("BSplineSurfaceSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }
        
        std::vector<int> ids;
        // Read control point selection:
        _file.get_entry(ids, section, "ControlPointSelection");
        selectControlPoints(o_it->id(), ids);
        ids.clear();
        // Read knot selection:
        std::vector<int> ids_u;
        std::vector<int> ids_v;
        
        _file.get_entry(ids_u, section, "KnotSelection_u");
        _file.get_entry(ids_v, section, "KnotSelection_v");
        
        selectKnots(o_it->id(), ids_u, ids_v);
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
        emit  createBackup(o_it->id(), "Load Selection", UPDATE_SELECTION);
    }
    
    setSelectionViewMode(CP);
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotSaveSelection(INIFile& _file) {
    
    // Iterate over all B-spline surfaces in the scene and save
    // the selections for all entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_SURFACE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        // Create section for each object
        // Append object name to section identifier
        QString section = QString("BSplineSurfaceSelection") + "//" + o_it->name();
        
        // Store control point selection
        _file.add_entry(section, "ControlPointSelection", getControlPointSelection(o_it->id()));
        // Store knot selection
        _file.add_entry(section, "KnotSelection_u", getKnotSelectionU(o_it->id()));
        _file.add_entry(section, "KnotSelection_v", getKnotSelectionV(o_it->id()));
    }
}

//==============================================================================================

void BSplineSurfaceSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {
    
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
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
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
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
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
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_BSPLINE_SURFACE)); 
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & controlPointType_) invertControlPointSelection(o_it->id());
                if(type & knotType_) invertKnotSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit  createBackup(o_it->id(), "Invert Selection", UPDATE_SELECTION);
        }
    }
}

void BSplineSurfaceSelectionPlugin::setSelectionViewMode(const SelectionViewMode _mode) {
    
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_BSPLINE_SURFACE)); 
         o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(o_it);
        if(_mode == CP) {
            so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
        }
        else {
            so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
        }
        
        emit updatedObject(o_it->id(), UPDATE_SELECTION);
    }
}

//==============================================================================================

Q_EXPORT_PLUGIN2(bsplinesurfaceselectionplugin, BSplineSurfaceSelectionPlugin);

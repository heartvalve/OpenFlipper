#include "BSplineSurfaceSelectionPlugin.hh"

void BSplineSurfaceSelectionPlugin::selectAllControlPoints(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(object);
    
    if(surface) {    
        for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
            for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
                surface->select_controlpoint(i, j);
            }
        }
    }
    
    // Switch to control point selection mode
    if(so)
        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
    
    emit scriptInfo("selectAllControlPoints(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::deselectAllControlPoints(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
            for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
                surface->deselect_controlpoint(i,j);
            }
        }
    }
    
    emit scriptInfo("deselectAllControlPoints(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::invertControlPointSelection(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(object);
    
    if(surface) {    
        for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
            for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
                if(surface->controlpoint_selected(i,j))
                    surface->deselect_controlpoint(i,j);
                else
                    surface->select_controlpoint(i,j);
            }
        }
    }
    
    // Switch to control point selection mode
    if(so)
        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
    
    emit scriptInfo("invertControlPointSelection(ObjectId)");
}

// void BSplineSurfaceSelectionPlugin::deleteSelectedControlPointsU(int _objectId) {
//     
//     BaseObjectData* object = 0;
//     
//     if(!PluginFunctions::getObject(_objectId, object)) {
//         emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//         return;
//     }
//     
//     BSplineSurface* surface = PluginFunctions::splineSurface(object);
//     if(surface) {
//         
//         bool finished = false;
//         while(!finished) {
//         
//             bool restartWhile = false;
//             for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
//                 for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
//                     if(surface->controlpoint_selected(i,j)) {
//                         surface->delete_vector_n(j);
//                         restartWhile = true;
//                         break;
//                     }
//                 }
//                 if(restartWhile) {
//                     break;
//                 }
//             }
//             
//             finished = true;
//         }        
//     }
//     
//     emit scriptInfo("deleteSelectedControlPoints(ObjectId)");
// }
// 
// void BSplineSurfaceSelectionPlugin::deleteSelectedControlPointsV(int _objectId) {
//     
//     BaseObjectData* object = 0;
//     
//     if(!PluginFunctions::getObject(_objectId, object)) {
//         emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//         return;
//     }
//     
//     BSplineSurface* surface = PluginFunctions::splineSurface(object);
//     if(surface) {
//         
//         bool finished = false;
//         while(!finished) {
//         
//             bool restartWhile = false;
//             for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
//                 for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
//                     if(surface->controlpoint_selected(i,j)) {
//                         surface->delete_vector_m(i);
//                         restartWhile = true;
//                         break;
//                     }
//                 }
//                 if(restartWhile) {
//                     break;
//                 }
//             }
//             
//             finished = true;
//         }        
//     }
//     
//     emit scriptInfo("deleteSelectedControlPoints(ObjectId)");
// }

void BSplineSurfaceSelectionPlugin::selectControlPoints(int _objectId, const IdList& _ids, bool _deselect) {
    
    if(_ids.empty() ) return;
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(IdList::const_iterator it = _ids.begin(); it != _ids.end(); ++it) {
            if(*it < (int)surface->n_control_points_m()*(int)surface->n_control_points_n()) {
                
                int idx_m = *it / surface->n_control_points_n();
                int idx_n = *it % surface->n_control_points_n();
                
                if(_deselect) surface->deselect_controlpoint(idx_m, idx_n);
                else surface->select_controlpoint(idx_m, idx_n);
            }
        }
    }
    
    QString selection = "selectControlPoints(ObjectId, [ " + QString::number(_ids[0]);

    for (uint i = 1 ; i < _ids.size(); ++i) {
        selection +=  ", " + QString::number(_ids[i]);
    }

    selection += " ])";
    

    emit scriptInfo(selection);
}

IdList BSplineSurfaceSelectionPlugin::getControlPointSelection(int _objectId) {
    
    BaseObjectData* object = 0;
    
    IdList list;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return list;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(unsigned int i = 0; i < surface->n_control_points_m(); ++i) {
            for(unsigned int j = 0; j < surface->n_control_points_n(); ++j) {
                if(surface->controlpoint_selected(i,j))
                    list.push_back((int)i*surface->n_control_points_n() + (int)j);
            }
        }
    }
    
    return list;
}

//=====================================================================

void BSplineSurfaceSelectionPlugin::selectAllKnots(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(object);
    
    if(surface) {    
        for(unsigned int i = 0; i < surface->n_knots_m(); ++i) {
            surface->get_knotvector_m_ref()->select(i);
        }
        for(unsigned int i = 0; i < surface->n_knots_n(); ++i) {
            surface->get_knotvector_n_ref()->select(i);
        }
    }
    
    // Switch to control point selection mode
    if(so)
        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
    
    emit scriptInfo("selectAllKnots(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::deselectAllKnots(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface*  surface = PluginFunctions::splineSurface(object);
    
    if(surface) {    
        for(unsigned int i = 0; i < surface->n_knots_m(); ++i) {
            surface->get_knotvector_m_ref()->deselect(i);
        }
        for(unsigned int i = 0; i < surface->n_knots_n(); ++i) {
            surface->get_knotvector_n_ref()->deselect(i);
        }
    }
    
    emit scriptInfo("deselectAllKnots(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::invertKnotSelection(int _objectId) {
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    BSplineSurfaceObject* so = PluginFunctions::bsplineSurfaceObject(object);
    
    if(surface) {    
        for(unsigned int i = 0; i < surface->n_knots_m(); ++i) {
            if(surface->get_knotvector_m_ref()->selected(i))
                surface->get_knotvector_m_ref()->deselect(i);
            else
                surface->get_knotvector_m_ref()->select(i);
        }
        for(unsigned int i = 0; i < surface->n_knots_n(); ++i) {
            if(surface->get_knotvector_n_ref()->selected(i))
                surface->get_knotvector_n_ref()->deselect(i);
            else
                surface->get_knotvector_n_ref()->select(i);
        }
    }
    
    // Switch to control point selection mode
    if(so)
        so->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
    
    emit scriptInfo("invertKnotSelection(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::deleteSelectedKnotsU(int _objectId) {
    
// 
//         // I know this is a bit crappy, but knot indices
//         // change forcably after each delete operation so we have to
//         // start over each time...
//         bool breakWhile = false;
//         while(!breakWhile) {
//             bool oneFound = false;
//             unsigned int i = 0;
//             for(; i < surface->n_knots_n(); ++i) {
//                 if(surface->get_knotvector_n_ref()->selected(i)) {
//                     surface->get_knotvector_n_ref()->deleteKnot(i);
//                     oneFound = true;
//                     break;
//                 }
//             }
//             
//             if((i >= surface->n_knots_n()) && !oneFound) {
//                 // We are through
//                 breakWhile = true;
//             }
//         }
//     }
//     
//     emit scriptInfo("deleteSelectedKnots(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::deleteSelectedKnotsV(int _objectId) {
    
//     BaseObjectData* object = 0;
//     
//     if(!PluginFunctions::getObject(_objectId, object)) {
//         emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//         return;
//     }
//     
//     BSplineSurface* surface = PluginFunctions::splineSurface(object);
//     if(surface) {
// 
//         // I know this is a bit crappy, but knot indices
//         // change forcably after each delete operation so we have to
//         // start over each time...
//         bool breakWhile = false;
//         while(!breakWhile) {
//             bool oneFound = false;
//             unsigned int i = 0;
//             for(; i < surface->n_knots(); ++i) {
//                 if(surface->get_knotvector_ref()->selected(i)) {
//                     surface->get_knotvector_ref()->deleteKnot(i);
//                     oneFound = true;
//                     break;
//                 }
//             }
//             
//             if((i >= surface->n_knots()) && !oneFound) {
//                 // We are through
//                 breakWhile = true;
//             }
//         }
//     }
//     
//     emit scriptInfo("deleteSelectedKnots(ObjectId)");
}

void BSplineSurfaceSelectionPlugin::selectKnots(int _objectId, const IdList& _ids_u, const IdList& _ids_v, bool _deselect) {
    
    if(_ids_u.empty()) return;
    
    BaseObjectData* object = 0;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(IdList::const_iterator it = _ids_u.begin(); it != _ids_u.end(); ++it) {
            if(*it < (int)surface->n_knots_m()) {
                if(_deselect) surface->get_knotvector_m_ref()->deselect(*it);
                else surface->get_knotvector_m_ref()->select(*it);
            }
        }
        for(IdList::const_iterator it = _ids_v.begin(); it != _ids_v.end(); ++it) {
            if(*it < (int)surface->n_knots_n()) {
                if(_deselect) surface->get_knotvector_n_ref()->deselect(*it);
                else surface->get_knotvector_n_ref()->select(*it);
            }
        }
    }
    
    QString selection = "selectKnots(ObjectId, [ " + QString::number(_ids_u[0]);

    for (uint i = 1 ; i < _ids_u.size(); ++i) {
        selection +=  ", " + QString::number(_ids_u[i]);
    }
    
    selection += "; ";
    
    for (uint i = 0 ; i < _ids_v.size(); ++i) {
        selection +=  ", " + QString::number(_ids_v[i]);
    }

    selection += " ])";
    

    emit scriptInfo(selection);
}

IdList BSplineSurfaceSelectionPlugin::getKnotSelectionU(int _objectId) {
    
    BaseObjectData* object = 0;
    
    IdList list;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return list;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(unsigned int i = 0; i < surface->n_knots_m(); ++i) {
            if(surface->get_knotvector_m_ref()->selected(i))
                list.push_back((int)i);
        }
    }
   
    return list;
}

IdList BSplineSurfaceSelectionPlugin::getKnotSelectionV(int _objectId) {
    
    BaseObjectData* object = 0;
    
    IdList list;
    
    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return list;
    }
    
    BSplineSurface* surface = PluginFunctions::splineSurface(object);
    if(surface) {
        for(unsigned int i = 0; i < surface->n_knots_n(); ++i) {
            if(surface->get_knotvector_n_ref()->selected(i))
                list.push_back((int)i);
        }
    }
   
    return list;
    
}

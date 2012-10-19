#include "ObjectSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

/// Select all objects
void ObjectSelectionPlugin::selectAllObjects() {

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        o_it->target(true);
    }

    emit scriptInfo("selectAllObjects()");
}

/// Select specified objects
void ObjectSelectionPlugin::selectObjects(IdList _list) {

    for(IdList::iterator it = _list.begin(); it != _list.end(); ++it) {
        BaseObjectData* object = 0;
        PluginFunctions::getObject(*it, object);
        if(object) {
            object->target(true);
        }
    }

    emit scriptInfo("selectObjects(IdList)");
}

/// Deselect all objects
void ObjectSelectionPlugin::deselectAllObjects() {

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        o_it->target(false);
    }

    emit scriptInfo("deselectAllObjects()");
}

/// Invert object selection
void ObjectSelectionPlugin::invertObjectSelection() {

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        o_it->target(!o_it->target());
    }

    emit scriptInfo("invertObjectSelection()");
}

/// Delete selected objects
void ObjectSelectionPlugin::deleteSelectedObjects() {

    if(!OpenFlipper::Options::nogui()) {
        int ret = QMessageBox::warning(0, tr("Open Flipper"),
                                    tr("Do you really want to delete the selected objects?"),
                                    QMessageBox::Yes | QMessageBox::Cancel,
                                    QMessageBox::Cancel);

        if(ret == QMessageBox::Cancel) return;
    }

    std::vector<int> deleteIds;

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        deleteIds.push_back(o_it->id());
    }

    for(std::vector<int>::iterator it = deleteIds.begin(); it != deleteIds.end(); ++it) {
        emit deleteObject(*it);
    }

    emit scriptInfo("invertObjectSelection()");
}

/// Get selected objects
IdList ObjectSelectionPlugin::getObjectSelection() {

    IdList list;
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DataType(DATA_ALL));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        list.push_back(o_it->id());
    }
    return list;
}

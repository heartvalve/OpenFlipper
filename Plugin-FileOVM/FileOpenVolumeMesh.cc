#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <QtGui>
#include <QFileInfo>

#include <ACG/GL/GLState.hh>

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#include "FileOpenVolumeMesh.hh"

FileOpenVolumeMeshPlugin::FileOpenVolumeMeshPlugin() :
loadOptions_(0),
saveOptions_(0),
typeCheck_(0),
loadCompMode_(0),
loadCorrectOrder_(0),
loadTopCheck_(0),
saveCompMode_(0) {

}

//----------------------------------------------------------------------------

void FileOpenVolumeMeshPlugin::initializePlugin() {

    loadOptions_ = new QWidget();

    QVBoxLayout* llayout = new QVBoxLayout();
    llayout->setAlignment(Qt::AlignTop);

    typeCheck_ = new QComboBox();
    typeCheck_->addItem("Autodetect");
    typeCheck_->addItem("Polyhedral Mesh");
    typeCheck_->addItem("Hexahedral Mesh");
    typeCheck_->setCurrentIndex(0);
    loadCompMode_ = new QCheckBox("Load PolyVolMesh format");
    loadCorrectOrder_= new QCheckBox("Correct face ordering");
    loadTopCheck_ = new QCheckBox("Perform topology checks");
    llayout->addWidget(typeCheck_);
    llayout->addWidget(loadCompMode_);
    llayout->addWidget(loadCorrectOrder_);
    llayout->addWidget(loadTopCheck_);

    loadOptions_->setLayout(llayout);

    saveOptions_ = new QWidget();

    QVBoxLayout* slayout = new QVBoxLayout();
    slayout->setAlignment(Qt::AlignTop);

    saveCompMode_ = new QCheckBox("Save in PolyVolMesh format");
    slayout->addWidget(saveCompMode_);

    saveOptions_->setLayout(slayout);
}

//----------------------------------------------------------------------------


QString FileOpenVolumeMeshPlugin::getLoadFilters() {
    return QString(tr("Polyhedral Volume Mesh files ( *.ovm *.polyvolmesh )"));
}
;

//----------------------------------------------------------------------------


QString FileOpenVolumeMeshPlugin::getSaveFilters() {
    return QString(tr("Polyhedral Volume Mesh files ( *.ovm )"));
}
;

//----------------------------------------------------------------------------


DataType FileOpenVolumeMeshPlugin::supportedType() {

    DataType type = DATA_POLYHEDRAL_MESH;// | DATA_HEXAHEDRAL_MESH;
    return type;
}

//----------------------------------------------------------------------------


int FileOpenVolumeMeshPlugin::loadObject(QString _filename) {

    bool compatibility_mode = false;
    if(!OpenFlipper::Options::nogui()) {
        compatibility_mode = loadCompMode_->isChecked();
    }

    bool correct_face_order = false;
    if(!OpenFlipper::Options::nogui()) {
        correct_face_order = loadCorrectOrder_->isChecked();
    }

    bool topology_checks = true;
    if(!OpenFlipper::Options::nogui()) {
        topology_checks = loadTopCheck_->isChecked();
    }

    int id = -1;
    bool hexMesh = false;

    if(!OpenFlipper::Options::nogui() && typeCheck_->currentIndex() == 0) {
        hexMesh = fileManager_.isHexahedralMesh(_filename.toStdString());
    } else if (!OpenFlipper::Options::nogui() && typeCheck_->currentIndex() == 2) {
        hexMesh = false;
    }

    // Hack
    hexMesh = false;

    if(hexMesh) {

//        emit addEmptyObject(DATA_HEXAHEDRAL_MESH, id);
//        HexahedralMeshObject* obj(0);
//
//        if (PluginFunctions::getObject(id, obj)) {
//
//            if(compatibility_mode) {
//                loadMesh((const char*) _filename.toAscii(), *(obj->mesh()), compatibility_mode,
//                         topology_checks, correct_face_order);
//            } else {
//                if(!fileManager_.readFile(_filename.toStdString(), *(obj->mesh()),
//                                          topology_checks,true, true)) {
//                    emit log(LOGERR, QString("Could not open file %1!").arg(_filename));
//                }
//            }
//
//            obj->setFromFileName(_filename);
//        }
//
//        emit openedFile(obj->id());
//
//        // Go into solid flat shaded mode
//        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED, PluginFunctions::activeExaminer());
//
//        // Scale polyhedra a bit
//        obj->meshNode()->set_scaling(0.8);

    } else {

        emit addEmptyObject(DATA_POLYHEDRAL_MESH, id);
        PolyhedralMeshObject* obj(0);

        if (PluginFunctions::getObject(id, obj)) {

            if(compatibility_mode) {

                loadMesh((const char*) _filename.toAscii(), *(obj->mesh()), compatibility_mode,
                         topology_checks, correct_face_order);

            } else {
                if(!fileManager_.readFile(_filename.toStdString(), *(obj->mesh()),
                                          topology_checks,true, true)) {
                    emit log(LOGERR, QString("Could not open file %1!").arg(_filename));
                }
            }
            obj->setFromFileName(_filename);
        }

        emit openedFile(obj->id());

        // Go into solid flat shaded mode
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED, PluginFunctions::activeExaminer());

        // Scale polyhedra a bit
        obj->meshNode()->set_scaling(0.8);
    }

    PluginFunctions::viewAll();

    return id;
}

//----------------------------------------------------------------------------


bool FileOpenVolumeMeshPlugin::saveObject(int _id, QString _filename) {

    BaseObjectData* obj(0);
    if (PluginFunctions::getObject(_id, obj)) {

        PolyhedralMeshObject* mesh_obj = PluginFunctions::polyhedralMeshObject(obj);
        //HexahedralMeshObject* hex_mesh_obj = PluginFunctions::hexahedralMeshObject(obj);
        if (mesh_obj) {

            obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(), -1));
            obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2));

            bool compatibility_mode = false;
            if(!OpenFlipper::Options::nogui()) {
                compatibility_mode = saveCompMode_->isChecked();
            }

            //saveMesh((const char*) _filename.toAscii(), *(mesh_obj->mesh()), compatibility_mode);
            fileManager_.writeFile(_filename.toStdString(), *(mesh_obj->mesh()));

        }
//        else if (hex_mesh_obj) {
//
//            obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(), -1));
//            obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2));
//
//            bool compatibility_mode = false;
//            if(!OpenFlipper::Options::nogui()) {
//                compatibility_mode = saveCompMode_->isChecked();
//            }
//
//            saveMesh((const char*) _filename.toAscii(), *(hex_mesh_obj->mesh()), compatibility_mode);
//        }
    }

    return true;
}

//----------------------------------------------------------------------------


void FileOpenVolumeMeshPlugin::loadIniFileLast(INIFile& _ini, int _id) {

    BaseObjectData* baseObject;
    if (!PluginFunctions::getObject(_id, baseObject)) {
        emit log(LOGERR, tr("Cannot find object for id %1 in saveFile!").arg(_id));
        return;
    }

    PolyhedralMeshObject* object = PluginFunctions::polyhedralMeshObject(baseObject);

    if (object) {
        ACG::Vec4f col(0.0, 0.0, 0.0, 0.0);

        if (_ini.get_entryVecf(col, object->name(), "BaseColor"))
            object->materialNode()->set_base_color(col);
    }

}

//----------------------------------------------------------------------------

void FileOpenVolumeMeshPlugin::saveIniFile(INIFile& _ini, int _id) {

    BaseObjectData* baseObject;
    if (!PluginFunctions::getObject(_id, baseObject)) {
        emit log(LOGERR, tr("Cannot find object for id %1 in saveFile!").arg(_id));
        return;
    }

    PolyhedralMeshObject* object = PluginFunctions::polyhedralMeshObject(baseObject);

    if (object) {
        _ini.add_entryVec(object->name(), "BaseColor", object->materialNode()->base_color());
    }
}

//----------------------------------------------------------------------------

QWidget* FileOpenVolumeMeshPlugin::saveOptionsWidget(QString _currentFilter) {

    return saveOptions_;
}

//----------------------------------------------------------------------------

QWidget* FileOpenVolumeMeshPlugin::loadOptionsWidget(QString _currentFilter) {

    return loadOptions_;
}

Q_EXPORT_PLUGIN2(fileopenvolumemeshplugin, FileOpenVolumeMeshPlugin)

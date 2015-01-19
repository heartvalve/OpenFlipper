/*===========================================================================*\
*                                                                            *
 *                              OpenFlipper                                   *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
 *                           www.openflipper.org                              *
 *                                                                            *
 *--------------------------------------------------------------------------- *
 *  This file is part of OpenFlipper.                                         *
 *                                                                            *
 *  OpenFlipper is free software: you can redistribute it and/or modify       *
 *  it under the terms of the GNU Lesser General Public License as            *
 *  published by the Free Software Foundation, either version 3 of            *
 *  the License, or (at your option) any later version with the               *
 *  following exceptions:                                                     *
 *                                                                            *
 *  If other files instantiate templates or use macros                        *
 *  or inline functions from this file, or you compile this file and          *
 *  link it with other files to produce an executable, this file does         *
 *  not by itself cause the resulting executable to be covered by the         *
 *  GNU Lesser General Public License. This exception does not however        *
 *  invalidate any other reasons why the executable file might be             *
 *  covered by the GNU Lesser General Public License.                         *
 *                                                                            *
 *  OpenFlipper is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU Lesser General Public License for more details.                       *
 *                                                                            *
 *  You should have received a copy of the GNU LesserGeneral Public           *
 *  License along with OpenFlipper. If not,                                   *
 *  see <http://www.gnu.org/licenses/>.                                       *
 *                                                                            *
 \*===========================================================================*/

/*===========================================================================*\
*                                                                            *
 *   $Revision: 13576 $                                                       *
 *   $LastChangedBy: kremer $                                                 *
 *   $Date: 2012-01-30 11:22:27 +0100 (Mo, 30 Jan 2012) $                    *
 *                                                                            *
 \*===========================================================================*/

#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <ACG/GL/GLState.hh>

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#include "FileOpenVolumeMesh.hh"

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QFileInfo>


FileOpenVolumeMeshPlugin::FileOpenVolumeMeshPlugin() :
loadOptions_(0),
saveOptions_(0),
typeCheck_(0),
loadCompMode_(0),
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
    loadTopCheck_ = new QCheckBox("Perform topology checks");
    llayout->addWidget(typeCheck_);
    llayout->addWidget(loadCompMode_);
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
    return QString(tr("Polyhedral Volume Mesh files ( *.ovm *.polyvolmesh *.tetmesh )"));
}
;

//----------------------------------------------------------------------------


QString FileOpenVolumeMeshPlugin::getSaveFilters() {
    return QString(tr("Polyhedral Volume Mesh files ( *.ovm )"));
}
;

//----------------------------------------------------------------------------


DataType FileOpenVolumeMeshPlugin::supportedType() {

    DataType type = DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH;
    return type;
}

//----------------------------------------------------------------------------


int FileOpenVolumeMeshPlugin::loadObject(QString _filename) {

    bool compatibility_mode = false;
    if(!OpenFlipper::Options::nogui()) {
        compatibility_mode = loadCompMode_->isChecked();
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
        hexMesh = true;
    }

    if(hexMesh) {

        emit addEmptyObject(DATA_HEXAHEDRAL_MESH, id);
        HexahedralMeshObject* obj(0);

        if (PluginFunctions::getObject(id, obj)) {

            if(compatibility_mode) {

                loadMesh((const char*) _filename.toLatin1(), *(obj->mesh()), compatibility_mode,
                         topology_checks);

            } else {
                if(!fileManager_.readFile(_filename.toStdString(), *(obj->mesh()),
                                          topology_checks,true)) {
                    emit log(LOGERR, QString("Could not open file %1!").arg(_filename));
                }
            }

            obj->setFromFileName(_filename);
            obj->setName(obj->filename());

            // Compute face normals
            emit updatedObject(obj->id(), UPDATE_ALL);
        }

        emit openedFile(obj->id());

        // Go into solid flat shaded mode
        obj->setObjectDrawMode(ACG::SceneGraph::DrawModes::getDrawMode("Cells (flat shaded)"));

        // Scale polyhedra a bit
        obj->meshNode()->set_scaling(0.8);

    } else {

        emit addEmptyObject(DATA_POLYHEDRAL_MESH, id);
        PolyhedralMeshObject* obj(0);

        if (PluginFunctions::getObject(id, obj)) {

            if(compatibility_mode) {

                loadMesh((const char*) _filename.toLatin1(), *(obj->mesh()), compatibility_mode,
                         topology_checks);

            } else {
                if(!fileManager_.readFile(_filename.toStdString(), *(obj->mesh()),
                                          topology_checks,true)) {
                    emit log(LOGERR, QString("Could not open file %1!").arg(_filename));
                }
            }
            obj->setFromFileName(_filename);
            obj->setName(obj->filename());

            // Compute face normals
            emit updatedObject(obj->id(), UPDATE_ALL);
        }

        emit openedFile(obj->id());

        // Go into solid flat shaded mode
        obj->setObjectDrawMode(ACG::SceneGraph::DrawModes::getDrawMode("Cells (flat shaded)"));

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
        HexahedralMeshObject* hex_mesh_obj = PluginFunctions::hexahedralMeshObject(obj);
        if (mesh_obj) {

          obj->setFromFileName(_filename);
          obj->setName(obj->filename());
          if(!fileManager_.writeFile(_filename.toStdString(), *(mesh_obj->mesh()))) {
            emit log(LOGERR, tr("Unable to save ") + _filename);
            return false;
          }
        }
        else if (hex_mesh_obj) {

          obj->setFromFileName(_filename);
          obj->setName(obj->filename());
          if (!fileManager_.writeFile(_filename.toStdString(), *(hex_mesh_obj->mesh()))) {
            emit log(LOGERR, tr("Unable to save ") + _filename);
            return false;
          }
        }

        return true;

    } else {
      emit log(LOGERR, tr("saveObject : cannot get object id %1 for save name %2").arg(_id).arg(_filename) );
      return false;
    }


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
#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(fileopenvolumemeshplugin, FileOpenVolumeMeshPlugin)
#endif


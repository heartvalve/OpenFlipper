/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QFileInfo>
#include <QSettings>

#include "FileSTL.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

/// Constructor
FileSTLPlugin::FileSTLPlugin() :
        saveOptions_(0),
        loadOptions_(0),
        saveBinary_(0),
        savePrecisionLabel_(0),
        savePrecision_(0),
        loadFaceNormal_(0),
        saveDefaultButton_(0),
        loadDefaultButton_(0)

{
}

//-----------------------------------------------------------------------------------------------------

void FileSTLPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileSTLPlugin::getLoadFilters() {
    return QString( tr("Stereolithography files ( *.stl *.stla *.stlb )") );
};

//-----------------------------------------------------------------------------------------------------

QString FileSTLPlugin::getSaveFilters() {
    return QString( tr("Stereolithography files ( *.stl *.stla *.stlb )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileSTLPlugin::supportedType() {
    DataType type = DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

int FileSTLPlugin::loadObject(QString _filename) {

    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);

    TriMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {

        if (PluginFunctions::objectCount() == 1 )
            object->target(true);

        object->setFromFileName(_filename);
        object->setName(object->filename());

        std::string filename = std::string( _filename.toUtf8() );

        object->mesh()->request_face_normals();

        bool loadNormals( (loadFaceNormal_ && loadFaceNormal_->isChecked()) ||
                          (!loadFaceNormal_ && OpenFlipperSettings().value("FileSTL/Load/FaceNormal", true).toBool())
            );

        // load file
        OpenMesh::IO::Options opt;
        // load face normals from the stl file if requested
        if (loadNormals) {
          opt += OpenMesh::IO::Options::FaceNormal;
        }

        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileSTL : Read error for stl mesh.\n";
            emit deleteObject( object->id() );
            return -1;

        }

        // only calculate the face normals if they are not read from the file
        if (!loadNormals || !opt.face_has_normal())
          object->mesh()->update_normals();
        else {
          if (object->mesh()->has_vertex_normals())
            object->mesh()->update_vertex_normals();
          if (object->mesh()->has_halfedge_normals())
            object->mesh()->update_halfedge_normals();
        }

        object->update();
        object->show();

        emit openedFile( object->id() );

        // Update viewport
        PluginFunctions::viewAll();

        return object->id();

    } else {
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
};

//-----------------------------------------------------------------------------------------------------

bool FileSTLPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);

    std::string filename = std::string( _filename.toUtf8() );

    if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

        object->setFromFileName(_filename);
        object->setName(object->filename());

        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );

        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;

        std::streamsize precision = 6;
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){

            if (!OpenFlipper::Options::nogui() && saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;

            if (!saveBinary_->isChecked())
                precision = savePrecision_->value();

        }

        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(), opt, precision) ) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename );
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a triangle mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileSTLPlugin::saveOptionsWidget(QString /*_currentFilter*/) {

    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);

        saveBinary_ = new QCheckBox("Save Binary");
        layout->addWidget(saveBinary_);

        savePrecisionLabel_ = new QLabel("Writer Precision");
        layout->addWidget(savePrecisionLabel_);

        savePrecision_ = new QSpinBox();
        savePrecision_->setMinimum(1);
        savePrecision_->setMaximum(12);
        savePrecision_->setValue(6);
        layout->addWidget(savePrecision_);

        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);

        saveOptions_->setLayout(layout);

        saveBinary_->setChecked( OpenFlipperSettings().value( "FileSTL/Save/Binary", false ).toBool() );

        connect(saveBinary_, SIGNAL(clicked(bool)), savePrecision_, SLOT(setDisabled(bool)));
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));

    }

    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileSTLPlugin::loadOptionsWidget(QString /*_currentFilter*/) {

    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);

        loadFaceNormal_ = new QCheckBox("Load Face Normals");
        layout->addWidget(loadFaceNormal_);

        loadFaceNormal_->setChecked( OpenFlipperSettings().value("FileSTL/Load/FaceNormal",true).toBool()  );

        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);

        loadOptions_->setLayout(layout);

        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
    }

    return loadOptions_;
}

//-----------------------------------------------------------------------------------------------------

void FileSTLPlugin::slotLoadDefault() {
  OpenFlipperSettings().setValue( "FileSTL/Load/FaceNormal",   loadFaceNormal_->isChecked()  );
  OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}

//-----------------------------------------------------------------------------------------------------

void FileSTLPlugin::slotSaveDefault() {
    OpenFlipperSettings().setValue( "FileSTL/Save/Binary",      saveBinary_->isChecked()  );
}
#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( filestlplugin , FileSTLPlugin );
#endif



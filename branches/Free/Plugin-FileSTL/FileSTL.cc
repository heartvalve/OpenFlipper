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

#include <QtGui>
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
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename );
        if (!ok)
        {
            std::cerr << "Plugin FileSTL : Read error for stl mesh.\n";
            emit deleteObject( object->id() );
            return -1;
            
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit openedFile( object->id() );
        
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
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
            if (!OpenFlipper::Options::nogui() && saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(), opt) ) {
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
                
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);
        
        saveOptions_->setLayout(layout);
        
        saveBinary_->setChecked( OpenFlipperSettings().value( "FileSTL/Save/Binary", false ).toBool() );
        
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
        
        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
    }
    
    return loadOptions_;
}

//-----------------------------------------------------------------------------------------------------

void FileSTLPlugin::slotLoadDefault() {
  OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );    
}

//-----------------------------------------------------------------------------------------------------

void FileSTLPlugin::slotSaveDefault() {
    OpenFlipperSettings().setValue( "FileSTL/Save/Binary",      saveBinary_->isChecked()  );
}

Q_EXPORT_PLUGIN2( filestlplugin , FileSTLPlugin );


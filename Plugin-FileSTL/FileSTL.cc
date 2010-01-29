
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
FileSTLPlugin::FileSTLPlugin()
: saveOptions_(0),
  loadOptions_(0) {
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
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
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
        
        emit log(LOGINFO,object->getObjectinfo());
        
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
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
        
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
            if (saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(), opt) ) {
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a triangle mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileSTLPlugin::saveOptionsWidget(QString _currentFilter) {
    
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


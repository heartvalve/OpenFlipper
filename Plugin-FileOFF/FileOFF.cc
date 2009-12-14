
#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileOFF.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

/// Constructor
FileOFFPlugin::FileOFFPlugin()
: loadOptions_(0),
triMeshHandling_(0) {
}

//-----------------------------------------------------------------------------------------------------

void FileOFFPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileOFFPlugin::getLoadFilters() {
    return QString( tr("Object File Format files ( *.off )") );
};

//-----------------------------------------------------------------------------------------------------

QString FileOFFPlugin::getSaveFilters() {
    return QString( tr("Object File Format files ( *.off )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileOFFPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

int FileOFFPlugin::loadObject(QString _filename) {

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
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
            std::cerr << "Plugin FileOFF : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        } else {
            
            int triMeshControl = 1; // 1 == keep polyMesh .. do nothing
            
            if ( OpenFlipper::Options::gui() ){
                if ( triMeshHandling_ != 0 ){
                    triMeshControl = triMeshHandling_->currentIndex();
                } else
                    triMeshControl = 0;
            }
            
            //check if it's actually a triangle mesh
            if (triMeshControl != 1){ // 1 == do nothing
                
                PolyMesh& mesh = *( object->mesh() );
                
                bool isTriangleMesh = true;
                
                for ( PolyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end() ; ++f_it) {
                    
                    // Count number of vertices for the current face
                    uint count = 0;
                    for ( PolyMesh::FaceVertexIter fv_it( mesh,f_it); fv_it; ++fv_it )
                        ++count;
                    
                    // Check if it is a triangle. If not, this is really a poly mesh
                    if ( count != 3 ) {
                        isTriangleMesh = false;
                        break;
                    }
                    
                }
                
                // Mesh loaded as polymesh is actually a triangle mesh. Ask the user to reload as triangle mesh or keep it as poly mesh.
                if ( isTriangleMesh )
                    
                    if ( triMeshControl == 0){ //ask what to do
                        
                        QMessageBox::StandardButton result = QMessageBox::question ( 0,
                            tr("TriMesh loaded as PolyMesh"),
                            tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
                            (QMessageBox::Yes | QMessageBox::No ),
                            QMessageBox::Yes );
                            
                            // User decided to reload as triangle mesh
                            if ( result == QMessageBox::No )
                                isTriangleMesh = false;
                    }
                    
                    if ( isTriangleMesh ){
                        std::cerr << "Plugin FileOFF : Reloading mesh as Triangle Mesh\n";
                        
                        emit deleteObject( object->id() );
                        
                        return loadTriMeshObject(_filename);
                    }
            }
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
};

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FileOFFPlugin::loadTriMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if ( PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            //ColorAlpha is only checked if loading binary off's
            if (loadAlpha_->isChecked())
                opt += OpenMesh::IO::Options::ColorAlpha;
            
            if (loadNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        }else{
            //let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOFF : Read error for Triangle Mesh\n";
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
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FileOFFPlugin::loadPolyMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
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
            std::cerr << "Plugin FileOFF : Read error for Poly Mesh\n";
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
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);
    
    std::string filename = std::string( _filename.toUtf8() );
    
    if ( object->dataType( DATA_POLY_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );
        
        if (OpenMesh::IO::write_mesh(*polyObj->mesh(), filename.c_str()) ){
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        }else{
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
        
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
            if (saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
            if (saveVertexColor_->isChecked()){
                opt += OpenMesh::IO::Options::VertexColor;
            }
            
            if (saveFaceColor_->isChecked()){
                opt += OpenMesh::IO::Options::FaceColor;
            }
            
            if (saveAlpha_->isChecked()){
                opt += OpenMesh::IO::Options::ColorAlpha;
            }
            
            if (saveNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (saveTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(),opt) ) {
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOFFPlugin::saveOptionsWidget(QString _currentFilter) {
    
    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        if( ! _currentFilter.contains("Wavefront") ){ //dont add 'save binary' for obj
            saveBinary_ = new QCheckBox("Save Binary");
            layout->addWidget(saveBinary_);
        }
        
        saveVertexColor_ = new QCheckBox("Save Vertex Colors");
        layout->addWidget(saveVertexColor_);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
        
        saveAlpha_ = new QCheckBox("Save Color Alpha");
        layout->addWidget(saveAlpha_);
        
        saveNormals_ = new QCheckBox("Save Normals");
        layout->addWidget(saveNormals_);
        
        saveTexCoords_ = new QCheckBox("Save TexCoords");
        layout->addWidget(saveTexCoords_);
        
        saveOptions_->setLayout(layout);
    } else {
        //adjust widget
        if( _currentFilter.contains("Wavefront") ) //dont add 'save binary' for obj
            saveBinary_->setVisible( false );
        else
            saveBinary_->setVisible( true );
    }
    
    saveBinary_->setChecked( Qt::Unchecked );
    saveVertexColor_->setChecked( Qt::Unchecked );
    saveFaceColor_->setChecked( Qt::Unchecked );
    saveAlpha_->setChecked( Qt::Unchecked );
    saveNormals_->setChecked( Qt::Unchecked );
    saveTexCoords_->setChecked( Qt::Unchecked );
    
    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOFFPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        QLabel* label = new QLabel(tr("If PolyMesh is a Triangle Mesh:"));
        
        layout->addWidget(label);
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Open as PolyMesh") );
        triMeshHandling_->addItem( tr("Open as TriangleMesh") );
        
        layout->addWidget(triMeshHandling_);
        
        loadVertexColor_ = new QCheckBox("Load Vertex Colors");
        layout->addWidget(loadVertexColor_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);
        
        loadAlpha_ = new QCheckBox("Load Color Alpha");
        layout->addWidget(loadAlpha_);
        
        loadNormals_ = new QCheckBox("Load Normals");
        layout->addWidget(loadNormals_);
        
        loadTexCoords_ = new QCheckBox("Load TexCoords");
        layout->addWidget(loadTexCoords_);
        
        loadOptions_->setLayout(layout);
    }
    
    loadVertexColor_->setChecked( Qt::Checked );
    loadFaceColor_->setChecked( Qt::Checked );
    loadAlpha_->setChecked( Qt::Checked );
    loadNormals_->setChecked( Qt::Checked );
    loadTexCoords_->setChecked( Qt::Checked );
    
    return loadOptions_;
}

Q_EXPORT_PLUGIN2( fileoffplugin , FileOFFPlugin );


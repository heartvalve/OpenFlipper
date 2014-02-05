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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef FILEOBJPLUGIN_HH
#define FILEOBJPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef ENABLE_BSPLINECURVE_SUPPORT
#include <ObjectTypes/BSplineCurve/BSplineCurve.hh>
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
#include <ObjectTypes/BSplineSurface/BSplineSurface.hh>
#endif

#include "OBJImporter.hh"
#include "Material.hh"

enum ReaderMode
{
  NONE    = 0,
  CURVE   = 1,
  SURFACE = 1 << 1
};

class FileOBJPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
    LoggingInterface, ScriptInterface, StatusbarInterface, RPCInterface, TextureInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(StatusbarInterface)
   Q_INTERFACES(RPCInterface)
   Q_INTERFACES(TextureInterface)
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-FileOBJ")
#endif
  signals:
    void openedFile( int _id );
    void addEmptyObject( DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id , QString _filename );
    void log(Logtype _type, QString _message);
    void log(QString _message);
    void updateView();

    void deleteObject( int _id );

    // StatusbarInterface
    void showStatusMessage(QString _message, int _timeout = 0);
    void setStatus( ApplicationStatus::applicationStatus _status);

    //RPCInterface
    void pluginExists( QString _pluginName , bool& _exists  );

    //TextureInterface
    void setTextureMode(QString _textureName, QString _mode, int _id );
    void switchTexture( QString _textureName, int _id );
    void addMultiTexture( QString _textureGroup, QString _name, QString _filename, int _id, int& _textureId);
    void textureFilename( int /*_id*/, QString /*_textureName*/, QString& /*_textureFilename*/ );
    void textureIndex(QString _name,int _id, int& _index);
    void getCurrentTexture(int /*_id*/, QString& /*_name*/);
    void textureName (int, int, QString &);
    void getSubTextures (int, QString, QStringList &);
    void textureIndexPropertyName(int, QString&);

  private slots:

    void fileOpened( int /*_id*/ ){};

    void noguiSupported( ) {} ;

    void initializePlugin();

    /// Slot called when user wants to save the given Load options as default
    void slotLoadDefault();

    /// Slot called when user wants to save the given Save options as default
    void slotSaveDefault();

    void slotHandleCheckBoxes(bool _checked);

  public :

     FileOBJPlugin();

     ~FileOBJPlugin() {};

     QString name() { return (QString("FileOBJ")); };
     QString description( ) { return (QString(tr("Load/Save OBJ-Files"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/);
     QWidget* loadOptionsWidget(QString /*_currentFilter*/);

  public slots:

    /// Loads Object and converts it to a triangle mesh if possible
    int loadObject(QString _filename);

    /// load object and force type
    int loadObject(QString _filename, DataType _type);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };

  private:

    /// Reader functions
    void checkTypes(QString _filename, OBJImporter& _importer, QStringList& _includes);

    bool readMaterial(QString _filename, OBJImporter& _importer);
    void readOBJFile(QString _filename, OBJImporter& _importer);
    void createAllGroupObjects(OBJImporter& _importer);
    //void addNewObject(OBJImporter& _importer, QString _name );
    template <class MeshT>
    void backupTextureCoordinates(MeshT& _mesh);
    void addTextures(OBJImporter& _importer, int _objectID );

    /// Convert non-valid filenames (e.g. of groups that end with .jpg) to valid .objs.
    void convertToOBJName(QString& _name);

  private :

    /// List that contains the material properties
    MaterialList materials_;

    template< class MeshT >
    Material& getMaterial(MeshT& _mesh, const OpenMesh::FaceHandle& _fh, int _objId);

    ///writer functions
    template< class MeshT >
    bool writeMaterial(QString _filename, MeshT& _mesh, int _objId );

    template< class MeshT >
    bool writeMesh(std::ostream& _out, QString _filename, MeshT& _mesh, int _objId);

    #ifdef ENABLE_BSPLINECURVE_SUPPORT
    bool writeCurve(std::ostream& _out, QString _filename, BSplineCurve* _curve );
    #endif

    #ifdef ENABLE_BSPLINESURFACE_SUPPORT
    bool writeSurface(std::ostream& _out, QString _filename, BSplineSurface* _surface );
    #endif


  private:

    //Option Widgets
    QWidget* loadOptions_;
    QWidget* saveOptions_;

    QCheckBox*   saveBinary_;
    QCheckBox*   saveVertexColor_;
    QCheckBox*   saveFaceColor_;
    QCheckBox*   saveAlpha_;
    QCheckBox*   saveNormals_;
    QCheckBox*   saveTexCoords_;
    QCheckBox*   saveTextures_;
    QCheckBox*   saveCopyTextures_;
    QCheckBox*   saveCreateTexFolder_;
    QLabel*      savePrecisionLabel_;
    QSpinBox*    savePrecision_;
    QPushButton* saveDefaultButton_;


    QComboBox*   triMeshHandling_;
    QCheckBox*   loadVertexColor_;
    QCheckBox*   loadFaceColor_;
    QCheckBox*   loadAlpha_;
    QCheckBox*   loadNormals_;
    QCheckBox*   loadTexCoords_;
    QCheckBox*   loadTextures_;
    QPushButton* loadDefaultButton_;

    bool forceTriangleMesh_;
    bool forcePolyMesh_;

    QString textureIndexPropertyName_;
    bool    textureIndexPropFetched_;
    std::map<int,QString> texIndexFileMap_;
};


#if defined(INCLUDE_TEMPLATES) && !defined(FILEOBJPLUGIN_C)
#define FILEOBJPLUGIN_TEMPLATES
#include "FileOBJT.cc"
#endif


#endif //FILEOBJPLUGIN_HH

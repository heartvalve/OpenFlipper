//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef TEXTURECONTROLPLUGIN_HH
#define TEXTURECONTROLPLUGIN_HH

#define OM_FORCE_STATIC_CAST

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>

#include <OpenFlipper/common/Types.hh>
#include "textureProperties.hh"

struct Texture {
  QString name;
  QString filename;
  uint dimension;

  double clamp_min;
  double clamp_max;

  bool clamp;
  bool repeat;

  double max_val;

  bool center;

  bool abs;

  bool scale;

  uint type;
};

class TextureControlPlugin : public QObject, BaseInterface, TextureInterface, MenuInterface, LoggingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(LoggingInterface)

  public:
    enum TextureType { VERTEXBASED = 1 << 0, HALFEDGEBASED = 1 << 1};

  signals:

    void update_view();
    void updateTexture( QString , int);

    void log(Logtype _type, QString _message);
    void log(QString _message);

    void addMenu(QMenu* _menu, MenuType _type);

  private slots:
    void slotObjectUpdated(int _identifier);
    void slotUpdateAllTextures( );
    void slotTextureAdded( QString _textureName , QString _filename , uint _dimension );
    void slotTextureUpdated( QString _textureName , int _identifier );
    void slotSetTextureMode(QString _textureName ,QString _mode);
    void slotSwitchTexture( QString _textureName );

  private slots:
    void slotSetTextureProperties();
    void slotTextureMenu(QAction* _action);

    void slotTexturePropertiesOk();
    void slotTexturePropertiesCancel();
    void slotTexturePropertiesApply();

    void pluginsInitialized();

  public :

    ~TextureControlPlugin() {delete(settingsDialog_);};

    QString name() { return (QString("TextureControl")); };
    QString description( ) { return (QString("Handles Textures which are written to mesh properties")); };

  private :
    QMenu *textureMenu_;

    QActionGroup* actionGroup_;

    QAction* AC_Texture_Settings;

    QString activeTexture_;

    std::vector< Texture > textures_;

    std::vector<QAction*> textureActions_;

    texturePropertiesWidget* settingsDialog_;

    /// Set the active texture values to the values in the dialog box.
    void applyDialogSettings();

    /// Update the values in the Dialog box to the current ones
    void updateDialog();

    /// For a given mesh compute the minimum and maximum values depending on texture settings (vertex based)
    template< typename MeshT >
    inline
    void computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::VPropHandleT< double > _texture,double& _min , double& _max);
    /// For a given mesh compute the minimum and maximum values depending on texture settings (halfedge based)
    template< typename MeshT >
    inline
    void computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::HPropHandleT< double > _texture,double& _min , double& _max);

    /// Take a scalar value and return a clamped,... depending on texture settings
    inline
    void computeValue(int _textureid, double _min, double _max, double& _value);

    /// Calls the correct \a copyTexture() function to copy the texture property into the displayed OM property
    template< typename MeshT >
    void doUpdateTexture ( int _textureid, MeshT& _mesh);

    /// Copy the supplied 1D vertex property to both coordinates of the 2D vertex OM texture property
    template< typename MeshT >
    void copyTexture(int _textureid, MeshT& _mesh, OpenMesh::VPropHandleT< double > _texProp );
    /// Copy the supplied 1D halfedge property to both coordinates of the 2D halfedge OM texture property
    template< typename MeshT >
    void copyTexture(int _textureid, MeshT& _mesh, OpenMesh::HPropHandleT< double > _texProp );
    /// Copy the supplied 2D vertex property to the 2D vertex OM property
    template< typename MeshT >
    void copyTexture(int _textureid, MeshT& _mesh, OpenMesh::VPropHandleT< ACG::Vec2d > _texProp );
    /// Copy the supplied 2D halfedge property to the 2D halfedge OM property
    template< typename MeshT >
    void copyTexture(int _textureid, MeshT& _mesh, OpenMesh::HPropHandleT< ACG::Vec2d > _texProp );

  public slots:
    QString version() { return QString("1.0"); };
 };

#endif //TEXTURECONTROLPLUGIN_HH

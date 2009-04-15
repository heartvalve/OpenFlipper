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
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenFlipper/common/Types.hh>
#include "textureProperties.hh"

#include "TextureData.hh"
/*
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
};*/

class TextureControlPlugin : public QObject, BaseInterface, TextureInterface, MenuInterface, LoggingInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)

  public:
    enum TextureType { VERTEXBASED = 1 << 0, HALFEDGEBASED = 1 << 1};

  signals:
    // BaseInterface
    void updateView();
    // TextureInterface
    void updateTexture( QString , int);
    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    // MenuInterface
    void addMenubarAction(QAction* _action, MenuActionType _type );



  private slots:
    // BaseInterface
    void pluginsInitialized();
    void slotObjectUpdated(int _identifier);
    // TextureInterface
    void slotUpdateAllTextures( );
    void slotTextureAdded( QString _textureName , QString _filename , uint _dimension );
    void slotTextureUpdated( QString _textureName , int _identifier );
    void slotSetTextureMode(QString _textureName ,QString _mode);
    void slotSwitchTexture( QString _textureName );

    // LoadSaveInterface
    void fileOpened( int _id );

  private slots:
    /// Called when an action in the TextureMenu is triggered
    void slotTextureMenu(QAction* _action);

    /// Slot for showing the TextureProperties Dialog
    void slotSetTextureProperties();
    /// Called when Ok was clicked in the dialog
    void slotTexturePropertiesOk();
    /// Called when Cancel was clicked in the dialog
    void slotTexturePropertiesCancel();
    /// Called when Apply was clicked in the dialog
    void slotTexturePropertiesApply();

  public :
    /// Constructor
    TextureControlPlugin() {};
    /// Destructor
    ~TextureControlPlugin() {delete(settingsDialog_);};

    QString name() { return (QString("TextureControl")); };
    QString description( ) { return (QString("Handles Textures which are written to mesh properties")); };

  private :
    QMenu *textureMenu_;

    QActionGroup* actionGroup_;

    QAction* AC_Texture_Settings;

    QString activeTexture_;

    TextureData globalTextures_;

    std::vector<QAction*> textureActions_;

    texturePropertiesWidget* settingsDialog_;

    /// Set the active texture values to the values in the dialog box.
    void applyDialogSettings();

    /// Update the values in the Dialog box to the current ones
    void updateDialog();


    /// Take a scalar value and return a clamped,... depending on texture settings
    inline
    void computeValue(Texture& _texture, double _min, double _max, double& _value);

    /// Calls the correct \a copyTexture() function to copy the texture property into the displayed OM property
    template< typename MeshT >
    void doUpdateTexture ( Texture& _texture , MeshT& _mesh);

    template< typename MeshT >
    void getOriginalHistogram(std::vector< double>& _x, std::vector< double>& _y,
                              int _textureid, MeshT& _mesh,
                              OpenMesh::VPropHandleT< double > _texProp);

    //===========================================================================
    /** @name 1D Texture copy functions( copy 1D input textures to target texture coordinates and transform values )
      * @{ */
    //===========================================================================

    /// Copy the supplied 1D vertex property to both coordinates of the 2D vertex OM texture property
    template< typename MeshT >
    void copyTexture(Texture& _texture , MeshT& _mesh, OpenMesh::VPropHandleT< double > _texProp );

    /// Copy the supplied 1D halfedge property to both coordinates of the 2D halfedge OM texture property
    template< typename MeshT >
    void copyTexture(Texture& _texture , MeshT& _mesh, OpenMesh::HPropHandleT< double > _texProp );


    /// For a given mesh compute the minimum and maximum values depending on texture settings (vertex based)
    template< typename MeshT >
    inline
    void computeMinMaxScalar(Texture& _textureData , MeshT& _mesh ,OpenMesh::VPropHandleT< double > _texture,double& _min , double& _max);

    /// For a given mesh compute the minimum and maximum values depending on texture settings (halfedge based)
    template< typename MeshT >
    inline
    void computeMinMaxScalar(Texture& _textureData , MeshT& _mesh,OpenMesh::HPropHandleT< double > _texture,double& _min , double& _max);

    /** @} */

    //===========================================================================
    /** @name 2D Texture copy functions( copy 2D input textures to target texture coordinates and transform values )
      * @{ */
    //===========================================================================


    /// Copy the supplied 2D vertex property to the 2D vertex OM property
    template< typename MeshT >
    void copyTexture(Texture& _texture, MeshT& _mesh, OpenMesh::VPropHandleT< ACG::Vec2d > _texProp );
    /// Copy the supplied 2D halfedge property to the 2D halfedge OM property
    template< typename MeshT >
    void copyTexture(Texture& _texture, MeshT& _mesh, OpenMesh::HPropHandleT< ACG::Vec2d > _texProp );

    /** @} */

  public slots:
    QString version() { return QString("1.0"); };
 };

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(TEXTURECONTROL_1D_TEXTURE_HANDLING_C)
#define TEXTURECONTROL_1D_TEXTURE_HANDLING_TEMPLATES
#include "TextureControl1DTextureHandlingT.cc"
#endif

#if defined(INCLUDE_TEMPLATES) && !defined(TEXTURECONTROL_2D_TEXTURE_HANDLING_C)
#define TEXTURECONTROL_2D_TEXTURE_HANDLING_TEMPLATES
#include "TextureControl2DTextureHandlingT.cc"
#endif

#if defined(INCLUDE_TEMPLATES) && !defined(TEXTURECONTROL_HISTOGRAMS_C)
#define TEXTURECONTROL_HISTOGRAMS_TEMPLATES
#include "TextureControlHistogramsT.cc"
#endif


#endif //TEXTURECONTROLPLUGIN_HH

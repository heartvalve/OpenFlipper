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
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenFlipper/common/Types.hh>
#include "textureProperties.hh"

#include "TextureData.hh"

class TextureControlPlugin : public QObject, BaseInterface, TextureInterface, MenuInterface, LoggingInterface, LoadSaveInterface,ContextMenuInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(ContextMenuInterface)

  public:

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

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action ,DataType _objectType , ContextMenuType _type );


  private slots:

    // BaseInterface
    void pluginsInitialized();
    void slotObjectUpdated(int _identifier);

    // TextureInterface
    void slotUpdateAllTextures( );
    void slotTextureAdded( QString _textureName , QString _filename , uint _dimension , int _id );
    void slotTextureAdded( QString _textureName , QString _filename , uint _dimension );
    void slotMultiTextureAdded( QString _textureGroup , QString _name , QString _filename , int _id , int& _textureId );
    void slotTextureUpdated( QString _textureName , int _identifier );
    void slotSetTextureMode(QString _textureName ,QString _mode, int _id);
    void slotSetTextureMode(QString _textureName ,QString _mode);
    void slotSwitchTexture( QString _textureName, int _id );
    void slotSwitchTexture( QString _textureName );

    // LoadSaveInterface
    void fileOpened( int _id );
    void addedEmptyObject( int _id );

  private slots:

    void doSwitchTexture( QString _textureName, int _id );

    /// Called when an action in the TextureMenu is triggered
    void slotTextureMenu(QAction* _action);

    /// Slot for showing the TextureProperties Dialog
    void slotSetTextureProperties();

  public :
    /// Constructor
    TextureControlPlugin() {};
    /// Destructor
    ~TextureControlPlugin() {delete(settingsDialog_);};

    QString name() { return (QString("TextureControl")); };
    QString description( ) { return (QString("Handles Textures which are written to mesh properties")); };

  private :

    bool StringToBool(QString _value);

    /// Load an Image from disk ( if filename contains leading "/ " its consedered global otherwise in texturedir)
    void getImage( QString _fileName, QImage& _image );

    /// Checks for a correct drawmode and changes if necessary
    void switchDrawMode( bool _multiTexture );

    TextureData globalTextures_;

    texturePropertiesWidget* settingsDialog_;

  private slots:
    void applyDialogSettings(TextureData* _texData, QString _textureName, int _id);

    //compute histogram for the given texture property
    void getCoordinates1D(QString _textureName, int _id, std::vector< double >& _x );

  private:

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

    //===========================================================================
    /** @name Global texture menu
      * @{ */
    //===========================================================================

  private:
    // Global Texture menu
    QMenu *textureMenu_;

    // Action group for global texture menu
    QActionGroup* actionGroup_;

    // All actions in the global texture menu
    std::vector<QAction*> textureActions_;

    /** @} */

    //===========================================================================
    /** @name Context Menu
      * @{ */
    //===========================================================================

  private slots:

    /** \brief Slot from the context menu interface.
     *
     * This slot is called, when the core requests a context menu and creates and updates
     * the context menu.
     */
    void slotUpdateContextMenu( int _objectId );


    /** \brief Called when the context menu has been triggered
     *
     *  This slot is called when the per object context menu is triggered.
     *  It will switch to the requested texture and mode.
     */
    void slotTextureContextMenu( QAction * _action );

  private:
    /// Stores the per object context menu
    QMenu* contextMenu_;

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

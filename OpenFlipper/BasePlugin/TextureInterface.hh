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




//
// C++ Interface: TexturePlugin
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef TEXTUREINTERFACE_HH 
#define TEXTUREINTERFACE_HH 

 #include <QtGui>
 #include <QMenuBar>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>
 
 /**
  * This Interface should be used by plugins which will provide a texture. The texture coordinates have
  * to be written into a mesh property. You do not need to write texture information into the texture
  * coordinates of the mesh. As other plugins may provide textures to, the visualization of the 
  * textures is handled by the main application (or a texture control plugin).
 */
class TextureInterface {
   signals :
      
      /** \brief Emit this Signal if a texture has been added (Property Name,filename,Dimension)
       * 
       *  @param  _name Name of the property which contains the tex coords (double or vec2d)
       *  @param _filename Filename of the texture ( "./Textures/<name>")
       *  @param _dimension 1D texture ( currenty only supports 1 )
       */
      virtual void addTexture( QString /*_name*/ , QString /*_filename*/ , uint /*_dimension*/ ) {};
      
      /** \brief Tell Plugins to update the given texture for the given identifier
       */
      virtual void updateTexture( QString /*_textureName*/  , int /*_identifier*/) {};
      
      /** \brief Tell Plugins to update all textures
       */
      virtual void updateAllTextures( ) {};
      
      /** \brief emit this signal if you updated a texture 
       * 
       *  Give the name of the texture and the id of the object or -1 if all objects were update      
      */
      virtual void updatedTextures( QString , int ) {};
      
      /** \brief emit this signal if you want to switch the viewer to a specific texture mode
       * 
       *  Give the name of the texture and the id of the object or -1 if all objects were update      
      */
      virtual void switchTexture( QString ) {};
      
      /** \brief emit this signal if you want to set a special mode for this texture (Clamping,...)
       * 
       *  emit this signal if you want to set a special mode for this texture
       * 
       *  Supported Mode Flags :\n
       *  Clamping : clamp=true/false \n
       *  Min Value for clamping  : clamp_min=value \n
       *  Max Value for clamping : clamp_max=value \n
       *  Use only absolute values : abs=true/false \n
       *  repeat texture : repeat=true/false \n
       *  Maximum value for repeat : max_val=value \n
       *  center texture : center=true/false \n
       *  \n
       * The system works like this :\n
       * First the abolute value of the property is taken if requested. Then this value is clamped against the given values. if the texture
       * should be repeated, the values are translated such that the minimum is at zero and than scaled such that the maximum is at 
       * max_val. If its not repeated, the decision is to center the values around 0.5 or not. If not centered, the values are mapped directly
       * to 0..1 .If centered, the negative values are mapped to 0..0.5 and the positive values to 0.5..1.
       *  @param _textureName Name of your Texture
       *  @param _mode colon seperated String describing your settings (e.g. clamp,abs )
      */
      virtual void setTextureMode(QString /*_textureName*/ ,QString /*_mode*/) {};
      
   private slots :
      /** \brief update the texture with the given Name ( if this plugin provides this texture ) for all meshes
       * @param _textureName Name of the Texture to be updated
       * @param _identifier The id of the object to update
      */
      virtual void slotUpdateTexture( QString /*_textureName*/ , int /*_identifier*/) {};
      
      /** \brief update all textures provided by this plugin
       */
      virtual void slotUpdateAllTextures( ) {};
      
      /** \brief A texture has been added by a plugin. 
       * 
       * This slot is called when a texture has been added by a plugin. 
       * @param _textureName Name of the Added texture (has to be equal to the property name)
       * @param _filename Filename of the Texture Image to be used
       * @param dimension Dimension of the Texture (currently only 1D and 2D Textures are supported
       */
      virtual void slotTextureAdded( QString /*_textureName*/ , QString /*_filename*/ , uint /*dimension*/ ) {};
      
      /** \brief A texture has been updated
       * 
       *  A plugin has updated a Texture
       *  @param _textureName The name of the updated texture
       *  @param _identifier -1 if all objects updated, otherwise the identifier of the object
       */
      virtual void slotTextureUpdated( QString /*_textureName*/ , int /*_identifier*/ ) {};
      
      /** \brief Texturemode for texture should be changed
       * 
       *  A plugin has updated the Texture settings for a texture
       *  @param _textureName The name of the updated texture
       *  @param _mode New mode flags for the given texture
       */
      virtual void slotSetTextureMode(QString /*_textureName*/ ,QString /*_mode*/) {};
      
      /** \brief This slot is called when a plugin requests to switch to a different texture mode
       * 
       * @param _textureName Name of the Texture
      */
      virtual void slotSwitchTexture( QString /*_textureName*/ ) {};
      
   public : 
       
      /// Destructor
      virtual ~TextureInterface() {};
      
};

Q_DECLARE_INTERFACE(TextureInterface,"OpenFlipper.TextureInterface/1.0")
      
#endif // TEXTUREINTERFACE_HH

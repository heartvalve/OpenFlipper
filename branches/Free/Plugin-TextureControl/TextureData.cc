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
//   $Revision: 5183 $
//   $Author: wilden $
//   $Date: 2009-03-03 12:51:24 +0100 (Tue, 03 Mar 2009) $
//
//=============================================================================

#include "TextureData.hh"

//-----------------------------------------------------------------------------------

TextureData::TextureData(){
  nextInternalID_ = 0;
}

//-----------------------------------------------------------------------------------

TextureData::~TextureData() {

}
/*
//-----------------------------------------------------------------------------------

/** \brief Check if a texture exists
 *
 * @param _textureName name of the texture
 * @return returns wether the texture exists
 */
bool TextureData::textureExists(QString _textureName)
{
  return ( getTextureIndex(_textureName) != -1);
}

//-----------------------------------------------------------------------------------

/** \brief Check if a texture is enabled
 *
 * @param _textureName name of the texture
 * @return returns wether the texture is enabled
 */
bool TextureData::isEnabled(QString _textureName)
{
  int id = getTextureIndex(_textureName);

  if ( id != -1)
    return textures_[id].enabled;
  else
    return false;
}

//-----------------------------------------------------------------------------------

/** \brief Enable a given texture
 *
 * @param _textureName name of the texture
 * @param _exclusive disable other textures?
 */
bool TextureData::enableTexture(QString _textureName, bool _exclusive)
{
  int id = getTextureIndex(_textureName);

  if ( id != -1){

    textures_[id].enabled = true;

    //disable other textures
    if (_exclusive)
      for ( int i = 0 ; i < (int)textures_.size() ; ++i )
        if (i != id)
          textures_[i].enabled = false;
    return true;
  }
}

//-----------------------------------------------------------------------------------

/** \brief Disable a given texture
 *
 * @param _textureName name of the texture
 */
void TextureData::disableTexture(QString _textureName)
{
  int id = getTextureIndex(_textureName);

  if ( id != -1)
    textures_[id].enabled = false;
}

//-----------------------------------------------------------------------------------

/** \brief Add a Texture
 *
 * @param _textureName name of the texture
 * @param _filename filename of the texture
 * @param _dimension dimension of the texture
 * @param _glName glName for the texture
 * @return persistent id of the texture
 */
int TextureData::addTexture(QString _textureName, QString _filename, uint _dimension, GLuint _glName)
{
  //generate texture object
  Texture tex;
  tex.id         = nextInternalID_++;
  tex.name       = _textureName;
  tex.glName     = _glName;
  tex.filename   = _filename;
  tex.dimension  = _dimension;
  tex.enabled    = true;
  tex.dirty      = false;
  tex.type       = VERTEXBASED;
//   tex.parameters = TexParameters;

  textures_.push_back( tex );

  textureMap_[ tex.id ]  = tex.glName;
  propertyMap_[ tex.id ] = tex.name.toStdString();

  return tex.id;
}

int TextureData::addTexture ( Texture _texture, GLuint _glName ) {
  _texture.id     = nextInternalID_++;
  _texture.glName = _glName;
  textures_.push_back(_texture);

  textureMap_[ _texture.id ]  = _texture.glName;
  propertyMap_[ _texture.id ] = _texture.name.toStdString();

  std::cerr << "Added texture " << _texture.name.toStdString() << std::endl;
  return _texture.id;
}

//-----------------------------------------------------------------------------------

/** \brief Delete a given texture
 *
 * @param _textureName name of the texture
 */
// void TextureData::deleteTexture(QString _textureName)
// {
//   int index = getTextureIndex(_textureName);
//
//   if ( index != -1){
//
//
//     textureMap_.erase( texture(_textureName).id );
//     propertyMap_.erase( texture(_textureName).id );
//     textures_.erase(textures_.begin()+index);
//   }
// }

//-----------------------------------------------------------------------------------

/** \brief get parameters of a given texture
 *
 * @param _textureName name of the texture
 * @return corresponding textureParameters or invalid TexParameters
 */
// TexParameters TextureData::textureParameters(QString _textureName)
// {
//   int id = getTextureIndex(_textureName);
//
//   if ( id != -1)
//     return textures_[id].parameters;
//   else
//     return TexParameters();
// }

//-----------------------------------------------------------------------------------

/** \brief Set Parameters for a given texture
 *
 * @param _textureName name of the texture
 * @param _params new parameters for the texture
 */
// void TextureData::setTextureParameters(QString _textureName, TexParameters _params)
// {
//   int id = getTextureIndex(_textureName);
//
//   if ( id != -1)
//     textures_[id].parameters = _params;
// }

//-----------------------------------------------------------------------------------

/** \brief get texture object of a given texture
 *
 * @param _textureName name of the texture
 * @return corresponding texture object
 */
Texture& TextureData::texture(QString _textureName)
{
  int id = getTextureIndex(_textureName);

  if ( id != -1)
    return textures_[id];
  else {
    std::cerr << "Invalid Texture" << _textureName.toStdString() << std::endl;
    return noTexture;
  }
}

//-----------------------------------------------------------------------------------

/** \brief Get the index of a given texture
 *
 * @param _textureName name of the texture
 * @return index in texture vector or -1 if not found
 */
int TextureData::getTextureIndex(QString _textureName)
{
  // Search the list of textures if we have the texture
  int textureid = -1;
  for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
    if ( textures_[i].name == _textureName ) {
      textureid = i;
      break;
    }
  }

  return textureid;
}

//-----------------------------------------------------------------------------------

/** \brief Get reference to the texture vector
 *
 * @return texture vector
 */
std::vector< Texture >& TextureData::textures(){
  return textures_;
}

//-----------------------------------------------------------------------------------

/** \brief Get pointer to the textureMap
 *
 * @return textureMap
 */
std::map< int, GLuint>* TextureData::textureMap(){
  return &textureMap_;
}

//-----------------------------------------------------------------------------------

/** \brief Get pointer to the propertyMap
 *
 * @return propertyMap
 */
std::map< int, std::string>* TextureData::propertyMap(){
  return &propertyMap_;
}


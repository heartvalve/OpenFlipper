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

#include <OpenFlipper/common/perObjectData.hh>

#include <QString>
#include <GL/gl.h>
#include <vector>
#include <map>

struct TexParameters {
  double clamp_min;
  double clamp_max;

  bool clamp;
  bool repeat;

  double max_val;

  bool center;

  bool abs;

  bool scale;
};

struct Texture {
  // Texture id
  int id;
  // glName
  GLuint glName;
  // Texture Name
  QString name;
  // Filename of the texture
  QString filename;
  // dimension
  uint dimension;
  // Status
  bool enabled;
  // does this texture need an update?
  bool dirty;
  // Texture Type
  uint type;
  // Parameters
  TexParameters parameters;
};

class TextureData : public PerObjectData
{

  public:
    enum TextureType { VERTEXBASED = 1 << 0, HALFEDGEBASED = 1 << 1};

  public :

    /// Konstruktor
    TextureData();
    /// Destruktor
    ~TextureData();

    /// Check if a texture exists
    bool textureExists(QString _textureName);
    /// Check if a texture is enabled
    bool isEnabled(QString _textureName);
    /// Enable a given texture
    void enableTexture(QString _textureName, bool _exclusive=false);
    /// Disable a given texture
    void disableTexture(QString _textureName);
    /// Add a Texture
    int addTexture(QString _textureName , QString _filename , uint _dimension, GLuint _glName);
    /// Delete a given texture
    void deleteTexture(QString _textureName);
    /// get parameters of a given texture
    TexParameters textureParameters(QString _textureName);
    /// Set Parameters for a given texture
    void setTextureParameters(QString _textureName, TexParameters _params);
    /// Get the texture object
    Texture texture(QString _textureName);
    /// Get reference to the texture vector
    std::vector< Texture >& textures();
    /// Get reference to the textureMap
    std::map< int, GLuint >* textureMap();
    /// Get reference to the propertyMap
    std::map< int, std::string >* propertyMap();

  private :

    std::map< int, GLuint> textureMap_;
    std::map< int, std::string> propertyMap_;

    int nextInternalID_;

    //vector containing all textures of an object
    std::vector< Texture > textures_;

    // Get the index of a given texture
    int getTextureIndex(QString _textureName);

};


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

#ifndef TEXTUREDATA_HH
#define TEXTUREDATA_HH

#include <OpenFlipper/common/perObjectData.hh>

#include <QString>
#include <QStringList>
#include <QImage>
#include <ACG/GL/gl.hh>
#include <vector>
#include <map>
#include <float.h>
#include <iostream>

enum TextureType { UNSET         = 1 << 0,
                   VERTEXBASED   = 1 << 1,
                   HALFEDGEBASED = 1 << 2,
                   MULTITEXTURE  = 1 << 3 };

class TexParameters
{
  public:
    TexParameters() :
          scale ( true ),
          clamp_min ( FLT_MIN ),
          clamp_max ( FLT_MAX ),
          clamp ( false ),
          repeat ( false ),
          center ( false ),
          abs ( false ),
          max_val ( 1.0 ) {};

    bool scale;

    double clamp_min;
    double clamp_max;

    bool clamp;
    bool repeat;

    bool center;

    bool abs;

    double max_val;
};

class Texture {
  public :
    Texture();


    void filename( QString _name ) { filename_ = _name; };
    QString filename() { return filename_; };


    void id( int _id ) { id_ = _id; };
    int id() { return id_; };


    void glName( GLuint _glName ) { glName_ = _glName; };
    GLuint glName() { return glName_; };


    void name( QString _name ) { name_ = _name; };
    QString name() { return name_; };


    void dimension( uint _dimension ) { dimension_ = _dimension; };
    uint dimension( ) { return dimension_; };


    void enabled( bool _enabled ) { enabled_ = _enabled; };
    bool enabled() { return enabled_; };
    void enable(){ enabled_ = true; };
    void disable(){ enabled_ = false; };

    void hidden( bool _hidden ) { hidden_ = _hidden; };
    bool hidden() { return hidden_; };
    void hide() { hidden_ = true; };

    void dirty( bool _dirty ) { dirty_ = _dirty; };
    bool dirty() { return dirty_; };
    void clean() { dirty_ = false; };
    void setDirty() { dirty_ = true; };


    void type( uint _type ) { type_ = _type; };
    uint type( ) { return type_; };


    QString indexMappingProperty() { return indexMappingProperty_; };
    void indexMappingProperty( QString _property ) { indexMappingProperty_ = _property; };


    /// Parameters of the texture
    TexParameters parameters;

    /// The image used as the texture
    QImage textureImage;

    /// If this is a multiTexture, the list will contain all textures for this multi Texture node.
    QStringList multiTextureList;

  private:
    /// Texture Name
    QString name_;

    /// Filename of the texture
    QString filename_;

    /// Texture id
    int id_;

    /// glName
    GLuint glName_;

    /// dimension
    uint dimension_;

    /// Status
    bool enabled_;

    /// Hidden flag ( If this texture belongs to a multitexture, it will be hidden in the context menu )
    bool hidden_;

    /// does this texture need an update?
    bool dirty_;

    /// Texture Type
    uint type_;

    /** If this is a multiTexture, indexMappingProperty_ will point to the property storing the mapping
     * Defaults to the f:textureindex
    */
    QString indexMappingProperty_;

};

class TextureData : public PerObjectData
{

  public :

      /// Constructor
      TextureData();
      /// Destructor
      ~TextureData();


      /// Check if a texture exists
      bool textureExists(QString _textureName);


      /// Check if a texture is enabled
      bool isEnabled(QString _textureName);


      /// Enable a given texture
      bool enableTexture(QString _textureName, bool _exclusive = false);

      /// Disable a given texture
      void disableTexture(QString _textureName);

      /// Add a Texture
      int addTexture ( QString _textureName , QString _filename , uint _dimension, GLuint _glName );

      /// Add a Texture ( Based on an existing specification )
      int addTexture ( Texture _texture, GLuint _glName );

      /// Adds a new multiTexture ( This texture will only contain a list of enabled textures for multitexturing )
      bool addMultiTexture( QString _textureName );

      /// Stores the given image in the texture information
      bool setImage( QString _textureName , QImage& _image );

      /*
      /// Delete a given texture
      void deleteTexture(QString _textureName);

      /// get parameters of a given texture
      TexParameters textureParameters(QString _textureName);

      /// Set Parameters for a given texture
      void setTextureParameters(QString _textureName, TexParameters _params);
      */

      /// Get the texture object
      Texture& texture(QString _textureName);

      /// Get reference to the texture vector
      std::vector< Texture >& textures();


      /** This map maps all available textures for the object which this
       *  class belongs to to their GLuint. The MeshNode will use this
       *  map to activate one texture for each face.
       *
       */
      std::map< int, GLuint >* textureMap();

    /** This map is used to store the available Textures and map them to their
     *  corresponding properties.
     */
      std::map< int, std::string >* propertyMap();

  private :

    std::map< int, GLuint> textureMap_;
    std::map< int, std::string> propertyMap_;

    /// internal id for the next texture
    int nextInternalID_;


    /// vector containing all textures of an object
    std::vector< Texture > textures_;


    /// Get the index of a given texture
    int getTextureIndex(QString _textureName);

    Texture noTexture;

};

#endif //TEXTUREDATA_HH
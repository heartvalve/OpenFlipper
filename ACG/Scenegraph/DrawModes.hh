/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS DrawModes
//
//=============================================================================

#ifndef ACG_DRAWMODES_HH
#define ACG_DRAWMODES_HH


//== FORWARD DECLARATIONS =====================================================


namespace ACG 
{
  namespace SceneGraph 
  {
    class BaseNode;
  }
}


//== INCLUDES =================================================================

#include <string>
#include <vector>
#include "../Config/ACGDefines.hh"
#include <bitset>

// Avoid compiler warning in MSC
#if defined (_MSC_VER)
#   pragma warning (disable:4251)
#endif

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {

/** This namespace consists of the definition of all available draw modes,  
    function to get their name (description()) and to include them in a
    QPopupMenu.
*/


namespace DrawModes {


  /** \brief Lighting stage of a mesh: unlit, smooth, phong
   *
   *   Don't confuse this with the interpolation mode of vertex attributes.
   *   This only says where to apply lighting, and nothing else.
   *
   *   Instead the interpolation mode is customizable for each attribute,
   *   making DrawModeProperties more flexible.
   *
   *   flat shading can be achieved by using LIGHTSTAGE_SMOOTH
   *   and setting the normal source to NORMAL_PER_FACE
   */
  enum DrawModeLightStage
  {
    LIGHTSTAGE_UNLIT,  /**< No lighting,  normals may be used by user defined shaders */
    LIGHTSTAGE_SMOOTH, /**< Perform lighting in vertex shader   */
    LIGHTSTAGE_PHONG   /**< Perform lighting in fragment shader */
  };

  /** \brief Primitive mode of a mesh.
   *
   *  Example: PRIMITIVE_EDGE on a polygon mesh renders only edges of the mesh.
   */
  enum DrawModePrimitive
  {
    PRIMITIVE_POINT,
    PRIMITIVE_EDGE,
    PRIMITIVE_HALFEDGE,
    PRIMITIVE_WIREFRAME,
    PRIMITIVE_HIDDENLINE,
    PRIMITIVE_POLYGON,
    PRIMITIVE_CELL
  };

  /** \brief Source of Primitive Colors.
   *
   *   This is interpreted as a per vertex diffuse and ambient color and multiplied with
   *   the material diffuse/ambient colors.
   */
  enum DrawModeColorSource
  {
    COLOR_NONE,         /**< Use material colors only */
    COLOR_PER_VERTEX,   /**< Load per vertex colors and modulate with material color */
    COLOR_PER_HALFEDGE, /**< Load per halfedge colors and modulate with material color */
    COLOR_PER_FACE      /**< Load per face colors and modulate with material color */
  };

  /** \brief Source of Texture Coordinates.
   *
   *   This must be specified for a textured draw.
   */
  enum DrawModeTexCoordSource
  {
    TEXCOORD_NONE,         /**< Disable texture mapping */
    TEXCOORD_PER_VERTEX,   /**< Use per vertex texcoords for texture mapping */
    TEXCOORD_PER_HALFEDGE, /**< Use per halfedge texcoords for texture mapping */
    //  TEXCOORD_PER_FACE // TODO: discuss texcoords per face? is it useful?
  };

  /** \brief Source of Normals.
   *
   *   This must be specified if lighting is enabled.
   */
  enum DrawModeNormalSource
  {
    NORMAL_NONE,          /**< Disable lighting */
    NORMAL_PER_VERTEX,    /**< Use per vertex normals */
    NORMAL_PER_HALFEDGE,  /**< Use per halfedge normals */
    NORMAL_PER_FACE       /**< Use per face normals \note per face is implicitly used in SHADE_FLAT mode  */
  };

  typedef std::bitset<128> ModeFlagSet;

  /** \brief DrawModeProperties stores a set of properties that defines, how to render an object.
   *
   * A property may be the primitive type, normal source, color source ...
   * Each property is atomic, i.e. it can not be combined with other properties of the same type.
   *  Example:  primitive may be PRIMITIVE_POLYGON or PRIMITIVE_POINTS, but not both at the same time.
   *  This restriction makes a property set well defined; it is always a valid set of properties.
   *  To have combined DrawModes i.e. flat + wireframe you have to use the layer approach of DrawMode.
   */
  class ACGDLLEXPORT DrawModeProperties {

  public:
    DrawModeProperties(
      DrawModePrimitive _primitive = PRIMITIVE_POLYGON,
      DrawModeLightStage _lightStage = LIGHTSTAGE_UNLIT,
      DrawModeNormalSource _normalSource = NORMAL_NONE,
      DrawModeColorSource _colorSource = COLOR_NONE,
      DrawModeTexCoordSource _texcoordSource = TEXCOORD_NONE,
      bool _envMapping = false);

    //===========================================================================
      /** @name Getter/Setter of all properties
      * @{ */
    //===========================================================================

    DrawModePrimitive primitive() const { return primitive_; }
    void primitive(DrawModePrimitive _val) { primitive_ = _val; }

    DrawModeLightStage lightStage() const { return lightStage_; }
    void lightStage(DrawModeLightStage _val) { lightStage_ = _val; }

    DrawModeColorSource colorSource() const { return colorSource_; }
    void colorSource(DrawModeColorSource _val) { colorSource_ = _val; }

    DrawModeNormalSource normalSource() const { return normalSource_; }
    void normalSource(DrawModeNormalSource _val) { normalSource_ = _val; }

    DrawModeTexCoordSource texcoordSource() const { return texcoordSource_; }
    void texcoordSource(DrawModeTexCoordSource _val) { texcoordSource_ = _val; }

    /** @} */

    //===========================================================================
      /** @name Helper functions for more convenient use in renderer
      * @{ */
    //===========================================================================

    /// Is lighting enabled?
    bool lighting() const { return lightStage_ != LIGHTSTAGE_UNLIT && normalSource_ != NORMAL_NONE; }

    /// Is texturing enabled?
    bool textured() const { return texcoordSource_ != TEXCOORD_NONE; }

    /// Are colors used?
    bool colored() const { return colorSource_ != COLOR_NONE; }

    /// Is flat shading used (Normals per face)?
    bool flatShaded() const { return normalSource_ == NORMAL_PER_FACE; }
    /** @} */

    //===========================================================================
      /** @name Comparison functions
      * @{ */
    //===========================================================================

    /** \brief compare two properties
     *
     * @param _other Right hand side
     */
    bool operator!=( const DrawModeProperties& _other ) const {
      return ( (envMapped_      != _other.envMapped_)      ||
               (primitive_      != _other.primitive_)      ||
               (lightStage_     != _other.lightStage_)     ||
               (colorSource_    != _other.colorSource_)    ||
               (texcoordSource_ != _other.texcoordSource_) ||
               (normalSource_   != _other.normalSource_)
              );
    }
    
    /** \brief compare two properties
     *
     * @param _other Right hand side
     */
    bool operator==( const DrawModeProperties& _other ) const {
      return ( (envMapped_      == _other.envMapped_)      &&
               (primitive_      == _other.primitive_)      &&
               (lightStage_     == _other.lightStage_)     &&
               (colorSource_    == _other.colorSource_)    &&
               (texcoordSource_ == _other.texcoordSource_) &&
               (normalSource_   == _other.normalSource_)
              );
    }

    /** @} */

  private:

    bool envMapped_;

    /// Specify which type of primitives will get uploaded to the graphics card
    DrawModePrimitive primitive_;

    DrawModeLightStage lightStage_;

    DrawModeColorSource colorSource_;

    DrawModeTexCoordSource texcoordSource_;

    DrawModeNormalSource normalSource_;

    // TODO: Shaders

  };

  /** \brief Specifies a DrawMode
   *
   * This class specifies a DrawMode. It can contain several properties and layers
   * that define, how objects will be rendered.
   *
   */
  class ACGDLLEXPORT DrawMode {
    public:
      
      DrawMode(); 
      
      /** \brief constructor for unsigned int. 
      *
      * This constructor creates a DrawMode with the given drawMode index.
      * Be CareFull! this constructor can only be used to construct atomic draw modes!
      * The int will not be handled like an bitset but really as an index.
      * See the list of draw modes below to check for the right numbers.
      * You should use the predefined drawModes or create new ones using the other functions
      * and ignore this constructor!
      *
      * @param _index Index of the new DrawMode
      */
      DrawMode(size_t _index);
      
      /** \brief constructor for ModeFlags. 
      *
      * This constructor creates a DrawMode from a bitset.
      * This makes it easier to predefine draw modes using a bitset.
      *
      * @param _flags Flags for the new drawmode defined by a bitset
      */      
      DrawMode( ModeFlagSet _flags );

      /* \brief Returns a registered draw mode based on the description passed or if none could be found the default one.
       *
       * @param _description A description created by description().
       */
      static DrawMode getFromDescription(std::string _description);


      //===========================================================================
      /** @name Operators
       * @{ */
      //===========================================================================
      DrawMode & operator = (const DrawMode& _mode );

      bool operator==(const DrawMode& _mode) const;

      DrawMode operator&(const DrawMode& _mode) const;
      
      DrawMode& operator++();
      
      DrawMode& operator|=( const DrawMode& _mode2  );
      
      DrawMode& operator&=( const DrawMode& _mode2  );
      
      bool operator!=( const DrawMode& _mode2  ) const;
      
      DrawMode operator|( const DrawMode& _mode2  ) const;
      
      DrawMode operator^( const DrawMode& _mode2  ) const;
      
      DrawMode operator~( ) const;
      
      operator bool() const;

      /** @} */

      //===========================================================================
      /** @name Layer Management
       * @{ */
      //===========================================================================

      /** \brief add another layer on top of this drawmode
       *
       * This allows for combinations of DrawModes.
       * The renderer will iterator over all layers of the drawmode
       *  and issue a new draw call for each layer.
       * Example:
       *  layer 0 : flat shading properties
       *  layer 1 : edge drawing properties
       *
       * The result will be a flat shaded object with it's wireframe on top of it.
       * Does duplicate check, in case the property set is already in the layer list.
       *
       * addLayer is also called in bitwise | operator for backwards compatibility with bitset DrawModes
       *
       * @param _props Property that should be added
       */
      void addLayer(const DrawModeProperties* _props); // same as bitwise or operator

      /** \brief returns the layer count
       */
      size_t getNumLayers() const;

      /** \brief returns the property set at layer i
       *
       * @param _i Layer that is requested
       */
      const DrawModeProperties* getLayer(unsigned int _i) const;

      /** \brief remove layer at index i
       *
       * @param _i Layer that is requested
       */
      bool removeLayer(unsigned int _i);

      /** \brief remove layer if it is in list
       *
       * @param _prop Property that should be removed if it's available
       */
      bool removeLayer(const DrawModeProperties* _prop);


      /** \brief returns layer index of a property, -1 if not in list
       *
       * @param _prop Property to be searched for
       */
      int getLayerIndex(const DrawModeProperties* _prop) const;

      /** @} */


      /** \brief get an index of the current drawMode
      *
      * If this drawMode is a combination of different drawModes, the returned value will be 0.
      * Otherwise the internal flag index will be returned
      */
      size_t getIndex() const;

      /** \brief filter out one drawmode
      *
      * This removes one or more drawmodes if this drawode is not atomic.
      */
      void filter( DrawMode _filter );


      /** \brief combine with another drawmode
      */
      void combine( DrawMode _mode );
      
      /** Get a description string for this DrawMode
      * An empty string is returned if this is not a valid draw mode.
      * this list has the format DrawModeName+DrawMOdeName+...
      */
      std::string description() const;
      
      /** \brief Check if this is an atomic draw Mode
      *
      * This function checks, if this is a atomic draw mode ( no combination of multiple draw modes )
      */
      bool isAtomic() const;
      
      /** \brief Check whether an Atomic DrawMode is active in this draw Mode
      */ 
      bool containsAtomicDrawMode( DrawMode _atomicDrawMode ) const;
      
      /** \brief Separates this drawMode into a list of all separate atomic draw modes
      *
      * A drawMode can consist of several atomic draw modes. This function returns a list of the separated
      * atomic draw modes.
      */
      std::vector< DrawMode > getAtomicDrawModes() const;
      
      /** \brief Get the number of maximum Modes which could be handled by the current implementation
      */
      size_t maxModes() const;
      

      /** \brief returns the base properties of this draw mode
       *
       * Base properties are the original properties that defined the DrawMode before any merge operation.
       * They are located at layer 0, so actually this function has the same effect as getLayer(0).
       * 
       * Every DrawMode is property based with only two exceptions:
       *  - NONE
       *  - DEFAULT
       *
       * getDrawModeProperties returns 0 for these.
       */
      const DrawModeProperties* getDrawModeProperties() const;

      /** \brief set the base properties of this draw mode
       *
       * @param _props Properties to be set
       */
      void setDrawModeProperties(const DrawModeProperties* _props);

      /** \brief set the base properties of this draw mode
       *
       * @param _props Properties to be set
       */
      void setDrawModeProperties(const DrawModeProperties& _props);


      /** \brief checks consistency of property layers
       *
       * There should only be at most one layer for each primitive type for example
       */
      bool checkConsistency() const;


      /** \brief search for layer with specified primitive
       *
       * @param _type primitive type
       * @return layer id if layer exists, -1 otherwise
       */
      int getLayerIndexByPrimitive(DrawModePrimitive _type) const;


    private:
      ModeFlagSet modeFlags_;

      /** vector for combined DrawModes
      * -> holds DrawModeProperties for each layer
      * -> original layer at index 0
      */
      std::vector<DrawModeProperties> layers_;
  };
  
    
  /// not a valid draw mode
  extern ACGDLLEXPORT DrawMode NONE;
  /// use the default (global) draw mode and not the node's own.
  extern ACGDLLEXPORT DrawMode DEFAULT;
  
  //======================================================================
  //  Point Based Rendering Modes
  //======================================================================
  /// draw unlighted points using the default base color
  extern ACGDLLEXPORT DrawMode POINTS;

  /// draw colored, but not lighted points (requires point colors)
  extern ACGDLLEXPORT DrawMode POINTS_COLORED;
  /// draw shaded points (requires point normals)
  extern ACGDLLEXPORT DrawMode POINTS_SHADED;
  
  //======================================================================
  //  Edge Based Rendering Modes
  //======================================================================
  /// draw edges
  extern ACGDLLEXPORT DrawMode EDGES;

  /// draw edges with colors (without shading)
  extern ACGDLLEXPORT DrawMode EDGES_COLORED;
  /// draw wireframe
  extern ACGDLLEXPORT DrawMode WIREFRAME;
  
  //======================================================================
  //  Face Based Rendering Modes
  //======================================================================  
  // draw faces
  extern ACGDLLEXPORT DrawMode FACES;

  /// draw hidden line (2 rendering passes needed)
  extern ACGDLLEXPORT DrawMode HIDDENLINE;
  /// draw flat shaded faces (requires face normals)
  extern ACGDLLEXPORT DrawMode SOLID_FLAT_SHADED;
  /// draw smooth shaded (Gouraud shaded) faces (requires halfedge normals)  
  extern ACGDLLEXPORT DrawMode SOLID_SMOOTH_SHADED;
  /// draw phong shaded faces
  extern ACGDLLEXPORT DrawMode SOLID_PHONG_SHADED;
  /// draw colored, but not lighted faces using face colors
  extern ACGDLLEXPORT DrawMode SOLID_FACES_COLORED;
  /// draw colored, but not lighted faces using interpolated vertex colors
  extern ACGDLLEXPORT DrawMode SOLID_POINTS_COLORED;
  /// draw faces, but use Gouraud shading to interpolate vertex colors
  extern ACGDLLEXPORT DrawMode SOLID_POINTS_COLORED_SHADED;
  /// draw environment mapped  
  extern ACGDLLEXPORT DrawMode SOLID_ENV_MAPPED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_TEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_TEXTURED_SHADED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_1DTEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_1DTEXTURED_SHADED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_3DTEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_3DTEXTURED_SHADED;
  /// draw flat shaded and colored faces (requires face normals and colors)
  extern ACGDLLEXPORT DrawMode SOLID_FACES_COLORED_FLAT_SHADED;
  /// draw smooth shaded and colored faces (requires vertex normals and face colors)
  extern ACGDLLEXPORT DrawMode SOLID_FACES_COLORED_SMOOTH_SHADED;
  /// draw per halfedge textured faces
  extern ACGDLLEXPORT DrawMode SOLID_2DTEXTURED_FACE;
  /// draw per halfedge textured faces
  extern ACGDLLEXPORT DrawMode SOLID_2DTEXTURED_FACE_SHADED;
  /// drawing is controlled by shaders
  /// ( Use shadernodes to set them before the actual object to render)
  extern ACGDLLEXPORT DrawMode SOLID_SHADER;
  /// draw smooth shaded (Gouraud shaded) faces (requires halfedge normals)
  extern ACGDLLEXPORT DrawMode SOLID_SMOOTH_SHADED_FEATURES;

  //======================================================================
  //  Face Based Rendering Modes
  //======================================================================  
  // draw cells
  extern ACGDLLEXPORT DrawMode CELLS;

  /// draw cells with colors (without shading)
  extern ACGDLLEXPORT DrawMode CELLS_COLORED;


  //======================================================================
  //  Halfedge Based Rendering Modes
  //======================================================================
  /// draw halfedges
  extern ACGDLLEXPORT DrawMode HALFEDGES;

  /// draw halfedges with colors (without shading)
  extern ACGDLLEXPORT DrawMode HALFEDGES_COLORED;


  /// marks the last used ID
  extern ACGDLLEXPORT DrawMode UNUSED;
  

  //=======================================================================
  // Non Member Functions
  //=======================================================================
  
  /** Initialize the default modes.
      This function has to be called at least once at application startup.
  */
  ACGDLLEXPORT 
  void initializeDefaultDrawModes( void );

  /** \brief Add a custom DrawMode.
   *
      The id of the new draw mode is returned. If it already exists, the id of the existing one
      is returned.

      Property based draw modes consist of various flags, which define which primitives and
      additional information are send to the gpu.

      @param _name Name of the draw mode to add
      @param _propertyBased If set to true a property based draw mode is created.
      @return Id of the new draw mode
  */
  ACGDLLEXPORT
  const DrawMode& addDrawMode( const std::string & _name, bool _propertyBased = false);

  /** \brief Add a custom property based DrawMode.
   *
   * The id of the new draw mode is returned. If it already exists, the id of the existing one
   * is returned.
   *
   * \note If the DrawMode already exists, the properties will be applied to the existing mode!!
   *
   * Property based draw modes consist of various flags, which define which primitives and
   * additional information are send to the gpu.
   *
   * @param _name       Name of the draw mode to add
   * @param _properties Properties of the drawmode
   * @return Id of the new draw mode
  */
  ACGDLLEXPORT
  const DrawMode& addDrawMode( const std::string & _name, const DrawModeProperties& _properties);

  /** \brief Get a custom DrawMode.
   *
      The id of the draw mode is returned or if it does not exist, DrawMode::NONE is returned.

      @param _name Name of the DrawMode
      @return Id of the draw mode or DrawModes::NONE
  */
  ACGDLLEXPORT 
  const DrawMode& getDrawMode( const std::string & _name);

  
  /** \brief Check if the given draw mode exists
  *
  */
  ACGDLLEXPORT   
  bool drawModeExists(const std::string & _name);
  
  /** \brief given an index of an atomic draw mode, return the drawmode
  */
  ACGDLLEXPORT 
  DrawMode getDrawModeFromIndex( unsigned int _index );

//=============================================================================
} // namespace DrawModes
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_DRAWMODES_HH defined
//=============================================================================


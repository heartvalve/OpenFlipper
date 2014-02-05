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

//================================================================
//
//  CLASS SplatCloud
//
//    SplatCloud stores the actual data for a SplatCloud.
//
//================================================================


#ifndef SPLATCLOUD_HH
#define SPLATCLOUD_HH


//== INCLUDES ====================================================


#include <OpenFlipper/common/GlobalDefines.hh>

#include <typeinfo>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include <ACG/Math/VectorT.hh>


//== CLASS DEFINITION ============================================


/** \class SplatCloud SplatCloud.hh <ObjectTypes/SplatCloud/SplatCloud/SplatCloud.hh>
 *
 * Storage of properties for each splat and for the whole splatcloud.
 */

class DLLEXPORT SplatCloud
{

  // == DECLARATIONS =====================================================

public:

  /// Standard constructor
  inline SplatCloud() : numSplats_( 0 )
  {
    resetPredefinedSplatPropertyPointers();
    resetPredefinedCloudPropertyPointers();
  }

  /// Copy constructor
  SplatCloud( const SplatCloud &_splatCloud );

  /// Destructor
  inline ~SplatCloud() { clear(); }

  /// Assign operator
  inline SplatCloud &operator=( const SplatCloud &_splatCloud ) { SplatCloud temp( _splatCloud ); swap( temp ); return *this; }

  /// Remove all properties and reset the number of splats.
  void clear();

  /// Swap the content.
  void swap( SplatCloud &_splatCloud );

  //----------------

  /** \brief Clear the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   */
  void clearSplats();

  //----------------

  /** \brief Add one element at the end of the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   */
  void pushbackSplat();

  //----------------

  /** \brief Resize the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   *
   * @param[in] _num The new size of all data vectors.
   */
  void resizeSplats( unsigned int _num );

  //----------------

  /** \brief Delete the elements with given indices from the data vector of all splat-properties.
   *
   * Use std::vector<T2> / std::list<T2> / std::set<T2> / ... for T and int / unsigned int / ... for T2.
   *
   * \note The number of splats will be updated accordingly.
   *
   * @param[in] _indices Indices of the elements to be erased.
   * @return Returns the number of deleted elements.
   */
  template <typename T> unsigned int eraseSplatsByIndex( const T &_indices );

  //----------------

  /** \brief Delete the elements with flag != 0 from the data vector of all splat-properties.
   *
   * Use bool / unsigned char / ... for T.
   * _flags.size() has to be equal to numSplats().
   * 
   * \note The number of splats will be updated accordingly.
   *
   * @param[in] _flags Vector of flag bits, one for each element.
   * @return Returns the number of deleted elements.
   */
  template <typename T> unsigned int eraseSplatsByFlag( const std::vector<T> &_flags );

  //----------------

  /** \brief Keep only the elements with given indices in the data vector of all splat-properties. The splats will be rearranged depending on the order of the indices.
   * 
   * Use std::vector<T2> / std::list<T2> / std::deque<T2> / ... for T and int / unsigned int / ... for T2.
   *
   * \note The number of splats will be updated accordingly.
   *
   * @param[in] _indices Indices of the elements to be kept in the order given by the indices.
   */
  template <typename T> void cropSplats( const T &_indices );

  //----------------

  /** \brief Get the number of splats.
   *
   * @return Returns the number of splats.
   */
  inline unsigned int numSplats() const { return numSplats_; }

  // == MEMBERS =====================================================

private:

  /// Number of splats
  unsigned int numSplats_;

  //----------------------------------------------------------------



  // +------------------------+
  // |                        |
  // |   General Properties   |
  // |                        |
  // +------------------------+

  // == TYPEDEFS =====================================================

public:

  /** Base class for all property-handles */
  typedef std::string BasePropertyHandle;

  //----------------

  /** Interface for all splat-properties */
  class SplatPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    virtual size_t               sizeOf()  const = 0; //!< Get the size of type.
    virtual const std::type_info &typeId() const = 0; //!< Get the runtime type information.
  private:
    virtual inline ~SplatPropertyInterface() { }       //!< Destructor
    virtual SplatPropertyInterface *clone() const = 0; //!< Return a deep copy of this.
    virtual void clear()                                  = 0; //!< Clear the data vector.
    virtual void pushback()                               = 0; //!< Add one element at the end of the data vector.
    virtual void resize( unsigned int _num )              = 0; //!< Resize the data vector.
    virtual void crop( const std::vector<int> &_indices ) = 0; //!< Keep only the elements with given indices in the data vector. The indices have to be valid, sorted and unique.
  private:
    void operator=( const SplatPropertyInterface & ); //!< Disallow the assign operator. (private and *not* implemented)
  };

  //----------------

  /** Interface for all cloud-properties */
  class CloudPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    virtual size_t               sizeOf()  const = 0; //!< Get the size of type.
    virtual const std::type_info &typeId() const = 0; //!< Get the runtime type information.
  private:
    virtual inline ~CloudPropertyInterface() { }       //!< Destructor
    virtual CloudPropertyInterface *clone() const = 0; //!< Return a deep copy of this.
  private:
    void operator=( const CloudPropertyInterface & ); //!< Disallow the assign operator. (private and *not* implemented)
  };

  //----------------

  /** Splat-property map entry type */
  class SplatPropertyMapEntry
  {
  public:
    SplatPropertyMapEntry()                                                   : property_( 0     ), numRequests_( 0    ) { } //!< Standard constructor
    SplatPropertyMapEntry( SplatPropertyInterface *_prop, unsigned int _num ) : property_( _prop ), numRequests_( _num ) { } //!< Constructor
  public:
    SplatPropertyInterface *property_;   //!< A valid pointer to a splat-property
    unsigned int           numRequests_; //!< The number of times requestSplatProperty() was called and has not been released yet
  };

  //----------------

  /** Cloud-property map entry type */
  class CloudPropertyMapEntry
  {
  public:
    CloudPropertyMapEntry()                                                   : property_( 0     ), numRequests_( 0    ) { } //!< Standard constructor
    CloudPropertyMapEntry( CloudPropertyInterface *_prop, unsigned int _num ) : property_( _prop ), numRequests_( _num ) { } //!< Constructor
  public:
    CloudPropertyInterface *property_;   //!< A valid pointer to a cloud-property
    unsigned int           numRequests_; //!< The number of times requestCloudProperty() was called and has not been released yet
  };

  //----------------

  /** Splat-property map type */
  typedef std::map<BasePropertyHandle, SplatPropertyMapEntry> SplatPropertyMap;

  //----------------

  /** Cloud-property map type */
  typedef std::map<BasePropertyHandle, CloudPropertyMapEntry> CloudPropertyMap;

  //----------------

public:

  /** Type specific class for property-handles */
  template <typename T>
  class PropertyHandleT : public BasePropertyHandle
  {
  public:
    explicit inline PropertyHandleT<T>( const BasePropertyHandle &_handle ) : BasePropertyHandle( _handle ) { } //!< Constructor
  };

  //----------------

  /** Type specific class for splat-properties */
  template <typename T>
  class SplatPropertyT : public SplatPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    typedef                      T                   Value;          //!< Value type
    typedef typename std::vector<T>::      reference      Reference; //!< These are references to T, *not* to SplatPropertyT<T>.
    typedef typename std::vector<T>::const_reference ConstReference; //!< These are used because a std::vector<bool>::reference is *not* the same as a reference to bool.
  public:
    explicit inline SplatPropertyT<T>( const PropertyHandleT<T> &_handle, unsigned int _num ) : data_( _num ) { } //!< Constructor
    inline      Reference data( int _idx )       { return data_[ _idx ]; } //!< Access the data as reference.
    inline ConstReference data( int _idx ) const { return data_[ _idx ]; } //!< Access the data as const reference.
  public:
    virtual inline size_t               sizeOf()  const { return sizeof( T ); } //!< Get the size of type.
    virtual inline const std::type_info &typeId() const { return typeid( T ); } //!< Get the runtime type information.
  private:
    virtual inline ~SplatPropertyT<T>() { }                                                    //!< Destructor
    virtual inline SplatPropertyT<T> *clone() const { return new SplatPropertyT<T>( *this ); } //!< Return a deep copy of this.
    virtual inline void clear()                     { std::vector<T>().swap( data_ ); } //!< Clear the data vector.
    virtual inline void pushback()                  { data_.push_back( T() );         } //!< Add one element at the end of the data vector.
    virtual inline void resize( unsigned int _num ) { data_.resize( _num );           } //!< Resize the data vector.
    virtual        void crop( const std::vector<int> &_indices );                       //!< Keep only the elements with given indices in the data vector. The indices have to be valid, sorted and unique.
  private:
    void operator=( const SplatPropertyT<T> & ); //!< Disallow the assign operator. (private and *not* implemented)
  private:
    std::vector<T> data_; //!< The actual stored data (one element per splat)
  };

  //----------------

  /** Type specific class for cloud-properties */
  template <typename T>
  class CloudPropertyT : public CloudPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    typedef       T   Value;          //!< Value type
    typedef       T &      Reference; //!< These are references to T, *not* to CloudPropertyT<T>.
    typedef const T & ConstReference; //!< These are used only out of a consistency reason to the class SplatPropertyT<T>.
  public:
    explicit inline CloudPropertyT<T>( const PropertyHandleT<T> &_handle ) { } //!< Constructor
    inline      Reference data()       { return data_; } //!< Access the data as reference.
    inline ConstReference data() const { return data_; } //!< Access the data as const reference.
  public:
    virtual inline size_t               sizeOf()  const { return sizeof( T ); } //!< Get the size of type.
    virtual inline const std::type_info &typeId() const { return typeid( T ); } //!< Get the runtime type information.
  private:
    virtual inline ~CloudPropertyT<T>() { }                                                    //!< Destructor
    virtual inline CloudPropertyT<T> *clone() const { return new CloudPropertyT<T>( *this ); } //!< Return a deep copy of this.
  private:
    void operator=( const CloudPropertyT<T> & ); //!< Disallow the assign operator. (private and *not* implemented)
  private:
    T data_; //!< The actual stored data
  };

  //-- DECLARATIONS ------------------------------------------------

public:

  /// Clear all splat-properties.
  void clearSplatProperties();

  /// Clear all cloud-properties.
  void clearCloudProperties();

  //----------------

  /** @name Request properties. */
  ///@{

  /** \brief Request a new property.
   * 
   * Adds the new property if *not* already present. Otherwise nothing is added.
   * 
   * \note Be careful, this method is *not* in O(1).
   * 
   * @param[in] _handle A handle of the desired property.
   *                    Do *not* use handle-strings inbetween '<' and '>'. These are reserved for predefined properties.
   * @return If the property is already present, a pointer to the old property is returned and no new property is created or added.
   *         Otherwise a pointer to the newly created property is returned.
   *         If the creation fails, 0 is returned and no new property is added.
   */
  ///@{
  template <typename T>        SplatPropertyT<T> *requestSplatProperty( const PropertyHandleT<T> &_handle );
  template <typename T>        CloudPropertyT<T> *requestCloudProperty( const PropertyHandleT<T> &_handle );
  template <typename T> inline SplatPropertyT<T> *requestSplatProperty( const BasePropertyHandle &_handle ) { return requestSplatProperty( PropertyHandleT<T>( _handle ) ); }
  template <typename T> inline CloudPropertyT<T> *requestCloudProperty( const BasePropertyHandle &_handle ) { return requestCloudProperty( PropertyHandleT<T>( _handle ) ); }
  ///@}

  ///@}

  //----------------

  /** @name Release properties. */
  ///@{

  /** \brief Release a property.
   * 
   * Removes the property if present. Otherwise nothing is removed.
   * 
   * \note Be careful, this method is *not* in O(1).
   * 
   * @param[in] _handle A handle of the property to remove.
   *                    Do *not* use handle-strings inbetween '<' and '>'. These are reserved for predefined properties.
   * @return If the property is present and will *not* be removed, a pointer to the old property is returned.
   *         Otherwise 0 is returned.
   */
  ///@{
  template <typename T>        SplatPropertyT<T> *releaseSplatProperty( const PropertyHandleT<T> &_handle );
  template <typename T>        CloudPropertyT<T> *releaseCloudProperty( const PropertyHandleT<T> &_handle );
  template <typename T> inline SplatPropertyT<T> *releaseSplatProperty( const BasePropertyHandle &_handle ) { return releaseSplatProperty( PropertyHandleT<T>( _handle ) ); }
  template <typename T> inline CloudPropertyT<T> *releaseCloudProperty( const BasePropertyHandle &_handle ) { return releaseCloudProperty( PropertyHandleT<T>( _handle ) ); }
  ///@}

  ///@}

  //----------------

  /** @name Get properties. */
  ///@{

  /** \brief Get a pointer to a property.
   * 
   * \note Be careful, this method is *not* in O(1).
   * 
   * @param[in] _handle A handle of the desired property.
   * return If the property is present, a pointer to the property is returned.
   *        Otherwise 0 is returned.
   */
  ///@{
  template <typename T>              SplatPropertyT<T> *getSplatProperty( const PropertyHandleT<T> &_handle );
  template <typename T>        const SplatPropertyT<T> *getSplatProperty( const PropertyHandleT<T> &_handle ) const;
  template <typename T>              CloudPropertyT<T> *getCloudProperty( const PropertyHandleT<T> &_handle );
  template <typename T>        const CloudPropertyT<T> *getCloudProperty( const PropertyHandleT<T> &_handle ) const;
  template <typename T> inline       SplatPropertyT<T> *getSplatProperty( const BasePropertyHandle &_handle )       { return getSplatProperty( PropertyHandleT<T>( _handle ) ); }
  template <typename T> inline const SplatPropertyT<T> *getSplatProperty( const BasePropertyHandle &_handle ) const { return getSplatProperty( PropertyHandleT<T>( _handle ) ); }
  template <typename T> inline       CloudPropertyT<T> *getCloudProperty( const BasePropertyHandle &_handle )       { return getCloudProperty( PropertyHandleT<T>( _handle ) ); }
  template <typename T> inline const CloudPropertyT<T> *getCloudProperty( const BasePropertyHandle &_handle ) const { return getCloudProperty( PropertyHandleT<T>( _handle ) ); }
  ///@}

  ///@}

  //----------------

  /** @name Get property maps. */
  ///@{

  /** \brief Get all splat-properties.
   * 
   * \note Be careful, std::map's count() method will *not* check for the appropiate type of a property.
   *       Instead use something like SplatPropertyT<MyPropType> *myProp = mySplatCloud.getSplatProperty<MyPropType>( "MyPropName" ); once
   *       and then check for myProp != 0 multiple times if needed.
   * 
   * @return Returns the property map as read-only reference.
   */
  inline const SplatPropertyMap &splatProperties() const { return splatProperties_; }

  /** \brief Get all cloud-properties.
   * 
   * \note Be careful, std::map's count() method will *not* check for the appropiate type of a property.
   *       Instead use something like CloudPropertyT<MyPropType> *myProp = mySplatCloud.getCloudProperty<MyPropType>( "MyPropName" ); once
   *       and then check for myProp != 0 multiple times if needed.
   * 
   * @return Returns the property map as read-only reference.
   */
  inline const CloudPropertyMap &cloudProperties() const { return cloudProperties_; }

  ///@}

  //----------------

private:

  /// Deep copy all splat-properties.
  void copySplatProperties( const SplatCloud &_splatCloud );

  /// Deep copy all cloud-properties.
  void copyCloudProperties( const SplatCloud &_splatCloud );

  // == MEMBERS =====================================================

private:

  /// Splat-property map
  SplatPropertyMap splatProperties_;

  /// Cloud-property map
  CloudPropertyMap cloudProperties_;

  //----------------------------------------------------------------



  // +---------------------------+
  // |                           |
  // |   Predefined Properties   |
  // |                           |
  // +---------------------------+

  // == TYPEDEFS =====================================================

public:

  /** View type */
  typedef struct
  {
    int   cameraObjectId_; //!< Camera-object id
    int   featureIdx_;     //!< SIFT-feature index
    float x_;              //!< x-coordinate of pixel position
    float y_;              //!< y-coordinate of pixel position
  } View;

  //----------------

  /** @name Value types for predefined properties */
  ///@{
  typedef ACG::Vec3f        Position;
  typedef ACG::Vec3uc       Color;
  typedef ACG::Vec3f        Normal;
  typedef float             Pointsize;
  typedef int               Index;
  typedef std::vector<View> Viewlist;
  typedef unsigned char     Selection; // Do *not* use bool to prevent further trouble because a std::vector<bool>::reference is *not* the same as a reference to bool.
  ///@}

  //----------------

  /** @name Vector types for predefined properties */
  ///@{
  typedef std::vector<Position > PositionVector;
  typedef std::vector<Color    > ColorVector;
  typedef std::vector<Normal   > NormalVector;
  typedef std::vector<Pointsize> PointsizeVector;
  typedef std::vector<Index    > IndexVector;
  typedef std::vector<Viewlist > ViewlistVector;
  typedef std::vector<Selection> SelectionVector;
  ///@}

  //----------------

private:

  /** @name Type specific property-handle types for predefined properties */
  ///@{
  typedef PropertyHandleT<Position > PositionsHandle;
  typedef PropertyHandleT<Color    > ColorsHandle;
  typedef PropertyHandleT<Normal   > NormalsHandle;
  typedef PropertyHandleT<Pointsize> PointsizesHandle;
  typedef PropertyHandleT<Index    > IndicesHandle;
  typedef PropertyHandleT<Viewlist > ViewlistsHandle;
  typedef PropertyHandleT<Selection> SelectionsHandle;
  ///@}

  //-- DECLARATIONS ------------------------------------------------

public:

  /** @name Request predefined properties. */
  ///@{

  /** \brief Request the predefined property.
   * 
   * See the requestSplatProperty() or requestCloudProperty() method for more detail.
   * 
   * \note Be careful, this method is *not* in O(1).
   * 
   * @return Returns true if the predefined property is available, otherwise false.
   */
  ///@{
  inline bool requestPositions()  { positionsProperty_  = requestSplatProperty( POSITIONS_HANDLE  ); return (positionsProperty_  != 0); }
  inline bool requestColors()     { colorsProperty_     = requestSplatProperty( COLORS_HANDLE     ); return (colorsProperty_     != 0); }
  inline bool requestNormals()    { normalsProperty_    = requestSplatProperty( NORMALS_HANDLE    ); return (normalsProperty_    != 0); }
  inline bool requestPointsizes() { pointsizesProperty_ = requestSplatProperty( POINTSIZES_HANDLE ); return (pointsizesProperty_ != 0); }
  inline bool requestIndices()    { indicesProperty_    = requestSplatProperty( INDICES_HANDLE    ); return (indicesProperty_    != 0); }
  inline bool requestViewlists()  { viewlistsProperty_  = requestSplatProperty( VIEWLISTS_HANDLE  ); return (viewlistsProperty_  != 0); }
  inline bool requestSelections() { selectionsProperty_ = requestSplatProperty( SELECTIONS_HANDLE ); return (selectionsProperty_ != 0); }
  ///@}

  ///@}

  //----------------

  /** @name Release predefined properties. */
  ///@{

  /** \brief Release the predefined property.
   * 
   * See the releaseSplatProperty() or releaseCloudProperty() method for more detail.
   * 
   * \note Be careful, this method is *not* in O(1).
   */
  ///@{
  inline void releasePositions()  { positionsProperty_  = releaseSplatProperty( POSITIONS_HANDLE  ); }
  inline void releaseColors()     { colorsProperty_     = releaseSplatProperty( COLORS_HANDLE     ); }
  inline void releaseNormals()    { normalsProperty_    = releaseSplatProperty( NORMALS_HANDLE    ); }
  inline void releasePointsizes() { pointsizesProperty_ = releaseSplatProperty( POINTSIZES_HANDLE ); }
  inline void releaseIndices()    { indicesProperty_    = releaseSplatProperty( INDICES_HANDLE    ); }
  inline void releaseViewlists()  { viewlistsProperty_  = releaseSplatProperty( VIEWLISTS_HANDLE  ); }
  inline void releaseSelections() { selectionsProperty_ = releaseSplatProperty( SELECTIONS_HANDLE ); }
  ///@}

  ///@}

  //----------------

  /** @name Get availability of predefined properties. */
  ///@{

  /** \brief Return the availability of the predefined property.
   * 
   * \note In contrast to the general properties this method is in O(1) so it can be provided safely.
   * 
   * @return Returns true if the predefined property is available, otherwise false.
   */
  ///@{
  inline bool hasPositions()  const { return (positionsProperty_  != 0); }
  inline bool hasColors()     const { return (colorsProperty_     != 0); }
  inline bool hasNormals()    const { return (normalsProperty_    != 0); }
  inline bool hasPointsizes() const { return (pointsizesProperty_ != 0); }
  inline bool hasIndices()    const { return (indicesProperty_    != 0); }
  inline bool hasViewlists()  const { return (viewlistsProperty_  != 0); }
  inline bool hasSelections() const { return (selectionsProperty_ != 0); }
  ///@}

  ///@}

  //----------------

  /** @name Get predefined properties. */
  ///@{

  /** \brief Get a reference of the predefined property's value.
   * 
   * \note Be careful, this method does *not* check the availability of the predefined property and will probably crash if it's not available.
   * 
   * @param[in] _idx The index of the desired entry in the predefined property's data vector. This is the same as the index of the splat.
   * @return Returns a reference of the predefined property's value.
   */
  ///@{
  inline       Position  &positions ( int _idx )       { return positionsProperty_-> data( _idx ); }
  inline const Position  &positions ( int _idx ) const { return positionsProperty_-> data( _idx ); }
  inline       Color     &colors    ( int _idx )       { return colorsProperty_->    data( _idx ); }
  inline const Color     &colors    ( int _idx ) const { return colorsProperty_->    data( _idx ); }
  inline       Normal    &normals   ( int _idx )       { return normalsProperty_->   data( _idx ); }
  inline const Normal    &normals   ( int _idx ) const { return normalsProperty_->   data( _idx ); }
  inline       Pointsize &pointsizes( int _idx )       { return pointsizesProperty_->data( _idx ); }
  inline const Pointsize &pointsizes( int _idx ) const { return pointsizesProperty_->data( _idx ); }
  inline       Index     &indices   ( int _idx )       { return indicesProperty_->   data( _idx ); }
  inline const Index     &indices   ( int _idx ) const { return indicesProperty_->   data( _idx ); }
  inline       Viewlist  &viewlists ( int _idx )       { return viewlistsProperty_-> data( _idx ); }
  inline const Viewlist  &viewlists ( int _idx ) const { return viewlistsProperty_-> data( _idx ); }
  inline       Selection &selections( int _idx )       { return selectionsProperty_->data( _idx ); }
  inline const Selection &selections( int _idx ) const { return selectionsProperty_->data( _idx ); }
  ///@}

  ///@}

  //----------------

private:

  /// Reset pointers to predefined splat-properties.
  inline void resetPredefinedSplatPropertyPointers()
  {
    positionsProperty_  = 0;
    colorsProperty_     = 0;
    normalsProperty_    = 0;
    pointsizesProperty_ = 0;
    indicesProperty_    = 0;
    viewlistsProperty_  = 0;
    selectionsProperty_ = 0;
  }

  /// Reset pointers to predefined cloud-properties.
  inline void resetPredefinedCloudPropertyPointers() { }

  /// Get pointers to predefined splat-properties.
  inline void getPredefinedSplatPropertyPointers()
  {
    positionsProperty_  = getSplatProperty( POSITIONS_HANDLE  );
    colorsProperty_     = getSplatProperty( COLORS_HANDLE     );
    normalsProperty_    = getSplatProperty( NORMALS_HANDLE    );
    pointsizesProperty_ = getSplatProperty( POINTSIZES_HANDLE );
    indicesProperty_    = getSplatProperty( INDICES_HANDLE    );
    viewlistsProperty_  = getSplatProperty( VIEWLISTS_HANDLE  );
    selectionsProperty_ = getSplatProperty( SELECTIONS_HANDLE );
  }

  /// Get pointers to predefined cloud-properties.
  inline void getPredefinedCloudPropertyPointers() { }

  /// Swap pointers to predefined splat-properties.
  inline void swapPredefinedSplatPropertyPointers( SplatCloud &_splatCloud )
  {
    std::swap( positionsProperty_,  _splatCloud.positionsProperty_  );
    std::swap( colorsProperty_,     _splatCloud.colorsProperty_     );
    std::swap( normalsProperty_,    _splatCloud.normalsProperty_    );
    std::swap( pointsizesProperty_, _splatCloud.pointsizesProperty_ );
    std::swap( indicesProperty_,    _splatCloud.indicesProperty_    );
    std::swap( viewlistsProperty_,  _splatCloud.viewlistsProperty_  );
    std::swap( selectionsProperty_, _splatCloud.selectionsProperty_ );
  }

  /// Swap pointers to predefined cloud-properties.
  inline void swapPredefinedCloudPropertyPointers( SplatCloud &_splatCloud ) { }

  //-- MEMBERS -----------------------------------------------------

private:

  /** @name Pointers to predefined properties */
  ///@{
  SplatPropertyT<Position>  *positionsProperty_;
  SplatPropertyT<Color>     *colorsProperty_;
  SplatPropertyT<Normal>    *normalsProperty_;
  SplatPropertyT<Pointsize> *pointsizesProperty_;
  SplatPropertyT<Index>     *indicesProperty_;
  SplatPropertyT<Viewlist>  *viewlistsProperty_;
  SplatPropertyT<Selection> *selectionsProperty_;
  ///@}

  //----------------

  /** @name Type specific property-handles for predefined properties */
  ///@{
  static const PositionsHandle  POSITIONS_HANDLE;
  static const ColorsHandle     COLORS_HANDLE;
  static const NormalsHandle    NORMALS_HANDLE;
  static const PointsizesHandle POINTSIZES_HANDLE;
  static const IndicesHandle    INDICES_HANDLE;
  static const ViewlistsHandle  VIEWLISTS_HANDLE;
  static const SelectionsHandle SELECTIONS_HANDLE;
  ///@}

  //----------------------------------------------------------------

};


//================================================================


#if defined(INCLUDE_TEMPLATES) && !defined(SPLATCLOUDT_CC)
#  include "SplatCloudT.cc"
#endif


//================================================================


#endif // SPLATCLOUD_HH

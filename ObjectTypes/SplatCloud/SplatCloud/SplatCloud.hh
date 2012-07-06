/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

  /** \brief Clear the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   */
  void clearSplats();

  /** \brief Add one element at the end of the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   */
  void pushbackSplat();

  /** \brief Resize the data vector of all splat-properties.
   *
   * \note The number of splats will be updated accordingly.
   *
   * @param _num The new size of the storage vector
   */
  void resizeSplats( unsigned int _num );

  /** \brief Delete the elements with given indices from the data vector of all splat-properties.
   *
   *  -
   *  - Use std::vector<T2> / std::list<T2> / std::set<T2> / ... for T and int / unsigned int / ... for T2.
   *
   * \note The number of splats will be updated accordingly.
   *
   * @param _indices Index of the splats to be erased
   * @return The number of deleted elements.
   */
  template <typename T> unsigned int eraseSplatsByIndex( const T &_indices );

  /** \brief Delete the elements with flag != 0 from the data vector of all splat-properties.
   *
   * Use bool / unsigned char / ... for T.
   *
   * \note _flags.size() has to be equal to numSplats().
   * \note The number of splats will be updated accordingly.
   *
   * @param _indices Index of the splats to be erased
   * @return Returns the number of deleted elements.
   */
  template <typename T> unsigned int eraseSplatsByFlag( const std::vector<T> &_flags );

  /** \brief Get the number of splats.
   *
   * @return Number of splats
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

  /// Base class for all Property-Handles
  typedef std::string BasePropertyHandle;

  //----------------

  /** Interface for all Splat-Properties
   *
   */
  class SplatPropertyInterface
  {
    friend class SplatCloud; /// Let class SplatCloud access methods like clone().
  public:
    virtual size_t               sizeOf()    const = 0;        //!< Get the size of type.
    virtual const std::type_info &typeInfo() const = 0;        //!< Get the runtime type information.
  private:
    virtual inline ~SplatPropertyInterface() { }               //!< Destructor
    virtual SplatPropertyInterface *clone() const = 0;         //!< Return a deep copy of this.
    virtual void clear()                                  = 0; //!< Clear the data vector.
    virtual void pushback()                               = 0; //!< Add one element at the end of the data vector.
    virtual void resize( unsigned int _num )              = 0; //!< Resize the data vector.
    virtual void crop( const std::vector<int> &_indices ) = 0; //!< Keep only the elements with given indices in the data vector. The indices have to be valid, sorted and unique.
  private:
    void operator=( const SplatPropertyInterface & );          //!< Disallow the assign operator. (private and *not* implemented)
  };

  //----------------

  /** Interface for all Cloud-Properties
   *
   */
  class CloudPropertyInterface
  {
    friend class SplatCloud;                            //!< Let class SplatCloud access methods like clone().
  public:
    virtual size_t               sizeOf()    const = 0; //!< Get the size of type.
    virtual const std::type_info &typeInfo() const = 0; //!< Get the runtime type information.
  private:
    virtual inline ~CloudPropertyInterface() { }        //!< Destructor
    virtual CloudPropertyInterface *clone() const = 0;  //!< Return a deep copy of this.
  private:
    void operator=( const CloudPropertyInterface & );   //!< Disallow the assign operator. (private and *not* implemented)
  };

  //----------------

  /// Splat-Property map type
  typedef std::map<BasePropertyHandle, SplatPropertyInterface *> SplatProperties;

  //----------------

  /// Cloud-Property map type
  typedef std::map<BasePropertyHandle, CloudPropertyInterface *> CloudProperties;

  //----------------

public:

  /** Type specific class for Property-Handles
   *
   */
  template <typename T>
  class PropertyHandleT : public BasePropertyHandle
  {
  public:
    explicit inline PropertyHandleT<T>( const BasePropertyHandle &_handle ) : BasePropertyHandle( _handle ) { } /// Standard constructor
  };

  //----------------

  /** Type specific class for Splat-Properties
   *
   */
  template <typename T>
  class SplatPropertyT : public SplatPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    typedef typename std::vector<T>::      reference      Reference; //!< These are references to T, *not* to SplatPropertyT<T>.
    typedef typename std::vector<T>::const_reference ConstReference; //!< These are used because a std::vector<bool>::reference is *not* the same as a reference to bool.
  public:
    explicit inline SplatPropertyT<T>( const PropertyHandleT<T> &_handle, unsigned int _num ) : data_( _num ) { } //!< Standard constructor
  inline SplatPropertyT<T>( const SplatPropertyT<T> &_property ) { data_ = _property.data_; } //!< Copy constructor
    inline      Reference data( int _idx )       { return data_[ _idx ]; } //!< Access the data as reference.
    inline ConstReference data( int _idx ) const { return data_[ _idx ]; } //!< Access the data as const reference.
  public:
    virtual inline size_t               sizeOf()    const { return sizeof( T ); } //!< Get the size of type.
    virtual inline const std::type_info &typeInfo() const { return typeid( T ); } //!< Get the runtime type information.
  private:
    virtual inline ~SplatPropertyT<T>() { } //!< Destructor
    virtual inline SplatPropertyT<T> *clone() const { return new SplatPropertyT<T>( *this ); } //!< Return a deep copy of this.
    virtual inline void clear()                     { std::vector<T>().swap( data_ ); } //!< Clear the data vector.
    virtual inline void pushback()                  { data_.push_back( T() );         } //!< Add one element at the end of the data vector.
    virtual inline void resize( unsigned int _num ) { data_.resize( _num );           } //!< Resize the data vector.
    virtual        void crop( const std::vector<int> &_indices );                       //!< Keep only the elements with given indices in the data vector. The indices have to be valid, sorted and unique.
  private:
    void operator=( const SplatPropertyT<T> & ); //!< Disallow the assign operator. (private and *not* implemented)
  private:
    std::vector<T> data_; //!< The actual stored data (one entry per splat)
  };

  //----------------

  /** Type specific class for Cloud-Properties
   *
   */
  template <typename T>
  class CloudPropertyT : public CloudPropertyInterface
  {
    friend class SplatCloud; //!< Let class SplatCloud access methods like clone().
  public:
    typedef       T &      Reference; //!< These are references to T, *not* to CloudPropertyT<T>.
    typedef const T & ConstReference; //!< These are used only out of a consistency reason to the class SplatPropertyT<T>.
  public:
    explicit inline CloudPropertyT<T>( const PropertyHandleT<T> &_handle ) { } //!< Standard constructor
  inline CloudPropertyT<T>( const CloudPropertyT<T> &_property ) { data_ = _property.data_; } //!< Copy constructor
    inline      Reference data()       { return data_; } //!< Access the data as reference.
    inline ConstReference data() const { return data_; } //!< Access the data as const reference.
  public:
    virtual inline size_t               sizeOf()    const { return sizeof( T ); } //!< Get the size of type.
    virtual inline const std::type_info &typeInfo() const { return typeid( T ); } //!< Get the runtime type information.
  private:
    virtual inline ~CloudPropertyT<T>() { } //!< Destructor
    virtual inline CloudPropertyT<T> *clone() const { return new CloudPropertyT<T>( *this ); } //!< Return a deep copy of this.
  private:
    void operator=( const CloudPropertyT<T> & ); //!< Disallow the assign operator. (private and *not* implemented)
  private:
    T data_; //!< The actual stored data
  };

  //-- DECLARATIONS ------------------------------------------------

public:

  /// Clear Splat-Properties.
  void clearSplatProperties();
  /// Clear Cloud-Properties.
  void clearCloudProperties();

  /** \brief Add a new property
   * Add a new property. If the property is already present nothing is done.
   * - Do *not* use handle-strings containing '<' and '>'. These are reserved for predefined properties.
   * - For pointer versions:
   *    If the property is already present the pointer is set to the old property and no new property is created or added.
   *    If the property is *not* already present the pointer is set to the newly created property.
   *    If the creation fails the pointer is set to 0 and no new property is added.
   * - For convenience the success is returned. This is the same as checking for pointer != 0, but
   * - be careful, these methods are *not* in O(1). This is the reason why there is no method returning a pointer directly.
   * - Do *not* misuse one of these methods as a has<Splat|Cloud>Property( handle ) method multiple times.
   *   Instead call the <add|get><Splat|Cloud>Property( handle, property ) method once and then check for property != 0 multiple times if needed.
   * - The standard constructor of T is required.
   */
  template <typename T> inline bool addSplatProperty( const PropertyHandleT<T> &_handle ) { SplatPropertyT<T> *temp; return addSplatProperty( _handle, temp ); }
  template <typename T> inline bool addCloudProperty( const PropertyHandleT<T> &_handle ) { CloudPropertyT<T> *temp; return addCloudProperty( _handle, temp ); }
  template <typename T>        bool addSplatProperty( const PropertyHandleT<T> &_handle, SplatPropertyT<T> *(&_property) ); // no separate methods for const pointers
  template <typename T>        bool addCloudProperty( const PropertyHandleT<T> &_handle, CloudPropertyT<T> *(&_property) ); //   because methods are not const anyway

  /** This is equivalent to the addCloudProperty( handle ) method above but sets the property's data for convenience.
   * - Notice that the data is set (overwritten) either if the property is already present or not.
   * - The standard constructor and assign operator of T are required.
   */
  template <typename T> inline bool addCloudProperty( const PropertyHandleT<T> &_handle, const T &_data )
    { CloudPropertyT<T> *prop; if( addCloudProperty( _handle, prop ) ) { prop->data() = _data; return true; } return false; }

  /** Remove a property. If the property is *not* present nothing is done.
   * - Do *not* use handle-strings containing '<' and '>'. These are reserved for predefined properties.
   */
  template <typename T> void removeSplatProperty( const PropertyHandleT<T> &_handle );
  template <typename T> void removeCloudProperty( const PropertyHandleT<T> &_handle );

  /** Get a pointer to a property. If the property is *not* present the pointer is set to 0.
   * - Be careful, these methods are *not* in O(1). This is the reason why there is no method returning a pointer directly.
   */
  template <typename T> void getSplatProperty( const PropertyHandleT<T> &_handle,       SplatPropertyT<T> *(&_property) );
  template <typename T> void getSplatProperty( const PropertyHandleT<T> &_handle, const SplatPropertyT<T> *(&_property) ) const;
  template <typename T> void getCloudProperty( const PropertyHandleT<T> &_handle,       CloudPropertyT<T> *(&_property) );
  template <typename T> void getCloudProperty( const PropertyHandleT<T> &_handle, const CloudPropertyT<T> *(&_property) ) const;

  /** Get all Splat- or Cloud-Properties.
   * - Be careful, the count( handle ) method will *not* check for the appropiate type of a property.
   * - There is no has<Splat|Cloud>Property( handle ) method because this would *not* be in O(1).
   *   Instead call the get<Splat|Cloud>Property( handle, property ) method once and then check for property != 0 multiple times if needed.
   */
  inline const SplatProperties &splatProperties() const { return splatProperties_; }
  inline const CloudProperties &cloudProperties() const { return cloudProperties_; }

private:

  /// Deep copy Splat- or Cloud-Properties.
  void copySplatProperties( const SplatCloud &_splatCloud );
  void copyCloudProperties( const SplatCloud &_splatCloud );

  // == MEMBERS =====================================================

private:

  /// Splat-Property map
  SplatProperties splatProperties_;
  /// Cloud-Property map
  CloudProperties cloudProperties_;

  //----------------------------------------------------------------


  // +---------------------------+
  // |                           |
  // |   Predefined Properties   |
  // |                           |
  // +---------------------------+

  // == TYPEDEFS =====================================================

public:

  typedef struct
  {
    int   cameraIdx_;
    int   featureIdx_;
    float x_, y_;
  } View;

  /// Value types for predefined properties
  typedef ACG::Vec3f        Position;
  typedef ACG::Vec3uc       Color;
  typedef ACG::Vec3f        Normal;
  typedef float             Pointsize;
  typedef int               Index;
  typedef std::vector<View> Viewlist;
  typedef unsigned char     Selection; // Do *not* use bool to prevent further trouble because a std::vector<bool>::reference is *not* the same as a reference to bool.

  /// Vector types for predefined properties
  typedef std::vector<Position > PositionVector;
  typedef std::vector<Color    > ColorVector;
  typedef std::vector<Normal   > NormalVector;
  typedef std::vector<Pointsize> PointsizeVector;
  typedef std::vector<Index    > IndexVector;
  typedef std::vector<Viewlist > ViewlistVector;
  typedef std::vector<Selection> SelectionVector;

private:

  /// Type specific Property-Handle types for predefined properties
  typedef PropertyHandleT<Position > PositionsHandle;
  typedef PropertyHandleT<Color    > ColorsHandle;
  typedef PropertyHandleT<Normal   > NormalsHandle;
  typedef PropertyHandleT<Pointsize> PointsizesHandle;
  typedef PropertyHandleT<Index    > IndicesHandle;
  typedef PropertyHandleT<Viewlist > ViewlistsHandle;
  typedef PropertyHandleT<Selection> SelectionsHandle;

  //-- DECLARATIONS ------------------------------------------------

public:

  // Make a predefined property available. If the predefined property is already available nothing is done.
  // - For convenience the availability of the predefined property is returned, but
  // - be careful, these methods are *not* in O(1).
  // - Do *not* misuse one of these methods as a has...() method multiple times.
  //   Instead use the has...() method because this is in O(1).
  inline bool requestPositions()  { return addSplatProperty( POSITIONS_HANDLE,  positionsProperty_  ); }
  inline bool requestColors()     { return addSplatProperty( COLORS_HANDLE,     colorsProperty_     ); }
  inline bool requestNormals()    { return addSplatProperty( NORMALS_HANDLE,    normalsProperty_    ); }
  inline bool requestPointsizes() { return addSplatProperty( POINTSIZES_HANDLE, pointsizesProperty_ ); }
  inline bool requestIndices()    { return addSplatProperty( INDICES_HANDLE,    indicesProperty_    ); }
  inline bool requestViewlists()  { return addSplatProperty( VIEWLISTS_HANDLE,  viewlistsProperty_  ); }
  inline bool requestSelections() { return addSplatProperty( SELECTIONS_HANDLE, selectionsProperty_ ); }

  // Remove a predefined property. If the predefined property is *not* available nothing is done.
  // - Be careful, these methods are *not* in O(1).
  inline void releasePositions()  { removeSplatProperty( POSITIONS_HANDLE  ); positionsProperty_  = 0; }
  inline void releaseColors()     { removeSplatProperty( COLORS_HANDLE     ); colorsProperty_     = 0; }
  inline void releaseNormals()    { removeSplatProperty( NORMALS_HANDLE    ); normalsProperty_    = 0; }
  inline void releasePointsizes() { removeSplatProperty( POINTSIZES_HANDLE ); pointsizesProperty_ = 0; }
  inline void releaseIndices()    { removeSplatProperty( INDICES_HANDLE    ); indicesProperty_    = 0; }
  inline void releaseViewlists()  { removeSplatProperty( VIEWLISTS_HANDLE  ); viewlistsProperty_  = 0; }
  inline void releaseSelections() { removeSplatProperty( SELECTIONS_HANDLE ); selectionsProperty_ = 0; }

  // Return the availability of a predefined property.
  // - In contrast to the general properties these methods are in O(1) so they can be provided safely.
  inline bool hasPositions()  const { return (positionsProperty_  != 0); }
  inline bool hasColors()     const { return (colorsProperty_     != 0); }
  inline bool hasNormals()    const { return (normalsProperty_    != 0); }
  inline bool hasPointsizes() const { return (pointsizesProperty_ != 0); }
  inline bool hasIndices()    const { return (indicesProperty_    != 0); }
  inline bool hasViewlists()  const { return (viewlistsProperty_  != 0); }
  inline bool hasSelections() const { return (selectionsProperty_ != 0); }

  // Get a reference of a predefined property's entry with given index.
  // - Be careful, these methods do *not* check if a predefined property is available and will probably crash if it's not.
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
    getSplatProperty( POSITIONS_HANDLE,  positionsProperty_  );
    getSplatProperty( COLORS_HANDLE,     colorsProperty_     );
    getSplatProperty( NORMALS_HANDLE,    normalsProperty_    );
    getSplatProperty( POINTSIZES_HANDLE, pointsizesProperty_ );
    getSplatProperty( INDICES_HANDLE,    indicesProperty_    );
    getSplatProperty( VIEWLISTS_HANDLE,  viewlistsProperty_  );
    getSplatProperty( SELECTIONS_HANDLE, selectionsProperty_ );
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

  /// Pointers to predefined properties
  SplatPropertyT<Position>  *positionsProperty_;
  SplatPropertyT<Color>     *colorsProperty_;
  SplatPropertyT<Normal>    *normalsProperty_;
  SplatPropertyT<Pointsize> *pointsizesProperty_;
  SplatPropertyT<Index>     *indicesProperty_;
  SplatPropertyT<Viewlist>  *viewlistsProperty_;
  SplatPropertyT<Selection> *selectionsProperty_;

  /// Type specific Property-Handles for predefined properties
  static const PositionsHandle  POSITIONS_HANDLE;
  static const ColorsHandle     COLORS_HANDLE;
  static const NormalsHandle    NORMALS_HANDLE;
  static const PointsizesHandle POINTSIZES_HANDLE;
  static const IndicesHandle    INDICES_HANDLE;
  static const ViewlistsHandle  VIEWLISTS_HANDLE;
  static const SelectionsHandle SELECTIONS_HANDLE;

  //----------------------------------------------------------------

};


//================================================================


#if defined(INCLUDE_TEMPLATES) && !defined(SPLATCLOUDT_CC)
#  include "SplatCloudT.cc"
#endif


//================================================================


#endif // SPLATCLOUD_HH

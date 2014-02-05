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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26 Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef PROPERTIES_HH
#define PROPERTIES_HH

/**
 * @file Properties.hh
 * @brief The header for the properties, one file for all objects
 *
 * All methods that are declared inside of template classes go into PropertiesT.cc, all other methods
 * (those that are declared in normal classes) go into Properties.cc. Otherwise including the .cc
 * (see end of this header) results in linker problems.
 */

#include <vector>
#include <map>
#include <string>

#include <OpenFlipper/common/GlobalDefines.hh>


/**
 * @brief The property handle, use it to access the properties
 *
 * The template parameter corresponds to the type stored by this property.
 */
template<typename T>
class PropertyHandleT
{
  friend class Properties;

protected:
  typedef T value_type;

public:
  PropertyHandleT(int _idx = -1);
  virtual ~PropertyHandleT();

  inline bool isValid();

protected:
  int idx_; ///< The properties index
};

/**
 * @brief The properties storage class
 *
 * This class is equipped with all methods you need to access the properties. Derive your class from it,
 * then call the Properties::insert_property_at and Properties::remove_property_at members, whenever the
 * vector holding the objects this property class refers to changes. The properties are bound to objects
 * by their integer based index.
 */
class DLLEXPORT Properties
{
protected:
  /**
    * @brief The base class for all property template instances
    *
    * This class is used as an abstraction layer for the numerous instances of the PropertyT template.
    * Its easier to store them in a vector this way.
    */
  class DLLEXPORT BaseProperty
  {
    friend class Properties;
  public:
    BaseProperty();
    virtual ~BaseProperty();

  protected:
    /**
      * @name Synchronization
      * Called by the Properties container to keep the indices in sync.
      */
    //@{
    virtual void insert_at(int _index) = 0;		///< Called by Properties as new objects are inserted in the derived class
    virtual void remove_at(int _index) = 0;		///< Called by Properties as objects are deleted in the derived class
    virtual void clear() = 0;					///< Called if all values have to be erased from this property
    //@}
  };

  /**
    * @brief A container storing a single property for all objects
    *
    * The template type is determined by the property handle PropertyHandleT used to add it to the
    * Properties container.
    */
  template<typename T>
  class PropertyT : public BaseProperty
  {
    friend class Properties;

  public:
    PropertyT(unsigned long _size = 0);
    virtual ~PropertyT();

  public:
    inline T &operator[](int _index);

  protected:
    /**
      * @name Synchronization
      * Called by the Properties container to keep the indices in sync.
      */
    //@{
    void insert_at(int _index);
    void remove_at(int _index);
    void clear();
    //@}

  protected:
    std::vector<T> values_;		///< Stores a property for every object in the class derived from the Properties class. Access by index.
  };

public:
  Properties();
  virtual ~Properties();

public:
  /**
    * @name Property management
    * Use these to edit the properties themself, not their values.
    */
  //@{
  template<typename T> bool add_property(PropertyHandleT<T> &_hProp, std::string _name);
  template<typename T> bool get_property(PropertyHandleT<T> &_hProp, std::string _name);
  bool has_property(std::string _name);
  template<typename T> bool remove_property(PropertyHandleT<T> &_hProp);
  void clear_properties();
  //@}

  /**
    * @name Property access
    * Edit the properties values.
    */
  //@{
  template<typename T> T &property(PropertyHandleT<T> &_hProp, int _index);
  //@}

protected:
  /**
    * @name Synchronization
    * Call these methods to keep the property indices in sync with the objects they are bound to.
    */
  //@{
  void insert_property_at(int _index);
  void remove_property_at(int _index);
  void clean_properties();
  //@}

protected:
  std::map<std::string, int> property_names_; ///< The property names, key holding the name, value the properties index in Properties::properties_.
  std::vector<BaseProperty*> properties_;
  unsigned long size_;                        ///< The number of fields in each property, used when new properties have to be created.
};

/**
 * @var Properties::properties_
 * @brief A vector holding the properties.
 *
 * Access them using a property handle, that in turn is storing a index derived from
 * Properties::property_names_. If a property is deleted its position in the vector is deleted and
 * set to zero. If a new property is added it will either take a place in the vector that is zero
 * or append itself to the end. This way the property handles stay valid all the time.
 */

//=============================================================================
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(PROPERTIES_C)
#define PROPERTIES_TEMPLATES
#include "PropertiesT.cc"
#endif
//=============================================================================
#endif // PROPERTIES_HH defined
//=============================================================================


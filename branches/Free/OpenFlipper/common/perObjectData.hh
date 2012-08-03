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




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file perObjectData.hh
 * This File contains a basic data class used to attach data to objects.
 */


#ifndef PEROBJECTDATA_HH
#define PEROBJECTDATA_HH


//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>

//== TYPEDEFS =================================================================


//== CLASS DEFINITION =========================================================

/** \brief Object Payload
 *
 * This class is used to add arbitrary data to objects
 * in OpenFlipper. You can derive any kind of class from
 * PerObjectData and attach it to an object. See
 * BaseObject::setObjectData() for more details.
 *
 * */
class DLLEXPORT PerObjectData {

  public :
    /// You have to provide your own constructor for your object
    PerObjectData();
    
    /** \brief Copy Constructor
    * You have to reimplement this copy constructor for your object to get copied by the core. Otherwise, 
    * per Object data will not be copied when the core creates a copy of your object.
    *
    * If you reimplemented the function, set copyOk_ at the end of the function.
    */


    virtual ~PerObjectData();

    /** \brief Copy Function
    * You have to reimplement this function to create a copy of your Object.
    * By default it will return 0;
    */
    virtual PerObjectData* copyPerObjectData( );

};

//=============================================================================

#define PER_OBJECT_DATA(_CLASS, _VALUE)                                      \
class DLLEXPORT _CLASS : public PerObjectData {                              \
                                                                             \
  public:                                                                    \
    inline _CLASS() { }                                                      \
    inline _CLASS(const _VALUE& _x) : data_(_x) { }                          \
    inline ~_CLASS() { }                                                     \
    inline virtual _CLASS* copyPerObjectData() { return new _CLASS(data_); } \
    inline       _VALUE& data()       { return data_; }                      \
    inline const _VALUE& data() const { return data_; }                      \
  private:                                                                   \
    _VALUE data_;                                                            \
}

//=============================================================================

PER_OBJECT_DATA(BoolPerObjectData,   bool         );
PER_OBJECT_DATA(CharPerObjectData,   char         );
PER_OBJECT_DATA(UCharPerObjectData,  unsigned char);
PER_OBJECT_DATA(IntPerObjectData,    int          );
PER_OBJECT_DATA(UIntPerObjectData,   unsigned int );
PER_OBJECT_DATA(FloatPerObjectData,  float        );
PER_OBJECT_DATA(DoublePerObjectData, double       );

//=============================================================================
#endif // PEROBJECTDATA_HH defined
//=============================================================================
